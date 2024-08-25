#include "pace_bms_switch_implementation.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.switch_impl";

void PaceBmsSwitchImplementation::add_on_write_state_callback(std::function<void(bool)>&& callback) {
	this->write_state_callback_.add(std::move(callback));

	// it's required to call publish_state per code documentation, and in testing, yes, some strange stuff is happening presumably due to (I assume) other 
	// listeners of state_callback which is called by publish_state -- even if I set the internal this->state properly, the UI doesn't work unless I do 
	// a full publish_state which calls (I assume) other state_callback listeners -- so even though I'd prefer to wait for the hardware to inform of the
	// new state being successfully set and call publish_state only then, I have to do so now even as the hardware request to set it is pending
	ESP_LOGV(TAG, "'%s': Publishing state in advance after sending request to the hardware: %s", this->name_.c_str(), ONOFF(state));
	this->publish_state(state);
}

void PaceBmsSwitchImplementation::write_state(bool state) {
	this->write_state_callback_.call(state);
}

float PaceBmsSwitchImplementation::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace pace_bms
}  // namespace esphome
