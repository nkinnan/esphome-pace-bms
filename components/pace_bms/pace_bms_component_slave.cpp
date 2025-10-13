
#include "pace_bms_component_slave.h"

// had to do a forard declaration in the header to avoid circular dependency
#include "pace_bms_component_master.h"

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
	ESP_LOGCONFIG(TAG, "  yaml id: %s", this->get_id_name().c_str());
	ESP_LOGCONFIG(TAG, "  Address: %i", this->address_);
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
void PaceBmsSlave::queue_sensor_update(std::function<void()> update) { this->parent_->queue_sensor_update(update); }

// child sensors call these to register for notification upon reciept of various types of data from the BMS, and the 
//     callbacks lists not being empty is what prompts update() to queue command_items for BMS communication in order to 
//     periodically gather these updates for fan-out to the sensors the first place
void PaceBmsSlave::register_bms_count_callback_v25(std::function<void(uint8_t&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_payload_count_callback_v25(std::function<void(uint8_t&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
// these are the ONLY TWO IMPLEMENTED CALLBACKS for slaves
void PaceBmsSlave::register_analog_information_callback_v25(std::function<void(PaceBmsProtocolV25::AnalogInformation&)> callback) { analog_information_callbacks_v25_.push_back(std::move(callback)); }
void PaceBmsSlave::register_status_information_callback_v25(std::function<void(PaceBmsProtocolV25::StatusInformation&)> callback) { status_information_callbacks_v25_.push_back(std::move(callback)); }
void PaceBmsSlave::register_hardware_version_callback_v25(std::function<void(std::string&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_serial_number_callback_v25(std::function<void(std::string&) > callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_protocols_callback_v25(std::function<void(PaceBmsProtocolV25::Protocols&) > callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_cell_over_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellOverVoltageConfiguration&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_pack_over_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::PackOverVoltageConfiguration&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_cell_under_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellUnderVoltageConfiguration&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_pack_under_voltage_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::PackUnderVoltageConfiguration&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_charge_over_current_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeOverCurrentConfiguration&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_discharge_over_current1_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::DischargeOverCurrent1Configuration&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_discharge_over_current2_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::DischargeOverCurrent2Configuration&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_short_circuit_protection_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ShortCircuitProtectionConfiguration&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_cell_balancing_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::CellBalancingConfiguration&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_sleep_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::SleepConfiguration&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_full_charge_low_charge_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::FullChargeLowChargeConfiguration&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_charge_and_discharge_over_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeAndDischargeOverTemperatureConfiguration&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_charge_and_discharge_under_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::ChargeAndDischargeUnderTemperatureConfiguration&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_mosfet_over_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::MosfetOverTemperatureConfiguration&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_environment_over_under_temperature_configuration_callback_v25(std::function<void(PaceBmsProtocolV25::EnvironmentOverUnderTemperatureConfiguration&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_system_datetime_callback_v25(std::function<void(PaceBmsProtocolV25::DateTime&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }

void PaceBmsSlave::register_analog_information_callback_v20(std::function<void(PaceBmsProtocolV20::AnalogInformation&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_status_information_callback_v20(std::function<void(PaceBmsProtocolV20::StatusInformation&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_hardware_version_callback_v20(std::function<void(std::string&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_serial_number_callback_v20(std::function<void(std::string&) > callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }
void PaceBmsSlave::register_system_datetime_callback_v20(std::function<void(PaceBmsProtocolV20::DateTime&)> callback) { ESP_LOGE(TAG, "non-analog/status information commands not available for slave devices"); }

// child sensors call these to schedule new values be written out to the hardware
void PaceBmsSlave::write_switch_state_v25(PaceBmsProtocolV25::SwitchCommand state) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_mosfet_state_v25(PaceBmsProtocolV25::MosfetType type, PaceBmsProtocolV25::MosfetState state) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_shutdown_v25() { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_protocols_v25(PaceBmsProtocolV25::Protocols& protocols) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_cell_over_voltage_configuration_v25(PaceBmsProtocolV25::CellOverVoltageConfiguration& config) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_pack_over_voltage_configuration_v25(PaceBmsProtocolV25::PackOverVoltageConfiguration& config) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_cell_under_voltage_configuration_v25(PaceBmsProtocolV25::CellUnderVoltageConfiguration& config) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_pack_under_voltage_configuration_v25(PaceBmsProtocolV25::PackUnderVoltageConfiguration& config) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_charge_over_current_configuration_v25(PaceBmsProtocolV25::ChargeOverCurrentConfiguration& config) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_discharge_over_current1_configuration_v25(PaceBmsProtocolV25::DischargeOverCurrent1Configuration& config) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_discharge_over_current2_configuration_v25(PaceBmsProtocolV25::DischargeOverCurrent2Configuration& config) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_short_circuit_protection_configuration_v25(PaceBmsProtocolV25::ShortCircuitProtectionConfiguration& config) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_cell_balancing_configuration_v25(PaceBmsProtocolV25::CellBalancingConfiguration& config) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_sleep_configuration_v25(PaceBmsProtocolV25::SleepConfiguration& config) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_full_charge_low_charge_configuration_v25(PaceBmsProtocolV25::FullChargeLowChargeConfiguration& config) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_charge_and_discharge_over_temperature_configuration_v25(PaceBmsProtocolV25::ChargeAndDischargeOverTemperatureConfiguration& config) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_charge_and_discharge_under_temperature_configuration_v25(PaceBmsProtocolV25::ChargeAndDischargeUnderTemperatureConfiguration& config) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_mosfet_over_temperature_configuration_v25(PaceBmsProtocolV25::MosfetOverTemperatureConfiguration& config) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_environment_over_under_temperature_configuration_v25(PaceBmsProtocolV25::EnvironmentOverUnderTemperatureConfiguration& config) { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_system_datetime_v25(PaceBmsProtocolV25::DateTime& dt) { ESP_LOGE(TAG, "write commands not available for slave devices"); }

void PaceBmsSlave::write_shutdown_v20() { ESP_LOGE(TAG, "write commands not available for slave devices"); }
void PaceBmsSlave::write_system_datetime_v20(PaceBmsProtocolV20::DateTime& dt) { ESP_LOGE(TAG, "write commands not available for slave devices"); }

}  // namespace pace_bms_slave
}  // namespace esphome
