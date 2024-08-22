#include "pace_bms.h"
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
    this->pace_bms_v25 = new PaceBms(error_log_func, warning_log_func, info_log_func, verbose_log_func);
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
      parse_data_frame_(this->raw_data_, this->raw_data_index_ + 1);
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

  std::vector<uint8_t> frame(frame_bytes, frame_bytes + frame_length);

  PaceBms::AnalogInformation analog_information;
  this->pace_bms_v25->ProcessReadAnalogInformationResponse(1, frame, analog_information);

   
}

void PaceBmsComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "CSE7766:");
  LOG_SENSOR("  ", "Voltage", this->voltage_sensor_);
  LOG_SENSOR("  ", "Current", this->current_sensor_);
  LOG_SENSOR("  ", "Power", this->power_sensor_);
  LOG_SENSOR("  ", "Energy", this->energy_sensor_);
  LOG_SENSOR("  ", "Apparent Power", this->apparent_power_sensor_);
  LOG_SENSOR("  ", "Reactive Power", this->reactive_power_sensor_);
  LOG_SENSOR("  ", "Power Factor", this->power_factor_sensor_);
  this->check_uart_settings(9600);
}

}  // namespace pace_bms
}  // namespace esphome
