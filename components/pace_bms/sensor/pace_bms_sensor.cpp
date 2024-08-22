#include "pace_bms_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.sensor";

void setup() {
}

void update() {
    //double val;
    //switch (this->type_) {
    //  case SUN_SENSOR_ELEVATION:
    //    val = this->parent_->elevation();
    //    break;
    //  case SUN_SENSOR_AZIMUTH:
    //    val = this->parent_->azimuth();
    //    break;
    //  default:
    //    return;
    //}
    //this->publish_state(val);
}

float PaceBmsSensor::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsSensor::dump_config() { 
	//LOG_SENSOR("", "Sun Sensor", this); 
	ESP_LOGCONFIG(TAG, "pace_bms_sensor:");
	LOG_SENSOR("  ", "Voltage", this->voltage_sensor_);
}

}  // namespace pace_bms
}  // namespace esphome
