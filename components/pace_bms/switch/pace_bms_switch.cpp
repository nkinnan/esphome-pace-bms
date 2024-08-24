#include "pace_bms_switch.h"
#include "esphome/core/log.h"

#include <functional>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.sensor";

void PaceBmsSwitch::setup() {
  this->buzzer_switch_->

  if (this->request_status_info_callback_ == true) {
    this->parent_->register_status_information_callback(std::bind(&esphome::pace_bms::PaceBmsSwitch::status_information_callback, this, std::placeholders::_1));
  }
}

float PaceBmsSwitch::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsSwitch::dump_config() { 
	ESP_LOGCONFIG(TAG, "pace_bms_switch:");
	LOG_TEXT_SENSOR("  ", "Warning Status", this->warning_status_sensor_);
}

void PaceBmsSwitch::status_information_callback(PaceBmsV25::StatusInformation& status_information) {
  if (this->buzzer_switch_ != nullptr) {
    this->buzzer_switch_->publish_state((status_information.configuration_value & PaceBmsV25::CF_WarningBuzzerEnabledBit) != 0);
  }
}

void PaceBmsSwitch::child_switch_state_changed(PaceBmsSwitchImplementation* child_switch, bool state) {
}

}  // namespace pace_bms
}  // namespace esphome
