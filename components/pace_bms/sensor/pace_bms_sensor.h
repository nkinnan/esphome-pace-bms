#pragma once

#include "esphome/core/component.h"
#include "esphome/components/pace_bms/pace_bms.h"
#include "esphome/components/sensor/sensor.h"

#include "../pace_bms_v25.h"
#include "../pace_bms_v20.h"

namespace esphome {
namespace pace_bms {

class PaceBmsSensor : public Component {
public:
	void set_parent(PaceBms* parent) { parent_ = parent; }

	// analog info
	void set_cell_count_sensor(sensor::Sensor* sens) { cell_count_sensor_ = sens;            request_analog_info_callback_ = true; }
	void set_cell_voltage_sensor(int index, sensor::Sensor* sens) { cell_voltage_sensor_[index] = sens;   request_analog_info_callback_ = true; }
	void set_temperature_count_sensor(sensor::Sensor* sens) { temperature_count_sensor_ = sens;     request_analog_info_callback_ = true; }
	void set_temperature_sensor(int index, sensor::Sensor* sens) { temperature_sensor_[index] = sens;    request_analog_info_callback_ = true; }
	void set_current_sensor(sensor::Sensor* sens) { current_sensor_ = sens;               request_analog_info_callback_ = true; }
	void set_total_voltage_sensor(sensor::Sensor* sens) { total_voltage_sensor_ = sens;         request_analog_info_callback_ = true; }
	void set_remaining_capacity_sensor(sensor::Sensor* sens) { remaining_capacity_sensor_ = sens;    request_analog_info_callback_ = true; }
	void set_full_capacity_sensor(sensor::Sensor* sens) { full_capacity_sensor_ = sens;         request_analog_info_callback_ = true; }
	void set_design_capacity_sensor(sensor::Sensor* sens) { design_capacity_sensor_ = sens;       request_analog_info_callback_ = true; }
	void set_cycle_count_sensor(sensor::Sensor* sens) { cycle_count_sensor_ = sens;           request_analog_info_callback_ = true; }
	void set_state_of_charge_sensor(sensor::Sensor* sens) { state_of_charge_sensor_ = sens;       request_analog_info_callback_ = true; }
	void set_state_of_health_sensor(sensor::Sensor* sens) { state_of_health_sensor_ = sens;       request_analog_info_callback_ = true; }
	void set_power_sensor(sensor::Sensor* sens) { power_sensor_ = sens;                 request_analog_info_callback_ = true; }
	void set_min_cell_voltage_sensor(sensor::Sensor* sens) { min_cell_voltage_sensor_ = sens;      request_analog_info_callback_ = true; }
	void set_max_cell_voltage_sensor(sensor::Sensor* sens) { max_cell_voltage_sensor_ = sens;      request_analog_info_callback_ = true; }
	void set_avg_cell_voltage_sensor(sensor::Sensor* sens) { avg_cell_voltage_sensor_ = sens;      request_analog_info_callback_ = true; }
	void set_max_cell_differential_sensor(sensor::Sensor* sens) { max_cell_differential_sensor_ = sens; request_analog_info_callback_ = true; }

