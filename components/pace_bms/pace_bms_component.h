#pragma once

#include <vector>
#include <functional>
#include <queue>
#include <list>

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

#include "pace_bms_protocol_v25.h"
#include "pace_bms_protocol_v20.h"

namespace esphome {
namespace pace_bms {


// this class encapsulates an instance of PaceBmsProtocolV25 (which handles protocol version 0x25) and injects the logging dependencies into it
//     in the future, other protocol versions may be supported
class PaceBms : public PollingComponent, public uart::UARTDevice {
public:
	// called by the codegen to set our YAML property values
	void set_flow_control_pin(GPIOPin* flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }
	void set_address(uint8_t address) { this->address_ = address; }
	void set_protocol_version(int protocol_version) { this->protocol_version_ = protocol_version; }
	void set_protocol_variant(std::string protocol_variant) { this->protocol_variant_ = protocol_variant; }
	void set_protocol_version_override(uint8_t protocol_version_override) { this->protocol_version_override_ = protocol_version_override; }
	void set_chemistry(uint8_t chemistry) { this->chemistry_ = chemistry; }
	void set_request_throttle(int request_throttle) { this->request_throttle_ = request_throttle; }
	void set_response_timeout(int response_timeout) { this->response_timeout_ = response_timeout; }

	// make accessible to sensors
	int get_protocol_version() { return this->protocol_version_; }

	// standard overrides to implement component behavior, update() queues periodic commands to request updates from the BMS
	void dump_config() override;
	void setup() override;
	void update() override;
	void loop() override;

	// preferably we'll be setup after all child sensors have registered their callbacks via their own setup(), but
	//     this class still handles the case where they register late gracefully, a single update cycle will simply 
	//     be missed in that case
	float get_setup_priority() const { return setup_priority::LATE; }

