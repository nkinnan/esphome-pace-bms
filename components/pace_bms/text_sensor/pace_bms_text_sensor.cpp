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
  if (request_hardware_version_callback_ == true) {
    this->parent_->register_hardware_version_callback(std::bind(&esphome::pace_bms::PaceBmsTextSensor::hardware_version_callback, this, std::placeholders::_1));
  }
  if (request_serial_number_callback_ == true) {
    this->parent_->register_serial_number_callback(std::bind(&esphome::pace_bms::PaceBmsTextSensor::serial_number_callback, this, std::placeholders::_1));
  }
}

float PaceBmsTextSensor::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsTextSensor::dump_config() { 
	ESP_LOGCONFIG(TAG, "pace_bms_text_sensor:");
	LOG_TEXT_SENSOR("  ", "Warning Status", this->warning_status_sensor_);
	LOG_TEXT_SENSOR("  ", "Balancing Status", this->balancing_status_sensor_);
	LOG_TEXT_SENSOR("  ", "System Status", this->system_status_sensor_);
	LOG_TEXT_SENSOR("  ", "Configuration Status", this->configuration_status_sensor_);
	LOG_TEXT_SENSOR("  ", "Protection Status", this->protection_status_sensor_);
	LOG_TEXT_SENSOR("  ", "Fault Status", this->fault_status_sensor_);
	LOG_TEXT_SENSOR("  ", "Hardware Version", this->hardware_version_sensor_);
	LOG_TEXT_SENSOR("  ", "Serial Number", this->serial_number_sensor_);
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

void PaceBmsTextSensor::hardware_version_callback(std::string& hardware_version) {
	if (this->hardware_version_sensor_ != nullptr) {
		this->hardware_version_sensor_->publish_state(hardware_version);
	}
}
void PaceBmsTextSensor::serial_number_callback(std::string& serial_number) {
	if (this->serial_number_sensor_ != nullptr) {
		this->serial_number_sensor_->publish_state(serial_number);
	}
}

}  // namespace pace_bms
}  // namespace esphome