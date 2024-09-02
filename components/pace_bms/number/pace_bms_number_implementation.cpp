#include "pace_bms_switch_implementation.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.switch_impl";

void PaceBmsNumberImplementation::control(float number) {
	// required for the UX not to get out of sync
	this->publish_state(state);
}

float PaceBmsNumberImplementation::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace pace_bms
}  // namespace esphome
