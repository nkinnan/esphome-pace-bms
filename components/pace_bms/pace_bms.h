#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
//#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

#include "pace_bms_v25.h"

namespace esphome {
namespace pace_bms {

class PaceBmsComponent : public PollingComponent, public uart::UARTDevice {
 public:
  void set_flow_control_pin(GPIOPin* flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }
  void set_voltage_sensor(sensor::Sensor *voltage_sensor) { voltage_sensor_ = voltage_sensor; }
  void set_current_sensor(sensor::Sensor *current_sensor) { current_sensor_ = current_sensor; }
  void set_power_sensor(sensor::Sensor *power_sensor) { power_sensor_ = power_sensor; }
  void set_energy_sensor(sensor::Sensor *energy_sensor) { energy_sensor_ = energy_sensor; }
  void set_apparent_power_sensor(sensor::Sensor *apparent_power_sensor) {
    apparent_power_sensor_ = apparent_power_sensor;
  }
  void set_reactive_power_sensor(sensor::Sensor *reactive_power_sensor) {
    reactive_power_sensor_ = reactive_power_sensor;
  }
  void set_power_factor_sensor(sensor::Sensor *power_factor_sensor) { power_factor_sensor_ = power_factor_sensor; }

  void setup() override;
  void update() override;
  void loop() override;
  float get_setup_priority() const override;
  void dump_config() override;

 protected:
  PaceBmsV25* pace_bms_v25;
  GPIOPin* flow_control_pin_{ nullptr };

  void parse_data_frame_(uint8_t* frame_bytes, uint8_t frame_length);

  static const uint8_t max_data_len_ = 150;
  uint8_t raw_data_[max_data_len_];
  uint8_t raw_data_index_{0};
  uint32_t last_transmission_{0};
  sensor::Sensor *voltage_sensor_{nullptr};
  sensor::Sensor *current_sensor_{nullptr};
  sensor::Sensor *power_sensor_{nullptr};
  sensor::Sensor *energy_sensor_{nullptr};
  sensor::Sensor *apparent_power_sensor_{nullptr};
  sensor::Sensor *reactive_power_sensor_{nullptr};
  sensor::Sensor *power_factor_sensor_{nullptr};
  uint32_t cf_pulses_total_{0};
  uint16_t cf_pulses_last_{0};
};
 
}  // namespace cse7766
}  // namespace esphome
