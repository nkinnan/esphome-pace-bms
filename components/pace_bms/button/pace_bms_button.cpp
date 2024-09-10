#include "pace_bms_button.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.button";

void PaceBmsButton::setup() {
	if (this->parent_->get_protocol_version() == 0x25) {
		if (this->shutdown_button_ != nullptr) {
			this->shutdown_button_->add_on_press_callback([this]() {
				ESP_LOGD(TAG, "Sending shutdown");
				this->parent_->send_shutdown_v25();
			});
		}
	}
	else {
		ESP_LOGE(TAG, "Protocol version not supported: 0x%02X", this->parent_->get_protocol_version());
	}
}

void PaceBmsButton::dump_config() {
	ESP_LOGCONFIG(TAG, "pace_bms_button:");
	LOG_BUTTON("  ", "Shutdown", this->shutdown_button_);
}

}  // namespace pace_bms
}  // namespace esphome
