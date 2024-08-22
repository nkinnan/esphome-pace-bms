#pragma once

#include "esphome/core/component.h"
#include "esphome/components/pace_bms/pace_bms.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace pace_bms {

enum SensorType {
  SUN_SENSOR_ELEVATION,
  SUN_SENSOR_AZIMUTH,
};

class PaceBmsSensor : 
    //public sensor::Sensor, 
    public PollingComponent {
 public:
  void set_parent(PaceBms *parent) { parent_ = parent; }
  //void set_type(SensorType type) { type_ = type; }
  void set_voltage_sensor(sensor::Sensor* voltage_sensor) { voltage_sensor_ = voltage_sensor; }

  void dump_config() override;
  void update() override {
    double val;
    //switch (this->type_) {
    //  case SUN_SENSOR_ELEVATION:
    //    val = this->parent_->elevation();
    //    break;
    //  case SUN_SENSOR_AZIMUTH:
    //    val = this->parent_->azimuth();
    //    break;
    //  default:
    //    return;
    //}
    //this->publish_state(val);
  }
  float get_setup_priority() const override;

 protected:
  pace_bms::PaceBms *parent_;
  //SensorType type_;
  sensor::Sensor* voltage_sensor_{ nullptr };
};

}  // namespace sun
}  // namespace esphome
