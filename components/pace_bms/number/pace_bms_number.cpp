#include "pace_bms_number.h"
#include "esphome/core/log.h"

#include <cmath>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.number";

/*
* wire up all the lambda callbacks
*/
void PaceBmsNumber::setup() {
	if (this->cell_over_voltage_alarm_number_ != nullptr ||
		this->cell_over_voltage_protection_number_ != nullptr ||
		this->cell_over_voltage_protection_release_number_ != nullptr ||
		this->cell_over_voltage_protection_delay_number_ != nullptr) {
		this->parent_->register_cell_over_voltage_configuration_callback([this](PaceBmsV25::CellOverVoltageConfiguration & configuration) {
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
		});
	}
	if (this->cell_over_voltage_alarm_number_ != nullptr) {
		this->cell_over_voltage_alarm_number_->add_on_control_callback([this](float value) {
			if (!cell_over_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "cell_over_voltage_alarm cannot be set because the BMS hasn't responded to a get cell over voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_over_voltage_alarm user selected value %f", value);
			this->cell_over_voltage_configuration_.AlarmMillivolts = std::lround(value * 1000.0f);
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
			this->cell_over_voltage_configuration_.ProtectionMillivolts = std::lround(value * 1000.0f);
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
			this->cell_over_voltage_configuration_.ProtectionReleaseMillivolts = std::lround(value * 1000.0f);
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
			this->cell_over_voltage_configuration_.ProtectionDelayMilliseconds = std::lround(value * 1000.0f);
			this->parent_->set_cell_over_voltage_configuration(this->cell_over_voltage_configuration_);
		});
	}

	if (this->pack_over_voltage_alarm_number_ != nullptr ||
		this->pack_over_voltage_protection_number_ != nullptr ||
		this->pack_over_voltage_protection_release_number_ != nullptr ||
		this->pack_over_voltage_protection_delay_number_ != nullptr) {
		this->parent_->register_pack_over_voltage_configuration_callback([this](PaceBmsV25::PackOverVoltageConfiguration & configuration) {
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
		});
	}
	if (this->pack_over_voltage_alarm_number_ != nullptr) {
		this->pack_over_voltage_alarm_number_->add_on_control_callback([this](float value) {
			if (!pack_over_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "pack_over_voltage_alarm cannot be set because the BMS hasn't responded to a get pack over voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting pack_over_voltage_alarm user selected value %f", value);
			this->pack_over_voltage_configuration_.AlarmMillivolts = std::lround(value * 1000.0f);
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
			this->pack_over_voltage_configuration_.ProtectionMillivolts = std::lround(value * 1000.0f);
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
			this->pack_over_voltage_configuration_.ProtectionReleaseMillivolts = std::lround(value * 1000.0f);
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
			this->pack_over_voltage_configuration_.ProtectionDelayMilliseconds = std::lround(value * 1000.0f);
			this->parent_->set_pack_over_voltage_configuration(this->pack_over_voltage_configuration_);
		});
	}

	if (this->cell_under_voltage_alarm_number_ != nullptr ||
		this->cell_under_voltage_protection_number_ != nullptr ||
		this->cell_under_voltage_protection_release_number_ != nullptr ||
		this->cell_under_voltage_protection_delay_number_ != nullptr) {
		this->parent_->register_cell_under_voltage_configuration_callback([this](PaceBmsV25::CellUnderVoltageConfiguration & configuration) {
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
		});
	}
	if (this->cell_under_voltage_alarm_number_ != nullptr) {
		this->cell_under_voltage_alarm_number_->add_on_control_callback([this](float value) {
			if (!cell_under_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "cell_under_voltage_alarm cannot be set because the BMS hasn't responded to a get cell under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_under_voltage_alarm user selected value %f", value);
			this->cell_under_voltage_configuration_.AlarmMillivolts = std::lround(value * 1000.0f);
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
			this->cell_under_voltage_configuration_.ProtectionMillivolts = std::lround(value * 1000.0f);
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
			this->cell_under_voltage_configuration_.ProtectionReleaseMillivolts = std::lround(value * 1000.0f);
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
			this->cell_under_voltage_configuration_.ProtectionDelayMilliseconds = std::lround(value * 1000.0f);
			this->parent_->set_cell_under_voltage_configuration(this->cell_under_voltage_configuration_);
		});
	}

	if (this->pack_under_voltage_alarm_number_ != nullptr ||
		this->pack_under_voltage_protection_number_ != nullptr ||
		this->pack_under_voltage_protection_release_number_ != nullptr ||
		this->pack_under_voltage_protection_delay_number_ != nullptr) {
		this->parent_->register_pack_under_voltage_configuration_callback([this](PaceBmsV25::PackUnderVoltageConfiguration& configuration) {
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
		});
	}
	if (this->pack_under_voltage_alarm_number_ != nullptr) {
		this->pack_under_voltage_alarm_number_->add_on_control_callback([this](float value) {
			if (!pack_under_voltage_configuration_seen_) {
				ESP_LOGE(TAG, "pack_under_voltage_alarm cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting pack_under_voltage_alarm user selected value %f", value);
			this->pack_under_voltage_configuration_.AlarmMillivolts = std::lround(value * 1000.0f);
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
			this->pack_under_voltage_configuration_.ProtectionMillivolts = std::lround(value * 1000.0f);
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
			this->pack_under_voltage_configuration_.ProtectionReleaseMillivolts = std::lround(value * 1000.0f);
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
			this->pack_under_voltage_configuration_.ProtectionDelayMilliseconds = std::lround(value * 1000.0f);
			this->parent_->set_pack_under_voltage_configuration(this->pack_under_voltage_configuration_);
		});
	}

	if (this->charge_over_current_alarm_number_ != nullptr ||
		this->charge_over_current_protection_number_ != nullptr ||
		this->charge_over_current_protection_delay_number_ != nullptr) {
		this->parent_->register_charge_over_current_configuration_callback([this](PaceBmsV25::ChargeOverCurrentConfiguration& configuration) {
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
		});
	}
	if (this->charge_over_current_alarm_number_ != nullptr) {
		this->charge_over_current_alarm_number_->add_on_control_callback([this](float value) {
			if (!charge_over_current_configuration_seen_) {
				ESP_LOGE(TAG, "charge_over_current_alarm cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting charge_over_current_alarm user selected value %f", value);
			this->charge_over_current_configuration_.AlarmAmperage = std::lround(value);
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
			this->charge_over_current_configuration_.ProtectionAmperage = std::lround(value);
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
			this->charge_over_current_configuration_.ProtectionDelayMilliseconds = std::lround(value * 1000.0f);
			this->parent_->set_charge_over_current_configuration(this->charge_over_current_configuration_);
		});
	}

	if (this->discharge_over_current1_alarm_number_ != nullptr ||
		this->discharge_over_current1_protection_number_ != nullptr ||
		this->discharge_over_current1_protection_delay_number_ != nullptr) {
		this->parent_->register_discharge_over_current1_configuration_callback([this](PaceBmsV25::DischargeOverCurrent1Configuration& configuration) {
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
		});
	}
	if (this->discharge_over_current1_alarm_number_ != nullptr) {
		this->discharge_over_current1_alarm_number_->add_on_control_callback([this](float value) {
			if (!discharge_over_current1_configuration_seen_) {
				ESP_LOGE(TAG, "discharge_over_current1_alarm cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting discharge_over_current1_alarm user selected value %f", value);
			this->discharge_over_current1_configuration_.AlarmAmperage = std::lround(value);
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
			this->discharge_over_current1_configuration_.ProtectionAmperage = std::lround(value);
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
			this->discharge_over_current1_configuration_.ProtectionDelayMilliseconds = std::lround(value * 1000.0f);
			this->parent_->set_discharge_over_current1_configuration(this->discharge_over_current1_configuration_);
		});
	}

	if (this->discharge_over_current2_protection_number_ != nullptr ||
		this->discharge_over_current2_protection_delay_number_ != nullptr) {
		this->parent_->register_discharge_over_current2_configuration_callback([this](PaceBmsV25::DischargeOverCurrent2Configuration& configuration) {
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
		});
	}
	if (this->discharge_over_current2_protection_number_ != nullptr) {
		this->discharge_over_current2_protection_number_->add_on_control_callback([this](float value) {
			if (!discharge_over_current2_configuration_seen_) {
				ESP_LOGE(TAG, "discharge_over_current2_protection cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting discharge_over_current2_protection user selected value %f", value);
			this->discharge_over_current2_configuration_.ProtectionAmperage = std::lround(value);
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
			this->discharge_over_current2_configuration_.ProtectionDelayMilliseconds = std::lround(value * 1000.0f);
			this->parent_->set_discharge_over_current2_configuration(this->discharge_over_current2_configuration_);
		});
	}

	if (this->short_circuit_protection_delay_number_ != nullptr) {
		this->parent_->register_short_circuit_protection_configuration_callback([this](PaceBmsV25::ShortCircuitProtectionConfiguration& configuration) {

			this->short_circuit_protection_configuration_ = configuration;
			this->short_circuit_protection_configuration_seen_ = true;
			if (this->short_circuit_protection_delay_number_ != nullptr) {
				float state = configuration.ProtectionDelayMicroseconds / 1000.0f;
				ESP_LOGV(TAG, "'short_circuit_protection_delay': Publishing state due to update from the hardware: %f", state);
				this->short_circuit_protection_delay_number_->publish_state(state);
			}
		});
	}
	if (this->short_circuit_protection_delay_number_ != nullptr) {
		this->short_circuit_protection_delay_number_->add_on_control_callback([this](float value) {
			if (!short_circuit_protection_configuration_seen_) {
				ESP_LOGE(TAG, "short_circuit_protection_delay cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting short_circuit_protection_delay user selected value %f", value);
			this->short_circuit_protection_configuration_.ProtectionDelayMicroseconds = std::lround(value * 1000.0f);
			this->parent_->set_short_circuit_protection_configuration(this->short_circuit_protection_configuration_);
		});
	}

	if (this->cell_balancing_threshold_number_ != nullptr ||
		this->cell_balancing_delta_number_ != nullptr) {
		this->parent_->register_cell_balancing_configuration_callback([this](PaceBmsV25::CellBalancingConfiguration& configuration) {

			this->cell_balancing_configuration_ = configuration;
			this->cell_balancing_configuration_seen_ = true;
			if (this->cell_balancing_threshold_number_ != nullptr) {
				float state = configuration.ThresholdMillivolts / 1000.0f;
				ESP_LOGV(TAG, "'cell_balancing_threshold': Publishing state due to update from the hardware: %f", state);
				this->cell_balancing_threshold_number_->publish_state(state);
			}
			if (this->cell_balancing_delta_number_ != nullptr) {
				float state = configuration.DeltaCellMillivolts / 1000.0f;
				ESP_LOGV(TAG, "'cell_balancing_delta': Publishing state due to update from the hardware: %f", state);
				this->cell_balancing_delta_number_->publish_state(state);
			}
		});
	}
	if (this->cell_balancing_threshold_number_ != nullptr) {
		this->cell_balancing_threshold_number_->add_on_control_callback([this](float value) {
			if (!cell_balancing_configuration_seen_) {
				ESP_LOGE(TAG, "cell_balancing_threshold cannot be set because the BMS hasn't responded to a get cell balancing configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_balancing_threshold user selected value %f", value);
			this->cell_balancing_configuration_.ThresholdMillivolts = std::lround(value * 1000.0f);
			this->parent_->set_cell_balancing_configuration(this->cell_balancing_configuration_);
		});
	}
	if (this->cell_balancing_delta_number_ != nullptr) {
		this->cell_balancing_delta_number_->add_on_control_callback([this](float value) {
			if (!cell_balancing_configuration_seen_) {
				ESP_LOGE(TAG, "cell_balancing_delta cannot be set because the BMS hasn't responded to a get cell balancing configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting cell_balancing_delta user selected value %f", value);
			this->cell_balancing_configuration_.DeltaCellMillivolts = std::lround(value * 1000.0f);
			this->parent_->set_cell_balancing_configuration(this->cell_balancing_configuration_);
		});
	}

	if (this->sleep_cell_voltage_number_ != nullptr ||
		this->sleep_delay_number_ != nullptr) {
		this->parent_->register_sleep_configuration_callback([this](PaceBmsV25::SleepConfiguration& configuration) {
			this->sleep_configuration_ = configuration;
			this->sleep_configuration_seen_ = true;
			if (this->sleep_cell_voltage_number_ != nullptr) {
				float state = configuration.CellMillivolts / 1000.0f;
				ESP_LOGV(TAG, "'sleep_cell_voltage': Publishing state due to update from the hardware: %f", state);
				this->sleep_cell_voltage_number_->publish_state(state);
			}
			if (this->sleep_delay_number_ != nullptr) {
				float state = configuration.DelayMinutes;
				ESP_LOGV(TAG, "'sleep_delay': Publishing state due to update from the hardware: %f", state);
				this->sleep_delay_number_->publish_state(state);
			}
		});
	}
	if (this->sleep_cell_voltage_number_ != nullptr) {
		this->sleep_cell_voltage_number_->add_on_control_callback([this](float value) {
			if (!sleep_configuration_seen_) {
				ESP_LOGE(TAG, "sleep_cell_voltage cannot be set because the BMS hasn't responded to a get sleep configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting sleep_cell_voltage user selected value %f", value);
			this->sleep_configuration_.CellMillivolts = std::lround(value * 1000.0f);
			this->parent_->set_sleep_configuration(this->sleep_configuration_);
		});
	}
	if (this->sleep_delay_number_ != nullptr) {
		this->sleep_delay_number_->add_on_control_callback([this](float value) {
			if (!sleep_configuration_seen_) {
				ESP_LOGE(TAG, "sleep_delay cannot be set because the BMS hasn't responded to a get sleep configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting sleep_delay user selected value %f", value);
			this->sleep_configuration_.DelayMinutes = std::lround(value);
			this->parent_->set_sleep_configuration(this->sleep_configuration_);
		});
	}

	if (this->full_charge_voltage_number_ != nullptr ||
		this->full_charge_amps_number_ != nullptr ||
		this->low_charge_alarm_percent_number_ != nullptr) {
		this->parent_->register_full_charge_low_charge_configuration_callback([this](PaceBmsV25::FullChargeLowChargeConfiguration& configuration) {
			this->full_charge_low_charge_configuration_ = configuration;
			this->full_charge_low_charge_configuration_seen_ = true;
			if (this->full_charge_voltage_number_ != nullptr) {
				float state = configuration.FullChargeMillivolts / 1000.0f;
				ESP_LOGV(TAG, "'full_charge_voltage': Publishing state due to update from the hardware: %f", state);
				this->full_charge_voltage_number_->publish_state(state);
			}
			if (this->full_charge_amps_number_ != nullptr) {
				float state = configuration.FullChargeMilliamps / 1000.0f;
				ESP_LOGV(TAG, "'full_charge_amps': Publishing state due to update from the hardware: %f", state);
				this->full_charge_amps_number_->publish_state(state);
			}
			if (this->low_charge_alarm_percent_number_ != nullptr) {
				float state = configuration.LowChargeAlarmPercent;
				ESP_LOGV(TAG, "'low_charge_alarm_percent': Publishing state due to update from the hardware: %f", state);
				this->low_charge_alarm_percent_number_->publish_state(state);
			}
		});
	}
	if (this->full_charge_voltage_number_ != nullptr) {
		this->full_charge_voltage_number_->add_on_control_callback([this](float value) {
			if (!full_charge_low_charge_configuration_seen_) {
				ESP_LOGE(TAG, "full_charge_voltage cannot be set because the BMS hasn't responded to a get full charge / low charge configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting full_charge_voltage user selected value %f", value);
			this->full_charge_low_charge_configuration_.FullChargeMillivolts = std::lround(value * 1000.0f);
			this->parent_->set_full_charge_low_charge_configuration(this->full_charge_low_charge_configuration_);
		});
	}
	if (this->full_charge_amps_number_ != nullptr) {
		this->full_charge_amps_number_->add_on_control_callback([this](float value) {
			if (!full_charge_low_charge_configuration_seen_) {
				ESP_LOGE(TAG, "full_charge_amps cannot be set because the BMS hasn't responded to a get full charge / low charge configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting full_charge_amps user selected value %f", value);
			this->full_charge_low_charge_configuration_.FullChargeMilliamps = std::lround(value * 1000.0f);
			this->parent_->set_full_charge_low_charge_configuration(this->full_charge_low_charge_configuration_);
		});
	}
	if (this->low_charge_alarm_percent_number_ != nullptr) {
		this->low_charge_alarm_percent_number_->add_on_control_callback([this](float value) {
			if (!full_charge_low_charge_configuration_seen_) {
				ESP_LOGE(TAG, "low_charge_alarm_percent cannot be set because the BMS hasn't responded to a get full charge / low charge configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting low_charge_alarm_percent user selected value %f", value);
			this->full_charge_low_charge_configuration_.LowChargeAlarmPercent = std::lround(value);
			this->parent_->set_full_charge_low_charge_configuration(this->full_charge_low_charge_configuration_);
		});
	}

	if (this->charge_over_temperature_alarm_number_ != nullptr ||
		this->charge_over_temperature_protection_number_ != nullptr ||
		this->charge_over_temperature_protection_release_number_ != nullptr ||
		this->discharge_over_temperature_alarm_number_ != nullptr ||
		this->discharge_over_temperature_protection_number_ != nullptr ||
		this->discharge_over_temperature_protection_release_number_ != nullptr) {
		this->parent_->register_charge_and_discharge_over_temperature_configuration_callback([this](PaceBmsV25::ChargeAndDischargeOverTemperatureConfiguration& configuration) {
			this->charge_and_discharge_over_temperature_configuration_ = configuration;
			this->charge_and_discharge_over_temperature_configuration_seen_ = true;
			if (this->charge_over_temperature_alarm_number_ != nullptr) {
				float state = configuration.ChargeAlarm;
				ESP_LOGV(TAG, "'charge_over_temperature_alarm': Publishing state due to update from the hardware: %f", state);
				this->charge_over_temperature_alarm_number_->publish_state(state);
			}
			if (this->charge_over_temperature_protection_number_ != nullptr) {
				float state = configuration.ChargeProtection;
				ESP_LOGV(TAG, "'charge_over_temperature_protection': Publishing state due to update from the hardware: %f", state);
				this->charge_over_temperature_protection_number_->publish_state(state);
			}
			if (this->charge_over_temperature_protection_release_number_ != nullptr) {
				float state = configuration.ChargeProtectionRelease;
				ESP_LOGV(TAG, "'charge_over_temperature_protection_release': Publishing state due to update from the hardware: %f", state);
				this->charge_over_temperature_protection_release_number_->publish_state(state);
			}
			if (this->discharge_over_temperature_alarm_number_ != nullptr) {
				float state = configuration.DischargeAlarm;
				ESP_LOGV(TAG, "'discharge_over_temperature_alarm': Publishing state due to update from the hardware: %f", state);
				this->discharge_over_temperature_alarm_number_->publish_state(state);
			}
			if (this->discharge_over_temperature_protection_number_ != nullptr) {
				float state = configuration.DischargeProtection;
				ESP_LOGV(TAG, "'discharge_over_temperature_protection': Publishing state due to update from the hardware: %f", state);
				this->discharge_over_temperature_protection_number_->publish_state(state);
			}
			if (this->discharge_over_temperature_protection_release_number_ != nullptr) {
				float state = configuration.DischargeProtectionRelease;
				ESP_LOGV(TAG, "'discharge_over_temperature_protection_release': Publishing state due to update from the hardware: %f", state);
				this->discharge_over_temperature_protection_release_number_->publish_state(state);
			}
		});
	}
	if (this->charge_over_temperature_alarm_number_ != nullptr) {
		this->charge_over_temperature_alarm_number_->add_on_control_callback([this](float value) {
			if (!charge_and_discharge_over_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "charge_over_temperature_alarm cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting charge_over_temperature_alarm user selected value %f", value);
			this->charge_and_discharge_over_temperature_configuration_.ChargeAlarm = std::lround(value);
			this->parent_->set_charge_and_discharge_over_temperature_configuration(this->charge_and_discharge_over_temperature_configuration_);
		});
	}
	if (this->charge_over_temperature_protection_number_ != nullptr) {
		this->charge_over_temperature_protection_number_->add_on_control_callback([this](float value) {
			if (!charge_and_discharge_over_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "charge_over_temperature_protection cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting charge_over_temperature_protection user selected value %f", value);
			this->charge_and_discharge_over_temperature_configuration_.ChargeProtection = std::lround(value);
			this->parent_->set_charge_and_discharge_over_temperature_configuration(this->charge_and_discharge_over_temperature_configuration_);
		});
	}
	if (this->charge_over_temperature_protection_release_number_ != nullptr) {
		this->charge_over_temperature_protection_release_number_->add_on_control_callback([this](float value) {
			if (!charge_and_discharge_over_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "charge_over_temperature_protection_release cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting charge_over_temperature_protection_release user selected value %f", value);
			this->charge_and_discharge_over_temperature_configuration_.ChargeProtectionRelease = std::lround(value);
			this->parent_->set_charge_and_discharge_over_temperature_configuration(this->charge_and_discharge_over_temperature_configuration_);
		});
	}
	if (this->discharge_over_temperature_alarm_number_ != nullptr) {
		this->discharge_over_temperature_alarm_number_->add_on_control_callback([this](float value) {
			if (!charge_and_discharge_over_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "discharge_over_temperature_alarm cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting discharge_over_temperature_alarm user selected value %f", value);
			this->charge_and_discharge_over_temperature_configuration_.DischargeAlarm = std::lround(value);
			this->parent_->set_charge_and_discharge_over_temperature_configuration(this->charge_and_discharge_over_temperature_configuration_);
		});
	}
	if (this->discharge_over_temperature_protection_number_ != nullptr) {
		this->discharge_over_temperature_protection_number_->add_on_control_callback([this](float value) {
			if (!charge_and_discharge_over_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "discharge_over_temperature_protection cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting discharge_over_temperature_protection user selected value %f", value);
			this->charge_and_discharge_over_temperature_configuration_.DischargeProtection = std::lround(value);
			this->parent_->set_charge_and_discharge_over_temperature_configuration(this->charge_and_discharge_over_temperature_configuration_);
		});
	}
	if (this->discharge_over_temperature_protection_release_number_ != nullptr) {
		this->discharge_over_temperature_protection_release_number_->add_on_control_callback([this](float value) {
			if (!charge_and_discharge_over_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "discharge_over_temperature_protection_release cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting discharge_over_temperature_protection_release user selected value %f", value);
			this->charge_and_discharge_over_temperature_configuration_.DischargeProtectionRelease = std::lround(value);
			this->parent_->set_charge_and_discharge_over_temperature_configuration(this->charge_and_discharge_over_temperature_configuration_);
		});
	}

	if (this->charge_under_temperature_alarm_number_ != nullptr ||
		this->charge_under_temperature_protection_number_ != nullptr ||
		this->charge_under_temperature_protection_release_number_ != nullptr ||
		this->discharge_under_temperature_alarm_number_ != nullptr ||
		this->discharge_under_temperature_protection_number_ != nullptr ||
		this->discharge_under_temperature_protection_release_number_ != nullptr) {
		this->parent_->register_charge_and_discharge_under_temperature_configuration_callback([this](PaceBmsV25::ChargeAndDischargeUnderTemperatureConfiguration& configuration) {
			this->charge_and_discharge_under_temperature_configuration_ = configuration;
			this->charge_and_discharge_under_temperature_configuration_seen_ = true;
			if (this->charge_under_temperature_alarm_number_ != nullptr) {
				float state = configuration.ChargeAlarm;
				ESP_LOGV(TAG, "'charge_under_temperature_alarm': Publishing state due to update from the hardware: %f", state);
				this->charge_under_temperature_alarm_number_->publish_state(state);
			}
			if (this->charge_under_temperature_protection_number_ != nullptr) {
				float state = configuration.ChargeProtection;
				ESP_LOGV(TAG, "'charge_under_temperature_protection': Publishing state due to update from the hardware: %f", state);
				this->charge_under_temperature_protection_number_->publish_state(state);
			}
			if (this->charge_under_temperature_protection_release_number_ != nullptr) {
				float state = configuration.ChargeProtectionRelease;
				ESP_LOGV(TAG, "'charge_under_temperature_protection_release': Publishing state due to update from the hardware: %f", state);
				this->charge_under_temperature_protection_release_number_->publish_state(state);
			}
			if (this->discharge_under_temperature_alarm_number_ != nullptr) {
				float state = configuration.DischargeAlarm;
				ESP_LOGV(TAG, "'discharge_under_temperature_alarm': Publishing state due to update from the hardware: %f", state);
				this->discharge_under_temperature_alarm_number_->publish_state(state);
			}
			if (this->discharge_under_temperature_protection_number_ != nullptr) {
				float state = configuration.DischargeProtection;
				ESP_LOGV(TAG, "'discharge_under_temperature_protection': Publishing state due to update from the hardware: %f", state);
				this->discharge_under_temperature_protection_number_->publish_state(state);
			}
			if (this->discharge_under_temperature_protection_release_number_ != nullptr) {
				float state = configuration.DischargeProtectionRelease;
				ESP_LOGV(TAG, "'discharge_under_temperature_protection_release': Publishing state due to update from the hardware: %f", state);
				this->discharge_under_temperature_protection_release_number_->publish_state(state);
			}
		});
	}
	if (this->charge_under_temperature_alarm_number_ != nullptr) {
		this->charge_under_temperature_alarm_number_->add_on_control_callback([this](float value) {
			if (!charge_and_discharge_under_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "charge_under_temperature_alarm cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting charge_under_temperature_alarm user selected value %f", value);
			this->charge_and_discharge_under_temperature_configuration_.ChargeAlarm = std::lround(value);
			this->parent_->set_charge_and_discharge_under_temperature_configuration(this->charge_and_discharge_under_temperature_configuration_);
			});
	}
	if (this->charge_under_temperature_protection_number_ != nullptr) {
		this->charge_under_temperature_protection_number_->add_on_control_callback([this](float value) {
			if (!charge_and_discharge_under_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "charge_under_temperature_protection cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting charge_under_temperature_protection user selected value %f", value);
			this->charge_and_discharge_under_temperature_configuration_.ChargeProtection = std::lround(value);
			this->parent_->set_charge_and_discharge_under_temperature_configuration(this->charge_and_discharge_under_temperature_configuration_);
			});
	}
	if (this->charge_under_temperature_protection_release_number_ != nullptr) {
		this->charge_under_temperature_protection_release_number_->add_on_control_callback([this](float value) {
			if (!charge_and_discharge_under_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "charge_under_temperature_protection_release cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting charge_under_temperature_protection_release user selected value %f", value);
			this->charge_and_discharge_under_temperature_configuration_.ChargeProtectionRelease = std::lround(value);
			this->parent_->set_charge_and_discharge_under_temperature_configuration(this->charge_and_discharge_under_temperature_configuration_);
			});
	}
	if (this->discharge_under_temperature_alarm_number_ != nullptr) {
		this->discharge_under_temperature_alarm_number_->add_on_control_callback([this](float value) {
			if (!charge_and_discharge_under_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "discharge_under_temperature_alarm cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting discharge_under_temperature_alarm user selected value %f", value);
			this->charge_and_discharge_under_temperature_configuration_.DischargeAlarm = std::lround(value);
			this->parent_->set_charge_and_discharge_under_temperature_configuration(this->charge_and_discharge_under_temperature_configuration_);
			});
	}
	if (this->discharge_under_temperature_protection_number_ != nullptr) {
		this->discharge_under_temperature_protection_number_->add_on_control_callback([this](float value) {
			if (!charge_and_discharge_under_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "discharge_under_temperature_protection cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting discharge_under_temperature_protection user selected value %f", value);
			this->charge_and_discharge_under_temperature_configuration_.DischargeProtection = std::lround(value);
			this->parent_->set_charge_and_discharge_under_temperature_configuration(this->charge_and_discharge_under_temperature_configuration_);
			});
	}
	if (this->discharge_under_temperature_protection_release_number_ != nullptr) {
		this->discharge_under_temperature_protection_release_number_->add_on_control_callback([this](float value) {
			if (!charge_and_discharge_under_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "discharge_under_temperature_protection_release cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting discharge_under_temperature_protection_release user selected value %f", value);
			this->charge_and_discharge_under_temperature_configuration_.DischargeProtectionRelease = std::lround(value);
			this->parent_->set_charge_and_discharge_under_temperature_configuration(this->charge_and_discharge_under_temperature_configuration_);
			});
	}

	if (this->mosfet_over_temperature_alarm_number_ != nullptr ||
		this->mosfet_over_temperature_protection_number_ != nullptr ||
		this->mosfet_over_temperature_protection_release_number_ != nullptr) {
		this->parent_->register_mosfet_over_temperature_configuration_callback([this](PaceBmsV25::MosfetOverTemperatureConfiguration& configuration) {
			this->mosfet_over_temperature_configuration_ = configuration;
			this->mosfet_over_temperature_configuration_seen_ = true;
			if (this->mosfet_over_temperature_alarm_number_ != nullptr) {
				float state = configuration.Alarm;
				ESP_LOGV(TAG, "'mosfet_over_temperature_alarm': Publishing state due to update from the hardware: %f", state);
				this->mosfet_over_temperature_alarm_number_->publish_state(state);
			}
			if (this->mosfet_over_temperature_protection_number_ != nullptr) {
				float state = configuration.Protection;
				ESP_LOGV(TAG, "'mosfet_over_temperature_protection': Publishing state due to update from the hardware: %f", state);
				this->mosfet_over_temperature_protection_number_->publish_state(state);
			}
			if (this->mosfet_over_temperature_protection_release_number_ != nullptr) {
				float state = configuration.ProtectionRelease;
				ESP_LOGV(TAG, "'mosfet_over_temperature_protection_release': Publishing state due to update from the hardware: %f", state);
				this->mosfet_over_temperature_protection_release_number_->publish_state(state);
			}
		});
	}
	if (this->mosfet_over_temperature_alarm_number_ != nullptr) {
		this->mosfet_over_temperature_alarm_number_->add_on_control_callback([this](float value) {
			if (!mosfet_over_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "mosfet_over_temperature_alarm cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting mosfet_over_temperature_alarm user selected value %f", value);
			this->mosfet_over_temperature_configuration_.Alarm = std::lround(value);
			this->parent_->set_mosfet_over_temperature_configuration(this->mosfet_over_temperature_configuration_);
			});
	}
	if (this->mosfet_over_temperature_protection_number_ != nullptr) {
		this->mosfet_over_temperature_protection_number_->add_on_control_callback([this](float value) {
			if (!mosfet_over_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "mosfet_over_temperature_protection cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting mosfet_over_temperature_protection user selected value %f", value);
			this->mosfet_over_temperature_configuration_.Protection = std::lround(value);
			this->parent_->set_mosfet_over_temperature_configuration(this->mosfet_over_temperature_configuration_);
			});
	}
	if (this->mosfet_over_temperature_protection_release_number_ != nullptr) {
		this->mosfet_over_temperature_protection_release_number_->add_on_control_callback([this](float value) {
			if (!mosfet_over_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "mosfet_over_temperature_protection_release cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting mosfet_over_temperature_protection_release user selected value %f", value);
			this->mosfet_over_temperature_configuration_.ProtectionRelease = std::lround(value);
			this->parent_->set_mosfet_over_temperature_configuration(this->mosfet_over_temperature_configuration_);
			});
	}

	if (this->environment_under_temperature_alarm_number_ != nullptr ||
		this->environment_under_temperature_protection_number_ != nullptr ||
		this->environment_under_temperature_protection_release_number_ != nullptr ||
		this->environment_over_temperature_alarm_number_ != nullptr ||
		this->environment_over_temperature_protection_number_ != nullptr ||
		this->environment_over_temperature_protection_release_number_ != nullptr) {
		this->parent_->register_environment_over_under_temperature_configuration_callback([this](PaceBmsV25::EnvironmentOverUnderTemperatureConfiguration& configuration) {
			this->environment_over_under_temperature_configuration_ = configuration;
			this->environment_over_under_temperature_configuration_seen_ = true;
			if (this->environment_under_temperature_alarm_number_ != nullptr) {
				float state = configuration.UnderAlarm;
				ESP_LOGV(TAG, "'environment_under_temperature_alarm': Publishing state due to update from the hardware: %f", state);
				this->environment_under_temperature_alarm_number_->publish_state(state);
			}
			if (this->environment_under_temperature_protection_number_ != nullptr) {
				float state = configuration.UnderProtection;
				ESP_LOGV(TAG, "'environment_under_temperature_protection': Publishing state due to update from the hardware: %f", state);
				this->environment_under_temperature_protection_number_->publish_state(state);
			}
			if (this->environment_under_temperature_protection_release_number_ != nullptr) {
				float state = configuration.UnderProtectionRelease;
				ESP_LOGV(TAG, "'environment_under_temperature_protection_release': Publishing state due to update from the hardware: %f", state);
				this->environment_under_temperature_protection_release_number_->publish_state(state);
			}
			if (this->environment_over_temperature_alarm_number_ != nullptr) {
				float state = configuration.OverAlarm;
				ESP_LOGV(TAG, "'environment_over_temperature_alarm': Publishing state due to update from the hardware: %f", state);
				this->environment_over_temperature_alarm_number_->publish_state(state);
			}
			if (this->environment_over_temperature_protection_number_ != nullptr) {
				float state = configuration.OverProtection;
				ESP_LOGV(TAG, "'environment_over_temperature_protection': Publishing state due to update from the hardware: %f", state);
				this->environment_over_temperature_protection_number_->publish_state(state);
			}
			if (this->environment_over_temperature_protection_release_number_ != nullptr) {
				float state = configuration.OverProtectionRelease;
				ESP_LOGV(TAG, "'environment_over_temperature_protection_release': Publishing state due to update from the hardware: %f", state);
				this->environment_over_temperature_protection_release_number_->publish_state(state);
			}
			});
	}
	if (this->environment_under_temperature_alarm_number_ != nullptr) {
		this->environment_under_temperature_alarm_number_->add_on_control_callback([this](float value) {
			if (!environment_over_under_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "environment_under_temperature_alarm cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting environment_under_temperature_alarm user selected value %f", value);
			this->environment_over_under_temperature_configuration_.UnderAlarm = std::lround(value);
			this->parent_->set_environment_over_under_temperature_configuration(this->environment_over_under_temperature_configuration_);
			});
	}
	if (this->environment_under_temperature_protection_number_ != nullptr) {
		this->environment_under_temperature_protection_number_->add_on_control_callback([this](float value) {
			if (!environment_over_under_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "environment_under_temperature_protection cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting environment_under_temperature_protection user selected value %f", value);
			this->environment_over_under_temperature_configuration_.UnderProtection = std::lround(value);
			this->parent_->set_environment_over_under_temperature_configuration(this->environment_over_under_temperature_configuration_);
			});
	}
	if (this->environment_under_temperature_protection_release_number_ != nullptr) {
		this->environment_under_temperature_protection_release_number_->add_on_control_callback([this](float value) {
			if (!environment_over_under_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "environment_under_temperature_protection_release cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting environment_under_temperature_protection_release user selected value %f", value);
			this->environment_over_under_temperature_configuration_.UnderProtectionRelease = std::lround(value);
			this->parent_->set_environment_over_under_temperature_configuration(this->environment_over_under_temperature_configuration_);
			});
	}
	if (this->environment_over_temperature_alarm_number_ != nullptr) {
		this->environment_over_temperature_alarm_number_->add_on_control_callback([this](float value) {
			if (!environment_over_under_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "environment_over_temperature_alarm cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting environment_over_temperature_alarm user selected value %f", value);
			this->environment_over_under_temperature_configuration_.OverAlarm = std::lround(value);
			this->parent_->set_environment_over_under_temperature_configuration(this->environment_over_under_temperature_configuration_);
			});
	}
	if (this->environment_over_temperature_protection_number_ != nullptr) {
		this->environment_over_temperature_protection_number_->add_on_control_callback([this](float value) {
			if (!environment_over_under_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "environment_over_temperature_protection cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting environment_over_temperature_protection user selected value %f", value);
			this->environment_over_under_temperature_configuration_.OverProtection = std::lround(value);
			this->parent_->set_environment_over_under_temperature_configuration(this->environment_over_under_temperature_configuration_);
			});
	}
	if (this->environment_over_temperature_protection_release_number_ != nullptr) {
		this->environment_over_temperature_protection_release_number_->add_on_control_callback([this](float value) {
			if (!environment_over_under_temperature_configuration_seen_) {
				ESP_LOGE(TAG, "environment_over_temperature_protection_release cannot be set because the BMS hasn't responded to a get pack under voltage configuration request");
				return;
			}
			ESP_LOGD(TAG, "Setting environment_over_temperature_protection_release user selected value %f", value);
			this->environment_over_under_temperature_configuration_.OverProtectionRelease = std::lround(value);
			this->parent_->set_environment_over_under_temperature_configuration(this->environment_over_under_temperature_configuration_);
			});
	}
}

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
	LOG_NUMBER("  ", "Charge Over Current Alarm", this->charge_over_current_alarm_number_);
	LOG_NUMBER("  ", "Charge Over Current Protection", this->charge_over_current_protection_number_);
	LOG_NUMBER("  ", "Charge Over Current Protection Delay", this->charge_over_current_protection_delay_number_);
	LOG_NUMBER("  ", "Discharge Over Current 1 Alarm", this->discharge_over_current1_alarm_number_);
	LOG_NUMBER("  ", "Discharge Over Current 1 Protection", this->discharge_over_current1_protection_number_);
	LOG_NUMBER("  ", "Discharge Over Current 1 Protection Delay", this->discharge_over_current1_protection_delay_number_);
	LOG_NUMBER("  ", "Discharge Over Current 2 Protection", this->discharge_over_current2_protection_number_);
	LOG_NUMBER("  ", "Discharge Over Current 2 Protection Delay", this->discharge_over_current2_protection_delay_number_);
	LOG_NUMBER("  ", "Short Circuit Protection Delay", this->short_circuit_protection_delay_number_);
	LOG_NUMBER("  ", "Cell Balancing Threshold", this->cell_balancing_threshold_number_);
	LOG_NUMBER("  ", "Cell Balancing Delta", this->cell_balancing_delta_number_);
	LOG_NUMBER("  ", "Sleep Cell Voltage", this->sleep_cell_voltage_number_);
	LOG_NUMBER("  ", "Sleep Delay", this->sleep_delay_number_);
	LOG_NUMBER("  ", "Full Charge Voltage", this->full_charge_voltage_number_);
	LOG_NUMBER("  ", "Full Charge Amps", this->full_charge_amps_number_);
	LOG_NUMBER("  ", "Low Charge Alarm Percent", this->low_charge_alarm_percent_number_);
	LOG_NUMBER("  ", "Charge Over Temperature Alarm", this->charge_over_temperature_alarm_number_);
	LOG_NUMBER("  ", "Charge Over Temperature Protection", this->charge_over_temperature_protection_number_);
	LOG_NUMBER("  ", "Charge Over Temperature Protection Release", this->charge_over_temperature_protection_release_number_);
	LOG_NUMBER("  ", "Discharge Over Temperature Alarm", this->discharge_over_temperature_alarm_number_);
	LOG_NUMBER("  ", "Discharge Over Temperature Protection", this->discharge_over_temperature_protection_number_);
	LOG_NUMBER("  ", "Discharge Over Temperature Protection Release", this->discharge_over_temperature_protection_release_number_);
	LOG_NUMBER("  ", "Charge Under Temperature Alarm", this->charge_under_temperature_alarm_number_);
	LOG_NUMBER("  ", "Charge Under Temperature Protection", this->charge_under_temperature_protection_number_);
	LOG_NUMBER("  ", "Charge Under Temperature Protection Release", this->charge_under_temperature_protection_release_number_);
	LOG_NUMBER("  ", "Discharge Under Temperature Alarm", this->discharge_under_temperature_alarm_number_);
	LOG_NUMBER("  ", "Discharge Under Temperature Protection", this->discharge_under_temperature_protection_number_);
	LOG_NUMBER("  ", "Discharge Under Temperature Protection Release", this->discharge_under_temperature_protection_release_number_);
	LOG_NUMBER("  ", "Mosfet Over Temperature Alarm", this->mosfet_over_temperature_alarm_number_);
	LOG_NUMBER("  ", "Mosfet Over Temperature Protection", this->mosfet_over_temperature_protection_number_);
	LOG_NUMBER("  ", "Mosfet Over Temperature Protection Release", this->mosfet_over_temperature_protection_release_number_);
	LOG_NUMBER("  ", "Environment Under Temperature Alarm", this->environment_under_temperature_alarm_number_);
	LOG_NUMBER("  ", "Environment Under Temperature Protection", this->environment_under_temperature_protection_number_);
	LOG_NUMBER("  ", "Environment Under Temperature Protection Release", this->environment_under_temperature_protection_release_number_);
	LOG_NUMBER("  ", "Environment Over Temperature Alarm", this->environment_over_temperature_alarm_number_);
	LOG_NUMBER("  ", "Environment Over Temperature Protection", this->environment_over_temperature_protection_number_);
	LOG_NUMBER("  ", "Environment Over Temperature Protection Release", this->environment_over_temperature_protection_release_number_);
}

}  // namespace pace_bms
}  // namespace esphome
