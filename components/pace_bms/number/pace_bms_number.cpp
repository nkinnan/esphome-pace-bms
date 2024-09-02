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
	  this->cell_over_voltage_alarm_number_->add_on_state_callback([this](float value) {
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
	  this->cell_over_voltage_protection_number_->add_on_state_callback([this](float value) {
		  if (!cell_over_voltage_configuration_seen_) {
			  ESP_LOGE(TAG, "Cell cell_over_voltage_protection cannot be set because the BMS hasn't responded to a get cell over voltage configuration request");
              return;
		  }
		  ESP_LOGD(TAG, "Setting cell_over_voltage_protection user selected value %f", value);
		  PaceBmsV25::CellOverVoltageConfiguration new_config = cell_over_voltage_configuration_;
		  new_config.ProtectionMillivolts = (uint16_t)std::roundl(value * 1000.0f);
          this->parent_->set_cell_over_voltage_configuration(new_config);
	  });
  }
  if (this->cell_over_voltage_protection_release_number_ != nullptr) {
	  this->cell_over_voltage_protection_release_number_->add_on_state_callback([this](float value) {
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
	  this->cell_over_voltage_protection_delay_number_->add_on_state_callback([this](float value) {
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
}

float PaceBmsNumber::get_setup_priority() const { return setup_priority::DATA; }

void PaceBmsNumber::dump_config() {
	ESP_LOGCONFIG(TAG, "pace_bms_number:");
	LOG_NUMBER("  ", "Cell Over Voltage Alarm", this->cell_over_voltage_alarm_number_);
	LOG_NUMBER("  ", "Cell Over Voltage Protection", this->cell_over_voltage_protection_number_);
	LOG_NUMBER("  ", "Cell Over Voltage Protection Release", this->cell_over_voltage_protection_release_number_);
	LOG_NUMBER("  ", "Cell Over Voltage Protection Delay", this->cell_over_voltage_protection_delay_number_);
}

void PaceBmsNumber::cell_over_voltage_configuration_callback(PaceBmsV25::CellOverVoltageConfiguration& configuration) {
	ESP_LOGE("0");
	this->cell_over_voltage_configuration_ = configuration;
	this->cell_over_voltage_configuration_seen_ = true;
	ESP_LOGE("1");
	if (this->cell_over_voltage_alarm_number_ != nullptr) {
		int state = configuration.AlarmMillivolts / 1000.0f;
		ESP_LOGV(TAG, "'cell_over_voltage_alarm': Publishing state due to update from the hardware: %s", state);
		this->cell_over_voltage_alarm_number_->publish_state(state);
	}
	ESP_LOGE("2");
	if (this->cell_over_voltage_protection_number_ != nullptr) {
		int state = configuration.ProtectionMillivolts / 1000.0f;
		ESP_LOGV(TAG, "'cell_over_voltage_protection': Publishing state due to update from the hardware: %s", state);
		this->cell_over_voltage_protection_number_->publish_state(state);
	}
	ESP_LOGE("3");
	if (this->cell_over_voltage_protection_release_number_ != nullptr) {
		int state = configuration.ProtectionReleaseMillivolts / 1000.0f;
		ESP_LOGV(TAG, "'cell_over_voltage_protection_release': Publishing state due to update from the hardware: %s", state);
		this->cell_over_voltage_protection_release_number_->publish_state(state);
	}
	ESP_LOGE("4");
	if (this->cell_over_voltage_protection_delay_number_ != nullptr) {
		int state = configuration.ProtectionDelayMilliseconds / 1000.0f;
		ESP_LOGV(TAG, "'cell_over_voltage_protection_delay': Publishing state due to update from the hardware: %s", state);
		this->cell_over_voltage_protection_delay_number_->publish_state(state);
	}
	ESP_LOGE("5");
}

}  // namespace pace_bms
}  // namespace esphome
