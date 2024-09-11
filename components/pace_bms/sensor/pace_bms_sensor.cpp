#include "pace_bms_sensor.h"
#include "esphome/core/log.h"

#include <functional>

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.sensor";

void PaceBmsSensor::setup() {
	if (this->parent_->get_protocol_version() == 0x25) {
		if (request_analog_info_callback_ == true) {
			this->parent_->register_analog_information_callback_v25([this](PaceBmsV25::AnalogInformation& analog_information) { this->analog_information_callback_v25(analog_information); });
		}
		if (request_status_info_callback_ == true) {
			this->parent_->register_status_information_callback_v25([this](PaceBmsV25::StatusInformation& status_information) { this->status_information_callback_v25(status_information); });
		}
	}
	if (this->parent_->get_protocol_version() == 0x20) {
		if (request_analog_info_callback_ == true) {
			this->parent_->register_analog_information_callback_v20([this](PaceBmsV20::AnalogInformation& analog_information) { this->analog_information_callback_v20(analog_information); });
		}
		//if (request_status_info_callback_ == true) {
		//	this->parent_->register_status_information_callback_v20([this](PaceBmsV20::StatusInformation& status_information) { this->status_information_callback_v20(status_information); });
		//}
	}
	else {
		ESP_LOGE(TAG, "Protocol version not supported: 0x%02X", this->parent_->get_protocol_version());
	}
}

void PaceBmsSensor::dump_config() {
	ESP_LOGCONFIG(TAG, "pace_bms_sensor:");
	LOG_SENSOR("  ", "Cell Count", this->cell_count_sensor_);
	for (int i = 0; i < 16; i++)
		LOG_SENSOR("  ", "Cell Voltage X of 16", this->cell_voltage_sensor_[i]);
	LOG_SENSOR("  ", "Temperature Count", this->temperature_count_sensor_);
	for (int i = 0; i < 6; i++)
		LOG_SENSOR("  ", "Temperature X of 6", this->temperature_sensor_[i]);
	LOG_SENSOR("  ", "Current", this->current_sensor_);
	LOG_SENSOR("  ", "Total Voltage", this->total_voltage_sensor_);
	LOG_SENSOR("  ", "Remaining Capacity", this->remaining_capacity_sensor_);
	LOG_SENSOR("  ", "Full Capacity", this->full_capacity_sensor_);
	LOG_SENSOR("  ", "Design Capacity", this->design_capacity_sensor_);
	LOG_SENSOR("  ", "Cycle Count", this->cycle_count_sensor_);
	LOG_SENSOR("  ", "State of Charge", this->state_of_charge_sensor_);
	LOG_SENSOR("  ", "State of Health", this->state_of_health_sensor_);
	LOG_SENSOR("  ", "Power", this->power_sensor_);
	LOG_SENSOR("  ", "Min Cell Voltage", this->min_cell_voltage_sensor_);
	LOG_SENSOR("  ", "Max Cell Voltage", this->max_cell_voltage_sensor_);
	LOG_SENSOR("  ", "Avg Cell Voltage", this->avg_cell_voltage_sensor_);
	LOG_SENSOR("  ", "Max Cell Differential", this->max_cell_differential_sensor_);
	for (int i = 0; i < 16; i++)
		LOG_SENSOR("  ", "Warning Status Value Cell X of 16", this->warning_status_value_cells_sensor_[i]);
	for (int i = 0; i < 6; i++)
		LOG_SENSOR("  ", "Warning Status Value Temperature X of 6", this->temperature_sensor_[i]);
	LOG_SENSOR("  ", "Warning Status Value Charge Current", this->warning_status_value_charge_current_sensor_);
	LOG_SENSOR("  ", "Warning Status Value Total Voltage", this->warning_status_value_total_voltage_sensor_);
	LOG_SENSOR("  ", "Warning Status Value Discharge Current", this->warning_status_value_discharge_current_sensor_);
	LOG_SENSOR("  ", "Warning Status Value 1", this->warning_status_value_1_sensor_);
	LOG_SENSOR("  ", "Warning Status Value 2", this->warning_status_value_2_sensor_);
	LOG_SENSOR("  ", "Balancing Status Value", this->balancing_status_value_sensor_);
	LOG_SENSOR("  ", "System Status Value", this->system_status_value_sensor_);
	LOG_SENSOR("  ", "Configuration Status Value", this->configuration_status_value_sensor_);
	LOG_SENSOR("  ", "Protection Status Value 1", this->protection_status_value_1_sensor_);
	LOG_SENSOR("  ", "Protection Status Value 2", this->protection_status_value_2_sensor_);
	LOG_SENSOR("  ", "Fault Status Value", this->fault_status_value_sensor_);
}

