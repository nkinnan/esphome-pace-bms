#include "pace_bms_switch_implementation.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.switch_impl";

void PaceBmsSwitchImplementation::add_on_write_state_callback(std::function<void(bool)>&& callback) {
	this->write_state_callback_.add(std::move(callback));
}

void PaceBmsSwitchImplementation::write_state(bool state) {
	this->write_state_callback_.call(state);
}

void PaceBmsSwitchImplementation::record_state_without_publish(bool state) {
	if (!this->publish_dedup_.next(state))
		return;
	this->state = state != this->inverted_;

	//if (restore_mode & RESTORE_MODE_PERSISTENT_MASK)
	//	this->rtc_.save(&this->state);

	//ESP_LOGD(TAG, "'%s': Sending state %s", this->name_.c_str(), ONOFF(this->state));
	//this->state_callback_.call(this->state);
}

float PaceBmsSwitchImplementation::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace pace_bms
}  // namespace esphome
