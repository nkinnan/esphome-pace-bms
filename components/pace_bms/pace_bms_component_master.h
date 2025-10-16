#pragma once

#include "esphome/components/uart/uart.h"

#include "pace_bms_component_base.h"
#include "pace_bms_component_slave.h"

namespace esphome {
namespace pace_bms_master {

enum SlaveDiscoveryMode : uint8_t {
	SLAVE_DISCOVERY_MODE_NONE = 0,
	SLAVE_DISCOVERY_MODE_RELAY = 1,
	SLAVE_DISCOVERY_MODE_BROADCAST = 2,
	SLAVE_DISCOVERY_MODE_RELAY_AND_BROADCAST = 3,
};

enum SlaveQueryMode : uint8_t {
	SLAVE_QUERY_MODE_BROADCAST = 0,
	SLAVE_QUERY_MODE_RELAY = 1,
};

// this class encapsulates an instance of PaceBmsProtocolV25 (which handles protocol version 0x25) and injects the logging dependencies into it
//     in the future, other protocol versions may be supported
class PaceBmsMaster : public pace_bms_base::PaceBmsBase, public PollingComponent, public uart::UARTDevice {
public:
	// called by the codegen to set our YAML property values
	void set_responding_address(uint8_t responding_address) { this->responding_address_ = responding_address; }
	void set_flow_control_pin(GPIOPin* flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }
	void set_protocol_commandset(int protocol_commandset) { this->protocol_commandset_ = protocol_commandset; }
	void set_protocol_variant(std::string protocol_variant) { this->protocol_variant_ = protocol_variant; }
	void set_protocol_version(uint8_t protocol_version_override) { this->protocol_version_ = protocol_version_override; }
	void set_chemistry(uint8_t chemistry) { this->chemistry_ = chemistry; }
	void set_request_throttle(int request_throttle) { this->request_throttle_ = request_throttle; }
	void set_response_timeout(int response_timeout) { this->response_timeout_ = response_timeout; }
	void set_slave_discovery_mode(SlaveDiscoveryMode mode) { this->slave_discovery_mode_ = mode; }
	void set_slave_query_mode(SlaveQueryMode mode) { this->slave_query_mode_ = mode; }
	void set_rx_buffer_size(uint16_t rx_buffer_size) { this->rx_buffer_size_ = rx_buffer_size; }

	// currently the master will dispatch BMS updates to slaves (or itself) through these two access points, could probably use an improved / cleaner 
	// design such as having slaves (or itself) process the payloads internally via a method such as "notify_analog_information" for example
	std::vector<std::function<void(PaceBmsProtocolV25::AnalogInformation&)>> get_analog_information_callbacks_v25() override { return analog_information_callbacks_v25_; };
	std::vector<std::function<void(PaceBmsProtocolV25::StatusInformation&)>> get_status_information_callbacks_v25() override { return status_information_callbacks_v25_; };

	// standard overrides to implement component behavior, update() queues periodic commands to request updates from the BMS
	void dump_config() override;
	void setup() override;
	void update() override;
	void loop() override;

	// preferably we'll be setup after all child sensors have registered their callbacks via their own setup(), but
	//     this class still handles the case where they register late gracefully, a single update cycle will simply 
	//     be missed in that case
	float get_setup_priority() const { return setup_priority::LATE; }

	// slave instances will have a pointer to us due to how the yaml is set up, but we also need to know about them so we can push updates
	// out to their sensors when we get responses from the master BMS, so they will call this method to register themselves with us
	// (sorted because broadcast responses do not include addresses, but should return in address order... probably)
	void register_slave(pace_bms_slave::PaceBmsSlave* slave)
	{
		this->slaves_.push_back(slave);
		std::sort(slaves_.begin(), slaves_.end(), [](pace_bms_slave::PaceBmsSlave* a, pace_bms_slave::PaceBmsSlave* b) {
			return a->get_address() < b->get_address();
		});
	}

	// make accessible to sensors
	int get_protocol_commandset() override { return this->protocol_commandset_; }
	// we don't push all API updates in a single loop, that'd stall the ESP out and overflow the API buffer besides
	void queue_sensor_update(std::function<void()> update) override { this->sensor_update_queue_.push(update); }