	// child sensors call these to register for notification upon reciept of various types of data from the BMS, and the 
	//     callbacks lists not being empty is what prompts update() to queue command_items for BMS communication in order to 
	//     periodically gather these updates for fan-out to the sensors the first place
	void register_analog_information_callback_v25(std::function<void(PaceBmsProtocolV25::AnalogInformation&)> callback) { analog_information_callbacks_v25_.push_back(std::move(callback)); }
	void register_status_information_callback_v25(std::function<void(PaceBmsProtocolV25::StatusInformation&)> callback) { status_information_callbacks_v25_.push_back(std::move(callback)); }
	void register_hardware_version_callback_v25(std::function<void(std::string&)> callback) { hardware_version_callbacks_v25_.push_back(std::move(callback)); }
	void register_serial_number_callback_v25(std::function<void(std::string&) > callback) { serial_number_callbacks_v25_.push_back(std::move(callback)); }
	void register_protocols_callback_v25(std::function<void(PaceBmsProtocolV25::Protocols&) > callback) { protocols_callbacks_v25_.push_back(std::move(callback)); }
	void register_cell_over_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellOverVoltageConfiguration&)> callback) { cell_over_voltage_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_pack_over_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::PackOverVoltageConfiguration&)> callback) { pack_over_voltage_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_cell_under_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellUnderVoltageConfiguration&)> callback) { cell_under_voltage_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_pack_under_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::PackUnderVoltageConfiguration&)> callback) { pack_under_voltage_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_charge_over_current_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeOverCurrentConfiguration&)> callback) { charge_over_current_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_discharge_over_current1_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::DischargeOverCurrent1Configuration&)> callback) { discharge_over_current1_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_discharge_over_current2_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::DischargeOverCurrent2Configuration&)> callback) { discharge_over_current2_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_short_circuit_protection_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ShortCircuitProtectionConfiguration&)> callback) { short_circuit_protection_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_cell_balancing_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellBalancingConfiguration&)> callback) { cell_balancing_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_sleep_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::SleepConfiguration&)> callback) { sleep_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_full_charge_low_charge_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::FullChargeLowChargeConfiguration&)> callback) { full_charge_low_charge_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_charge_and_discharge_over_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeAndDischargeOverTemperatureConfiguration&)> callback) { charge_and_discharge_over_temperature_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_charge_and_discharge_under_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeAndDischargeUnderTemperatureConfiguration&)> callback) { charge_and_discharge_under_temperature_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_mosfet_over_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::MosfetOverTemperatureConfiguration&)> callback) { mosfet_over_temperature_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_environment_over_under_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::EnvironmentOverUnderTemperatureConfiguration&)> callback) { environment_over_under_temperature_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_system_datetime_callback_v25(std::function<void(PaceBmsProtocolV25::DateTime&)> callback) { system_datetime_callbacks_v25_.push_back(std::move(callback)); }
	
	void register_analog_information_callback_v20(std::function<void(PaceBmsProtocolV20::AnalogInformation&)> callback) { analog_information_callbacks_v20_.push_back(std::move(callback)); }
	void register_status_information_callback_v20(std::function<void(PaceBmsProtocolV20::StatusInformation&)> callback) { status_information_callbacks_v20_.push_back(std::move(callback)); }
	void register_hardware_version_callback_v20(std::function<void(std::string&)> callback) { hardware_version_callbacks_v20_.push_back(std::move(callback)); }
	void register_serial_number_callback_v20(std::function<void(std::string&) > callback) { serial_number_callbacks_v20_.push_back(std::move(callback)); }

	// child sensors call these to schedule new values be written out to the hardware
	void write_switch_state_v25(PaceBmsProtocolV25::SwitchCommand state);
	void write_mosfet_state_v25(PaceBmsProtocolV25::MosfetType type, PaceBmsProtocolV25::MosfetState state);
	void write_shutdown_v25();
	void write_protocols_v25(PaceBmsProtocolV25::Protocols& protocols);
	void write_cell_over_voltage_configuration_v25(PaceBmsProtocolV25::CellOverVoltageConfiguration& config);
	void write_pack_over_voltage_configuration_v25(PaceBmsProtocolV25::PackOverVoltageConfiguration& config);
	void write_cell_under_voltage_configuration_v25(PaceBmsProtocolV25::CellUnderVoltageConfiguration& config);
	void write_pack_under_voltage_configuration_v25(PaceBmsProtocolV25::PackUnderVoltageConfiguration& config);
	void write_charge_over_current_configuration_v25(PaceBmsProtocolV25::ChargeOverCurrentConfiguration& config);
	void write_discharge_over_current1_configuration_v25(PaceBmsProtocolV25::DischargeOverCurrent1Configuration& config);
	void write_discharge_over_current2_configuration_v25(PaceBmsProtocolV25::DischargeOverCurrent2Configuration& config);
	void write_short_circuit_protection_configuration_v25(PaceBmsProtocolV25::ShortCircuitProtectionConfiguration& config);
	void write_cell_balancing_configuration_v25(PaceBmsProtocolV25::CellBalancingConfiguration& config);
	void write_sleep_configuration_v25(PaceBmsProtocolV25::SleepConfiguration& config);
	void write_full_charge_low_charge_configuration_v25(PaceBmsProtocolV25::FullChargeLowChargeConfiguration& config);
	void write_charge_and_discharge_over_temperature_configuration_v25(PaceBmsProtocolV25::ChargeAndDischargeOverTemperatureConfiguration& config);
	void write_charge_and_discharge_under_temperature_configuration_v25(PaceBmsProtocolV25::ChargeAndDischargeUnderTemperatureConfiguration& config);
	void write_mosfet_over_temperature_configuration_v25(PaceBmsProtocolV25::MosfetOverTemperatureConfiguration& config);
	void write_environment_over_under_temperature_configuration_v25(PaceBmsProtocolV25::EnvironmentOverUnderTemperatureConfiguration& config);
	void write_system_datetime_v25(PaceBmsProtocolV25::DateTime& dt);

	void write_shutdown_v20();


protected:
	// config values set in YAML
	GPIOPin* flow_control_pin_{ nullptr };
	uint8_t address_{ 0 };

	int protocol_version_{ 0 };
	OPTIONAL_NS::optional<std::string> protocol_variant_;
	OPTIONAL_NS::optional<uint8_t> protocol_version_override_;
	OPTIONAL_NS::optional<uint8_t> chemistry_;

	uint32_t design_capacity_mah_override_{ 0 };

	int request_throttle_{ 0 };
	int response_timeout_{ 0 };

	// put into command_item as a pointer to handle the BMS response
	void handle_read_analog_information_response_v25(std::vector<uint8_t>& response);
	void handle_read_status_information_response_v25(std::vector<uint8_t>& response);
	void handle_read_hardware_version_response_v25(std::vector<uint8_t>& response);
	void handle_read_serial_number_response_v25(std::vector<uint8_t>& response);
	void handle_write_switch_command_response_v25(PaceBmsProtocolV25::SwitchCommand, std::vector<uint8_t>& response);
	void handle_write_mosfet_switch_command_response_v25(PaceBmsProtocolV25::MosfetType type, PaceBmsProtocolV25::MosfetState state, std::vector<uint8_t>& response);
	void handle_write_shutdown_command_response_v25(std::vector<uint8_t>& response);
	void handle_read_protocols_response_v25(std::vector<uint8_t>& response);
	void handle_write_protocols_response_v25(PaceBmsProtocolV25::Protocols protocols, std::vector<uint8_t>& response);
	void handle_read_cell_over_voltage_configuration_response_v25(std::vector<uint8_t>& response);
	void handle_read_pack_over_voltage_configuration_response_v25(std::vector<uint8_t>& response);
	void handle_read_cell_under_voltage_configuration_response_v25(std::vector<uint8_t>& response);
	void handle_read_pack_under_voltage_configuration_response_v25(std::vector<uint8_t>& response);
	void handle_read_charge_over_current_configuration_response_v25(std::vector<uint8_t>& response);
	void handle_read_discharge_over_current1_configuration_response_v25(std::vector<uint8_t>& response);
	void handle_read_discharge_over_current2_configuration_response_v25(std::vector<uint8_t>& response);
	void handle_read_short_circuit_protection_configuration_response_v25(std::vector<uint8_t>& response);
	void handle_read_cell_balancing_configuration_response_v25(std::vector<uint8_t>& response);
	void handle_read_sleep_configuration_response_v25(std::vector<uint8_t>& response);
	void handle_read_full_charge_low_charge_configuration_response_v25(std::vector<uint8_t>& response);
	void handle_read_charge_and_discharge_over_temperature_configuration_response_v25(std::vector<uint8_t>& response);
	void handle_read_charge_and_discharge_under_temperature_configuration_response_v25(std::vector<uint8_t>& response);
	void handle_read_mosfet_over_temperature_configuration_response_v25(std::vector<uint8_t>& response);
	void handle_read_environment_over_under_temperature_configuration_response_v25(std::vector<uint8_t>& response);
	void handle_read_system_datetime_response_v25(std::vector<uint8_t>& response);
	void handle_write_system_datetime_response_v25(std::vector<uint8_t>& response);
	void handle_write_configuration_response_v25(std::vector<uint8_t>& response);

	void handle_read_analog_information_response_v20(std::vector<uint8_t>& response);
	void handle_read_status_information_response_v20(std::vector<uint8_t>& response);
	void handle_read_hardware_version_response_v20(std::vector<uint8_t>& response);
	void handle_read_serial_number_response_v20(std::vector<uint8_t>& response);
	void handle_write_shutdown_command_response_v20(std::vector<uint8_t>& response);

	// child sensor requested callback lists
	std::vector<std::function<void(PaceBmsProtocolV25::AnalogInformation&)>>                               analog_information_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::StatusInformation&)>>                               status_information_callbacks_v25_;
	std::vector<std::function<void(std::string&)>>                                                 hardware_version_callbacks_v25_;
	std::vector<std::function<void(std::string&)>>                                                 serial_number_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::Protocols&)>>                                       protocols_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::CellOverVoltageConfiguration&)>>                    cell_over_voltage_configuration_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::PackOverVoltageConfiguration&)>>                    pack_over_voltage_configuration_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::CellUnderVoltageConfiguration&)>>                   cell_under_voltage_configuration_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::PackUnderVoltageConfiguration&)>>                   pack_under_voltage_configuration_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::ChargeOverCurrentConfiguration&)>>                  charge_over_current_configuration_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::DischargeOverCurrent1Configuration&)>>              discharge_over_current1_configuration_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::DischargeOverCurrent2Configuration&)>>              discharge_over_current2_configuration_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::ShortCircuitProtectionConfiguration&)>>             short_circuit_protection_configuration_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::CellBalancingConfiguration&)>>                      cell_balancing_configuration_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::SleepConfiguration&)>>                              sleep_configuration_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::FullChargeLowChargeConfiguration&)>>                full_charge_low_charge_configuration_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::ChargeAndDischargeOverTemperatureConfiguration&)>>  charge_and_discharge_over_temperature_configuration_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::ChargeAndDischargeUnderTemperatureConfiguration&)>> charge_and_discharge_under_temperature_configuration_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::MosfetOverTemperatureConfiguration&)>>              mosfet_over_temperature_configuration_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::EnvironmentOverUnderTemperatureConfiguration&)>>    environment_over_under_temperature_configuration_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::DateTime&)>>                                        system_datetime_callbacks_v25_;

	std::vector<std::function<void(PaceBmsProtocolV20::AnalogInformation&)>>                               analog_information_callbacks_v20_;
	std::vector<std::function<void(PaceBmsProtocolV20::StatusInformation&)>>                               status_information_callbacks_v20_;
	std::vector<std::function<void(std::string&)>>                                                 hardware_version_callbacks_v20_;
	std::vector<std::function<void(std::string&)>>                                                 serial_number_callbacks_v20_;

	// along with loop() this is the "engine" of BMS communications
	//     - send_next_request_frame_ will pop a command_item from the queue and dispatch a frame to the BMS
	//     - process_response_frame_ will call next_response_handler_ (which was saved from the command_item popped in 
	//           send_next_request_frame_) once a response arrives
	PaceBmsProtocolV25* pace_bms_v25_;
	PaceBmsProtocolV20* pace_bms_v20_;
	// this is currently "right sized" as it's only slightly larger than the largest 0x20 response I've seen
	static const uint16_t max_data_len_ = 256;
	uint8_t raw_data_[max_data_len_];
	uint8_t raw_data_index_{ 0 };
	uint32_t last_transmit_{ 0 };
	uint32_t last_receive_{ 0 };
	bool request_outstanding_ = false;
	void send_next_request_frame_();
	void process_response_frame_(uint8_t* frame_bytes, uint8_t frame_length);

	// each item points to:
	//     a description of what is happening such as "Read Analog Information" for logging purposes
	//     a function pointer that will generate the request frame (to avoid holding the memory prior to it being required)
	//     a function pointer that will process the response frame and dispatch the results to any child sensors registered via the callback vectors
	struct command_item
	{
		std::string description_;
		std::function<bool(std::vector<uint8_t>&)> create_request_frame_;
		std::function<void(std::vector<uint8_t>&)> process_response_frame_;
	};
	// when the bus is clear:
	//     the next command_item will be popped from either the read or the write queue (writes always take priority)
	//     the request frame generated and dispatched via command_item.create_request_frame_
	//     the expected response handler (command_item.process_response_frame_) will be assigned to next_response_handler_ to be called once a response frame arrives
	//     last_request_description is also saved for logging purposes as:
	//     once this sequence starts, the command_item is thrown away - it's all bytes and saved pointers from this point
	//         see section: "along with loop() this is the "engine" of BMS communications" for how this works
	// commands generated as a result of user interaction are pushed to the write queue which has priority over the read queue
	// the read queue is filled each update() with only the commands necessary to refresh child components that have been declared in the yaml config and requested a callback for the information
	std::queue<command_item*> read_queue_;
	std::list<command_item*> write_queue_;
	std::function<void(std::vector<uint8_t>&)> next_response_handler_ = nullptr;
	std::string last_request_description;

	// helper to avoid pushing redundant write requests
	void write_queue_push_back_with_deduplication(command_item* item);
};

}  // namespace pace_bms
}  // namespace esphome
