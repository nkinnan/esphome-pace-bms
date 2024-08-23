#pragma once

#include "esphome/core/component.h"
#include "esphome/components/pace_bms/pace_bms.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace pace_bms {

class PaceBmsSensor : public Component {
 public:
  void set_parent(PaceBms *parent) { parent_ = parent; }
  void set_voltage_sensor(sensor::Sensor* voltage_sensor) { voltage_sensor_ = voltage_sensor; }

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;

 protected:
  pace_bms::PaceBms *parent_;
  sensor::Sensor* voltage_sensor_{ nullptr };
};

}  // namespace pace_bms
}  // namespace esphome
