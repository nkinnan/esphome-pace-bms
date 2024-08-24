#include "pace_bms.h"
#include "esphome/core/log.h"
#include <cinttypes>
#include <iomanip>
#include <sstream>
#include <functional>

namespace esphome {
namespace pace_bms { 

static const char* const TAG = "pace_bms";
static const char* const TAG_V25 = "pace_bms_v25";

void error_log_func(std::string message) {
    ESP_LOGE(TAG_V25, "%s", message.c_str());
}

void warning_log_func(std::string message) {
    ESP_LOGW(TAG_V25, "%s", message.c_str());
}

void info_log_func(std::string message) {
    ESP_LOGI(TAG_V25, "%s", message.c_str());
}

void verbose_log_func(std::string message) {
    ESP_LOGV(TAG_V25, "%s", message.c_str());
}

void PaceBms::setup() {
    if (this->protocol_version_ != 0x25) {
      this->status_set_error();
      ESP_LOGE(TAG, "Protocol version %02X is not supported", this->protocol_version_);
    }
    else {
      this->pace_bms_v25_ = new PaceBmsV25(error_log_func, warning_log_func, info_log_func, verbose_log_func);
      if (this->flow_control_pin_ != nullptr) {
          this->flow_control_pin_->setup();
      }
    }
}

// fills up command_queue_ for a full cycle of request/response + dispatch to sensors
void PaceBms::update() {
    if (this->pace_bms_v25_ == nullptr)
        return;

    if (!command_queue_.empty()) {
        ESP_LOGW(TAG, "Commands still in queue on update(), skipping this cycle: Could not speak with the BMS fast enough, reduce update_interval setting in YAML.", command_queue_.size());
    }
    else {
        if (this->analog_information_callbacks_.size() > 0) {
            command_item* item = new command_item;
            item->description_ = "analog information";
            item->create_request_frame_ = std::bind(&PaceBmsV25::CreateReadAnalogInformationRequest, this->pace_bms_v25_, this->address_, std::placeholders::_1);
            item->process_response_frame_ = std::bind(&esphome::pace_bms::PaceBms::handle_analog_information_response, this, std::placeholders::_1);
            command_queue_.push(item);
        }
        if (this->status_information_callbacks_.size() > 0) {
            command_item* item = new command_item;
            item->description_ = "status information";
            item->create_request_frame_ = std::bind(&PaceBmsV25::CreateReadStatusInformationRequest, this->pace_bms_v25_, this->address_, std::placeholders::_1);
            item->process_response_frame_ = std::bind(&esphome::pace_bms::PaceBms::handle_status_information_response, this, std::placeholders::_1);
            command_queue_.push(item);
        }
        ESP_LOGV(TAG, "Update commands queued: %i", command_queue_.size());
    }
}

// incrementally process incoming bytes off the bus
//     eventually dispatching a response to process_response_frame_ and calling send_next_request_frame to continue the command_queue_
//     or calling send_next_request_frame_ if previous request timed out and there are still commands in queue
void PaceBms::loop() {
  if (this->pace_bms_v25_ == nullptr)
    return;

  const uint32_t now = millis();
  if (now - this->last_transmission_ >= 500) { // todo: settable read timeout
    // last transmission too long ago. Reset RX index. 
    if (this->raw_data_index_ > 0) {
        std::string str(this->raw_data_, this->raw_data_ + this->raw_data_index_ + 1);
        ESP_LOGV(TAG, "Response frame timeout, partial frame: %s", str.c_str());
    }
    else {
        ESP_LOGV(TAG, "Response frame timeout (or first request)");
    }
    request_outstanding_ = false;
    this->raw_data_index_ = 0;
    if(this->command_queue_.size() > 0)
      this->send_next_request_frame_();
  }

  if (this->available() == 0) {
    return;
  }

  this->last_transmission_ = now;
  while (this->available() != 0) {
    this->read_byte(&this->raw_data_[this->raw_data_index_]);

    if (this->raw_data_index_ == 0 && this->raw_data_[this->raw_data_index_] != '~') {
      // don't reset request_outstanding_, instead wait for timeout to ensure no garbage is still streaming in when we dispatch the next request
      ESP_LOGV(TAG, "Response frame does not begin with '~': 0x%02X = '%c'", this->raw_data_[this->raw_data_index_], this->raw_data_[this->raw_data_index_]);
      this->raw_data_index_ = 0;
      continue;
    }

    if (this->raw_data_[this->raw_data_index_] == '\r') {
      request_outstanding_ = false;
      this->process_response_frame_(this->raw_data_, this->raw_data_index_ + 1);
      this->raw_data_index_ = 0;
      this->send_next_request_frame_();
      continue;
    }

    if (this->raw_data_index_ + 1 >= this->max_data_len_) {
      // don't reset request_outstanding_, instead wait for timeout to ensure no garbage is still streaming in when we dispatch the next request
      std::string str(this->raw_data_, this->raw_data_ + this->raw_data_index_ + 1);
      ESP_LOGV(TAG, "Response frame exceeds maximum supported length, partial frame: %s", str.c_str());
      this->raw_data_index_ = 0;
      continue;
    }

    this->raw_data_index_++;
  }
}

// preferably we'll be setup after all child sensors have registered their callbacks via their own setup(), but this will still handle the case where they register late, a single update cycle will simply be skipped in that case
float PaceBms::get_setup_priority() const { return setup_priority::LATE; }

// pops the next item off of this->command_queue_, generates and dispatches a request frame, and sets up this->next_response_handler_
void PaceBms::send_next_request_frame_() {

    if (command_queue_.empty()) {
      ESP_LOGV(TAG, "command queue empty on send_next_request_frame");
      return;
    }
    PaceBms::command_item* command = command_queue_.front();
    command_queue_.pop();
    std::vector<uint8_t> request;
    command->create_request_frame_(request);

#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERY_VERBOSE
    {
        std::string str(request.data(), request.data() + request.size());
        ESP_LOGVV(TAG, "Sending request frame: %s", str);
    }
#endif

    this->request_outstanding_ = true;
    // process_response_frame_ will call this on the next frame received
    this->next_response_handler_ = command->process_response_frame_;

    ESP_LOGV(TAG, "Sending %s request", command->description_.c_str());
    if (this->flow_control_pin_ != nullptr)
        this->flow_control_pin_->digital_write(true);
    this->write_array(request.data(), request.size());
    // if flow control is required (rs485 does read/write on the same differential lines) then I don't see any other option than to block on flush()
    // if using rs232, a flow control pin should not be assigned in order to avoid this block
    if (this->flow_control_pin_ != nullptr)
        this->flush();
    if (this->flow_control_pin_ != nullptr)
        this->flow_control_pin_->digital_write(false);

    free(command);
}

// calls this->next_response_handler_ (set up from the previously dispatched command_queue_ item)
void PaceBms::process_response_frame_(uint8_t* frame_bytes, uint8_t frame_length) {
#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERY_VERBOSE
  {
    std::string str(frame_bytes, frame_bytes + frame_length);
    ESP_LOGVV(TAG, "Response frame received: %s", str);
  }
#endif

  std::vector<uint8_t> response(frame_bytes, frame_bytes + frame_length);

  if (next_response_handler_ != nullptr)
    next_response_handler_(response);
  else
    ESP_LOGE(TAG, "Response frame received but no response handler set");
}


void PaceBms::handle_analog_information_response(std::vector<uint8_t>& response) {
  ESP_LOGV(TAG, "Processing analog information response");
  PaceBmsV25::AnalogInformation analog_information;
  this->pace_bms_v25_->ProcessReadAnalogInformationResponse(this->address_, response, analog_information);

  // dispatch to any child sensor components that registered for a callback with us
  for (int i = 0; i < this->analog_information_callbacks_.size(); i++) {
    analog_information_callbacks_[i](analog_information);
  }
}

void PaceBms::handle_status_information_response(std::vector<uint8_t>& response) {
  ESP_LOGV(TAG, "Processing status information response");
  PaceBmsV25::StatusInformation status_information;
  this->pace_bms_v25_->ProcessReadStatusInformationResponse(this->address_, response, status_information);

  // dispatch to any child sensor components that registered for a callback with us
  for (int i = 0; i < this->status_information_callbacks_.size(); i++) {
    status_information_callbacks_[i](status_information);
  }
}


void PaceBms::dump_config() {
  ESP_LOGCONFIG(TAG, "pace_bms:");
  LOG_PIN(           "  Flow Control Pin: ", this->flow_control_pin_);
  ESP_LOGCONFIG(TAG, "  Address: %i", this->address_);
  ESP_LOGCONFIG(TAG, "  Protocol Version: 0x%02X", this->protocol_version_);
  this->check_uart_settings(9600);
}

}  // namespace pace_bms
}  // namespace esphome
