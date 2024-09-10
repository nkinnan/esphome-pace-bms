#include "pace_bms_select.h"
#include "esphome/core/log.h"

#include <functional>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.select";

void PaceBmsSelect::setup() {
	if (this->parent_->get_protocol_version() == 0x25) {
		if (this->charge_current_limiter_gear_select_ != nullptr) {
			this->parent_->register_status_information_callback_v25([this](PaceBmsV25::StatusInformation& status_information) {
				if (this->charge_current_limiter_gear_select_ != nullptr) {
					std::string state = this->charge_current_limiter_gear_select_->option_from_value(
						// values stored in init.py but we get them back from the hardware as a status flag rather than a value so have to map here unfortunately
						(status_information.configuration_value & PaceBmsV25::CF_ChargeCurrentLimiterLowGearSetBit ?
							PaceBmsV25::SC_SetChargeCurrentLimiterCurrentLimitHighGear :
							PaceBmsV25::SC_SetChargeCurrentLimiterCurrentLimitLowGear));
					ESP_LOGV(TAG, "'charge_current_limiter_gear': Publishing state due to update from the hardware: %s", state.c_str());
					this->charge_current_limiter_gear_select_->publish_state(state);
				}
			});
		}
		if (this->charge_current_limiter_gear_select_ != nullptr) {
			this->charge_current_limiter_gear_select_->add_on_control_callback([this](std::string text, uint8_t value) {
				ESP_LOGD(TAG, "Setting Charge Current Limiter Gear user selected value %s = %02X", text.c_str(), value);
				this->parent_->set_switch_state_v25((PaceBmsV25::SwitchCommand)value);
			});
		}

		if (this->protocol_can_select_ != nullptr ||
			this->protocol_rs485_select_ != nullptr ||
			this->protocol_type_select_ != nullptr) {
			this->parent_->register_protocols_callback_v25([this](PaceBmsV25::Protocols& protocols) {
				this->protocols_ = protocols;
				this->protocols_seen_ = true;

				if (this->protocol_can_select_ != nullptr) {
					std::string state = this->protocol_can_select_->option_from_value(protocols.CAN);
					ESP_LOGV(TAG, "'protocol_can': Publishing state due to update from the hardware: %s", state.c_str());
					this->protocol_can_select_->publish_state(state);
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
			});
		}
		if (this->protocol_can_select_ != nullptr) {
			this->protocol_can_select_->add_on_control_callback([this](std::string text, uint8_t value) {
				if (!protocols_seen_) {
					ESP_LOGE(TAG, "Protocol CAN cannot be set because the BMS hasn't responded to a get protocols request");
					return;
				}
				ESP_LOGD(TAG, "Setting protocol CAN user selected value '%s' = %02X", text.c_str(), value);
				protocols_.CAN = (PaceBmsV25::ProtocolList_CAN)value;
				this->parent_->set_protocols_v25(protocols_);
			});
		}
		if (this->protocol_rs485_select_ != nullptr) {
			this->protocol_rs485_select_->add_on_control_callback([this](std::string text, uint8_t value) {
				if (!protocols_seen_) {
					ESP_LOGE(TAG, "Protocol RS485 cannot be set because the BMS hasn't responded to a get protocols request");
					return;
				}
				ESP_LOGD(TAG, "Setting protocol RS485 user selected value '%s' = %02X", text.c_str(), value);
				protocols_.RS485 = (PaceBmsV25::ProtocolList_RS485)value;
				this->parent_->set_protocols_v25(protocols_);
			});
		}
		if (this->protocol_type_select_ != nullptr) {
			this->protocol_type_select_->add_on_control_callback([this](std::string text, uint8_t value) {
				if (!protocols_seen_) {
					ESP_LOGE(TAG, "Protocol Type cannot be set because the BMS hasn't responded to a get protocols request");
					return;
				}
				ESP_LOGD(TAG, "Setting protocol Type user selected value '%s' = %02X", text.c_str(), value);
				protocols_.Type = (PaceBmsV25::ProtocolList_Type)value;
				this->parent_->set_protocols_v25(protocols_);
			});
		}
		else {
			ESP_LOGE(TAG, "Protocol version not supported: 0x%02X", this->parent_->get_protocol_version());
		}
	}
}
void PaceBmsSelect::dump_config() {
	ESP_LOGCONFIG(TAG, "pace_bms_select:");
	LOG_SELECT("  ", "Charge Current Limiter Gear", this->charge_current_limiter_gear_select_);
	LOG_SELECT("  ", "Protocol CAN", this->protocol_can_select_);
	LOG_SELECT("  ", "Protocol RS485", this->protocol_rs485_select_);
	LOG_SELECT("  ", "Protocol Type", this->protocol_type_select_);
}

}  // namespace pace_bms
}  // namespace esphome
