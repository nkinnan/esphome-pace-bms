#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

#include "pace_bms_v25.h"

namespace esphome {
namespace pace_bms {

class PaceBms : public PollingComponent, public uart::UARTDevice {
 public:
  void set_flow_control_pin(GPIOPin* flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }
  void set_address(int address) { this->address_ = address; }

  void setup() override;
  void update() override;
  void loop() override;
  float get_setup_priority() const override;
  void dump_config() override;

 protected:
  GPIOPin* flow_control_pin_{ nullptr };
  int address_{ 0 };

  void parse_data_frame_(uint8_t* frame_bytes, uint8_t frame_length);

  PaceBmsV25* pace_bms_v25_;
  static const uint8_t max_data_len_ = 150;
  uint8_t raw_data_[max_data_len_];
  uint8_t raw_data_index_{0};
  uint32_t last_transmission_{0};
};
 
}  // namespace pace_bms
}  // namespace esphome
