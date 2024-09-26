#include <functional>

#include "esphome/core/log.h"

#include "pace_bms_sensor.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.sensor";

void PaceBmsSensor::setup() {
	if (this->parent_->get_protocol_commandset() == 0x25) {
		if (request_analog_info_callback_ == true) {
			this->parent_->register_analog_information_callback_v25([this](PaceBmsProtocolV25::AnalogInformation& analog_information) { this->analog_information_callback_v25(analog_information); });
		}
		if (request_status_info_callback_ == true) {
			this->parent_->register_status_information_callback_v25([this](PaceBmsProtocolV25::StatusInformation& status_information) { this->status_information_callback_v25(status_information); });
		}
	}
	else if (this->parent_->get_protocol_commandset() == 0x20) {
		if (request_analog_info_callback_ == true) {
			this->parent_->register_analog_information_callback_v20([this](PaceBmsProtocolV20::AnalogInformation& analog_information) { this->analog_information_callback_v20(analog_information); });
		}
		if (request_status_info_callback_ == true) {
			this->parent_->register_status_information_callback_v20([this](PaceBmsProtocolV20::StatusInformation& status_information) { this->status_information_callback_v20(status_information); });
		}
	}
	else {
		ESP_LOGE(TAG, "Protocol version not supported: 0x%02X", this->parent_->get_protocol_commandset());
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
	LOG_SENSOR("  ", "Status 1 Value", this->status1_value_sensor_);
	LOG_SENSOR("  ", "Status 2 Value", this->status2_value_sensor_);
	LOG_SENSOR("  ", "Status 3 Value", this->status3_value_sensor_);
	LOG_SENSOR("  ", "Status 4 Value", this->status4_value_sensor_);
	LOG_SENSOR("  ", "Status 5 Value", this->status5_value_sensor_);
}

void PaceBmsSensor::analog_information_callback_v25(PaceBmsProtocolV25::AnalogInformation& analog_information) {
	if (this->cell_count_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.cellCount]() { this->cell_count_sensor_->publish_state(value); });
	}
	for (int i = 0; i < 16; i++) {
		if (this->cell_voltage_sensor_[i] != nullptr) {
			this->parent_->queue_sensor_update([this, i, value = analog_information.cellVoltagesMillivolts[i] / 1000.0f]() { this->cell_voltage_sensor_[i]->publish_state(value); });
		}
	}
	if (this->temperature_count_sensor_ != nullptr) {
		this->temperature_count_sensor_->publish_state(analog_information.temperatureCount);
	}
	for (int i = 0; i < 6; i++) {
		if (this->temperature_sensor_[i] != nullptr) {
			this->parent_->queue_sensor_update([this, i, value = analog_information.temperaturesTenthsCelcius[i] / 10.0f]() { this->temperature_sensor_[i]->publish_state(value); });
		}
	}
	if (this->current_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.currentMilliamps / 1000.0f]() { this->current_sensor_->publish_state(value); });
	}
	if (this->total_voltage_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.totalVoltageMillivolts / 1000.0f]() { this->total_voltage_sensor_->publish_state(value); });
	}
	if (this->remaining_capacity_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.remainingCapacityMilliampHours / 1000.0f]() { this->remaining_capacity_sensor_->publish_state(value); });
	}
	if (this->full_capacity_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.fullCapacityMilliampHours / 1000.0f]() { this->full_capacity_sensor_->publish_state(value); });
	}
	if (this->design_capacity_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.designCapacityMilliampHours / 1000.0f]() { this->design_capacity_sensor_->publish_state(value); });
	}
	if (this->cycle_count_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.cycleCount]() { this->cycle_count_sensor_->publish_state(value); });
	}
	if (this->state_of_charge_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.SoC]() { this->state_of_charge_sensor_->publish_state(value); });
	}
	if (this->state_of_health_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.SoH]() { this->state_of_health_sensor_->publish_state(value); });
	}
	if (this->power_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.powerWatts]() { this->power_sensor_->publish_state(value); });
	}
	if (this->min_cell_voltage_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.minCellVoltageMillivolts / 1000.0f]() { this->min_cell_voltage_sensor_->publish_state(value); });
	}
	if (this->max_cell_voltage_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.maxCellVoltageMillivolts / 1000.0f]() { this->max_cell_voltage_sensor_->publish_state(value); });
	}
	if (this->avg_cell_voltage_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.avgCellVoltageMillivolts / 1000.0f]() { this->avg_cell_voltage_sensor_->publish_state(value); });
	}
	if (this->max_cell_differential_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.maxCellDifferentialMillivolts / 1000.0f]() { this->max_cell_differential_sensor_->publish_state(value); });
	}
}

