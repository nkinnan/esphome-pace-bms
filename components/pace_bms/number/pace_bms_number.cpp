#include "pace_bms_number.h"
#include "esphome/core/log.h"

#include <functional>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.number";

void PaceBmsNumber::setup() {
	if (this->request_test_callback_ == true) {
    //this->parent_->register_test_callback(std::bind(&esphome::pace_bms::PaceBmsNumber::test_callback, this, std::placeholders::_1));
  }
}

float PaceBmsNumber::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsNumber::dump_config() {
	ESP_LOGCONFIG(TAG, "pace_bms_number:");
	LOG_NUMBER("  ", "test", this->test_number_);
}

void PaceBmsNumber::test_callback(PaceBmsV25::StatusInformation& test) {
  if (this->test_number_ != nullptr) {
	int state = test.system_value;
	ESP_LOGV(TAG, "'test': Publishing state due to update from the hardware: %s", state);
    this->test_number_->publish_state(state);
  }
}

}  // namespace pace_bms
}  // namespace esphome
