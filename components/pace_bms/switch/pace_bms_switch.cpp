#include "pace_bms_switch.h"
#include "esphome/core/log.h"

#include <functional>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.switch";

void PaceBmsSwitch::setup() {
	if (this->parent_->get_protocol_version() == 0x25) {
		if (this->buzzer_alarm_switch_ != nullptr ||
			this->led_alarm_switch_ != nullptr ||
			this->charge_current_limiter_switch_ != nullptr ||
			this->charge_mosfet_switch_ != nullptr ||
			this->discharge_mosfet_switch_ != nullptr) {
			this->parent_->register_status_information_callback_v25([this](PaceBmsProtocolV25::StatusInformation& status_information) {
				if (this->buzzer_alarm_switch_ != nullptr) {
					bool state = (status_information.configuration_value & PaceBmsProtocolV25::CF_BuzzerAlarmEnabledBit);
					ESP_LOGV(TAG, "'buzzer_switch': Publishing state due to update from the hardware: %s", ONOFF(state));
					this->buzzer_alarm_switch_->publish_state(state);
				}
				if (this->led_alarm_switch_ != nullptr) {
					bool state = (status_information.configuration_value & PaceBmsProtocolV25::CF_LedAlarmEnabledBit);
					ESP_LOGV(TAG, "'led_switch': Publishing state due to update from the hardware: %s", ONOFF(state));
					this->led_alarm_switch_->publish_state(state);
				}
				if (this->charge_current_limiter_switch_ != nullptr) {
					bool state = (status_information.configuration_value & PaceBmsProtocolV25::CF_ChargeCurrentLimiterEnabledBit);
					ESP_LOGV(TAG, "'charge_current_limiter_switch': Publishing state due to update from the hardware: %s", ONOFF(state));
					this->charge_current_limiter_switch_->publish_state(state);
				}
				if (this->charge_mosfet_switch_ != nullptr) {
					bool state = (status_information.system_value & PaceBmsProtocolV25::SF_ChargeMosfetOnBit);
					ESP_LOGV(TAG, "'charge_mosfet_switch': Publishing state due to update from the hardware: %s", ONOFF(state));
					this->charge_mosfet_switch_->publish_state(state);
				}
				if (this->discharge_mosfet_switch_ != nullptr) {
					bool state = (status_information.system_value & PaceBmsProtocolV25::SF_DischargeMosfetOnBit);
					ESP_LOGV(TAG, "'discharge_mosfet_switch': Publishing state due to update from the hardware: %s", ONOFF(state));
					this->discharge_mosfet_switch_->publish_state(state);
				}
			});
		}
		if (this->buzzer_alarm_switch_ != nullptr) {
			this->buzzer_alarm_switch_->add_on_write_state_callback([this](bool state) {
				this->parent_->set_switch_state_v25(state ? PaceBmsProtocolV25::SC_EnableBuzzer : PaceBmsProtocolV25::SC_DisableBuzzer);
			});
		}
		if (this->led_alarm_switch_ != nullptr) {
			this->led_alarm_switch_->add_on_write_state_callback([this](bool state) {
				this->parent_->set_switch_state_v25(state ? PaceBmsProtocolV25::SC_EnableLedWarning : PaceBmsProtocolV25::SC_DisableLedWarning);
			});
		}
		if (this->charge_current_limiter_switch_ != nullptr) {
			this->charge_current_limiter_switch_->add_on_write_state_callback([this](bool state) {
				this->parent_->set_switch_state_v25(state ? PaceBmsProtocolV25::SC_EnableChargeCurrentLimiter : PaceBmsProtocolV25::SC_DisableChargeCurrentLimiter);
			});
		}
		if (this->charge_mosfet_switch_ != nullptr) {
			this->charge_mosfet_switch_->add_on_write_state_callback([this](bool state) {
				this->parent_->set_mosfet_state_v25(PaceBmsProtocolV25::MT_Charge, state ? PaceBmsProtocolV25::MS_Close : PaceBmsProtocolV25::MS_Open);
			});
		}
		if (this->discharge_mosfet_switch_ != nullptr) {
			this->discharge_mosfet_switch_->add_on_write_state_callback([this](bool state) {
				this->parent_->set_mosfet_state_v25(PaceBmsProtocolV25::MT_Discharge, state ? PaceBmsProtocolV25::MS_Close : PaceBmsProtocolV25::MS_Open);
			});
		}
	}
	else {
		ESP_LOGE(TAG, "Protocol version not supported: 0x%02X", this->parent_->get_protocol_version());
	}
}

void PaceBmsSwitch::dump_config() {
	ESP_LOGCONFIG(TAG, "pace_bms_switch:");
	LOG_SWITCH("  ", "Buzzer Alarm", this->buzzer_alarm_switch_);
	LOG_SWITCH("  ", "LED Alarm", this->led_alarm_switch_);
	LOG_SWITCH("  ", "Charge Current Limiter", this->charge_current_limiter_switch_);
	LOG_SWITCH("  ", "Charge MOSFET", this->charge_mosfet_switch_);
	LOG_SWITCH("  ", "Discharge MOSFET", this->discharge_mosfet_switch_);
}

}  // namespace pace_bms
}  // namespace esphome