	// status info
	void set_warning_status_value_cells_sensor(int index, sensor::Sensor* sens) { warning_status_value_cells_sensor_[index] = sens;      request_status_info_callback_ = true; }
	void set_warning_status_value_temps_sensor(int index, sensor::Sensor* sens) { warning_status_value_temps_sensor_[index] = sens;      request_status_info_callback_ = true; }
	void set_warning_status_value_charge_current_sensor(sensor::Sensor* sens) { warning_status_value_charge_current_sensor_ = sens;    request_status_info_callback_ = true; }
	void set_warning_status_value_total_voltage_sensor(sensor::Sensor* sens) { warning_status_value_total_voltage_sensor_ = sens;     request_status_info_callback_ = true; }
	void set_warning_status_value_discharge_current_sensor(sensor::Sensor* sens) { warning_status_value_discharge_current_sensor_ = sens; request_status_info_callback_ = true; }
	void set_warning_status_value_1_sensor(sensor::Sensor* sens) { warning_status_value_1_sensor_ = sens;                 request_status_info_callback_ = true; }
	void set_warning_status_value_2_sensor(sensor::Sensor* sens) { warning_status_value_2_sensor_ = sens;                 request_status_info_callback_ = true; }
	void set_balancing_status_value_sensor(sensor::Sensor* sens) { balancing_status_value_sensor_ = sens;                 request_status_info_callback_ = true; }
	void set_system_status_value_sensor(sensor::Sensor* sens) { system_status_value_sensor_ = sens;                    request_status_info_callback_ = true; }
	void set_configuration_status_value_sensor(sensor::Sensor* sens) { configuration_status_value_sensor_ = sens;             request_status_info_callback_ = true; }
	void set_protection_status_value_1_sensor(sensor::Sensor* sens) { protection_status_value_1_sensor_ = sens;              request_status_info_callback_ = true; }
	void set_protection_status_value_2_sensor(sensor::Sensor* sens) { protection_status_value_2_sensor_ = sens;              request_status_info_callback_ = true; }
	void set_fault_status_value_sensor(sensor::Sensor* sens) { fault_status_value_sensor_ = sens;                     request_status_info_callback_ = true; }

	void setup() override;
	float get_setup_priority() const override { return setup_priority::DATA; };
	void dump_config() override;

protected:
	pace_bms::PaceBms* parent_;

	// analog info
	sensor::Sensor* cell_count_sensor_{ nullptr };
	sensor::Sensor* cell_voltage_sensor_[16]{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	sensor::Sensor* temperature_count_sensor_{ nullptr };
	sensor::Sensor* temperature_sensor_[6]{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	sensor::Sensor* current_sensor_{ nullptr };
	sensor::Sensor* total_voltage_sensor_{ nullptr };
	sensor::Sensor* remaining_capacity_sensor_{ nullptr };
	sensor::Sensor* full_capacity_sensor_{ nullptr };
	sensor::Sensor* design_capacity_sensor_{ nullptr };
	sensor::Sensor* cycle_count_sensor_{ nullptr };
	sensor::Sensor* state_of_charge_sensor_{ nullptr };
	sensor::Sensor* state_of_health_sensor_{ nullptr };
	sensor::Sensor* power_sensor_{ nullptr };
	sensor::Sensor* min_cell_voltage_sensor_{ nullptr };
	sensor::Sensor* max_cell_voltage_sensor_{ nullptr };
	sensor::Sensor* avg_cell_voltage_sensor_{ nullptr };
	sensor::Sensor* max_cell_differential_sensor_{ nullptr };

	// status info
	sensor::Sensor* warning_status_value_cells_sensor_[16]{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	sensor::Sensor* warning_status_value_temps_sensor_[6]{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	sensor::Sensor* warning_status_value_charge_current_sensor_{ nullptr };
	sensor::Sensor* warning_status_value_total_voltage_sensor_{ nullptr };
	sensor::Sensor* warning_status_value_discharge_current_sensor_{ nullptr };
	sensor::Sensor* warning_status_value_1_sensor_{ nullptr };
	sensor::Sensor* warning_status_value_2_sensor_{ nullptr };
	sensor::Sensor* balancing_status_value_sensor_{ nullptr };
	sensor::Sensor* system_status_value_sensor_{ nullptr };
	sensor::Sensor* configuration_status_value_sensor_{ nullptr };
	sensor::Sensor* protection_status_value_1_sensor_{ nullptr };
	sensor::Sensor* protection_status_value_2_sensor_{ nullptr };
	sensor::Sensor* fault_status_value_sensor_{ nullptr };

	bool request_analog_info_callback_ = false;
	bool request_status_info_callback_ = false;

	void analog_information_callback_v25(PaceBmsV25::AnalogInformation& analog_information);
	void status_information_callback_v25(PaceBmsV25::StatusInformation& status_information);

	void analog_information_callback_v20(PaceBmsV20::AnalogInformation& analog_information);
	//void status_information_callback_v25(PaceBmsV25::StatusInformation& status_information);
};

}  // namespace pace_bms
}  // namespace esphome
