#include "pace_bms_sensor.h"
#include "esphome/core/log.h"

#include <functional>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.sensor";

void PaceBmsButton::setup() {
  if (this->shutdown_button_ != nullptr) {
	this->shutdown_button_->add_on_press_callback([this]() {
	  ESP_LOGD(TAG, "Sending shutdown");
	  this->parent_->send_shutdown();
	});
  }
}

float PaceBmsButton::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsButton::dump_config() {
	ESP_LOGCONFIG(TAG, "pace_bms_sensor:");
	LOG_BUTTON("  ", "Shutdown", this->shutdown_);
}

}  // namespace pace_bms
}  // namespace esphome
