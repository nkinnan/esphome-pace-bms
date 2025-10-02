#pragma once

#include "pace_bms_component_base.h"

// forward declaration to avoid circular dependency, code using PaceBmsMaster is in the .cpp where the actual header is included
//#include "../pace_bms/pace_bms_master_component.h"
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

	// standard overrides to implement component behavior, update() queues periodic commands to request updates from the BMS
	void dump_config() override;
	void setup() override;
	void loop() override;

	// preferably we'll be setup after all child sensors have registered their callbacks via their own setup(), but
	//     this class still handles the case where they register late gracefully, a single update cycle will simply 
	//     be missed in that case
	float get_setup_priority() const { return setup_priority::LATE; }

	// make accessible to sensors
	int get_protocol_commandset() override;

	// child sensors call these to register for notification upon reciept of various types of data from the BMS, and the 
	//     callbacks lists not being empty is what prompts update() to queue command_items for BMS communication in order to 
	//     periodically gather these updates for fan-out to the sensors the first place
	virtual void register_analog_information_callback_v25(std::function<void(PaceBmsProtocolV25::AnalogInformation&)> callback);
	virtual void register_status_information_callback_v25(std::function<void(PaceBmsProtocolV25::StatusInformation&)> callback);
	virtual void register_hardware_version_callback_v25(std::function<void(std::string&)> callback);
	virtual void register_serial_number_callback_v25(std::function<void(std::string&) > callback);
	virtual void register_protocols_callback_v25(std::function<void(PaceBmsProtocolV25::Protocols&) > callback);
	virtual void register_cell_over_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellOverVoltageConfiguration&)> callback);
	virtual void register_pack_over_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::PackOverVoltageConfiguration&)> callback);
	virtual void register_cell_under_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellUnderVoltageConfiguration&)> callback);
	virtual void register_pack_under_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::PackUnderVoltageConfiguration&)> callback);
	virtual void register_charge_over_current_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeOverCurrentConfiguration&)> callback);
	virtual void register_discharge_over_current1_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::DischargeOverCurrent1Configuration&)> callback);
	virtual void register_discharge_over_current2_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::DischargeOverCurrent2Configuration&)> callback);
	virtual void register_short_circuit_protection_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ShortCircuitProtectionConfiguration&)> callback);
	virtual void register_cell_balancing_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellBalancingConfiguration&)> callback);
	virtual void register_sleep_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::SleepConfiguration&)> callback);
	virtual void register_full_charge_low_charge_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::FullChargeLowChargeConfiguration&)> callback);
	virtual void register_charge_and_discharge_over_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeAndDischargeOverTemperatureConfiguration&)> callback);
	virtual void register_charge_and_discharge_under_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeAndDischargeUnderTemperatureConfiguration&)> callback);
	virtual void register_mosfet_over_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::MosfetOverTemperatureConfiguration&)> callback);
	virtual void register_environment_over_under_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::EnvironmentOverUnderTemperatureConfiguration&)> callback);
	virtual void register_system_datetime_callback_v25(std::function<void(PaceBmsProtocolV25::DateTime&)> callback);

	virtual void register_analog_information_callback_v20(std::function<void(PaceBmsProtocolV20::AnalogInformation&)> callback);
	virtual void register_status_information_callback_v20(std::function<void(PaceBmsProtocolV20::StatusInformation&)> callback);
	virtual void register_hardware_version_callback_v20(std::function<void(std::string&)> callback);
	virtual void register_serial_number_callback_v20(std::function<void(std::string&) > callback);
	virtual void register_system_datetime_callback_v20(std::function<void(PaceBmsProtocolV20::DateTime&)> callback);

	// child sensors call these to schedule new values be written out to the hardware
	virtual void write_switch_state_v25(PaceBmsProtocolV25::SwitchCommand state);
	virtual void write_mosfet_state_v25(PaceBmsProtocolV25::MosfetType type, PaceBmsProtocolV25::MosfetState state);
	virtual void write_shutdown_v25();
	virtual void write_protocols_v25(PaceBmsProtocolV25::Protocols& protocols);
	virtual void write_cell_over_voltage_configuration_v25(PaceBmsProtocolV25::CellOverVoltageConfiguration& config);
	virtual void write_pack_over_voltage_configuration_v25(PaceBmsProtocolV25::PackOverVoltageConfiguration& config);
	virtual void write_cell_under_voltage_configuration_v25(PaceBmsProtocolV25::CellUnderVoltageConfiguration& config);
	virtual void write_pack_under_voltage_configuration_v25(PaceBmsProtocolV25::PackUnderVoltageConfiguration& config);
	virtual void write_charge_over_current_configuration_v25(PaceBmsProtocolV25::ChargeOverCurrentConfiguration& config);
	virtual void write_discharge_over_current1_configuration_v25(PaceBmsProtocolV25::DischargeOverCurrent1Configuration& config);
	virtual void write_discharge_over_current2_configuration_v25(PaceBmsProtocolV25::DischargeOverCurrent2Configuration& config);
	virtual void write_short_circuit_protection_configuration_v25(PaceBmsProtocolV25::ShortCircuitProtectionConfiguration& config);
	virtual void write_cell_balancing_configuration_v25(PaceBmsProtocolV25::CellBalancingConfiguration& config);
	virtual void write_sleep_configuration_v25(PaceBmsProtocolV25::SleepConfiguration& config);
	virtual void write_full_charge_low_charge_configuration_v25(PaceBmsProtocolV25::FullChargeLowChargeConfiguration& config);
	virtual void write_charge_and_discharge_over_temperature_configuration_v25(PaceBmsProtocolV25::ChargeAndDischargeOverTemperatureConfiguration& config);
	virtual void write_charge_and_discharge_under_temperature_configuration_v25(PaceBmsProtocolV25::ChargeAndDischargeUnderTemperatureConfiguration& config);
	virtual void write_mosfet_over_temperature_configuration_v25(PaceBmsProtocolV25::MosfetOverTemperatureConfiguration& config);
	virtual void write_environment_over_under_temperature_configuration_v25(PaceBmsProtocolV25::EnvironmentOverUnderTemperatureConfiguration& config);
	virtual void write_system_datetime_v25(PaceBmsProtocolV25::DateTime& dt);

	virtual void write_shutdown_v20();
	virtual void write_system_datetime_v20(PaceBmsProtocolV20::DateTime& dt);

protected:
	pace_bms_master::PaceBmsMaster* parent_{ nullptr };
};

}  // namespace pace_bms_slave
}  // namespace esphome
