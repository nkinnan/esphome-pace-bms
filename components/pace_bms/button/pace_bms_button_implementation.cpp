#include "pace_bms_button_implementation.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.button_impl";

void press_action() {
	// nothing to do, not even a publish
}

float PaceBmsButtonImplementation::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace pace_bms
}  // namespace esphome
