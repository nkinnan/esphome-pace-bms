#include "pace_bms_select.h"
#include "esphome/core/log.h"

#include <functional>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.select";

void PaceBmsSelect::setup() {
  if (this->request_status_information_callback_ == true) {
    this->parent_->register_status_information_callback(std::bind(&esphome::pace_bms::PaceBmsSelect::status_information_callback, this, std::placeholders::_1));
  }
  if (this->request_protocols_callback_ == true) {
	  this->parent_->register_protocols_callback(std::bind(&esphome::pace_bms::PaceBmsSelect::protocols_callback, this, std::placeholders::_1));
  }
  if (this->charge_current_limiter_gear_select_ != nullptr) {
	this->charge_current_limiter_gear_select_->add_on_control_callback([this](std::string value) {
	  if(value == "LowGear")
	    this->parent_->set_charge_current_limiter_gear(PaceBms::CLG_LowGear);
	  else if(value == "HighGear")
	    this->parent_->set_charge_current_limiter_gear(PaceBms::CLG_HighGear);
	  else
		ESP_LOGE(TAG, "Select value not understood: %s", value.c_str());
	});
  }
  if (this->protocol_can_select_ != nullptr) {
	this->protocol_can_select_->add_on_control_callback([this](std::string text, uint8_t value) {
	  ESP_LOGD(TAG, "CAN selected value %s = %02X", text.c_str(), value);
	  this->parent_->set_protocol_can(value);
	});
  }
}

float PaceBmsSelect::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsSelect::dump_config() { 
	ESP_LOGCONFIG(TAG, "pace_bms_select:");
	LOG_SELECT("  ", "Charge Current Limiter Gear", this->charge_current_limiter_gear_select_);
}

void PaceBmsSelect::status_information_callback(PaceBmsV25::StatusInformation& status_information) {
  if (this->charge_current_limiter_gear_select_ != nullptr) {
	std::string state = (status_information.configuration_value & PaceBmsV25::CF_ChargeCurrentLimiterLowGearSetBit ? "LowGear" : "HighGear");
	ESP_LOGV(TAG, "'charge_current_limiter_gear': Publishing state due to update from the hardware: %s", state.c_str());
    this->charge_current_limiter_gear_select_->publish_state(state);
  }
}

}  // namespace pace_bms
}  // namespace esphome
