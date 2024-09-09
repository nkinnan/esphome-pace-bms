#include "pace_bms_number_implementation.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.switch_impl";

void PaceBmsNumberImplementation::control(float number) {
	ESP_LOGD(TAG, "control: new value %f", number);

	// callbacks
	this->control_callbacks_.call(number);

	// required for the UX not to get out of sync
	this->publish_state(number);
}

}  // namespace pace_bms
}  // namespace esphome
