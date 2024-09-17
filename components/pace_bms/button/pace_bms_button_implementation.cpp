#include "esphome/core/log.h"

#include "pace_bms_button_implementation.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.button_impl";

void PaceBmsButtonImplementation::press_action() {
	// nothing to do, not even a publish
}

}  // namespace pace_bms
}  // namespace esphome