void PaceBmsSensor::analog_information_callback_v25(PaceBmsV25::AnalogInformation& analog_information) {
	if (this->cell_count_sensor_ != nullptr) {
		this->cell_count_sensor_->publish_state(analog_information.cellCount);
	}
	for (int i = 0; i < 16; i++) {
		if (this->cell_voltage_sensor_[i] != nullptr) {
			this->cell_voltage_sensor_[i]->publish_state(analog_information.cellVoltagesMillivolts[i] / 1000.0f);
		}
	}
	if (this->temperature_count_sensor_ != nullptr) {
		this->temperature_count_sensor_->publish_state(analog_information.temperatureCount);
	}
	for (int i = 0; i < 6; i++) {
		if (this->temperature_sensor_[i] != nullptr) {
			this->temperature_sensor_[i]->publish_state(analog_information.temperaturesTenthsCelcius[i] / 10.0f);
		}
	}
	if (this->current_sensor_ != nullptr) {
		this->current_sensor_->publish_state(analog_information.currentMilliamps / 1000.0f);
	}
	if (this->total_voltage_sensor_ != nullptr) {
		this->total_voltage_sensor_->publish_state(analog_information.totalVoltageMillivolts / 1000.0f);
	}
	if (this->remaining_capacity_sensor_ != nullptr) {
		this->remaining_capacity_sensor_->publish_state(analog_information.remainingCapacityMilliampHours / 1000.0f);
	}
	if (this->full_capacity_sensor_ != nullptr) {
		this->full_capacity_sensor_->publish_state(analog_information.fullCapacityMilliampHours / 1000.0f);
	}
	if (this->design_capacity_sensor_ != nullptr) {
		this->design_capacity_sensor_->publish_state(analog_information.designCapacityMilliampHours / 1000.0f);
	}
	if (this->cycle_count_sensor_ != nullptr) {
		this->cycle_count_sensor_->publish_state(analog_information.cycleCount);
	}
	if (this->state_of_charge_sensor_ != nullptr) {
		this->state_of_charge_sensor_->publish_state(analog_information.SoC);
	}
	if (this->state_of_health_sensor_ != nullptr) {
		this->state_of_health_sensor_->publish_state(analog_information.SoH);
	}
	if (this->power_sensor_ != nullptr) {
		this->power_sensor_->publish_state(analog_information.powerWatts);
	}
	if (this->min_cell_voltage_sensor_ != nullptr) {
		this->min_cell_voltage_sensor_->publish_state(analog_information.minCellVoltageMillivolts / 1000.0f);
	}
	if (this->max_cell_voltage_sensor_ != nullptr) {
		this->max_cell_voltage_sensor_->publish_state(analog_information.maxCellVoltageMillivolts / 1000.0f);
	}
	if (this->avg_cell_voltage_sensor_ != nullptr) {
		this->avg_cell_voltage_sensor_->publish_state(analog_information.avgCellVoltageMillivolts / 1000.0f);
	}
	if (this->max_cell_differential_sensor_ != nullptr) {
		this->max_cell_differential_sensor_->publish_state(analog_information.maxCellDifferentialMillivolts / 1000.0f);
	}
}