	// child sensors call these to register for notification upon reciept of various types of data from the BMS, and the 
	//     callbacks lists not being empty is what prompts update() to queue command_items for BMS communication in order to 
	//     periodically gather these updates for fan-out to the sensors the first place
	void register_bms_count_callback_v25(std::function<void(uint8_t&)> callback) override { bms_count_callbacks_v25_.push_back(std::move(callback)); }
	void register_payload_count_callback_v25(std::function<void(uint8_t&)> callback) override { payload_count_callbacks_v25_.push_back(std::move(callback)); }
	void register_analog_information_callback_v25(std::function<void(PaceBmsProtocolV25::AnalogInformation&)> callback) override { analog_information_callbacks_v25_.push_back(std::move(callback)); }
	void register_status_information_callback_v25(std::function<void(PaceBmsProtocolV25::StatusInformation&)> callback) override { status_information_callbacks_v25_.push_back(std::move(callback)); }
	void register_hardware_version_callback_v25(std::function<void(std::string&)> callback) override { hardware_version_callbacks_v25_.push_back(std::move(callback)); }
	void register_serial_number_callback_v25(std::function<void(std::string&) > callback) override { serial_number_callbacks_v25_.push_back(std::move(callback)); }
	void register_protocols_callback_v25(std::function<void(PaceBmsProtocolV25::Protocols&) > callback) override { protocols_callbacks_v25_.push_back(std::move(callback)); }
	void register_cell_over_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellOverVoltageConfiguration&)> callback) override { cell_over_voltage_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_pack_over_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::PackOverVoltageConfiguration&)> callback) override { pack_over_voltage_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_cell_under_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellUnderVoltageConfiguration&)> callback) override { cell_under_voltage_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_pack_under_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::PackUnderVoltageConfiguration&)> callback) override { pack_under_voltage_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_charge_over_current_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeOverCurrentConfiguration&)> callback) override { charge_over_current_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_discharge_over_current1_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::DischargeOverCurrent1Configuration&)> callback) override { discharge_over_current1_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_discharge_over_current2_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::DischargeOverCurrent2Configuration&)> callback) override { discharge_over_current2_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_short_circuit_protection_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ShortCircuitProtectionConfiguration&)> callback) override { short_circuit_protection_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_cell_balancing_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellBalancingConfiguration&)> callback) override { cell_balancing_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_sleep_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::SleepConfiguration&)> callback) override { sleep_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_full_charge_low_charge_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::FullChargeLowChargeConfiguration&)> callback) override { full_charge_low_charge_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_charge_and_discharge_over_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeAndDischargeOverTemperatureConfiguration&)> callback) override { charge_and_discharge_over_temperature_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_charge_and_discharge_under_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeAndDischargeUnderTemperatureConfiguration&)> callback) override { charge_and_discharge_under_temperature_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_mosfet_over_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::MosfetOverTemperatureConfiguration&)> callback) override { mosfet_over_temperature_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_environment_over_under_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::EnvironmentOverUnderTemperatureConfiguration&)> callback) override { environment_over_under_temperature_configuration_callbacks_v25_.push_back(std::move(callback)); }
	void register_system_datetime_callback_v25(std::function<void(PaceBmsProtocolV25::DateTime&)> callback) override { system_datetime_callbacks_v25_.push_back(std::move(callback)); }
	
	void register_analog_information_callback_v20(std::function<void(PaceBmsProtocolV20::AnalogInformation&)> callback) override { analog_information_callbacks_v20_.push_back(std::move(callback)); }
	void register_status_information_callback_v20(std::function<void(PaceBmsProtocolV20::StatusInformation&)> callback) override { status_information_callbacks_v20_.push_back(std::move(callback)); }
	void register_hardware_version_callback_v20(std::function<void(std::string&)> callback) override { hardware_version_callbacks_v20_.push_back(std::move(callback)); }
	void register_serial_number_callback_v20(std::function<void(std::string&) > callback) override { serial_number_callbacks_v20_.push_back(std::move(callback)); }
	void register_system_datetime_callback_v20(std::function<void(PaceBmsProtocolV20::DateTime&)> callback) override { system_datetime_callbacks_v20_.push_back(std::move(callback)); }

	// child sensors call these to schedule new values be written out to the hardware
	void write_switch_state_v25(PaceBmsProtocolV25::SwitchCommand state) override;
	void write_mosfet_state_v25(PaceBmsProtocolV25::MosfetType type, PaceBmsProtocolV25::MosfetState state) override;
	void write_shutdown_v25() override;
	void write_protocols_v25(PaceBmsProtocolV25::Protocols& protocols) override;
	void write_cell_over_voltage_configuration_v25(PaceBmsProtocolV25::CellOverVoltageConfiguration& config) override;
	void write_pack_over_voltage_configuration_v25(PaceBmsProtocolV25::PackOverVoltageConfiguration& config) override;
	void write_cell_under_voltage_configuration_v25(PaceBmsProtocolV25::CellUnderVoltageConfiguration& config) override;
	void write_pack_under_voltage_configuration_v25(PaceBmsProtocolV25::PackUnderVoltageConfiguration& config) override;
	void write_charge_over_current_configuration_v25(PaceBmsProtocolV25::ChargeOverCurrentConfiguration& config) override;
	void write_discharge_over_current1_configuration_v25(PaceBmsProtocolV25::DischargeOverCurrent1Configuration& config) override;
	void write_discharge_over_current2_configuration_v25(PaceBmsProtocolV25::DischargeOverCurrent2Configuration& config) override;
	void write_short_circuit_protection_configuration_v25(PaceBmsProtocolV25::ShortCircuitProtectionConfiguration& config) override;
	void write_cell_balancing_configuration_v25(PaceBmsProtocolV25::CellBalancingConfiguration& config) override;
	void write_sleep_configuration_v25(PaceBmsProtocolV25::SleepConfiguration& config) override;
	void write_full_charge_low_charge_configuration_v25(PaceBmsProtocolV25::FullChargeLowChargeConfiguration& config) override;
	void write_charge_and_discharge_over_temperature_configuration_v25(PaceBmsProtocolV25::ChargeAndDischargeOverTemperatureConfiguration& config) override;
	void write_charge_and_discharge_under_temperature_configuration_v25(PaceBmsProtocolV25::ChargeAndDischargeUnderTemperatureConfiguration& config) override;
	void write_mosfet_over_temperature_configuration_v25(PaceBmsProtocolV25::MosfetOverTemperatureConfiguration& config) override;
	void write_environment_over_under_temperature_configuration_v25(PaceBmsProtocolV25::EnvironmentOverUnderTemperatureConfiguration& config) override;
	void write_system_datetime_v25(PaceBmsProtocolV25::DateTime& dt) override;

	void write_shutdown_v20() override;
	void write_system_datetime_v20(PaceBmsProtocolV20::DateTime& dt) override;


