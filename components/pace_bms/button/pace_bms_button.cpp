#include "pace_bms_button.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.button";

void PaceBmsButton::setup() {
  if (this->shutdown_button_ != nullptr) {
	this->shutdown_button_->add_on_press_callback([this]() {
	  ESP_LOGD(TAG, "Sending shutdown");
	  this->parent_->send_shutdown_v25();
	});
  }
}

void PaceBmsButton::dump_config() {
	ESP_LOGCONFIG(TAG, "pace_bms_button:");
	LOG_BUTTON("  ", "Shutdown", this->shutdown_button_);
}

}  // namespace pace_bms
}  // namespace esphome
