#pragma once

#include "esphome/core/component.h"
#include "esphome/components/pace_bms/pace_bms.h"
#include "esphome/components/sensor/sensor.h"

#include "../pace_bms_v25.h"

namespace esphome {
namespace pace_bms {

class PaceBmsTextSensor : public Component {
 public:
  void set_parent(PaceBms *parent) { parent_ = parent; }
  void set_warning_status_sensor(sensor::Sensor* warning_status_sensor) { warning_status_sensor_ = warning_status_sensor; request_status_info_callback_ = true; }
  void set_balancing_status_sensor(sensor::Sensor* balancing_status_sensor) { balancing_status_sensor_ = balancing_status_sensor; request_status_info_callback_ = true; }
  void set_system_status_sensor(sensor::Sensor* system_status_sensor) { system_status_sensor_ = system_status_sensor; request_status_info_callback_ = true; }
  void set_configuration_status_sensor(sensor::Sensor* configuration_status_sensor) { configuration_status_sensor_ = configuration_status_sensor; request_status_info_callback_ = true; }
  void set_protection_status_sensor(sensor::Sensor* protection_status_sensor) { protection_status_sensor_ = protection_status_sensor; request_status_info_callback_ = true; }
  void set_fault_status_sensor(sensor::Sensor* fault_status_sensor) { fault_status_sensor_ = fault_status_sensor; request_status_info_callback_ = true; }

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;

  void status_information_callback(PaceBmsV25::StatusInformation& analog_information);

 protected:
  pace_bms::PaceBms *parent_;
  sensor::Sensor* warning_status_sensor_{ nullptr };
  sensor::Sensor* balancing_status_sensor_{ nullptr };
  sensor::Sensor* system_status_sensor_{ nullptr };
  sensor::Sensor* configuration_status_sensor_{ nullptr };
  sensor::Sensor* protection_status_sensor_{ nullptr };
  sensor::Sensor* fault_status_sensor_{ nullptr };

  bool request_status_info_callback_ = false;
};

}  // namespace pace_bms
}  // namespace esphome