protected:
	// config values set in YAML
	std::optional<uint8_t> responding_address_;
	GPIOPin* flow_control_pin_{ nullptr };

	int protocol_commandset_{ 0 };
	std::optional<std::string> protocol_variant_;
	std::optional<uint8_t> protocol_version_;
	std::optional<uint8_t> chemistry_;

	int request_throttle_{ 0 };
	int response_timeout_{ 0 };

	SlaveDiscoveryMode slave_discovery_mode_{ SLAVE_DISCOVERY_MODE_NONE };
	SlaveQueryMode slave_query_mode_{ SLAVE_QUERY_MODE_BROADCAST };

	// put into command_item as a pointer to handle the BMS response
	void handle_read_bms_count_response_v25(std::span<uint8_t>& response);
	void handle_slave_discovery_broadcast_read_analog_information_response_v25(std::span<uint8_t>& response);
	void handle_slave_discovery_broadcast_read_status_information_response_v25(std::span<uint8_t>& response);
	void handle_slave_discovery_relay_read_analog_information_response_v25(uint8_t slaveAddress, std::span<uint8_t>& response);
	void handle_slave_discovery_relay_read_status_information_response_v25(uint8_t slaveAddress, std::span<uint8_t>& response);
	void handle_read_analog_information_response_v25(std::span<uint8_t>& response, pace_bms_base::PaceBmsBase* target);
	void handle_read_status_information_response_v25(std::span<uint8_t>& response, pace_bms_base::PaceBmsBase* target);
	void handle_broadcast_read_analog_information_response_v25(std::span<uint8_t>& response);
	void handle_broadcast_read_status_information_response_v25(std::span<uint8_t>& response);
	void handle_read_hardware_version_response_v25(std::span<uint8_t>& response);
	void handle_read_serial_number_response_v25(std::span<uint8_t>& response);
	void handle_write_switch_command_response_v25(PaceBmsProtocolV25::SwitchCommand, std::span<uint8_t>& response);
	void handle_write_mosfet_switch_command_response_v25(PaceBmsProtocolV25::MosfetType type, PaceBmsProtocolV25::MosfetState state, std::span<uint8_t>& response);
	void handle_write_shutdown_command_response_v25(std::span<uint8_t>& response);
	void handle_read_protocols_response_v25(std::span<uint8_t>& response);
	void handle_write_protocols_response_v25(PaceBmsProtocolV25::Protocols protocols, std::span<uint8_t>& response);
	void handle_read_cell_over_voltage_configuration_response_v25(std::span<uint8_t>& response);
	void handle_read_pack_over_voltage_configuration_response_v25(std::span<uint8_t>& response);
	void handle_read_cell_under_voltage_configuration_response_v25(std::span<uint8_t>& response);
	void handle_read_pack_under_voltage_configuration_response_v25(std::span<uint8_t>& response);
	void handle_read_charge_over_current_configuration_response_v25(std::span<uint8_t>& response);
	void handle_read_discharge_over_current1_configuration_response_v25(std::span<uint8_t>& response);
	void handle_read_discharge_over_current2_configuration_response_v25(std::span<uint8_t>& response);
	void handle_read_short_circuit_protection_configuration_response_v25(std::span<uint8_t>& response);
	void handle_read_cell_balancing_configuration_response_v25(std::span<uint8_t>& response);
	void handle_read_sleep_configuration_response_v25(std::span<uint8_t>& response);
	void handle_read_full_charge_low_charge_configuration_response_v25(std::span<uint8_t>& response);
	void handle_read_charge_and_discharge_over_temperature_configuration_response_v25(std::span<uint8_t>& response);
	void handle_read_charge_and_discharge_under_temperature_configuration_response_v25(std::span<uint8_t>& response);
	void handle_read_mosfet_over_temperature_configuration_response_v25(std::span<uint8_t>& response);
	void handle_read_environment_over_under_temperature_configuration_response_v25(std::span<uint8_t>& response);
	void handle_read_system_datetime_response_v25(std::span<uint8_t>& response);
	void handle_write_system_datetime_response_v25(std::span<uint8_t>& response);
	void handle_write_configuration_response_v25(std::span<uint8_t>& response);

	void handle_read_analog_information_response_v20(std::span<uint8_t>& response);
	void handle_read_status_information_response_v20(std::span<uint8_t>& response);
	void handle_read_hardware_version_response_v20(std::span<uint8_t>& response);
	void handle_read_serial_number_response_v20(std::span<uint8_t>& response);
	void handle_write_shutdown_command_response_v20(std::span<uint8_t>& response);
	void handle_read_system_datetime_response_v20(std::span<uint8_t>& response);
	void handle_write_system_datetime_response_v20(std::span<uint8_t>& response);

	// child sensor requested callback lists
	std::vector<std::function<void(uint8_t&)>>                                                             bms_count_callbacks_v25_;
	std::vector<std::function<void(uint8_t&)>>                                                             payload_count_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::AnalogInformation&)>>                               analog_information_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::StatusInformation&)>>                               status_information_callbacks_v25_;
	std::vector<std::function<void(std::string&)>>                                                         hardware_version_callbacks_v25_;
	std::vector<std::function<void(std::string&)>>                                                         serial_number_callbacks_v25_;
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
	std::vector<std::function<void(std::string&)>>                                                         hardware_version_callbacks_v20_;
	std::vector<std::function<void(std::string&)>>                                                         serial_number_callbacks_v20_;
	std::vector<std::function<void(PaceBmsProtocolV20::DateTime&)>>                                        system_datetime_callbacks_v20_;

	// along with loop() this is the "engine" of BMS communications
	//     - send_next_request_frame_ will pop a command_item from the queue and dispatch a frame to the BMS
	//     - process_response_frame_ will call next_response_handler_ (which was saved from the command_item popped in 
	//           send_next_request_frame_) once a response arrives
	PaceBmsProtocolV25* pace_bms_v25_;
	PaceBmsProtocolV20* pace_bms_v20_;
	uint16_t rx_buffer_size_ = 0;
	uint8_t *raw_data_;
	uint16_t raw_data_index_{ 0 };
	uint32_t last_transmit_{ 0 };
	uint32_t last_receive_{ 0 };
	bool request_outstanding_ = false;
	void send_next_request_frame_();
	void process_response_frame_(uint8_t* frame_bytes, uint16_t frame_length);

	// each item points to:
	//     a description of what is happening such as "Read Analog Information" for logging purposes
	//     a function pointer that will generate the request frame (to avoid holding the memory prior to it being required)
	//     a function pointer that will process the response frame and dispatch the results to any child sensors registered via the callback vectors
	struct command_item
	{
		std::string description_;
		std::function<bool(std::vector<uint8_t>&)> create_request_frame_;
		std::function<void(std::span<uint8_t>&)> process_response_frame_;
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
	std::queue<std::function<void()>> sensor_update_queue_;
	std::queue<command_item*> read_queue_;
	std::list<command_item*> write_queue_;
	std::function<void(std::span<uint8_t>&)> next_response_handler_ = nullptr;
	std::string last_request_description;

	// list of slaves that have registered with us
	std::vector<pace_bms_slave::PaceBmsSlave*> slaves_;

	// helper to avoid pushing redundant write requests (if the user hits a button multiple times quickly for example)
	void write_queue_push_back_with_deduplication(command_item* item);
};

}  // namespace pace_bms_master
}  // namespace esphome
