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
  if (this->charge_current_limiter_gear_select_ != nullptr) {
	this->charge_current_limiter_gear_select_->add_on_control_callback([this](std::string text, uint8_t value) {
	  ESP_LOGD(TAG, "Setting Charge Current Limiter Gear user selected value %s = %02X", text.c_str(), value);
	  this->parent_->set_switch_state((PaceBmsV25::SwitchCommand)value);
	});
  }

  if (this->request_protocols_callback_ == true) {
	  this->parent_->register_protocols_callback(std::bind(&esphome::pace_bms::PaceBmsSelect::protocols_callback, this, std::placeholders::_1));
  }
  if (this->protocol_can_select_ != nullptr) {
	this->protocol_can_select_->add_on_control_callback([this](std::string text, uint8_t value) {
	  if(!protocols_seen) {
	    ESP_LOGE(TAG, "Protocol CAN cannot be set because the BMS hasn't responded to a get protocols request");
		return;
	  }
	  ESP_LOGD(TAG, "Setting protocol CAN user selected value '%s' = %02X", text.c_str(), value);
	  PaceBmsV25::Protocols new_protocols = last_seen_protocols;
	  new_protocols.CAN = (PaceBmsV25::NewStyleProtocolList_CAN)value;
	  this->parent_->set_protocols(new_protocols);
	});
  }
  if (this->protocol_rs485_select_ != nullptr) {
	this->protocol_rs485_select_->add_on_control_callback([this](std::string text, uint8_t value) {
	  if(!protocols_seen) {
	    ESP_LOGE(TAG, "Protocol RS485 cannot be set because the BMS hasn't responded to a get protocols request");
		return;
	  }
	  ESP_LOGD(TAG, "Setting protocol RS485 user selected value '%s' = %02X", text.c_str(), value);
	  PaceBmsV25::Protocols new_protocols = last_seen_protocols;
	  new_protocols.RS485 = (PaceBmsV25::NewStyleProtocolList_RS485)value;
	  this->parent_->set_protocols(new_protocols);
	});
  }
  if (this->protocol_type_select_ != nullptr) {
	this->protocol_type_select_->add_on_control_callback([this](std::string text, uint8_t value) {
	  if(!protocols_seen) {
	    ESP_LOGE(TAG, "Protocol Type cannot be set because the BMS hasn't responded to a get protocols request");
		return;
	  }
	  ESP_LOGD(TAG, "Setting protocol Type user selected value '%s' = %02X", text.c_str(), value);
	  PaceBmsV25::Protocols new_protocols = last_seen_protocols;
	  new_protocols.Type = (PaceBmsV25::NewStyleProtocolList_Type)value;
	  this->parent_->set_protocols(new_protocols);
	});
  }
}

float PaceBmsSelect::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsSelect::dump_config() { 
	ESP_LOGCONFIG(TAG, "pace_bms_select:");
	LOG_SELECT("  ", "Charge Current Limiter Gear", this->charge_current_limiter_gear_select_);
	LOG_SELECT("  ", "Protocol CAN", this->protocol_can_select_);
	LOG_SELECT("  ", "Protocol RS485", this->protocol_rs485_select_);
	LOG_SELECT("  ", "Protocol Type", this->protocol_type_select_);
}

void PaceBmsSelect::status_information_callback(PaceBmsV25::StatusInformation& status_information) {
  if (this->charge_current_limiter_gear_select_ != nullptr) {
	std::string state = (status_information.configuration_value & PaceBmsV25::CF_ChargeCurrentLimiterLowGearSetBit ? "LowGear" : "HighGear");
	ESP_LOGV(TAG, "'charge_current_limiter_gear': Publishing state due to update from the hardware: %s", state.c_str());
    this->charge_current_limiter_gear_select_->publish_state(state);
  }
}

void PaceBmsSelect::protocols_callback(PaceBmsV25::Protocols& protocols) {
  this->last_seen_protocols = protocols;
  this->protocols_seen = true;

  if (this->protocol_can_select_ != nullptr) {
	std::string state = this->protocol_can_select_->option_from_value(protocols.CAN);
	ESP_LOGV(TAG, "'protocol_can': Publishing state due to update from the hardware: %s", state.c_str());
    this->protocol_can_select_->publish_state(state);
	ESP_LOGV(TAG, "published");
  }
  if (this->protocol_rs485_select_ != nullptr) {
	std::string state = this->protocol_rs485_select_->option_from_value(protocols.RS485);
	ESP_LOGV(TAG, "'protocol_rs485': Publishing state due to update from the hardware: %s", state.c_str());
    this->protocol_rs485_select_->publish_state(state);
  }
  if (this->protocol_type_select_ != nullptr) {
	std::string state = this->protocol_type_select_->option_from_value(protocols.Type);
	ESP_LOGV(TAG, "'protocol_type': Publishing state due to update from the hardware: %s", state.c_str());
    this->protocol_type_select_->publish_state(state);
  }
}

}  // namespace pace_bms
}  // namespace esphome
