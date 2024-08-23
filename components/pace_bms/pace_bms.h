#pragma once

#include <vector>

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

#include "pace_bms_v25.h"
#include <functional>

namespace esphome {
namespace pace_bms {

// this class encapsulates an instance of PaceBmsV25 (which handles protocol version 0x25) and injects the logging dependency
// in the future, other protocol versions may be supported
class PaceBms : public PollingComponent, public uart::UARTDevice {
 public:
  void set_flow_control_pin(GPIOPin* flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }
  void set_address(int address) { this->address_ = address; }
  void set_protocol_version(int protocol_version) { this->protocol_version_ = protocol_version; }

  void setup() override;
  void update() override;
  void loop() override;
  float get_setup_priority() const override;
  void dump_config() override;

  void register_analog_information_callback(std::function<void(PaceBmsV25::AnalogInformation)> callback) { analog_information_callbacks_.push_back(callback); }

 protected:
  GPIOPin* flow_control_pin_{ nullptr };
  int address_{ 0 };
  int protocol_version_{ 0 };

  void parse_data_frame_(uint8_t* frame_bytes, uint8_t frame_length);

  PaceBmsV25* pace_bms_v25_;
  static const uint8_t max_data_len_ = 150;
  uint8_t raw_data_[max_data_len_];
  uint8_t raw_data_index_{0};
  uint32_t last_transmission_{0};

  std::vector<std::function<void(PaceBmsV25::AnalogInformation)>> analog_information_callbacks_;
};
 
}  // namespace pace_bms
}  // namespace esphome
