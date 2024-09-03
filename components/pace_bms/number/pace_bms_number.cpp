#include "pace_bms_number.h"
#include "esphome/core/log.h"

#include <functional>
#include <cmath>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.number";




void PaceBmsNumber::setup() {

	if (this->request_cell_over_voltage_configuration_callback_ == true) {
		this->parent_->register_cell_over_voltage_configuration_callback(std::bind(&esphome::pace_bms::PaceBmsNumber::cell_over_voltage_configuration_callback, this, std::placeholders::_1));
	}
	if (this->cell_over_voltage_alarm_number_ != nullptr) {
		this->cell_over_voltage_alarm_number_->add_on_control_callback([this](float value) {
			if (!cell_over_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "cell_over_voltage_alarm cannot be set because the BMS hasn't responded to a get cell over voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_over_voltage_alarm user selected value %f", value);
			this->cell_over_voltage_configuration_.AlarmMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_cell_over_voltage_configuration(this->cell_over_voltage_configuration_);
			});
	}
	if (this->cell_over_voltage_protection_number_ != nullptr) {
		this->cell_over_voltage_protection_number_->add_on_control_callback([this](float value) {
			if (!cell_over_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "cell_over_voltage_protection cannot be set because the BMS hasn't responded to a get cell over voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_over_voltage_protection user selected value %f", value);
			this->cell_over_voltage_configuration_.ProtectionMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_cell_over_voltage_configuration(this->cell_over_voltage_configuration_);
			});
	}
	if (this->cell_over_voltage_protection_release_number_ != nullptr) {
		this->cell_over_voltage_protection_release_number_->add_on_control_callback([this](float value) {
			if (!cell_over_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "cell_over_voltage_protection_release cannot be set because the BMS hasn't responded to a get cell over voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_over_voltage_protection_release user selected value %f", value);
			this->cell_over_voltage_configuration_.ProtectionReleaseMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_cell_over_voltage_configuration(this->cell_over_voltage_configuration_);
			});
	}
	if (this->cell_over_voltage_protection_delay_number_ != nullptr) {
		this->cell_over_voltage_protection_delay_number_->add_on_control_callback([this](float value) {
			if (!cell_over_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "cell_over_voltage_protection_delay cannot be set because the BMS hasn't responded to a get cell over voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_over_voltage_protection_delay user selected value %f", value);
			this->cell_over_voltage_configuration_.ProtectionDelayMilliseconds = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_cell_over_voltage_configuration(this->cell_over_voltage_configuration_);
			});
	}

	if (this->request_pack_over_voltage_configuration_callback_ == true) {
		this->parent_->register_pack_over_voltage_configuration_callback(std::bind(&esphome::pace_bms::PaceBmsNumber::pack_over_voltage_configuration_callback, this, std::placeholders::_1));
	}
	if (this->pack_over_voltage_alarm_number_ != nullptr) {
		this->pack_over_voltage_alarm_number_->add_on_control_callback([this](float value) {
			if (!pack_over_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "pack_over_voltage_alarm cannot be set because the BMS hasn't responded to a get pack over voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting pack_over_voltage_alarm user selected value %f", value);
			this->pack_over_voltage_configuration_.AlarmMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_pack_over_voltage_configuration(this->pack_over_voltage_configuration_);
			});
	}
	if (this->pack_over_voltage_protection_number_ != nullptr) {
		this->pack_over_voltage_protection_number_->add_on_control_callback([this](float value) {
			if (!pack_over_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "pack_over_voltage_protection cannot be set because the BMS hasn't responded to a get pack over voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting pack_over_voltage_protection user selected value %f", value);
			this->pack_over_voltage_configuration_.ProtectionMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_pack_over_voltage_configuration(this->pack_over_voltage_configuration_);
			});
	}
	if (this->pack_over_voltage_protection_release_number_ != nullptr) {
		this->pack_over_voltage_protection_release_number_->add_on_control_callback([this](float value) {
			if (!pack_over_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "pack_over_voltage_protection_release cannot be set because the BMS hasn't responded to a get pack over voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting pack_over_voltage_protection_release user selected value %f", value);
			this->pack_over_voltage_configuration_.ProtectionReleaseMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_pack_over_voltage_configuration(this->pack_over_voltage_configuration_);
			});
	}
	if (this->pack_over_voltage_protection_delay_number_ != nullptr) {
		this->pack_over_voltage_protection_delay_number_->add_on_control_callback([this](float value) {
			if (!pack_over_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "pack_over_voltage_protection_delay cannot be set because the BMS hasn't responded to a get pack over voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting pack_over_voltage_protection_delay user selected value %f", value);
			this->pack_over_voltage_configuration_.ProtectionDelayMilliseconds = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_pack_over_voltage_configuration(this->pack_over_voltage_configuration_);
			});
	}

	if (this->request_cell_under_voltage_configuration_callback_ == true) {
		this->parent_->register_cell_under_voltage_configuration_callback(std::bind(&esphome::pace_bms::PaceBmsNumber::cell_under_voltage_configuration_callback, this, std::placeholders::_1));
	}
	if (this->cell_under_voltage_alarm_number_ != nullptr) {
		this->cell_under_voltage_alarm_number_->add_on_control_callback([this](float value) {
			if (!cell_under_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "cell_under_voltage_alarm cannot be set because the BMS hasn't responded to a get cell under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_under_voltage_alarm user selected value %f", value);
			this->cell_under_voltage_configuration_.AlarmMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_cell_under_voltage_configuration(this->cell_under_voltage_configuration_);
			});
	}
	if (this->cell_under_voltage_protection_number_ != nullptr) {
		this->cell_under_voltage_protection_number_->add_on_control_callback([this](float value) {
			if (!cell_under_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "cell_under_voltage_protection cannot be set because the BMS hasn't responded to a get cell under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_under_voltage_protection user selected value %f", value);
			this->cell_under_voltage_configuration_.ProtectionMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_cell_under_voltage_configuration(this->cell_under_voltage_configuration_);
			});
	}
	if (this->cell_under_voltage_protection_release_number_ != nullptr) {
		this->cell_under_voltage_protection_release_number_->add_on_control_callback([this](float value) {
			if (!cell_under_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "cell_under_voltage_protection_release cannot be set because the BMS hasn't responded to a get cell under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_under_voltage_protection_release user selected value %f", value);
			this->cell_under_voltage_configuration_.ProtectionReleaseMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_cell_under_voltage_configuration(this->cell_under_voltage_configuration_);
			});
	}
	if (this->cell_under_voltage_protection_delay_number_ != nullptr) {
		this->cell_under_voltage_protection_delay_number_->add_on_control_callback([this](float value) {
			if (!cell_under_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "cell_under_voltage_protection_delay cannot be set because the BMS hasn't responded to a get cell under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_under_voltage_protection_delay user selected value %f", value);
			this->cell_under_voltage_configuration_.ProtectionDelayMilliseconds = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_cell_under_voltage_configuration(this->cell_under_voltage_configuration_);
			});
	}

	if (this->request_pack_under_voltage_configuration_callback_ == true) {
		this->parent_->register_pack_under_voltage_configuration_callback(std::bind(&esphome::pace_bms::PaceBmsNumber::pack_under_voltage_configuration_callback, this, std::placeholders::_1));
	}
	if (this->pack_under_voltage_alarm_number_ != nullptr) {
		this->pack_under_voltage_alarm_number_->add_on_control_callback([this](float value) {
			if (!pack_under_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "pack_under_voltage_alarm cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting pack_under_voltage_alarm user selected value %f", value);
			this->pack_under_voltage_configuration_.AlarmMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_pack_under_voltage_configuration(this->pack_under_voltage_configuration_);
			});
	}
	if (this->pack_under_voltage_protection_number_ != nullptr) {
		this->pack_under_voltage_protection_number_->add_on_control_callback([this](float value) {
			if (!pack_under_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "pack_under_voltage_protection cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting pack_under_voltage_protection user selected value %f", value);
			this->pack_under_voltage_configuration_.ProtectionMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_pack_under_voltage_configuration(this->pack_under_voltage_configuration_);
			});
	}
	if (this->pack_under_voltage_protection_release_number_ != nullptr) {
		this->pack_under_voltage_protection_release_number_->add_on_control_callback([this](float value) {
			if (!pack_under_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "pack_under_voltage_protection_release cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting pack_under_voltage_protection_release user selected value %f", value);
			this->pack_under_voltage_configuration_.ProtectionReleaseMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_pack_under_voltage_configuration(this->pack_under_voltage_configuration_);
			});
	}
	if (this->pack_under_voltage_protection_delay_number_ != nullptr) {
		this->pack_under_voltage_protection_delay_number_->add_on_control_callback([this](float value) {
			if (!pack_under_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "pack_under_voltage_protection_delay cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting pack_under_voltage_protection_delay user selected value %f", value);
			this->pack_under_voltage_configuration_.ProtectionDelayMilliseconds = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_pack_under_voltage_configuration(this->pack_under_voltage_configuration_);
			});
	}

	if (this->request_charge_over_current_configuration_callback_ == true) {
		this->parent_->register_charge_over_current_configuration_callback(std::bind(&esphome::pace_bms::PaceBmsNumber::charge_over_current_configuration_callback, this, std::placeholders::_1));
	}
	if (this->charge_over_current_alarm_number_ != nullptr) {
		this->charge_over_current_alarm_number_->add_on_control_callback([this](float value) {
			if (!charge_over_current_configuration_seen_) {
				ESP_LOGE(TAG, "charge_over_current_alarm cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting charge_over_current_alarm user selected value %f", value);
			this->charge_over_current_configuration_.AlarmAmperage = (uint16_t)std::roundl(value);
			this->parent_->set_charge_over_current_configuration(this->charge_over_current_configuration_);
			});
	}
	if (this->charge_over_current_protection_number_ != nullptr) {
		this->charge_over_current_protection_number_->add_on_control_callback([this](float value) {
			if (!charge_over_current_configuration_seen_) {
				ESP_LOGE(TAG, "charge_over_current_protection cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting charge_over_current_protection user selected value %f", value);
			this->charge_over_current_configuration_.ProtectionAmperage = (uint16_t)std::roundl(value);
			this->parent_->set_charge_over_current_configuration(this->charge_over_current_configuration_);
			});
	}
	if (this->charge_over_current_protection_delay_number_ != nullptr) {
		this->charge_over_current_protection_delay_number_->add_on_control_callback([this](float value) {
			if (!charge_over_current_configuration_seen_) {
				ESP_LOGE(TAG, "charge_over_current_protection_delay cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting charge_over_current_protection_delay user selected value %f", value);
			this->charge_over_current_configuration_.ProtectionDelayMilliseconds = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_charge_over_current_configuration(this->charge_over_current_configuration_);
			});
	}

	if (this->request_discharge_over_current1_configuration_callback_ == true) {
		this->parent_->register_discharge_over_current1_configuration_callback(std::bind(&esphome::pace_bms::PaceBmsNumber::discharge_over_current1_configuration_callback, this, std::placeholders::_1));
	}
	if (this->discharge_over_current1_alarm_number_ != nullptr) {
		this->discharge_over_current1_alarm_number_->add_on_control_callback([this](float value) {
			if (!discharge_over_current1_configuration_seen_) {
				ESP_LOGE(TAG, "discharge_over_current1_alarm cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting discharge_over_current1_alarm user selected value %f", value);
			this->discharge_over_current1_configuration_.AlarmAmperage = (uint16_t)std::roundl(value);
			this->parent_->set_discharge_over_current1_configuration(this->discharge_over_current1_configuration_);
			});
	}
	if (this->discharge_over_current1_protection_number_ != nullptr) {
		this->discharge_over_current1_protection_number_->add_on_control_callback([this](float value) {
			if (!discharge_over_current1_configuration_seen_) {
				ESP_LOGE(TAG, "discharge_over_current1_protection cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting discharge_over_current1_protection user selected value %f", value);
			this->discharge_over_current1_configuration_.ProtectionAmperage = (uint16_t)std::roundl(value);
			this->parent_->set_discharge_over_current1_configuration(this->discharge_over_current1_configuration_);
			});
	}
	if (this->discharge_over_current1_protection_delay_number_ != nullptr) {
		this->discharge_over_current1_protection_delay_number_->add_on_control_callback([this](float value) {
			if (!discharge_over_current1_configuration_seen_) {
				ESP_LOGE(TAG, "discharge_over_current1_protection_delay cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting discharge_over_current1_protection_delay user selected value %f", value);
			this->discharge_over_current1_configuration_.ProtectionDelayMilliseconds = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_discharge_over_current1_configuration(this->discharge_over_current1_configuration_);
			});
	}

	if (this->request_discharge_over_current2_configuration_callback_ == true) {
		this->parent_->register_discharge_over_current2_configuration_callback(std::bind(&esphome::pace_bms::PaceBmsNumber::discharge_over_current2_configuration_callback, this, std::placeholders::_1));
	}
	if (this->discharge_over_current2_protection_number_ != nullptr) {
		this->discharge_over_current2_protection_number_->add_on_control_callback([this](float value) {
			if (!discharge_over_current2_configuration_seen_) {
				ESP_LOGE(TAG, "discharge_over_current2_protection cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting discharge_over_current2_protection user selected value %f", value);
			this->discharge_over_current2_configuration_.ProtectionAmperage = (uint16_t)std::roundl(value);
			this->parent_->set_discharge_over_current2_configuration(this->discharge_over_current2_configuration_);
			});
	}
	if (this->discharge_over_current2_protection_delay_number_ != nullptr) {
		this->discharge_over_current2_protection_delay_number_->add_on_control_callback([this](float value) {
			if (!discharge_over_current2_configuration_seen_) {
				ESP_LOGE(TAG, "discharge_over_current2_protection_delay cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting discharge_over_current2_protection_delay user selected value %f", value);
			this->discharge_over_current2_configuration_.ProtectionDelayMilliseconds = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_discharge_over_current2_configuration(this->discharge_over_current2_configuration_);
			});
	}

	if (this->request_short_circuit_protection_configuration_callback_ == true) {
		this->parent_->register_short_circuit_protection_configuration_callback(std::bind(&esphome::pace_bms::PaceBmsNumber::short_circuit_protection_configuration_callback, this, std::placeholders::_1));
	}
	if (this->short_circuit_protection_delay_number_ != nullptr) {
		this->short_circuit_protection_delay_number_->add_on_control_callback([this](float value) {
			if (!short_circuit_protection_configuration_seen_) {
				ESP_LOGE(TAG, "short_circuit_protection_delay cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting short_circuit_protection_delay user selected value %f", value);
			this->short_circuit_protection_configuration_.ProtectionDelayMicroseconds = (uint16_t)std::roundl(value * 1000000.0f);
			this->parent_->set_short_circuit_protection_configuration(this->short_circuit_protection_configuration_);
			});
	}
}




float PaceBmsNumber::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsNumber::dump_config() {
	ESP_LOGCONFIG(TAG, "pace_bms_number:");
	LOG_NUMBER("  ", "Cell Over Voltage Alarm", this->cell_over_voltage_alarm_number_);
	LOG_NUMBER("  ", "Cell Over Voltage Protection", this->cell_over_voltage_protection_number_);
	LOG_NUMBER("  ", "Cell Over Voltage Protection Release", this->cell_over_voltage_protection_release_number_);
	LOG_NUMBER("  ", "Cell Over Voltage Protection Delay", this->cell_over_voltage_protection_delay_number_);
	LOG_NUMBER("  ", "Pack Over Voltage Alarm", this->pack_over_voltage_alarm_number_);
	LOG_NUMBER("  ", "Pack Over Voltage Protection", this->pack_over_voltage_protection_number_);
	LOG_NUMBER("  ", "Pack Over Voltage Protection Release", this->pack_over_voltage_protection_release_number_);
	LOG_NUMBER("  ", "Pack Over Voltage Protection Delay", this->pack_over_voltage_protection_delay_number_);
	LOG_NUMBER("  ", "Cell Under Voltage Alarm", this->cell_under_voltage_alarm_number_);
	LOG_NUMBER("  ", "Cell Under Voltage Protection", this->cell_under_voltage_protection_number_);
	LOG_NUMBER("  ", "Cell Under Voltage Protection Release", this->cell_under_voltage_protection_release_number_);
	LOG_NUMBER("  ", "Cell Under Voltage Protection Delay", this->cell_under_voltage_protection_delay_number_);
	LOG_NUMBER("  ", "Pack Under Voltage Alarm", this->pack_under_voltage_alarm_number_);
	LOG_NUMBER("  ", "Pack Under Voltage Protection", this->pack_under_voltage_protection_number_);
	LOG_NUMBER("  ", "Pack Under Voltage Protection Release", this->pack_under_voltage_protection_release_number_);
	LOG_NUMBER("  ", "Pack Under Voltage Protection Delay", this->pack_under_voltage_protection_delay_number_);
}

void PaceBmsNumber::cell_over_voltage_configuration_callback(PaceBmsV25::CellOverVoltageConfiguration& configuration) {

	this->cell_over_voltage_configuration_ = configuration;
	this->cell_over_voltage_configuration_seen_ = true;
	if (this->cell_over_voltage_alarm_number_ != nullptr) {
		float state = configuration.AlarmMillivolts / 1000.0f;
		ESP_LOGV(TAG, "'cell_over_voltage_alarm': Publishing state due to update from the hardware: %f", state);
		this->cell_over_voltage_alarm_number_->publish_state(state);
	}
	if (this->cell_over_voltage_protection_number_ != nullptr) {
		float state = configuration.ProtectionMillivolts / 1000.0f;
		ESP_LOGV(TAG, "'cell_over_voltage_protection': Publishing state due to update from the hardware: %f", state);
		this->cell_over_voltage_protection_number_->publish_state(state);
	}
	if (this->cell_over_voltage_protection_release_number_ != nullptr) {
		float state = configuration.ProtectionReleaseMillivolts / 1000.0f;
		ESP_LOGV(TAG, "'cell_over_voltage_protection_release': Publishing state due to update from the hardware: %f", state);
		this->cell_over_voltage_protection_release_number_->publish_state(state);
	}
	if (this->cell_over_voltage_protection_delay_number_ != nullptr) {
		float state = configuration.ProtectionDelayMilliseconds / 1000.0f;
		ESP_LOGV(TAG, "'cell_over_voltage_protection_delay': Publishing state due to update from the hardware: %f", state);
		this->cell_over_voltage_protection_delay_number_->publish_state(state);
	}
}

void PaceBmsNumber::pack_over_voltage_configuration_callback(PaceBmsV25::PackOverVoltageConfiguration& configuration) {

	this->pack_over_voltage_configuration_ = configuration;
	this->pack_over_voltage_configuration_seen_ = true;
	if (this->pack_over_voltage_alarm_number_ != nullptr) {
		float state = configuration.AlarmMillivolts / 1000.0f;
		ESP_LOGV(TAG, "'pack_over_voltage_alarm': Publishing state due to update from the hardware: %f", state);
		this->pack_over_voltage_alarm_number_->publish_state(state);
	}
	if (this->pack_over_voltage_protection_number_ != nullptr) {
		float state = configuration.ProtectionMillivolts / 1000.0f;
		ESP_LOGV(TAG, "'pack_over_voltage_protection': Publishing state due to update from the hardware: %f", state);
		this->pack_over_voltage_protection_number_->publish_state(state);
	}
	if (this->pack_over_voltage_protection_release_number_ != nullptr) {
		float state = configuration.ProtectionReleaseMillivolts / 1000.0f;
		ESP_LOGV(TAG, "'pack_over_voltage_protection_release': Publishing state due to update from the hardware: %f", state);
		this->pack_over_voltage_protection_release_number_->publish_state(state);
	}
	if (this->pack_over_voltage_protection_delay_number_ != nullptr) {
		float state = configuration.ProtectionDelayMilliseconds / 1000.0f;
		ESP_LOGV(TAG, "'pack_over_voltage_protection_delay': Publishing state due to update from the hardware: %f", state);
		this->pack_over_voltage_protection_delay_number_->publish_state(state);
	}
}

void PaceBmsNumber::cell_under_voltage_configuration_callback(PaceBmsV25::CellUnderVoltageConfiguration& configuration) {

	this->cell_under_voltage_configuration_ = configuration;
	this->cell_under_voltage_configuration_seen_ = true;
	if (this->cell_under_voltage_alarm_number_ != nullptr) {
		float state = configuration.AlarmMillivolts / 1000.0f;
		ESP_LOGV(TAG, "'cell_under_voltage_alarm': Publishing state due to update from the hardware: %f", state);
		this->cell_under_voltage_alarm_number_->publish_state(state);
	}
	if (this->cell_under_voltage_protection_number_ != nullptr) {
		float state = configuration.ProtectionMillivolts / 1000.0f;
		ESP_LOGV(TAG, "'cell_under_voltage_protection': Publishing state due to update from the hardware: %f", state);
		this->cell_under_voltage_protection_number_->publish_state(state);
	}
	if (this->cell_under_voltage_protection_release_number_ != nullptr) {
		float state = configuration.ProtectionReleaseMillivolts / 1000.0f;
		ESP_LOGV(TAG, "'cell_under_voltage_protection_release': Publishing state due to update from the hardware: %f", state);
		this->cell_under_voltage_protection_release_number_->publish_state(state);
	}
	if (this->cell_under_voltage_protection_delay_number_ != nullptr) {
		float state = configuration.ProtectionDelayMilliseconds / 1000.0f;
		ESP_LOGV(TAG, "'cell_under_voltage_protection_delay': Publishing state due to update from the hardware: %f", state);
		this->cell_under_voltage_protection_delay_number_->publish_state(state);
	}
}

void PaceBmsNumber::pack_under_voltage_configuration_callback(PaceBmsV25::PackUnderVoltageConfiguration& configuration) {

	this->pack_under_voltage_configuration_ = configuration;
	this->pack_under_voltage_configuration_seen_ = true;
	if (this->pack_under_voltage_alarm_number_ != nullptr) {
		float state = configuration.AlarmMillivolts / 1000.0f;
		ESP_LOGV(TAG, "'pack_under_voltage_alarm': Publishing state due to update from the hardware: %f", state);
		this->pack_under_voltage_alarm_number_->publish_state(state);
	}
	if (this->pack_under_voltage_protection_number_ != nullptr) {
		float state = configuration.ProtectionMillivolts / 1000.0f;
		ESP_LOGV(TAG, "'pack_under_voltage_protection': Publishing state due to update from the hardware: %f", state);
		this->pack_under_voltage_protection_number_->publish_state(state);
	}
	if (this->pack_under_voltage_protection_release_number_ != nullptr) {
		float state = configuration.ProtectionReleaseMillivolts / 1000.0f;
		ESP_LOGV(TAG, "'pack_under_voltage_protection_release': Publishing state due to update from the hardware: %f", state);
		this->pack_under_voltage_protection_release_number_->publish_state(state);
	}
	if (this->pack_under_voltage_protection_delay_number_ != nullptr) {
		float state = configuration.ProtectionDelayMilliseconds / 1000.0f;
		ESP_LOGV(TAG, "'pack_under_voltage_protection_delay': Publishing state due to update from the hardware: %f", state);
		this->pack_under_voltage_protection_delay_number_->publish_state(state);
	}
}

void PaceBmsNumber::charge_over_current_configuration_callback(PaceBmsV25::ChargeOverCurrentConfiguration& configuration) {
	ESP_LOGE(TAG, "charge over current configuration callback");
	this->charge_over_current_configuration_ = configuration;
	this->charge_over_current_configuration_seen_ = true;
	if (this->charge_over_current_alarm_number_ != nullptr) {
		float state = configuration.AlarmAmperage;
		ESP_LOGV(TAG, "'charge_over_current_alarm': Publishing state due to update from the hardware: %f", state);
		this->charge_over_current_alarm_number_->publish_state(state);
	}
	if (this->charge_over_current_protection_number_ != nullptr) {
		float state = configuration.ProtectionAmperage;
		ESP_LOGV(TAG, "'charge_over_current_protection': Publishing state due to update from the hardware: %f", state);
		this->charge_over_current_protection_number_->publish_state(state);
	}
	if (this->charge_over_current_protection_delay_number_ != nullptr) {
		float state = configuration.ProtectionDelayMilliseconds / 1000.0f;
		ESP_LOGV(TAG, "'charge_over_current_protection_delay': Publishing state due to update from the hardware: %f", state);
		this->charge_over_current_protection_delay_number_->publish_state(state);
	}
}

void PaceBmsNumber::discharge_over_current1_configuration_callback(PaceBmsV25::DischargeOverCurrent1Configuration& configuration) {

	this->discharge_over_current1_configuration_ = configuration;
	this->discharge_over_current1_configuration_seen_ = true;
	if (this->discharge_over_current1_alarm_number_ != nullptr) {
		float state = configuration.AlarmAmperage;
		ESP_LOGV(TAG, "'discharge_over_current1_alarm': Publishing state due to update from the hardware: %f", state);
		this->discharge_over_current1_alarm_number_->publish_state(state);
	}
	if (this->discharge_over_current1_protection_number_ != nullptr) {
		float state = configuration.ProtectionAmperage;
		ESP_LOGV(TAG, "'discharge_over_current1_protection': Publishing state due to update from the hardware: %f", state);
		this->discharge_over_current1_protection_number_->publish_state(state);
	}
	if (this->discharge_over_current1_protection_delay_number_ != nullptr) {
		float state = configuration.ProtectionDelayMilliseconds / 1000.0f;
		ESP_LOGV(TAG, "'discharge_over_current1_protection_delay': Publishing state due to update from the hardware: %f", state);
		this->discharge_over_current1_protection_delay_number_->publish_state(state);
	}
}

void PaceBmsNumber::discharge_over_current2_configuration_callback(PaceBmsV25::DischargeOverCurrent2Configuration& configuration) {

	this->discharge_over_current2_configuration_ = configuration;
	this->discharge_over_current2_configuration_seen_ = true;
	if (this->discharge_over_current2_protection_number_ != nullptr) {
		float state = configuration.ProtectionAmperage;
		ESP_LOGV(TAG, "'discharge_over_current2_protection': Publishing state due to update from the hardware: %f", state);
		this->discharge_over_current2_protection_number_->publish_state(state);
	}
	if (this->discharge_over_current2_protection_delay_number_ != nullptr) {
		float state = configuration.ProtectionDelayMilliseconds / 1000.0f;
		ESP_LOGV(TAG, "'discharge_over_current2_protection_delay': Publishing state due to update from the hardware: %f", state);
		this->discharge_over_current2_protection_delay_number_->publish_state(state);
	}
}

void PaceBmsNumber::short_circuit_protection_configuration_callback(PaceBmsV25::ShortCircuitProtectionConfiguration& configuration) {

	this->short_circuit_protection_configuration_ = configuration;
	this->short_circuit_protection_configuration_seen_ = true;
	if (this->short_circuit_protection_delay_number_ != nullptr) {
		float state = configuration.ProtectionDelayMicroseconds / 1000000.0f;
		ESP_LOGV(TAG, "'short_circuit_protection_delay': Publishing state due to update from the hardware: %f", state);
		this->short_circuit_protection_delay_number_->publish_state(state);
	}
}

}  // namespace pace_bms
}  // namespace esphome










