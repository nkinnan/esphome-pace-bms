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
	for (int i = 1; i <= 16; i++) {
	  char name[16];
	  sprintf(name, "Cell Voltage %02i", i);
	  LOG_SENSOR("  ", name, this->cell_voltage_sensor_[i]);
	}
	LOG_SENSOR("  ", "Voltage", this->total_voltage_sensor_);
}

void PaceBmsSensor::analog_information_callback(PaceBmsV25::AnalogInformation analog_information) {
	if (this->cell_count_sensor_ != nullptr) {
		this->cell_count_sensor_->publish_state(analog_information.cellCount);
	}



	if (this->total_voltage_sensor_ != nullptr) {
		this->total_voltage_sensor_->publish_state(analog_information.totalVoltageMillivolts / 1000.0f);
	}
}


}  // namespace pace_bms
}  // namespace esphome
