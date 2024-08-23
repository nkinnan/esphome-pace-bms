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
  void set_cell_count_sensor(sensor::Sensor* cell_count_sensor) { cell_count_sensor_ = cell_count_sensor; request_analog_info_callback_ = true; }
  void set_cell_voltage_sensor(int index, sensor::Sensor* cell_voltage_sensor) { cell_voltage_sensor_[index] = cell_voltage_sensor; request_analog_info_callback_ = true; }
  void set_temperature_count_sensor(sensor::Sensor* temperature_count_sensor) { temperature_count_sensor_ = temperature_count_sensor; request_analog_info_callback_ = true; }
  void set_temperature_sensor(int index, sensor::Sensor* temperature_sensor) { temperature_sensor_[index] = temperature_sensor; request_analog_info_callback_ = true; }
  void set_current_sensor(int index, sensor::Sensor* temperature_sensor) { set_current_sensor_ = set_current_sensor; request_analog_info_callback_ = true; }


  void set_total_voltage_sensor(sensor::Sensor* total_voltage_sensor) { total_voltage_sensor_ = total_voltage_sensor; request_analog_info_callback_ = true; }

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;

  void analog_information_callback(PaceBmsV25::AnalogInformation analog_information);

 protected:
  pace_bms::PaceBms *parent_;
  sensor::Sensor* cell_count_sensor_{ nullptr };
  sensor::Sensor* cell_voltage_sensor_[16]{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
  sensor::Sensor* temperature_count_sensor_{ nullptr };
  sensor::Sensor* temperature_sensor_[6]{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
  sensor::Sensor* current_sensor_{ nullptr };
  sensor::Sensor* total_voltage_sensor_{ nullptr };

  bool request_analog_info_callback_ = false;
};

}  // namespace pace_bms
}  // namespace esphome
