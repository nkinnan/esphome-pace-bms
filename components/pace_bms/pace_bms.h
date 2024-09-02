#pragma once

#include <vector>

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

#include "pace_bms_v25.h"
#include <functional>
#include <queue>

namespace esphome {
namespace pace_bms {

// this class encapsulates an instance of PaceBmsV25 (which handles protocol version 0x25) and injects the logging dependencies into it
// in the future, other protocol versions may be supported
class PaceBms : public PollingComponent, public uart::UARTDevice {
 public:
  // called by the codegen to set our YAML property values
  void set_flow_control_pin(GPIOPin* flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }
  void set_address(int address) { this->address_ = address; }
  void set_protocol_version(int protocol_version) { this->protocol_version_ = protocol_version; }
  void set_request_throttle(int request_throttle) { this->request_throttle_ = request_throttle; }
  void set_response_timeout(int response_timeout) { this->response_timeout_ = response_timeout; }

  // standard overrides to implement component behavior, update() queues periodic commands to request updates from the BMS
  void setup() override;
  void update() override;
  void loop() override;
  float get_setup_priority() const override;
  void dump_config() override;

  // child sensors call these to request notification upon reciept of various types of data from the BMS
  // the callbacks lists not being empty is what prompts update() to queue command_items for BMS communication
  void register_analog_information_callback(std::function<void(PaceBmsV25::AnalogInformation&)> callback) { analog_information_callbacks_.push_back(callback); }
  void register_status_information_callback(std::function<void(PaceBmsV25::StatusInformation&)> callback) { status_information_callbacks_.push_back(callback); }
  void register_hardware_version_callback(std::function<void(std::string&)> callback) { hardware_version_callbacks_.push_back(callback); }
  void register_serial_number_callback(std::function<void(std::string&) > callback) { serial_number_callbacks_.push_back(callback); }
  void register_protocols_callback(std::function<void(PaceBmsV25::Protocols&) > callback) { protocols_callbacks_.push_back(callback); }
  void register_cell_over_voltage_configuration_callback(std::function<void(PaceBmsV25::CellOverVoltageConfiguration&) > callback) { cell_over_voltage_configuration_callbacks_.push_back(callback); }

  // child sensors call these to request new values be sent to the hardware
  void set_switch_state(PaceBmsV25::SwitchCommand state);
  void set_mosfet_state(PaceBmsV25::MosfetType type, PaceBmsV25::MosfetState state);
  void send_shutdown();
  void set_protocols(PaceBmsV25::Protocols& protocols);

 protected:
  // config values set in YAML
  GPIOPin* flow_control_pin_{ nullptr };
  int address_{ 0 };
  int protocol_version_{ 0 };
  int request_throttle_{ 0 };
  int response_timeout_{ 0 };

  // along with loop() this is the "engine" of BMS communications
  // send_next_request_frame will pop a command_item from the queue and dispatch a frame to the BMS
  // process_response_frame_ will call next_response_handler_ (which was saved from the command_item) once a response arrives
  // see: "each item points to:"
  // see: "when the bus is clear:"
  PaceBmsV25* pace_bms_v25_;
  static const uint8_t max_data_len_ = 200;
  uint8_t raw_data_[max_data_len_];
  uint8_t raw_data_index_{0};
  uint32_t last_transmit_{ 0 };
  uint32_t last_receive_{ 0 };
  bool request_outstanding_ = false;
  void send_next_request_frame_();
  void process_response_frame_(uint8_t* frame_bytes, uint8_t frame_length);

  // put into command_item as a pointer to handle the BMS response
  void handle_analog_information_response(std::vector<uint8_t>& response);
  void handle_status_information_response(std::vector<uint8_t>& response);
  void handle_hardware_version_response(std::vector<uint8_t>& response);
  void handle_serial_number_response(std::vector<uint8_t>& response);
  void handle_write_switch_command_response(PaceBmsV25::SwitchCommand, std::vector<uint8_t>& response);
  void handle_write_mosfet_switch_command_response(PaceBmsV25::MosfetType type, PaceBmsV25::MosfetState state, std::vector<uint8_t>& response);
  void handle_write_shutdown_command_response(std::vector<uint8_t>& response);
  void handle_read_protocols_response(std::vector<uint8_t>& response);
  void handle_write_protocols_response(PaceBmsV25::Protocols protocols, std::vector<uint8_t>&response);

  // each item points to:
  //     a description of what is happening such as "Read Analog Information" for logging
  //     a function pointer that will generate the request frame (to avoid holding the memory prior to it being required)
  //     a function pointer that will process the response frame and dispatch the results to any child sensors registered via the callback vectors
  struct command_item
  {
	  std::string description_;
	  std::function<void(std::vector<uint8_t>&)> create_request_frame_;
	  std::function<void(std::vector<uint8_t>&)> process_response_frame_;
  };
  // when the bus is clear:
  //     the next command_item will be popped from command_queue_
  //     the request frame generated and dispatched via command_item.create_request_frame_
  //     the expected response handler (command_item.process_response_frame_) will be assigned to next_response_handler_ to be called once a response frame arrives
  //     last_request_description is also saved for logging purposes as:
  //     once this sequence starts, the command_item is thrown away - it's all bytes and pointers from this point
  //         see section: "along with loop() this is the "engine" of BMS communications" for how this works
  std::queue<command_item*> command_queue_;
  std::function<void(std::vector<uint8_t>&)> next_response_handler_ = nullptr;
  std::string last_request_description;

  // child sensor requested callbacks
  std::vector<std::function<void(PaceBmsV25::AnalogInformation&)>> analog_information_callbacks_;
  std::vector<std::function<void(PaceBmsV25::StatusInformation&)>> status_information_callbacks_;
  std::vector<std::function<void(std::string&)>> hardware_version_callbacks_;
  std::vector<std::function<void(std::string&)>> serial_number_callbacks_;
  std::vector<std::function<void(PaceBmsV25::Protocols&)>> protocols_callbacks_;
  std::vector<std::function<void(PaceBmsV25::CellOverVoltageConfiguration&)>> cell_over_voltage_configuration_callbacks_;
};
 
}  // namespace pace_bms
}  // namespace esphome
