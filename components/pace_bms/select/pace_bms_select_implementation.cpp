#include "pace_bms_select_implementation.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.select_impl";

void PaceBmsSelectImplementation::add_on_control_callback(std::function<void(const std::string&)>&& callback) {
	this->control_callback_.add(std::move(callback));
}

void PaceBmsSelectImplementation::control(const std::string& value) {
	ESP_LOGE(TAG, "control method override: %s", value.c_str());

	std::string copy = value;

	this->control_callback_.call(copy);

	if(value == "HighGear")
	  this->control_callback_.call(std::string("HighGear"));
	if(value == "LowGear")
	  this->control_callback_.call(std::string("LowGear"));

	this->publish_state(value);
}

float PaceBmsSelectImplementation::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace pace_bms
}  // namespace esphome
