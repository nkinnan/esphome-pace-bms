#include "pace_bms_sensor.h"
#include "esphome/core/log.h"

#include <functional>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.sensor";

void PaceBmsSensor::setup() {
  if (request_analog_info_callback_ == true) {
    this->parent_->register_analog_information_callback(std::bind(&esphome::pace_bms::PaceBmsSensor::analog_information_callback, this));
  }
}

float PaceBmsSensor::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsSensor::dump_config() { 
	//LOG_SENSOR("", "Sun Sensor", this); 
	ESP_LOGCONFIG(TAG, "pace_bms_sensor:");
	LOG_SENSOR("  ", "Voltage", this->voltage_sensor_);
}

void PaceBmsSensor::analog_information_callback(PaceBmsV25::AnalogInformation analog_information) {
  if(this->voltage_sensor_ != nullptr) {
	this->voltage_sensor_.publish(analog_information.totalVoltageMillivolts / 1000.0f);
  }
}


}  // namespace pace_bms
}  // namespace esphome
