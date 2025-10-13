#pragma once

#include "pace_bms_component_base.h"

// forward declaration to avoid circular dependency, code using PaceBmsMaster is in the .cpp where the actual pace_bms_component_master.h header is included
//#include "../pace_bms/pace_bms_component_master.h"
namespace esphome {
	namespace pace_bms_master {
		class PaceBmsMaster;
	}
}

namespace esphome {
namespace pace_bms_slave {

// this class encapsulates an instance of PaceBmsProtocolV25 (which handles protocol version 0x25) and injects the logging dependencies into it
//     in the future, other protocol versions may be supported
class PaceBmsSlave : public pace_bms_base::PaceBmsBase, public Component {
public:

	void set_parent(pace_bms_master::PaceBmsMaster* parent) { parent_ = parent; }

	// standard overrides to implement component behavior
	void dump_config() override;
	void setup() override;
	void loop() override;

	// preferably we'll be setup after all child sensors have registered their callbacks via their own setup(), but
	//     this class still handles the case where they register late gracefully, a single update cycle will simply 
	//     be missed in that case
	float get_setup_priority() const { return setup_priority::LATE; }

	// make accessible to sensors
	// this pulls from the master device
	int get_protocol_commandset() override;
	// we don't push all updates in a single loop, that'd stall the ESP out
	// this writes to the master device's queue
	void queue_sensor_update(std::function<void()> update) override;
	// currently the master will dispatch BMS updates to slaves (or itself) through these two access points, could probably use an improved / cleaner 
	// design such as having slaves (or itself) process the payloads internally via a method such as "notify_analog_information" for example
	std::vector<std::function<void(PaceBmsProtocolV25::AnalogInformation&)>> get_analog_information_callbacks_v25() override { return analog_information_callbacks_v25_; };
	std::vector<std::function<void(PaceBmsProtocolV25::StatusInformation&)>> get_status_information_callbacks_v25() override { return status_information_callbacks_v25_; };

	// child sensors call these to register for notification upon reciept of various types of data from the BMS, in the case of
	// slave BMSes this is analog and status information *only*, other methods do nothing but log an error
	void register_bms_count_callback_v25(std::function<void(uint8_t&)> callback) override;
	void register_payload_count_callback_v25(std::function<void(uint8_t&)> callback) override;
	void register_analog_information_callback_v25(std::function<void(PaceBmsProtocolV25::AnalogInformation&)> callback) override;
	void register_status_information_callback_v25(std::function<void(PaceBmsProtocolV25::StatusInformation&)> callback) override;
	void register_hardware_version_callback_v25(std::function<void(std::string&)> callback) override;
	void register_serial_number_callback_v25(std::function<void(std::string&) > callback) override;
	void register_protocols_callback_v25(std::function<void(PaceBmsProtocolV25::Protocols&) > callback) override;
	void register_cell_over_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellOverVoltageConfiguration&)> callback) override;
	void register_pack_over_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::PackOverVoltageConfiguration&)> callback) override;
	void register_cell_under_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellUnderVoltageConfiguration&)> callback) override;
	void register_pack_under_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::PackUnderVoltageConfiguration&)> callback) override;
	void register_charge_over_current_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeOverCurrentConfiguration&)> callback) override;
	void register_discharge_over_current1_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::DischargeOverCurrent1Configuration&)> callback) override;
	void register_discharge_over_current2_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::DischargeOverCurrent2Configuration&)> callback) override;
	void register_short_circuit_protection_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ShortCircuitProtectionConfiguration&)> callback) override;
	void register_cell_balancing_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellBalancingConfiguration&)> callback) override;
	void register_sleep_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::SleepConfiguration&)> callback) override;
	void register_full_charge_low_charge_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::FullChargeLowChargeConfiguration&)> callback) override;
	void register_charge_and_discharge_over_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeAndDischargeOverTemperatureConfiguration&)> callback) override;
	void register_charge_and_discharge_under_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeAndDischargeUnderTemperatureConfiguration&)> callback) override;
	void register_mosfet_over_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::MosfetOverTemperatureConfiguration&)> callback) override;
	void register_environment_over_under_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::EnvironmentOverUnderTemperatureConfiguration&)> callback) override;
	void register_system_datetime_callback_v25(std::function<void(PaceBmsProtocolV25::DateTime&)> callback) override;

	void register_analog_information_callback_v20(std::function<void(PaceBmsProtocolV20::AnalogInformation&)> callback) override;
	void register_status_information_callback_v20(std::function<void(PaceBmsProtocolV20::StatusInformation&)> callback) override;
	void register_hardware_version_callback_v20(std::function<void(std::string&)> callback) override;
	void register_serial_number_callback_v20(std::function<void(std::string&) > callback) override;
	void register_system_datetime_callback_v20(std::function<void(PaceBmsProtocolV20::DateTime&)> callback) override;

	// child sensors call these to schedule new values be written out to the hardware, in the case of slave BMSes these all 
	// do nothing but log an error
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
	pace_bms_master::PaceBmsMaster* parent_{ nullptr };

	// child sensor requested callback lists, these two are the only ones supported for a slave BMS
	std::vector<std::function<void(PaceBmsProtocolV25::AnalogInformation&)>>                               analog_information_callbacks_v25_;
	std::vector<std::function<void(PaceBmsProtocolV25::StatusInformation&)>>                               status_information_callbacks_v25_;
};

}  // namespace pace_bms_slave
}  // namespace esphome
