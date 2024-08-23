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
  void set_cell_count_sensor(sensor::Sensor* voltage_sensor) { cell_count_sensor_ = voltage_sensor; request_analog_info_callback_ = true; }


  void set_total_voltage_sensor(sensor::Sensor* voltage_sensor) { voltage_sensor_ = voltage_sensor; request_analog_info_callback_ = true; }

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;

  void analog_information_callback(PaceBmsV25::AnalogInformation analog_information);

 protected:
  pace_bms::PaceBms *parent_;
  sensor::Sensor* cell_count_sensor_{ nullptr };



  sensor::Sensor* voltage_sensor_{ nullptr };

  bool request_analog_info_callback_ = false;
};

}  // namespace pace_bms
}  // namespace esphome
