#include "pace_bms_text_sensor.h"
#include "esphome/core/log.h"

#include <functional>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.sensor";

void PaceBmsTextSensor::setup() {
	if (this->parent_->get_protocol_version() == 0x25) {
		if (this->warning_status_sensor_ != nullptr ||
			this->balancing_status_sensor_ != nullptr ||
			this->system_status_sensor_ != nullptr ||
			this->configuration_status_sensor_ != nullptr ||
			this->protection_status_sensor_ != nullptr ||
			this->fault_status_sensor_ != nullptr) {
			this->parent_->register_status_information_callback_v25([this](PaceBmsV25::StatusInformation& status_information) {
				if (this->warning_status_sensor_ != nullptr) {
					this->warning_status_sensor_->publish_state(status_information.warningText);
				}
				if (this->balancing_status_sensor_ != nullptr) {
					this->balancing_status_sensor_->publish_state(status_information.balancingText);
				}
				if (this->system_status_sensor_ != nullptr) {
					this->system_status_sensor_->publish_state(status_information.systemText);
				}
				if (this->configuration_status_sensor_ != nullptr) {
					this->configuration_status_sensor_->publish_state(status_information.configurationText);
				}
				if (this->protection_status_sensor_ != nullptr) {
					this->protection_status_sensor_->publish_state(status_information.protectionText);
				}
				if (this->fault_status_sensor_ != nullptr) {
					this->fault_status_sensor_->publish_state(status_information.faultText);
				}
			});
		}
		if (this->hardware_version_sensor_ != nullptr) {
			this->parent_->register_hardware_version_callback_v25([this](std::string& hardware_version) {
				if (this->hardware_version_sensor_ != nullptr) {
					this->hardware_version_sensor_->publish_state(hardware_version);
				}
			});
		}
		if (this->serial_number_sensor_ != nullptr) {
			this->parent_->register_serial_number_callback_v25([this](std::string& serial_number) {
				if (this->serial_number_sensor_ != nullptr) {
					this->serial_number_sensor_->publish_state(serial_number);
				}
			});
		}
	}
	else if (this->parent_->get_protocol_version() == 0x20) {
		if (this->warning_status_sensor_ != nullptr ||
			//this->balancing_status_sensor_ != nullptr ||
			this->system_status_sensor_ != nullptr ||
			this->configuration_status_sensor_ != nullptr ||
			this->protection_status_sensor_ != nullptr ||
			this->fault_status_sensor_ != nullptr) {
			this->parent_->register_status_information_callback_v20([this](PaceBmsV20::StatusInformation& status_information) {
				if (this->warning_status_sensor_ != nullptr) {
					this->warning_status_sensor_->publish_state(status_information.warningText);
				}
				//if (this->balancing_status_sensor_ != nullptr) {
				//	this->balancing_status_sensor_->publish_state(status_information.balancingText);
				//}
				if (this->system_status_sensor_ != nullptr) {
					this->system_status_sensor_->publish_state(status_information.systemText);
				}
				if (this->configuration_status_sensor_ != nullptr) {
					this->configuration_status_sensor_->publish_state(status_information.configurationText);
				}
				if (this->protection_status_sensor_ != nullptr) {
					this->protection_status_sensor_->publish_state(status_information.protectionText);
				}
				if (this->fault_status_sensor_ != nullptr) {
					this->fault_status_sensor_->publish_state(status_information.faultText);
				}
				});
		}
		//if (this->hardware_version_sensor_ != nullptr) {
		//	this->parent_->register_hardware_version_callback_v25([this](std::string& hardware_version) {
		//		if (this->hardware_version_sensor_ != nullptr) {
		//			this->hardware_version_sensor_->publish_state(hardware_version);
		//		}
		//		});
		//}
		//if (this->serial_number_sensor_ != nullptr) {
		//	this->parent_->register_serial_number_callback_v25([this](std::string& serial_number) {
		//		if (this->serial_number_sensor_ != nullptr) {
		//			this->serial_number_sensor_->publish_state(serial_number);
		//		}
		//		});
		//}
	}
	else {
		ESP_LOGE(TAG, "Protocol version not supported: 0x%02X", this->parent_->get_protocol_version());
	}
}

void PaceBmsTextSensor::dump_config() {
	ESP_LOGCONFIG(TAG, "pace_bms_text_sensor:");
	LOG_TEXT_SENSOR("  ", "Warning Status", this->warning_status_sensor_);
	LOG_TEXT_SENSOR("  ", "Balancing Status", this->balancing_status_sensor_);
	LOG_TEXT_SENSOR("  ", "System Status", this->system_status_sensor_);
	LOG_TEXT_SENSOR("  ", "Configuration Status", this->configuration_status_sensor_);
	LOG_TEXT_SENSOR("  ", "Protection Status", this->protection_status_sensor_);
	LOG_TEXT_SENSOR("  ", "Fault Status", this->fault_status_sensor_);
	LOG_TEXT_SENSOR("  ", "Hardware Version", this->hardware_version_sensor_);
	LOG_TEXT_SENSOR("  ", "Serial Number", this->serial_number_sensor_);
}

}  // namespace pace_bms
}  // namespace esphome
