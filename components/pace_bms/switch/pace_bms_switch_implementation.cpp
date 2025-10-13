#include "esphome/core/log.h"

#include "pace_bms_switch_implementation.h"

namespace esphome {
namespace pace_bms_base {

static const char* const TAG = "pace_bms_base.switch_impl";

void PaceBmsSwitchImplementation::write_state(bool state) {
	if(readonly_ == true)
		return;

	this->write_state_callback_.call(state);

	// required for the UX not to get out of sync
	this->publish_state(state);
}

}  // namespace pace_bms_base
}  // namespace esphome
