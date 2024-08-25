#include "pace_bms_switch_implementation.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.sensor_implementation";

// the only purpose of this class is to fill in the pure virtual and call the parent container component on state change
void PaceBmsSwitchImplementation::write_state(bool state) {
	//this->parent_->child_switch_state_changed(this, state);
	ESP_LOGV(TAG, "New state requested by user: %b", state);
}

void PaceBmsSwitchImplementation::setup() {
}

float PaceBmsSwitchImplementation::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsSwitchImplementation::dump_config() {
	ESP_LOGCONFIG(TAG, "pace_bms_switch_implementation:");
}

}  // namespace pace_bms
}  // namespace esphome
