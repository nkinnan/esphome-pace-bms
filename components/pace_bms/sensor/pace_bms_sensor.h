#pragma once

#include "esphome/core/component.h"
#include "esphome/components/pace_bms/pace_bms.h"
#include "esphome/components/sensor/sensor.h"

#include "../pace_bms_v25.h"

namespace esphome {
namespace pace_bms {

class PaceBmsSensor : public Component {
 public:
  void set_parent(PaceBms *parent) { parent_ = parent; }

  // analog info
  void set_cell_count_sensor(sensor::Sensor* cell_count_sensor) { cell_count_sensor_ = cell_count_sensor; request_analog_info_callback_ = true; }
  void set_cell_voltage_sensor(int index, sensor::Sensor* cell_voltage_sensor) { cell_voltage_sensor_[index] = cell_voltage_sensor; request_analog_info_callback_ = true; }
  void set_temperature_count_sensor(sensor::Sensor* temperature_count_sensor) { temperature_count_sensor_ = temperature_count_sensor; request_analog_info_callback_ = true; }
  void set_temperature_sensor(int index, sensor::Sensor* temperature_sensor) { temperature_sensor_[index] = temperature_sensor; request_analog_info_callback_ = true; }
  void set_current_sensor(sensor::Sensor* current_sensor) { current_sensor_ = current_sensor; request_analog_info_callback_ = true; }
  void set_total_voltage_sensor(sensor::Sensor* total_voltage_sensor) { total_voltage_sensor_ = total_voltage_sensor; request_analog_info_callback_ = true; }
  void set_remaining_capacity_sensor(sensor::Sensor* remaining_capacity_sensor) { remaining_capacity_sensor_ = remaining_capacity_sensor; request_analog_info_callback_ = true; }
  void set_full_capacity_sensor(sensor::Sensor* full_capacity_sensor) { full_capacity_sensor_ = full_capacity_sensor; request_analog_info_callback_ = true; }
  void set_design_capacity_sensor(sensor::Sensor* design_capacity_sensor) { design_capacity_sensor_ = design_capacity_sensor; request_analog_info_callback_ = true; }
  void set_cycle_count_sensor(sensor::Sensor* cycle_count_sensor) { cycle_count_sensor_ = cycle_count_sensor; request_analog_info_callback_ = true; }
  void set_state_of_charge_sensor(sensor::Sensor* state_of_charge_sensor) { state_of_charge_sensor_ = state_of_charge_sensor; request_analog_info_callback_ = true; }
  void set_state_of_health_sensor(sensor::Sensor* state_of_health_sensor) { state_of_health_sensor_ = state_of_health_sensor; request_analog_info_callback_ = true; }
  void set_power_sensor(sensor::Sensor* power_sensor) { power_sensor_ = power_sensor; request_analog_info_callback_ = true; }
  void set_min_cell_voltage_sensor(sensor::Sensor* min_cell_voltage_sensor) { min_cell_voltage_sensor_ = min_cell_voltage_sensor; request_analog_info_callback_ = true; }
  void set_max_cell_voltage_sensor(sensor::Sensor* max_cell_voltage_sensor) { max_cell_voltage_sensor_ = max_cell_voltage_sensor; request_analog_info_callback_ = true; }
  void set_avg_cell_voltage_sensor(sensor::Sensor* avg_cell_voltage_sensor) { avg_cell_voltage_sensor_ = avg_cell_voltage_sensor; request_analog_info_callback_ = true; }
  void set_max_cell_differential_sensor(sensor::Sensor* max_cell_differential_sensor) { max_cell_differential_sensor_ = max_cell_differential_sensor; request_analog_info_callback_ = true; }

  // status info
  void set_warning_status_value_cells_sensor(int index, sensor::Sensor* warning_status_value_cells_sensor) { warning_status_value_cells_sensor_[index] = warning_status_value_cells_sensor; request_status_info_callback_ = true; }
  void set_warning_status_value_temps_sensor(int index, sensor::Sensor* warning_status_value_temps_sensor) { warning_status_value_temps_sensor_[index] = warning_status_value_temps_sensor; request_status_info_callback_ = true; }
  void set_warning_status_value_charge_current_sensor(sensor::Sensor* warning_status_value_charge_current_sensor) { warning_status_value_charge_current_sensor_ = warning_status_value_charge_current_sensor; request_status_info_callback_ = true; }
  void set_warning_status_value_total_voltage_sensor(sensor::Sensor* warning_status_value_total_voltage_sensor) { warning_status_value_total_voltage_sensor_ = warning_status_value_total_voltage_sensor; request_status_info_callback_ = true; }
  void set_warning_status_value_discharge_current_sensor(sensor::Sensor* warning_status_value_discharge_current_sensor) { warning_status_value_discharge_current_sensor_ = warning_status_value_discharge_current_sensor; request_status_info_callback_ = true; }
  void set_warning_status_value_1_sensor(sensor::Sensor* warning_status_value_1_sensor) { warning_status_value_1_sensor_ = warning_status_value_1_sensor; request_status_info_callback_ = true; }
  void set_warning_status_value_2_sensor(sensor::Sensor* warning_status_value_2_sensor) { warning_status_value_2_sensor_ = warning_status_value_2_sensor; request_status_info_callback_ = true; }
  void set_balancing_status_value_sensor(sensor::Sensor* balancing_status_value_sensor) { balancing_status_value_sensor_ = balancing_status_value_sensor; request_status_info_callback_ = true; }
  void set_system_status_value_sensor(sensor::Sensor* system_status_value_sensor) { system_status_value_sensor_ = system_status_value_sensor; request_status_info_callback_ = true; }
  void set_configuration_status_value_sensor(sensor::Sensor* configuration_status_value_sensor) { configuration_status_value_sensor_ = configuration_status_value_sensor; request_status_info_callback_ = true; }
  void set_protection_status_value_1_sensor(sensor::Sensor* protection_status_value_1_sensor) { protection_status_value_1_sensor_ = protection_status_value_1_sensor; request_status_info_callback_ = true; }
  void set_protection_status_value_2_sensor(sensor::Sensor* protection_status_value_2_sensor) { protection_status_value_2_sensor_ = protection_status_value_2_sensor; request_status_info_callback_ = true; }
  void set_fault_status_value_sensor(sensor::Sensor* fault_status_value_sensor) { fault_status_value_sensor_ = fault_status_value_sensor; request_status_info_callback_ = true; }

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;

  void analog_information_callback(PaceBmsV25::AnalogInformation& analog_information);
  void status_information_callback(PaceBmsV25::StatusInformation& analog_information);

 protected:
  pace_bms::PaceBms *parent_;

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
};

}  // namespace pace_bms
}  // namespace esphome
