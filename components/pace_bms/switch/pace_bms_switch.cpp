#include "pace_bms_switch.h"
#include "esphome/core/log.h"

#include <functional>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.sensor";

void PaceBmsSwitch::setup() {
  if (this->request_status_info_callback_ == true) {
    this->parent_->register_status_information_callback(std::bind(&esphome::pace_bms::PaceBmsSwitch::status_information_callback, this, std::placeholders::_1));
  }
}

void PaceBmsSwitch::buzzer_switch_request_state_change(bool state) {
	ESP_LOGV(TAG, "Buzzer switch requests state change to %s", state ? "true" : "false");
}


float PaceBmsSwitch::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsSwitch::dump_config() { 
	ESP_LOGCONFIG(TAG, "pace_bms_switch:");
	LOG_SWITCH("  ", "Buzzer", this->buzzer_switch_);
}

void PaceBmsSwitch::status_information_callback(PaceBmsV25::StatusInformation& status_information) {
  if (this->buzzer_switch_ != nullptr) {
	  ESP_LOGV(TAG, "Setting buzzer switch state %s", (status_information.configuration_value & PaceBmsV25::CF_WarningBuzzerEnabledBit) != 0 ? "true" : "false");
    this->buzzer_switch_->publish_state((status_information.configuration_value & PaceBmsV25::CF_WarningBuzzerEnabledBit) != 0);
  }
}

void PaceBmsSwitch::child_switch_state_changed(PaceBmsSwitchImplementation* child_switch, bool state) {
}

}  // namespace pace_bms
}  // namespace esphome
