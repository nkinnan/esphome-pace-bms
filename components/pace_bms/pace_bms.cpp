#include "pace_bms.h"
#include "esphome/core/log.h"
#include <cinttypes>
#include <iomanip>
#include <sstream>
#include <functional>

namespace esphome {
namespace pace_bms { 

static const char* const TAG = "pace_bms";
void PaceBms::setup() {
  if (this->protocol_version_ != 0x25) {
    this->status_set_error();
    ESP_LOGE(TAG, "Protocol version %02X is not supported", this->protocol_version_);
    return;
  }
  else {
    // the protocol en/decoder PaceBmsV25 is meant to be standalone with no dependencies - inject esphome logging function wrappers on construction
    this->pace_bms_v25_ = new PaceBmsV25(error_log_func, warning_log_func, info_log_func, verbose_log_func);
  }

  if (this->flow_control_pin_ != nullptr) 
    this->flow_control_pin_->setup();
}

// fills up command_queue_ based on what was subscribed for by sensor instances via callbacks, for a full update cycle of BMS request/response pairs 
void PaceBms::update() {
  if (this->pace_bms_v25_ == nullptr)
    return;

 if (!command_queue_.empty()) {
    ESP_LOGW(TAG, "Commands still in queue on update(), skipping this cycle: Could not speak with the BMS fast enough, increase update_interval, or reduce request_throttle setting in YAML, or reduce response_timeout.", command_queue_.size());
  }
  else {
    const int item_count_to_queue = 100;

    for(int i = 0; i < 100; i++)
    {
      command_item* item = new command_item;
      item->description_ = std::string("read analog information");
      command_queue_.push(item);
    }

    ESP_LOGV(TAG, "Update commands queued: %i", command_queue_.size());
  }
}

// incrementally process incoming bytes off the bus
//     eventually dispatching a full response to process_response_frame_ and then calling send_next_request_frame to continue poping the command_queue_
//     or just calling send_next_request_frame_ if previous request timed out and there are still commands in queue
void PaceBms::loop() {
  if (this->pace_bms_v25_ == nullptr)
    return;

  // if there is no request active, throw away any incoming data before proceeding
  if (this->request_outstanding_ == false &&
      this->available() != 0) {
    ESP_LOGV(TAG, "Throwing away incoming data because there is no request active");
    uint8_t byte;
    while (this->available() != 0) {
        this->read_byte(&byte);
    }
  }

  const uint32_t now = millis();

  // if no request is active, we are not throttled, and there are pending requests to send, do so
  if (this->request_outstanding_ == false &&
      now - this->last_transmit_ >= this->request_throttle_ &&
      this->command_queue_.size() > 0) {
    this->send_next_request_frame_();
  }
}

// preferably we'll be setup after all child sensors have registered their callbacks via their own setup(), but this class still handle the case where they register late, a single update cycle will simply be skipped in that case
float PaceBms::get_setup_priority() const { return setup_priority::LATE; }

// pops the next item off of this->command_queue_, generates and dispatches a request frame, and sets up this->next_response_handler_
void PaceBms::send_next_request_frame_() {

    if (command_queue_.empty()) {
      ESP_LOGV(TAG, "command queue empty on send_next_request_frame");
      return;
    }
    PaceBms::command_item* command = command_queue_.front();
    command_queue_.pop();
    free(command);

}

void PaceBms::dump_config() {
  ESP_LOGCONFIG(TAG, "pace_bms:");
  LOG_PIN(           "  Flow Control Pin: ", this->flow_control_pin_);
  ESP_LOGCONFIG(TAG, "  Address: %i", this->address_);
  ESP_LOGCONFIG(TAG, "  Protocol Version: 0x%02X", this->protocol_version_);
  ESP_LOGCONFIG(TAG, "  Request Throttle (ms): %i", this->request_throttle_);
  ESP_LOGCONFIG(TAG, "  Response Timeout (ms): %i", this->response_timeout_);
  this->check_uart_settings(9600);
}

}  // namespace pace_bms
}  // namespace esphome
