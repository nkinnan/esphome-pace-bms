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
			PaceBmsV25::CellOverVoltageConfiguration new_config = cell_over_voltage_configuration_;
			new_config.AlarmMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_cell_over_voltage_configuration(new_config);
			});
	}
	if (this->cell_over_voltage_protection_number_ != nullptr) {
		this->cell_over_voltage_protection_number_->add_on_control_callback([this](float value) {
			if (!cell_over_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "cell_over_voltage_protection cannot be set because the BMS hasn't responded to a get cell over voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_over_voltage_protection user selected value %f", value);
			PaceBmsV25::CellOverVoltageConfiguration new_config = cell_over_voltage_configuration_;
			new_config.ProtectionMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_cell_over_voltage_configuration(new_config);
			});
	}
	if (this->cell_over_voltage_protection_release_number_ != nullptr) {
		this->cell_over_voltage_protection_release_number_->add_on_control_callback([this](float value) {
			if (!cell_over_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "cell_over_voltage_protection_release cannot be set because the BMS hasn't responded to a get cell over voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_over_voltage_protection_release user selected value %f", value);
			PaceBmsV25::CellOverVoltageConfiguration new_config = cell_over_voltage_configuration_;
			new_config.ProtectionReleaseMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_cell_over_voltage_configuration(new_config);
			});
	}
	if (this->cell_over_voltage_protection_delay_number_ != nullptr) {
		this->cell_over_voltage_protection_delay_number_->add_on_control_callback([this](float value) {
			if (!cell_over_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "cell_over_voltage_protection_delay cannot be set because the BMS hasn't responded to a get cell over voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_over_voltage_protection_delay user selected value %f", value);
			PaceBmsV25::CellOverVoltageConfiguration new_config = cell_over_voltage_configuration_;
			new_config.ProtectionDelayMilliseconds = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_cell_over_voltage_configuration(new_config);
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
			PaceBmsV25::PackOverVoltageConfiguration new_config = pack_over_voltage_configuration_;
			new_config.AlarmMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_pack_over_voltage_configuration(new_config);
			});
	}
	if (this->pack_over_voltage_protection_number_ != nullptr) {
		this->pack_over_voltage_protection_number_->add_on_control_callback([this](float value) {
			if (!pack_over_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "pack_over_voltage_protection cannot be set because the BMS hasn't responded to a get pack over voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting pack_over_voltage_protection user selected value %f", value);
			PaceBmsV25::PackOverVoltageConfiguration new_config = pack_over_voltage_configuration_;
			new_config.ProtectionMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_pack_over_voltage_configuration(new_config);
			});
	}
	if (this->pack_over_voltage_protection_release_number_ != nullptr) {
		this->pack_over_voltage_protection_release_number_->add_on_control_callback([this](float value) {
			if (!pack_over_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "pack_over_voltage_protection_release cannot be set because the BMS hasn't responded to a get pack over voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting pack_over_voltage_protection_release user selected value %f", value);
			PaceBmsV25::PackOverVoltageConfiguration new_config = pack_over_voltage_configuration_;
			new_config.ProtectionReleaseMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_pack_over_voltage_configuration(new_config);
			});
	}
	if (this->pack_over_voltage_protection_delay_number_ != nullptr) {
		this->pack_over_voltage_protection_delay_number_->add_on_control_callback([this](float value) {
			if (!pack_over_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "pack_over_voltage_protection_delay cannot be set because the BMS hasn't responded to a get pack over voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting pack_over_voltage_protection_delay user selected value %f", value);
			PaceBmsV25::PackOverVoltageConfiguration new_config = pack_over_voltage_configuration_;
			new_config.ProtectionDelayMilliseconds = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_pack_over_voltage_configuration(new_config);
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
			PaceBmsV25::CellUnderVoltageConfiguration new_config = cell_under_voltage_configuration_;
			new_config.AlarmMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_cell_under_voltage_configuration(new_config);
			});
	}
	if (this->cell_under_voltage_protection_number_ != nullptr) {
		this->cell_under_voltage_protection_number_->add_on_control_callback([this](float value) {
			if (!cell_under_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "cell_under_voltage_protection cannot be set because the BMS hasn't responded to a get cell under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_under_voltage_protection user selected value %f", value);
			PaceBmsV25::CellUnderVoltageConfiguration new_config = cell_under_voltage_configuration_;
			new_config.ProtectionMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_cell_under_voltage_configuration(new_config);
			});
	}
	if (this->cell_under_voltage_protection_release_number_ != nullptr) {
		this->cell_under_voltage_protection_release_number_->add_on_control_callback([this](float value) {
			if (!cell_under_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "cell_under_voltage_protection_release cannot be set because the BMS hasn't responded to a get cell under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_under_voltage_protection_release user selected value %f", value);
			PaceBmsV25::CellUnderVoltageConfiguration new_config = cell_under_voltage_configuration_;
			new_config.ProtectionReleaseMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_cell_under_voltage_configuration(new_config);
			});
	}
	if (this->cell_under_voltage_protection_delay_number_ != nullptr) {
		this->cell_under_voltage_protection_delay_number_->add_on_control_callback([this](float value) {
			if (!cell_under_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "cell_under_voltage_protection_delay cannot be set because the BMS hasn't responded to a get cell under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_under_voltage_protection_delay user selected value %f", value);
			PaceBmsV25::CellUnderVoltageConfiguration new_config = cell_under_voltage_configuration_;
			new_config.ProtectionDelayMilliseconds = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_cell_under_voltage_configuration(new_config);
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
			PaceBmsV25::PackUnderVoltageConfiguration new_config = pack_under_voltage_configuration_;
			new_config.AlarmMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_pack_under_voltage_configuration(new_config);
			});
	}
	if (this->pack_under_voltage_protection_number_ != nullptr) {
		this->pack_under_voltage_protection_number_->add_on_control_callback([this](float value) {
			if (!pack_under_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "pack_under_voltage_protection cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting pack_under_voltage_protection user selected value %f", value);
			PaceBmsV25::PackUnderVoltageConfiguration new_config = pack_under_voltage_configuration_;
			new_config.ProtectionMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_pack_under_voltage_configuration(new_config);
			});
	}
	if (this->pack_under_voltage_protection_release_number_ != nullptr) {
		this->pack_under_voltage_protection_release_number_->add_on_control_callback([this](float value) {
			if (!pack_under_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "pack_under_voltage_protection_release cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting pack_under_voltage_protection_release user selected value %f", value);
			PaceBmsV25::PackUnderVoltageConfiguration new_config = pack_under_voltage_configuration_;
			new_config.ProtectionReleaseMillivolts = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_pack_under_voltage_configuration(new_config);
			});
	}
	if (this->pack_under_voltage_protection_delay_number_ != nullptr) {
		this->pack_under_voltage_protection_delay_number_->add_on_control_callback([this](float value) {
			if (!pack_under_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "pack_under_voltage_protection_delay cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting pack_under_voltage_protection_delay user selected value %f", value);
			PaceBmsV25::PackUnderVoltageConfiguration new_config = pack_under_voltage_configuration_;
			new_config.ProtectionDelayMilliseconds = (uint16_t)std::roundl(value * 1000.0f);
			this->parent_->set_pack_under_voltage_configuration(new_config);
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

}  // namespace pace_bms
}  // namespace esphome
