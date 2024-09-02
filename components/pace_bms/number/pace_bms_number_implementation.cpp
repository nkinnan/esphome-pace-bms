#include "pace_bms_number_implementation.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.switch_impl";

void PaceBmsNumberImplementation::add_on_control_callback(std::function<void(bool)>&& callback) {
	this->control_callbacks_.add(std::move(callback));
}

void PaceBmsNumberImplementation::control(float number) {
	ESP_LOGE(TAG, "control: new value %f", number);

	// callbacks
	this->control_callbacks_.call(number);

	// required for the UX not to get out of sync
	this->publish_state(number);
}

float PaceBmsNumberImplementation::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace pace_bms
}  // namespace esphome
