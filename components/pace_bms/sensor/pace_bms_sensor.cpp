#include "pace_bms_sensor.h"
#include "esphome/core/log.h"

#include <functional>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.sensor";

void PaceBmsSensor::setup() {
  if (request_analog_info_callback_ == true) {
    this->parent_->register_analog_information_callback(std::bind(&esphome::pace_bms::PaceBmsSensor::analog_information_callback, this, std::placeholders::_1));
  }
}

float PaceBmsSensor::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsSensor::dump_config() { 
	//LOG_SENSOR("", "Sun Sensor", this); 
	ESP_LOGCONFIG(TAG, "pace_bms_sensor:");
	LOG_SENSOR("  ", "Cell Count", this->cell_count_sensor_);
	for (int i = 0; i < 16; i++)
	  LOG_SENSOR("  ", "Cell Voltage X of 16", this->cell_voltage_sensor_[i]);
	LOG_SENSOR("  ", "Temperature Count", this->temperature_count_sensor_);
	for (int i = 0; i < 6; i++)
	  LOG_SENSOR("  ", "Temperature X of 6", this->temperature_sensor_[i]);
	LOG_SENSOR("  ", "Current", this->current_sensor_);
	LOG_SENSOR("  ", "Voltage", this->total_voltage_sensor_);
	LOG_SENSOR("  ", "Remaining Capacity", this->remaining_capacity_sensor_);
	LOG_SENSOR("  ", "Full Capacity", this->full_capacity_sensor_);
	LOG_SENSOR("  ", "Design Capacity", this->design_capacity_sensor_);
	LOG_SENSOR("  ", "Cycle Count", this->cycle_count_sensor_);
	LOG_SENSOR("  ", "State of Charge", this->state_of_charge_sensor_);
	LOG_SENSOR("  ", "State of Health", this->state_of_health_sensor_);
	LOG_SENSOR("  ", "Power", this->power_sensor_);
}

void PaceBmsSensor::analog_information_callback(PaceBmsV25::AnalogInformation analog_information) {
	if (this->cell_count_sensor_ != nullptr) {
		this->cell_count_sensor_->publish_state(analog_information.cellCount);
	}
	for (int i = 0; i < 16; i++) {
	  if (this->cell_voltage_sensor_[i] != nullptr) {
	    this->cell_voltage_sensor_[i]->publish_state(analog_information.cellVoltagesMillivolts[i] / 1000.0f);
	  }
	}
	if (this->temperature_count_sensor_ != nullptr) {
		this->temperature_count_sensor_->publish_state(analog_information.temperatureCount);
	}
	for (int i = 0; i < 6; i++) {
	  if (this->temperature_sensor_[i] != nullptr) {
	    this->temperature_sensor_[i]->publish_state(analog_information.temperaturesTenthsCelcius[i] / 10.0f);
	  }
	}
	if (this->current_sensor_ != nullptr) {
	  this->current_sensor_->publish_state(analog_information.currentMilliamps / 1000.0f);
	}
	if (this->total_voltage_sensor_ != nullptr) {
	  this->total_voltage_sensor_->publish_state(analog_information.totalVoltageMillivolts / 1000.0f);
	}
	if (this->remaining_capacity_sensor_ != nullptr) {
	  this->remaining_capacity_sensor_->publish_state(analog_information.remainingCapacityMilliampHours / 1000.0f);
	}
	if (this->full_capacity_sensor_ != nullptr) {
	  this->full_capacity_sensor_->publish_state(analog_information.fullCapacityMilliampHours / 1000.0f);
	}
	if (this->design_capacity_sensor_ != nullptr) {
	  this->design_capacity_sensor_->publish_state(analog_information.designCapacityMilliampHours / 1000.0f);
	}
	if (this->cycle_count_sensor_ != nullptr) {
	  this->cycle_count_sensor_->publish_state(analog_information.cycleCount);
	}
	if (this->state_of_charge_sensor_ != nullptr) {
	  this->state_of_charge_sensor_->publish_state(analog_information.SoC);
	}
	else
	  ESP_LOGE(TAG, "state of charge sensor is nullptr!");
	if (this->state_of_health_sensor_ != nullptr) {
	  this->state_of_health_sensor_->publish_state(analog_information.SoH);
	}
	if (this->power_sensor_ != nullptr) {
	  this->power_sensor_->publish_state(analog_information.powerWatts);
	}
}


}  // namespace pace_bms
}  // namespace esphome
