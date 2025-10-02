
#include "pace_bms_slave_component.h"

// had to do a forard declaration in the header to avoid circular dependency
#include "pace_bms_master_component.h"

namespace esphome {
namespace pace_bms_slave {

static const char* const TAG = "pace_bms_slave";

/*
* log configuration
*/

void PaceBmsSlave::dump_config() {
	ESP_LOGCONFIG(TAG, "pace_bms_slave:");
	ESP_LOGCONFIG(TAG, "  pace_bms master yaml id: %s", this->parent_->get_id_name().c_str());
	ESP_LOGCONFIG(TAG, "  pace_bms master address: %i", this->parent_->get_address());
}

/*
* setup this component
*/

void PaceBmsSlave::setup() {
	this->parent_->register_slave(this);
}

/*
* main processing loop
*/

void PaceBmsSlave::loop() {
}

/* 
* these must be in here instead of the header since we forward-declared PaceBmsMaster
*/

// make accessible to sensors
int PaceBmsSlave::get_protocol_commandset() { return this->parent_->get_protocol_commandset(); }
// we don't push all updates in a single loop, that'd stall the ESP out
void PaceBmsSlave::queue_sensor_update(std::function<void()> update) { this->parent_->sensor_update_queue_.push(update); }

// child sensors call these to register for notification upon reciept of various types of data from the BMS, and the 
//     callbacks lists not being empty is what prompts update() to queue command_items for BMS communication in order to 
//     periodically gather these updates for fan-out to the sensors the first place
void PaceBmsSlave::register_analog_information_callback_v25(std::function<void(PaceBmsProtocolV25::AnalogInformation&)> callback) { this->parent_->register_analog_information_callback_v25(callback); }
void PaceBmsSlave::register_status_information_callback_v25(std::function<void(PaceBmsProtocolV25::StatusInformation&)> callback) { this->parent_->register_status_information_callback_v25(callback); }
void PaceBmsSlave::register_hardware_version_callback_v25(std::function<void(std::string&)> callback) { this->parent_->register_hardware_version_callback_v25(callback); }
void PaceBmsSlave::register_serial_number_callback_v25(std::function<void(std::string&) > callback) { this->parent_->register_serial_number_callback_v25(callback); }
void PaceBmsSlave::register_protocols_callback_v25(std::function<void(PaceBmsProtocolV25::Protocols&) > callback) { this->parent_->register_protocols_callback_v25(callback); }
void PaceBmsSlave::register_cell_over_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellOverVoltageConfiguration&)> callback) { this->parent_->register_cell_over_voltage_configuration_callback_v25(callback); }
void PaceBmsSlave::register_pack_over_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::PackOverVoltageConfiguration&)> callback) { this->parent_->register_pack_over_voltage_configuration_callback_v25(callback); }
void PaceBmsSlave::register_cell_under_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellUnderVoltageConfiguration&)> callback) { this->parent_->register_cell_under_voltage_configuration_callback_v25(callback); }
void PaceBmsSlave::register_pack_under_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::PackUnderVoltageConfiguration&)> callback) { this->parent_->register_pack_under_voltage_configuration_callback_v25(callback); }
void PaceBmsSlave::register_charge_over_current_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeOverCurrentConfiguration&)> callback) { this->parent_->register_charge_over_current_configuration_callback_v25(callback); }
void PaceBmsSlave::register_discharge_over_current1_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::DischargeOverCurrent1Configuration&)> callback) { this->parent_->register_discharge_over_current1_configuration_callback_v25(callback); }
void PaceBmsSlave::register_discharge_over_current2_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::DischargeOverCurrent2Configuration&)> callback) { this->parent_->register_discharge_over_current2_configuration_callback_v25(callback); }
void PaceBmsSlave::register_short_circuit_protection_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ShortCircuitProtectionConfiguration&)> callback) { this->parent_->register_short_circuit_protection_configuration_callback_v25(callback); }
void PaceBmsSlave::register_cell_balancing_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellBalancingConfiguration&)> callback) { this->parent_->register_cell_balancing_configuration_callback_v25(callback); }
void PaceBmsSlave::register_sleep_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::SleepConfiguration&)> callback) { this->parent_->register_sleep_configuration_callback_v25(callback); }
void PaceBmsSlave::register_full_charge_low_charge_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::FullChargeLowChargeConfiguration&)> callback) { this->parent_->register_full_charge_low_charge_configuration_callback_v25(callback); }
void PaceBmsSlave::register_charge_and_discharge_over_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeAndDischargeOverTemperatureConfiguration&)> callback) { this->parent_->register_charge_and_discharge_over_temperature_configuration_callback_v25(callback); }
void PaceBmsSlave::register_charge_and_discharge_under_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeAndDischargeUnderTemperatureConfiguration&)> callback) { this->parent_->register_charge_and_discharge_under_temperature_configuration_callback_v25(callback); }
void PaceBmsSlave::register_mosfet_over_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::MosfetOverTemperatureConfiguration&)> callback) { this->parent_->register_mosfet_over_temperature_configuration_callback_v25(callback); }
void PaceBmsSlave::register_environment_over_under_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::EnvironmentOverUnderTemperatureConfiguration&)> callback) { this->parent_->register_environment_over_under_temperature_configuration_callback_v25(callback); }
void PaceBmsSlave::register_system_datetime_callback_v25(std::function<void(PaceBmsProtocolV25::DateTime&)> callback) { this->parent_->register_system_datetime_callback_v25(callback); }

