#include "esphome/core/log.h"

#include "pace_bms_button_implementation.h"

namespace esphome {
namespace pace_bms_base {

static const char* const TAG = "pace_bms_base.button_impl";

void PaceBmsButtonImplementation::press_action() {
	// nothing to do, not even a publish
}

}  // namespace pace_bms_base
}  // namespace esphome
