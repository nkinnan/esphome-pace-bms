#include "pace_bms_select_implementation.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.select_impl";

void PaceBmsSelectImplementation::add_on_control_callback(std::function<void(const std::string&, uint8_t value)>&& callback) {
	this->control_callback_.add(std::move(callback));
}

void PaceBmsSelectImplementation::control(const std::string& text) {
	auto options = this->traits.get_options();
	auto opt_it = std::find(options.cbegin(), options.cend(), text);
	size_t idx = std::distance(options.cbegin(), opt_it);
	uint8_t value = this->values_[idx];
	ESP_LOGD(TAG, "%s found value %02X for option '%s'", this->name_, value, text.c_str());

	this->control_callback_.call(text, value);

	// required for the UX not to get out of sync
	this->publish_state(text);
}

float PaceBmsSelectImplementation::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace pace_bms
}  // namespace esphome
