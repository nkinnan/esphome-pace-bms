#pragma once

#include "esphome/core/component.h"
#include "esphome/components/pace_bms/pace_bms.h"
#include "esphome/components/text_sensor/text_sensor.h"

#include "../pace_bms_v25.h"

namespace esphome {
namespace pace_bms {

class PaceBmsTextSensor : public Component {
 public:
  void set_parent(PaceBms *parent) { parent_ = parent; }
  void set_warning_status_sensor(text_sensor::TextSensor* warning_status_sensor) { warning_status_sensor_ = warning_status_sensor; request_status_info_callback_ = true; }
  void set_balancing_status_sensor(text_sensor::TextSensor* balancing_status_sensor) { balancing_status_sensor_ = balancing_status_sensor; request_status_info_callback_ = true; }
  void set_system_status_sensor(text_sensor::TextSensor* system_status_sensor) { system_status_sensor_ = system_status_sensor; request_status_info_callback_ = true; }
  void set_configuration_status_sensor(text_sensor::TextSensor* configuration_status_sensor) { configuration_status_sensor_ = configuration_status_sensor; request_status_info_callback_ = true; }
  void set_protection_status_sensor(text_sensor::TextSensor* protection_status_sensor) { protection_status_sensor_ = protection_status_sensor; request_status_info_callback_ = true; }
  void set_fault_status_sensor(text_sensor::TextSensor* fault_status_sensor) { fault_status_sensor_ = fault_status_sensor; request_status_info_callback_ = true; }

  void set_hardware_version_sensor(text_sensor::TextSensor* hardware_version_sensor) { hardware_version_sensor_ = hardware_version_sensor; request_hardware_version_callback_ = true; }
  void set_serial_number_sensor(text_sensor::TextSensor* serial_number_sensor) { serial_number_sensor_ = serial_number_sensor; request_serial_number_callback_ = true; }

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;

  void status_information_callback(PaceBmsV25::StatusInformation& status_information);
  void hardware_version_callback(std::string& hardware_version);
  void serial_number_callback(std::string &serial_number);

 protected:
  pace_bms::PaceBms *parent_;
  text_sensor::TextSensor* warning_status_sensor_{ nullptr };
  text_sensor::TextSensor* balancing_status_sensor_{ nullptr };
  text_sensor::TextSensor* system_status_sensor_{ nullptr };
  text_sensor::TextSensor* configuration_status_sensor_{ nullptr };
  text_sensor::TextSensor* protection_status_sensor_{ nullptr };
  text_sensor::TextSensor* fault_status_sensor_{ nullptr };

  text_sensor::TextSensor* hardware_version_sensor_{ nullptr };
  text_sensor::TextSensor* serial_number_sensor_{ nullptr };

  bool request_status_info_callback_ = false;
  bool request_hardware_version_callback_ = false;
  bool request_serial_number_callback_ = false;
};

}  // namespace pace_bms
}  // namespace esphome