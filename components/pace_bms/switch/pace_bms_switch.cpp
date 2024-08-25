#include "pace_bms_switch.h"
#include "esphome/core/log.h"

#include <functional>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.sensor";

void PaceBmsSwitch::setup() {
  if (this->request_status_information_callback_ == true) {
    this->parent_->register_status_information_callback(std::bind(&esphome::pace_bms::PaceBmsSwitch::status_information_callback, this, std::placeholders::_1));
  }
  if (this->buzzer_switch_ != nullptr) {
	this->buzzer_switch_->add_on_write_state_callback([this](bool state) {
	  this->parent_->set_switch_state(PaceBms::ST_BuzzerAlarm, state);
	  // set internal (requested) state but do not save/publish yet until the device confirms
	  this->buzzer_switch_->publish_state(state);
	});
  }
}

float PaceBmsSwitch::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsSwitch::dump_config() { 
	ESP_LOGCONFIG(TAG, "pace_bms_switch:");
	LOG_SWITCH("  ", "Buzzer", this->buzzer_switch_);
}

void PaceBmsSwitch::status_information_callback(PaceBmsV25::StatusInformation& status_information) {
  if (this->buzzer_switch_ != nullptr) {
	// unlike record_state_without_publish, upon confirmation from the device that the state was actually set, do a full save/publish
    this->buzzer_switch_->publish_state((status_information.configuration_value & PaceBmsV25::CF_WarningBuzzerEnabledBit) != 0);
  }
}

}  // namespace pace_bms
}  // namespace esphome