void PaceBmsSensor::status_information_callback_v25(PaceBmsProtocolV25::StatusInformation& status_information) {
	for (int i = 0; i < 16; i++) {
		if (this->warning_status_value_cells_sensor_[i] != nullptr) {
			this->parent_->queue_sensor_update([this, i, value = status_information.warning_value_cell[i]]() { this->warning_status_value_cells_sensor_[i]->publish_state(value); });
		}
	}
	for (int i = 0; i < 6; i++) {
		if (this->warning_status_value_temps_sensor_[i] != nullptr) {
			this->parent_->queue_sensor_update([this, i, value = status_information.warning_value_temp[i]]() { this->warning_status_value_temps_sensor_[i]->publish_state(value); });
		}
	}
	if (this->warning_status_value_charge_current_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.warning_value_charge_current]() { this->warning_status_value_charge_current_sensor_->publish_state(value); });
	}
	if (this->warning_status_value_total_voltage_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.warning_value_total_voltage]() { this->warning_status_value_total_voltage_sensor_->publish_state(value); });
	}
	if (this->warning_status_value_discharge_current_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.warning_value_discharge_current]() { this->warning_status_value_discharge_current_sensor_->publish_state(value); });
	}
	if (this->warning_status_value_1_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.warning_value1]() { this->warning_status_value_1_sensor_->publish_state(value); });
	}
	if (this->warning_status_value_2_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.warning_value2]() { this->warning_status_value_2_sensor_->publish_state(value); });
	}
	if (this->balancing_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.balancing_value]() { this->balancing_status_value_sensor_->publish_state(value); });
	}
	if (this->system_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.system_value]() { this->system_status_value_sensor_->publish_state(value); });
	}
	if (this->configuration_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.configuration_value]() { this->configuration_status_value_sensor_->publish_state(value); });
	}
	if (this->protection_status_value_1_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.protection_value1]() { this->protection_status_value_1_sensor_->publish_state(value); });
	}
	if (this->protection_status_value_2_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.protection_value2]() { this->protection_status_value_2_sensor_->publish_state(value); });
	}
	if (this->fault_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.fault_value]() { this->fault_status_value_sensor_->publish_state(value); });
	}
}

void PaceBmsSensor::analog_information_callback_v20(PaceBmsProtocolV20::AnalogInformation& analog_information) {
	if (this->cell_count_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.cellCount]() { this->cell_count_sensor_->publish_state(value); });
	}
	for (int i = 0; i < 16; i++) {
		if (this->cell_voltage_sensor_[i] != nullptr) {
			this->parent_->queue_sensor_update([this, i, value = analog_information.cellVoltagesMillivolts[i] / 1000.0f]() { this->cell_voltage_sensor_[i]->publish_state(value); });
		}
	}
	if (this->temperature_count_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.temperatureCount]() { this->temperature_count_sensor_->publish_state(value); });
	}
	for (int i = 0; i < 6; i++) {
		if (this->temperature_sensor_[i] != nullptr) {
			this->parent_->queue_sensor_update([this, i, value = analog_information.temperaturesTenthsCelcius[i] / 10.0f]() { this->temperature_sensor_[i]->publish_state(value); });
		}
	}
	if (this->current_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.currentMilliamps / 1000.0f]() { this->current_sensor_->publish_state(value); });
	}
	if (this->total_voltage_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.totalVoltageMillivolts / 1000.0f]() { this->total_voltage_sensor_->publish_state(value); });
	}
	if (this->remaining_capacity_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.remainingCapacityMilliampHours / 1000.0f]() { this->remaining_capacity_sensor_->publish_state(value); });
	}
	if (this->full_capacity_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.fullCapacityMilliampHours / 1000.0f]() { this->full_capacity_sensor_->publish_state(value); });
	}
	if (this->design_capacity_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.designCapacityMilliampHours / 1000.0f]() { this->design_capacity_sensor_->publish_state(value); });
	}
	if (this->cycle_count_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.cycleCount]() { this->cycle_count_sensor_->publish_state(value); });
	}
	if (this->state_of_charge_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.SoC]() { this->state_of_charge_sensor_->publish_state(value); });
	}
	if (this->state_of_health_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.SoH]() { this->state_of_health_sensor_->publish_state(value); });
	}
	if (this->power_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.powerWatts]() { this->power_sensor_->publish_state(value); });
	}
	if (this->min_cell_voltage_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.minCellVoltageMillivolts / 1000.0f]() { this->min_cell_voltage_sensor_->publish_state(value); });
	}
	if (this->max_cell_voltage_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.maxCellVoltageMillivolts / 1000.0f]() { this->max_cell_voltage_sensor_->publish_state(value); });
	}
	if (this->avg_cell_voltage_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.avgCellVoltageMillivolts / 1000.0f]() { this->avg_cell_voltage_sensor_->publish_state(value); });
	}
	if (this->max_cell_differential_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = analog_information.maxCellDifferentialMillivolts / 1000.0f]() { this->max_cell_differential_sensor_->publish_state(value); });
	}
}

