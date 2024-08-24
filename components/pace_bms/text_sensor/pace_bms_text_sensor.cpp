#include "pace_bms_text_sensor.h"
#include "esphome/core/log.h"

#include <functional>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.sensor";

void PaceBmsTextSensor::setup() {
  if (request_status_info_callback_ == true) {
    this->parent_->register_status_information_callback(std::bind(&esphome::pace_bms::PaceBmsTextSensor::status_information_callback, this, std::placeholders::_1));
  }
}

float PaceBmsTextSensor::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsTextSensor::dump_config() { 
	ESP_LOGCONFIG(TAG, "pace_bms_sensor:");
	LOG_SENSOR("  ", "Warning Status", this->warning_status_sensor_);
	LOG_SENSOR("  ", "Balancing Status", this->balancing_status_sensor_);
	LOG_SENSOR("  ", "System Status", this->system_status_sensor_);
	LOG_SENSOR("  ", "Configuration Status", this->configuration_status_sensor_);
	LOG_SENSOR("  ", "Protection Status", this->protection_status_sensor_);
	LOG_SENSOR("  ", "Fault Status", this->fault_status_sensor_);
}

void PaceBmsTextSensor::status_information_callback(PaceBmsV25::StatusInformation& status_information) {
  if (this->warning_status_sensor_ != nullptr) {
    this->warning_status_sensor_->publish_state(status_information.warningText);
  }
  if (this->balancing_status_sensor_ != nullptr) {
    this->balancing_status_sensor_->publish_state(status_information.balancingText);
  }
  if (this->system_status_sensor_ != nullptr) {
    this->system_status_sensor_->publish_state(status_information.systemText);
  }
  if (this->configuration_status_sensor_ != nullptr) {
    this->configuration_status_sensor_->publish_state(status_information.configurationText);
  }
  if (this->protection_status_sensor_ != nullptr) {
    this->protection_status_sensor_->publish_state(status_information.protectionText);
  }
  if (this->fault_status_sensor_ != nullptr) {
    this->fault_status_sensor_->publish_state(status_information.faultText);
  }
}


}  // namespace pace_bms
}  // namespace esphome