void PaceBmsSlave::register_analog_information_callback_v20(std::function<void(PaceBmsProtocolV20::AnalogInformation&)> callback) { this->parent_->register_analog_information_callback_v20(callback); }
void PaceBmsSlave::register_status_information_callback_v20(std::function<void(PaceBmsProtocolV20::StatusInformation&)> callback) { this->parent_->register_status_information_callback_v20(callback); }
void PaceBmsSlave::register_hardware_version_callback_v20(std::function<void(std::string&)> callback) { this->parent_->register_hardware_version_callback_v20(callback); }
void PaceBmsSlave::register_serial_number_callback_v20(std::function<void(std::string&) > callback) { this->parent_->register_serial_number_callback_v20(callback); }
void PaceBmsSlave::register_system_datetime_callback_v20(std::function<void(PaceBmsProtocolV20::DateTime&)> callback) { this->parent_->register_system_datetime_callback_v20(callback); }

// child sensors call these to schedule new values be written out to the hardware
void PaceBmsSlave::write_switch_state_v25(PaceBmsProtocolV25::SwitchCommand state) { this->parent_->write_switch_state_v25(state); }
void PaceBmsSlave::write_mosfet_state_v25(PaceBmsProtocolV25::MosfetType type, PaceBmsProtocolV25::MosfetState state) { this->parent_->write_mosfet_state_v25(type, state); }
void PaceBmsSlave::write_shutdown_v25() { this->parent_->write_shutdown_v25(); }
void PaceBmsSlave::write_protocols_v25(PaceBmsProtocolV25::Protocols& protocols) { this->parent_->write_protocols_v25(protocols); }
void PaceBmsSlave::write_cell_over_voltage_configuration_v25(PaceBmsProtocolV25::CellOverVoltageConfiguration& config) { this->parent_->write_cell_over_voltage_configuration_v25(config); }
void PaceBmsSlave::write_pack_over_voltage_configuration_v25(PaceBmsProtocolV25::PackOverVoltageConfiguration& config) { this->parent_->write_pack_over_voltage_configuration_v25(config); }
void PaceBmsSlave::write_cell_under_voltage_configuration_v25(PaceBmsProtocolV25::CellUnderVoltageConfiguration& config) { this->parent_->write_cell_under_voltage_configuration_v25(config); }
void PaceBmsSlave::write_pack_under_voltage_configuration_v25(PaceBmsProtocolV25::PackUnderVoltageConfiguration& config) { this->parent_->write_pack_under_voltage_configuration_v25(config); }
void PaceBmsSlave::write_charge_over_current_configuration_v25(PaceBmsProtocolV25::ChargeOverCurrentConfiguration& config) { this->parent_->write_charge_over_current_configuration_v25(config); }
void PaceBmsSlave::write_discharge_over_current1_configuration_v25(PaceBmsProtocolV25::DischargeOverCurrent1Configuration& config) { this->parent_->write_discharge_over_current1_configuration_v25(config); }
void PaceBmsSlave::write_discharge_over_current2_configuration_v25(PaceBmsProtocolV25::DischargeOverCurrent2Configuration& config) { this->parent_->write_discharge_over_current2_configuration_v25(config); }
void PaceBmsSlave::write_short_circuit_protection_configuration_v25(PaceBmsProtocolV25::ShortCircuitProtectionConfiguration& config) { this->parent_->write_short_circuit_protection_configuration_v25(config); }
void PaceBmsSlave::write_cell_balancing_configuration_v25(PaceBmsProtocolV25::CellBalancingConfiguration& config) { this->parent_->write_cell_balancing_configuration_v25(config); }
void PaceBmsSlave::write_sleep_configuration_v25(PaceBmsProtocolV25::SleepConfiguration& config) { this->parent_->write_sleep_configuration_v25(config); }
void PaceBmsSlave::write_full_charge_low_charge_configuration_v25(PaceBmsProtocolV25::FullChargeLowChargeConfiguration& config) { this->parent_->write_full_charge_low_charge_configuration_v25(config); }
void PaceBmsSlave::write_charge_and_discharge_over_temperature_configuration_v25(PaceBmsProtocolV25::ChargeAndDischargeOverTemperatureConfiguration& config) { this->parent_->write_charge_and_discharge_over_temperature_configuration_v25(config); }
void PaceBmsSlave::write_charge_and_discharge_under_temperature_configuration_v25(PaceBmsProtocolV25::ChargeAndDischargeUnderTemperatureConfiguration& config) { this->parent_->write_charge_and_discharge_under_temperature_configuration_v25(config); }
void PaceBmsSlave::write_mosfet_over_temperature_configuration_v25(PaceBmsProtocolV25::MosfetOverTemperatureConfiguration& config) { this->parent_->write_mosfet_over_temperature_configuration_v25(config); }
void PaceBmsSlave::write_environment_over_under_temperature_configuration_v25(PaceBmsProtocolV25::EnvironmentOverUnderTemperatureConfiguration& config) { this->parent_->write_environment_over_under_temperature_configuration_v25(config); }
void PaceBmsSlave::write_system_datetime_v25(PaceBmsProtocolV25::DateTime& dt) { this->parent_->write_system_datetime_v25(dt); }

void PaceBmsSlave::write_shutdown_v20() { this->parent_->write_shutdown_v20(); }
void PaceBmsSlave::write_system_datetime_v20(PaceBmsProtocolV20::DateTime& dt) { this->parent_->write_system_datetime_v20(dt); }

}  // namespace pace_bms_slave
}  // namespace esphome