void PaceBmsSensor::status_information_callback_v20(PaceBmsProtocolV20::StatusInformation& status_information) {
	for (int i = 0; i < 16; i++) {
		if (this->warning_status_value_cells_sensor_[i] != nullptr) {
			this->parent_->queue_sensor_update([this, i, value = status_information.warning_value_cell[i]]() { this->warning_status_value_cells_sensor_[i]->publish_state(value); });
		}
	}
	for (int i = 0; i < 6; i++) {
		if (this->warning_status_value_temps_sensor_[i] != nullptr) {
			this->parent_->queue_sensor_update([this, i, value = status_information.warning_value_temp[i]]() { this->warning_status_value_temps_sensor_[i]->publish_state(value); });
		}
	}
	if (this->warning_status_value_charge_current_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.warning_value_charge_current]() { this->warning_status_value_charge_current_sensor_->publish_state(value); });
	}
	if (this->warning_status_value_total_voltage_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.warning_value_total_voltage]() { this->warning_status_value_total_voltage_sensor_->publish_state(value); });
	}
	if (this->warning_status_value_discharge_current_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.warning_value_discharge_current]() { this->warning_status_value_discharge_current_sensor_->publish_state(value); });
	}

	if (this->balancing_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.balancing_value]() { this->balancing_status_value_sensor_->publish_state(value); });
	}
	if (this->system_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.system_value]() { this->system_status_value_sensor_->publish_state(value); });
	}

	// PYLON
	if (this->status1_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.status1_value]() { this->status1_value_sensor_->publish_state(value); });
	}
	if (this->status2_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.status2_value]() { this->status2_value_sensor_->publish_state(value); });
	}
	if (this->status3_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.status3_value]() { this->status3_value_sensor_->publish_state(value); });
	}
	if (this->status4_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.status4_value]() { this->status4_value_sensor_->publish_state(value); });
	}
	if (this->status5_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.status5_value]() { this->status5_value_sensor_->publish_state(value); });
	}

	// SEPLOS
	if (this->warning1_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.warning1_value]() { this->warning1_status_value_sensor_->publish_state(value); });
	}
	if (this->warning2_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.warning2_value]() { this->warning2_status_value_sensor_->publish_state(value); });
	}
	if (this->warning3_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.warning3_value]() { this->warning3_status_value_sensor_->publish_state(value); });
	}
	if (this->warning4_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.warning4_value]() { this->warning4_status_value_sensor_->publish_state(value); });
	}
	if (this->warning5_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.warning5_value]() { this->warning5_status_value_sensor_->publish_state(value); });
	}
	if (this->warning6_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.warning6_value]() { this->warning6_status_value_sensor_->publish_state(value); });
	}
	if (this->power_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.power_value]() { this->power_status_value_sensor_->publish_state(value); });
	}
	if (this->disconnection_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.disconnection_value]() { this->disconnection_status_value_sensor_->publish_state(value); });
	}
	if (this->warning7_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.warning7_value]() { this->warning7_status_value_sensor_->publish_state(value); });
	}
	if (this->warning8_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.warning8_value]() { this->warning8_status_value_sensor_->publish_state(value); });
	}

	// EG4
	if (this->balance_event_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.balance_event_value]() { this->balance_event_value_sensor_->publish_state(value); });
	}
	if (this->voltage_event_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.voltage_event_value]() { this->voltage_event_value_sensor_->publish_state(value); });
	}
	if (this->temperature_event_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.temperature_event_value]() { this->temperature_event_value_sensor_->publish_state(value); });
	}
	if (this->current_event_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.current_event_value]() { this->current_event_value_sensor_->publish_state(value); });
	}
	if (this->remaining_capacity_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.remaining_capacity_value]() { this->remaining_capacity_value_sensor_->publish_state(value); });
	}
	if (this->fet_status_value_sensor_ != nullptr) {
		this->parent_->queue_sensor_update([this, value = status_information.fet_status_value]() { this->fet_status_value_sensor_->publish_state(value); });
	}
}

}  // namespace pace_bms
}  // namespace esphome
