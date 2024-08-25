#include "pace_bms_switch_implementation.h"

namespace esphome {
namespace pace_bms {

// the only purpose of this class is to fill in the pure virtual and call the parent container component on state change
void PaceBmsSwitchImplementation::write_state(bool state) {
	//this->parent_->child_switch_state_changed(this, state);
}

void PaceBmsSensor::setup() {
}

float PaceBmsSensor::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsSensor::dump_config() {
}

}  // namespace pace_bms
}  // namespace esphome
