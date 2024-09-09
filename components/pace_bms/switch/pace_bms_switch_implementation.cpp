#include "pace_bms_switch_implementation.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.switch_impl";

void PaceBmsSwitchImplementation::write_state(bool state) {
	this->write_state_callback_.call(state);

	// required for the UX not to get out of sync
	this->publish_state(state);
}

}  // namespace pace_bms
}  // namespace esphome
