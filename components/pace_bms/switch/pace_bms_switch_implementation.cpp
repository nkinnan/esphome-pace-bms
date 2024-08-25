#include "pace_bms_switch_implementation.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.switch_impl";

// the only purpose of this class is to fill in the pure virtual and call the parent container component on state change via publish_state executing callbacks
void PaceBmsSwitchImplementation::write_state(bool state) {
	this->write_state_callback_.call(this->state);
}

void PaceBmsSwitchImplementation::add_on_write_state_callback(std::function<void(bool)>&& callback) {
	this->write_state_callback_.add(std::move(callback));
}

float PaceBmsSwitchImplementation::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace pace_bms
}  // namespace esphome
