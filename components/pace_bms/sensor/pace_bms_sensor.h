#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include "esphome/components/pace_bms/pace_bms_component.h"

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

	void set_status1_value_sensor(sensor::Sensor* sens) { status1_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_status2_value_sensor(sensor::Sensor* sens) { status2_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_status3_value_sensor(sensor::Sensor* sens) { status3_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_status4_value_sensor(sensor::Sensor* sens) { status4_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_status5_value_sensor(sensor::Sensor* sens) { status5_value_sensor_ = sens;                     request_status_info_callback_ = true; }

	void set_warning1_status_value_sensor(sensor::Sensor* sens) { warning1_status_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_warning2_status_value_sensor(sensor::Sensor* sens) { warning2_status_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_warning3_status_value_sensor(sensor::Sensor* sens) { warning3_status_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_warning4_status_value_sensor(sensor::Sensor* sens) { warning4_status_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_warning5_status_value_sensor(sensor::Sensor* sens) { warning5_status_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_warning6_status_value_sensor(sensor::Sensor* sens) { warning6_status_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_power_status_value_sensor(sensor::Sensor* sens) { power_status_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_disconnection_status_value_sensor(sensor::Sensor* sens) { disconnection_status_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_warning7_status_value_sensor(sensor::Sensor* sens) { warning7_status_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_warning8_status_value_sensor(sensor::Sensor* sens) { warning8_status_value_sensor_ = sens;                     request_status_info_callback_ = true; }

	void set_balance_event_value_sensor(sensor::Sensor* sens)      { balance_event_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_voltage_event_value_sensor(sensor::Sensor* sens)      { voltage_event_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_temperature_event_value_sensor(sensor::Sensor* sens)  { temperature_event_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_current_event_value_sensor(sensor::Sensor* sens)      { current_event_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_remaining_capacity_value_sensor(sensor::Sensor* sens) { remaining_capacity_value_sensor_ = sens;                     request_status_info_callback_ = true; }
	void set_fet_status_value_sensor(sensor::Sensor* sens)         { fet_status_value_sensor_ = sens;                     request_status_info_callback_ = true; }

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

	sensor::Sensor* status1_value_sensor_{ nullptr };
	sensor::Sensor* status2_value_sensor_{ nullptr };
	sensor::Sensor* status3_value_sensor_{ nullptr };
	sensor::Sensor* status4_value_sensor_{ nullptr };
	sensor::Sensor* status5_value_sensor_{ nullptr };

	sensor::Sensor* warning1_status_value_sensor_{ nullptr };
	sensor::Sensor* warning2_status_value_sensor_{ nullptr };
	sensor::Sensor* warning3_status_value_sensor_{ nullptr };
	sensor::Sensor* warning4_status_value_sensor_{ nullptr };
	sensor::Sensor* warning5_status_value_sensor_{ nullptr };
	sensor::Sensor* warning6_status_value_sensor_{ nullptr };
	sensor::Sensor* power_status_value_sensor_{ nullptr };
	sensor::Sensor* disconnection_status_value_sensor_{ nullptr };
	sensor::Sensor* warning7_status_value_sensor_{ nullptr };
	sensor::Sensor* warning8_status_value_sensor_{ nullptr };

	sensor::Sensor* balance_event_value_sensor_{ nullptr };
	sensor::Sensor* voltage_event_value_sensor_{ nullptr };
	sensor::Sensor* temperature_event_value_sensor_{ nullptr };
	sensor::Sensor* current_event_value_sensor_{ nullptr };
	sensor::Sensor* remaining_capacity_value_sensor_{ nullptr };
	sensor::Sensor* fet_status_value_sensor_{ nullptr };

	bool request_analog_info_callback_ = false;
	bool request_status_info_callback_ = false;

	void analog_information_callback_v25(PaceBmsProtocolV25::AnalogInformation& analog_information);
	void status_information_callback_v25(PaceBmsProtocolV25::StatusInformation& status_information);

	void analog_information_callback_v20(PaceBmsProtocolV20::AnalogInformation& analog_information);
	void status_information_callback_v20(PaceBmsProtocolV20::StatusInformation& status_information);
};

}  // namespace pace_bms
}  // namespace esphome
