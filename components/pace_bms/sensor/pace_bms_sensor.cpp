/*nclude "pace_bms.h"
#include "esphome/core/log.h"
#include <cinttypes>
#include <iomanip>
#include <sstream>

namespace esphome {
namespace pace_bms {

static const char *const TAG = "pace_bms";

void error_log_func(std::string message) {
    ESP_LOGE(TAG, "%s", message);
}

void warning_log_func(std::string message) {
    ESP_LOGW(TAG, "%s", message);
}

void info_log_func(std::string message) {
    ESP_LOGI(TAG, "%s", message);
}

void verbose_log_func(std::string message) {
    ESP_LOGV(TAG, "%s", message);
}

void PaceBmsComponent::setup() {
    this->pace_bms_v25_ = new PaceBmsV25(error_log_func, warning_log_func, info_log_func, verbose_log_func);
    if (this->flow_control_pin_ != nullptr) {
        this->flow_control_pin_->setup();
    }
}

void PaceBmsComponent::update() {
    //if (!command_queue_.empty()) {
    //    ESP_LOGV(TAG, "%zu modbus commands already in queue", command_queue_.size());
    //}
    //else {
    ESP_LOGV(TAG, "***********Requesting analog information");
    std::vector<uint8_t> request;
    this->pace_bms_v25_->CreateReadAnalogInformationRequest(1, request);

    if (this->flow_control_pin_ != nullptr)
        this->flow_control_pin_->digital_write(true);
    this->write_array(request.data(), request.size());
    this->flush();
    if (this->flow_control_pin_ != nullptr)
        this->flow_control_pin_->digital_write(false);
    //}

    //for (auto& r : this->register_ranges_) {
    //    ESP_LOGVV(TAG, "Updating range 0x%X", r.start_address);
    //    update_range_(r);
    //}
}

void PaceBmsComponent::loop() {
  const uint32_t now = millis();
  if (now - this->last_transmission_ >= 500) {
    // last transmission too long ago. Reset RX index.
    if (this->raw_data_index_ > 0) {
        this->raw_data_[this->raw_data_index_] = 0;
        ESP_LOGV(TAG, "Data frame timeout, partial frame: %s", this->raw_data_);
    }
    this->raw_data_index_ = 0;
  }

  if (this->available() == 0) {
    return;
  }

  this->last_transmission_ = now;
  while (this->available() != 0) {
    this->read_byte(&this->raw_data_[this->raw_data_index_]);

    if (this->raw_data_index_ == 0 && this->raw_data_[this->raw_data_index_] != '~') {
      ESP_LOGE(TAG, "Data frame does not start with '~'");
      continue;
    }

    if (this->raw_data_[this->raw_data_index_] == '\r') {
      this->parse_data_frame_(this->raw_data_, this->raw_data_index_ + 1);
      continue;
    }

    this->raw_data_index_++;
    if (this->raw_data_index_ >= this->max_data_len_) {
      std::string str(this->raw_data_, this->raw_data_ + this->raw_data_index_);
      ESP_LOGE(TAG, "Data frame exceeds maximum length, partial frame: %s", str);
      this->raw_data_index_ = 0;
      continue;
    }
  }
}

float PaceBmsComponent::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsComponent::parse_data_frame_(uint8_t* frame_bytes, uint8_t frame_length) {
#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERY_VERBOSE
  {
    std::string str(frame_bytes, frame_bytes + frame_length);
    ESP_LOGVV(TAG, "Raw data: %s", str);
  }
#endif

  std::vector<uint8_t> response(frame_bytes, frame_bytes + frame_length);

  ESP_LOGV(TAG, "Processing analog information response");
  PaceBmsV25::AnalogInformation analog_information;
  this->pace_bms_v25_->ProcessReadAnalogInformationResponse(1, response, analog_information);

  if (this->voltage_sensor_ != nullptr)
      this->voltage_sensor_->publish_state(analog_information.totalVoltageMillivolts / 1000.0f);
}

void PaceBmsComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "pace_bms:");
  LOG_PIN("  Flow Control Pin: ", this->flow_control_pin_);
  LOG_SENSOR("  ", "Voltage", this->voltage_sensor_);
  this->check_uart_settings(9600);
}

}  // namespace pace_bms
}  // namespace esphome
*/