void PaceBmsSensor::status_information_callback_v25(PaceBmsV25::StatusInformation& status_information) {
	for (int i = 0; i < 16; i++) {
		if (this->warning_status_value_cells_sensor_[i] != nullptr) {
			this->warning_status_value_cells_sensor_[i]->publish_state(status_information.warning_value_cell[i]);
		}
	}
	for (int i = 0; i < 6; i++) {
		if (this->warning_status_value_temps_sensor_[i] != nullptr) {
			this->warning_status_value_temps_sensor_[i]->publish_state(status_information.warning_value_temp[i]);
		}
	}
	if (this->warning_status_value_charge_current_sensor_ != nullptr) {
		this->warning_status_value_charge_current_sensor_->publish_state(status_information.warning_value_charge_current);
	}
	if (this->warning_status_value_total_voltage_sensor_ != nullptr) {
		this->warning_status_value_total_voltage_sensor_->publish_state(status_information.warning_value_total_voltage);
	}
	if (this->warning_status_value_discharge_current_sensor_ != nullptr) {
		this->warning_status_value_discharge_current_sensor_->publish_state(status_information.warning_value_discharge_current);
	}
	if (this->warning_status_value_1_sensor_ != nullptr) {
		this->warning_status_value_1_sensor_->publish_state(status_information.warning_value1);
	}
	if (this->warning_status_value_2_sensor_ != nullptr) {
		this->warning_status_value_2_sensor_->publish_state(status_information.warning_value2);
	}
	if (this->balancing_status_value_sensor_ != nullptr) {
		this->balancing_status_value_sensor_->publish_state(status_information.balancing_value);
	}
	if (this->system_status_value_sensor_ != nullptr) {
		this->system_status_value_sensor_->publish_state(status_information.system_value);
	}
	if (this->configuration_status_value_sensor_ != nullptr) {
		this->configuration_status_value_sensor_->publish_state(status_information.configuration_value);
	}
	if (this->protection_status_value_1_sensor_ != nullptr) {
		this->protection_status_value_1_sensor_->publish_state(status_information.protection_value1);
	}
	if (this->protection_status_value_2_sensor_ != nullptr) {
		this->protection_status_value_2_sensor_->publish_state(status_information.protection_value2);
	}
	if (this->fault_status_value_sensor_ != nullptr) {
		this->fault_status_value_sensor_->publish_state(status_information.fault_value);
	}
}

void PaceBmsSensor::analog_information_callback_v20(PaceBmsV20::AnalogInformation& analog_information) {
	if (this->cell_count_sensor_ != nullptr) {
		this->cell_count_sensor_->publish_state(analog_information.cellCount);
	}
	for (int i = 0; i < 16; i++) {
		if (this->cell_voltage_sensor_[i] != nullptr) {
			this->cell_voltage_sensor_[i]->publish_state(analog_information.cellVoltagesMillivolts[i] / 1000.0f);
		}
	}
	if (this->temperature_count_sensor_ != nullptr) {
		this->temperature_count_sensor_->publish_state(analog_information.temperatureCount);
	}
	for (int i = 0; i < 6; i++) {
		if (this->temperature_sensor_[i] != nullptr) {
			this->temperature_sensor_[i]->publish_state(analog_information.temperaturesTenthsCelcius[i] / 10.0f);
		}
	}
	if (this->current_sensor_ != nullptr) {
		this->current_sensor_->publish_state(analog_information.currentMilliamps / 1000.0f);
	}
	if (this->total_voltage_sensor_ != nullptr) {
		this->total_voltage_sensor_->publish_state(analog_information.totalVoltageMillivolts / 1000.0f);
	}
	if (this->remaining_capacity_sensor_ != nullptr) {
		this->remaining_capacity_sensor_->publish_state(analog_information.remainingCapacityMilliampHours / 1000.0f);
	}
	if (this->full_capacity_sensor_ != nullptr) {
		this->full_capacity_sensor_->publish_state(analog_information.fullCapacityMilliampHours / 1000.0f);
	}
	if (this->design_capacity_sensor_ != nullptr) {
		this->design_capacity_sensor_->publish_state(analog_information.designCapacityMilliampHours / 1000.0f);
	}
	if (this->cycle_count_sensor_ != nullptr) {
		this->cycle_count_sensor_->publish_state(analog_information.cycleCount);
	}
	if (this->state_of_charge_sensor_ != nullptr) {
		this->state_of_charge_sensor_->publish_state(analog_information.SoC);
	}
	if (this->state_of_health_sensor_ != nullptr) {
		this->state_of_health_sensor_->publish_state(analog_information.SoH);
	}
	if (this->power_sensor_ != nullptr) {
		this->power_sensor_->publish_state(analog_information.powerWatts);
	}
	if (this->min_cell_voltage_sensor_ != nullptr) {
		this->min_cell_voltage_sensor_->publish_state(analog_information.minCellVoltageMillivolts / 1000.0f);
	}
	if (this->max_cell_voltage_sensor_ != nullptr) {
		this->max_cell_voltage_sensor_->publish_state(analog_information.maxCellVoltageMillivolts / 1000.0f);
	}
	if (this->avg_cell_voltage_sensor_ != nullptr) {
		this->avg_cell_voltage_sensor_->publish_state(analog_information.avgCellVoltageMillivolts / 1000.0f);
	}
	if (this->max_cell_differential_sensor_ != nullptr) {
		this->max_cell_differential_sensor_->publish_state(analog_information.maxCellDifferentialMillivolts / 1000.0f);
	}
}

}  // namespace pace_bms
}  // namespace esphome
