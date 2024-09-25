#pragma once

#include "esphome/core/component.h"
#include "esphome/components/pace_bms/pace_bms_component.h"

#include "pace_bms_number_implementation.h"

#include "../pace_bms_protocol_v25.h"

namespace esphome {
namespace pace_bms {

class PaceBmsNumber : public Component {
public:
	void set_parent(PaceBms* parent) { parent_ = parent; }

	void set_cell_over_voltage_alarm_number(PaceBmsNumberImplementation* number) { this->cell_over_voltage_alarm_number_ = number; }
	void set_cell_over_voltage_protection_number(PaceBmsNumberImplementation* number) { this->cell_over_voltage_protection_number_ = number; }
	void set_cell_over_voltage_protection_release_number(PaceBmsNumberImplementation* number) { this->cell_over_voltage_protection_release_number_ = number; }
	void set_cell_over_voltage_protection_delay_number(PaceBmsNumberImplementation* number) { this->cell_over_voltage_protection_delay_number_ = number; }

	void set_pack_over_voltage_alarm_number(PaceBmsNumberImplementation* number) { this->pack_over_voltage_alarm_number_ = number; }
	void set_pack_over_voltage_protection_number(PaceBmsNumberImplementation* number) { this->pack_over_voltage_protection_number_ = number; }
	void set_pack_over_voltage_protection_release_number(PaceBmsNumberImplementation* number) { this->pack_over_voltage_protection_release_number_ = number; }
	void set_pack_over_voltage_protection_delay_number(PaceBmsNumberImplementation* number) { this->pack_over_voltage_protection_delay_number_ = number; }

	void set_cell_under_voltage_alarm_number(PaceBmsNumberImplementation* number) { this->cell_under_voltage_alarm_number_ = number; }
	void set_cell_under_voltage_protection_number(PaceBmsNumberImplementation* number) { this->cell_under_voltage_protection_number_ = number; }
	void set_cell_under_voltage_protection_release_number(PaceBmsNumberImplementation* number) { this->cell_under_voltage_protection_release_number_ = number; }
	void set_cell_under_voltage_protection_delay_number(PaceBmsNumberImplementation* number) { this->cell_under_voltage_protection_delay_number_ = number; }

	void set_pack_under_voltage_alarm_number(PaceBmsNumberImplementation* number) { this->pack_under_voltage_alarm_number_ = number; }
	void set_pack_under_voltage_protection_number(PaceBmsNumberImplementation* number) { this->pack_under_voltage_protection_number_ = number; }
	void set_pack_under_voltage_protection_release_number(PaceBmsNumberImplementation* number) { this->pack_under_voltage_protection_release_number_ = number; }
	void set_pack_under_voltage_protection_delay_number(PaceBmsNumberImplementation* number) { this->pack_under_voltage_protection_delay_number_ = number; }

	void set_charge_over_current_alarm_number(PaceBmsNumberImplementation* number) { this->charge_over_current_alarm_number_ = number; }
	void set_charge_over_current_protection_number(PaceBmsNumberImplementation* number) { this->charge_over_current_protection_number_ = number; }
	void set_charge_over_current_protection_delay_number(PaceBmsNumberImplementation* number) { this->charge_over_current_protection_delay_number_ = number; }

	void set_discharge_over_current1_alarm_number(PaceBmsNumberImplementation* number) { this->discharge_over_current1_alarm_number_ = number; }
	void set_discharge_over_current1_protection_number(PaceBmsNumberImplementation* number) { this->discharge_over_current1_protection_number_ = number; }
	void set_discharge_over_current1_protection_delay_number(PaceBmsNumberImplementation* number) { this->discharge_over_current1_protection_delay_number_ = number; }

	void set_discharge_over_current2_protection_number(PaceBmsNumberImplementation* number) { this->discharge_over_current2_protection_number_ = number; }
	void set_discharge_over_current2_protection_delay_number(PaceBmsNumberImplementation* number) { this->discharge_over_current2_protection_delay_number_ = number; }

	void set_short_circuit_protection_delay_number(PaceBmsNumberImplementation* number) { this->short_circuit_protection_delay_number_ = number; }

	void set_cell_balancing_threshold_number(PaceBmsNumberImplementation* number) { this->cell_balancing_threshold_number_ = number; }
	void set_cell_balancing_delta_number(PaceBmsNumberImplementation* number) { this->cell_balancing_delta_number_ = number; }

	void set_sleep_cell_voltage_number(PaceBmsNumberImplementation* number) { this->sleep_cell_voltage_number_ = number; }
	void set_sleep_delay_number(PaceBmsNumberImplementation* number) { this->sleep_delay_number_ = number; }

	void set_full_charge_voltage_number(PaceBmsNumberImplementation* number) { this->full_charge_voltage_number_ = number; }
	void set_full_charge_amps_number(PaceBmsNumberImplementation* number) { this->full_charge_amps_number_ = number; }
	void set_low_charge_alarm_percent_number(PaceBmsNumberImplementation* number) { this->low_charge_alarm_percent_number_ = number; }

	void set_charge_over_temperature_alarm_number(PaceBmsNumberImplementation* number) { this->charge_over_temperature_alarm_number_ = number; }
	void set_charge_over_temperature_protection_number(PaceBmsNumberImplementation* number) { this->charge_over_temperature_protection_number_ = number; }
	void set_charge_over_temperature_protection_release_number(PaceBmsNumberImplementation* number) { this->charge_over_temperature_protection_release_number_ = number; }

	void set_discharge_over_temperature_alarm_number(PaceBmsNumberImplementation* number) { this->discharge_over_temperature_alarm_number_ = number; }
	void set_discharge_over_temperature_protection_number(PaceBmsNumberImplementation* number) { this->discharge_over_temperature_protection_number_ = number; }
	void set_discharge_over_temperature_protection_release_number(PaceBmsNumberImplementation* number) { this->discharge_over_temperature_protection_release_number_ = number; }

	void set_charge_under_temperature_alarm_number(PaceBmsNumberImplementation* number) { this->charge_under_temperature_alarm_number_ = number; }
	void set_charge_under_temperature_protection_number(PaceBmsNumberImplementation* number) { this->charge_under_temperature_protection_number_ = number; }
	void set_charge_under_temperature_protection_release_number(PaceBmsNumberImplementation* number) { this->charge_under_temperature_protection_release_number_ = number; }

	void set_discharge_under_temperature_alarm_number(PaceBmsNumberImplementation* number) { this->discharge_under_temperature_alarm_number_ = number; }
	void set_discharge_under_temperature_protection_number(PaceBmsNumberImplementation* number) { this->discharge_under_temperature_protection_number_ = number; }
	void set_discharge_under_temperature_protection_release_number(PaceBmsNumberImplementation* number) { this->discharge_under_temperature_protection_release_number_ = number; }

	void set_mosfet_over_temperature_alarm_number(PaceBmsNumberImplementation* number) { this->mosfet_over_temperature_alarm_number_ = number; }
	void set_mosfet_over_temperature_protection_number(PaceBmsNumberImplementation* number) { this->mosfet_over_temperature_protection_number_ = number; }
	void set_mosfet_over_temperature_protection_release_number(PaceBmsNumberImplementation* number) { this->mosfet_over_temperature_protection_release_number_ = number; }

	void set_environment_under_temperature_alarm_number(PaceBmsNumberImplementation* number) { this->environment_under_temperature_alarm_number_ = number; }
	void set_environment_under_temperature_protection_number(PaceBmsNumberImplementation* number) { this->environment_under_temperature_protection_number_ = number; }
	void set_environment_under_temperature_protection_release_number(PaceBmsNumberImplementation* number) { this->environment_under_temperature_protection_release_number_ = number; }
	void set_environment_over_temperature_alarm_number(PaceBmsNumberImplementation* number) { this->environment_over_temperature_alarm_number_ = number; }
	void set_environment_over_temperature_protection_number(PaceBmsNumberImplementation* number) { this->environment_over_temperature_protection_number_ = number; }
	void set_environment_over_temperature_protection_release_number(PaceBmsNumberImplementation* number) { this->environment_over_temperature_protection_release_number_ = number; }


	void setup() override;
	float get_setup_priority() const { return setup_priority::DATA; }
	void dump_config() override;

protected:
	pace_bms::PaceBms* parent_;

	PaceBmsProtocolV25::CellOverVoltageConfiguration cell_over_voltage_configuration_;
	bool cell_over_voltage_configuration_seen_{ false };
	pace_bms::PaceBmsNumberImplementation* cell_over_voltage_alarm_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* cell_over_voltage_protection_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* cell_over_voltage_protection_release_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* cell_over_voltage_protection_delay_number_{ nullptr };

	PaceBmsProtocolV25::PackOverVoltageConfiguration pack_over_voltage_configuration_;
	bool pack_over_voltage_configuration_seen_{ false };
	pace_bms::PaceBmsNumberImplementation* pack_over_voltage_alarm_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* pack_over_voltage_protection_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* pack_over_voltage_protection_release_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* pack_over_voltage_protection_delay_number_{ nullptr };

	PaceBmsProtocolV25::CellUnderVoltageConfiguration cell_under_voltage_configuration_;
	bool cell_under_voltage_configuration_seen_{ false };
	pace_bms::PaceBmsNumberImplementation* cell_under_voltage_alarm_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* cell_under_voltage_protection_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* cell_under_voltage_protection_release_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* cell_under_voltage_protection_delay_number_{ nullptr };

	PaceBmsProtocolV25::PackUnderVoltageConfiguration pack_under_voltage_configuration_;
	bool pack_under_voltage_configuration_seen_{ false };
	pace_bms::PaceBmsNumberImplementation* pack_under_voltage_alarm_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* pack_under_voltage_protection_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* pack_under_voltage_protection_release_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* pack_under_voltage_protection_delay_number_{ nullptr };

	PaceBmsProtocolV25::ChargeOverCurrentConfiguration charge_over_current_configuration_;
	bool charge_over_current_configuration_seen_{ false };
	pace_bms::PaceBmsNumberImplementation* charge_over_current_alarm_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* charge_over_current_protection_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* charge_over_current_protection_delay_number_{ nullptr };

	PaceBmsProtocolV25::DischargeOverCurrent1Configuration discharge_over_current1_configuration_;
	bool discharge_over_current1_configuration_seen_{ false };
	pace_bms::PaceBmsNumberImplementation* discharge_over_current1_alarm_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* discharge_over_current1_protection_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* discharge_over_current1_protection_delay_number_{ nullptr };

	PaceBmsProtocolV25::DischargeOverCurrent2Configuration discharge_over_current2_configuration_;
	bool discharge_over_current2_configuration_seen_{ false };
	pace_bms::PaceBmsNumberImplementation* discharge_over_current2_protection_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* discharge_over_current2_protection_delay_number_{ nullptr };

	PaceBmsProtocolV25::ShortCircuitProtectionConfiguration short_circuit_protection_configuration_;
	bool short_circuit_protection_configuration_seen_{ false };
	pace_bms::PaceBmsNumberImplementation* short_circuit_protection_delay_number_{ nullptr };

	PaceBmsProtocolV25::CellBalancingConfiguration cell_balancing_configuration_;
	bool cell_balancing_configuration_seen_{ false };
	pace_bms::PaceBmsNumberImplementation* cell_balancing_threshold_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* cell_balancing_delta_number_{ nullptr };

	PaceBmsProtocolV25::SleepConfiguration sleep_configuration_;
	bool sleep_configuration_seen_{ false };
	pace_bms::PaceBmsNumberImplementation* sleep_cell_voltage_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* sleep_delay_number_{ nullptr };

	PaceBmsProtocolV25::FullChargeLowChargeConfiguration full_charge_low_charge_configuration_;
	bool full_charge_low_charge_configuration_seen_{ false };
	pace_bms::PaceBmsNumberImplementation* full_charge_voltage_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* full_charge_amps_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* low_charge_alarm_percent_number_{ nullptr };

	PaceBmsProtocolV25::ChargeAndDischargeOverTemperatureConfiguration charge_and_discharge_over_temperature_configuration_;
	bool charge_and_discharge_over_temperature_configuration_seen_{ false };
	pace_bms::PaceBmsNumberImplementation* charge_over_temperature_alarm_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* charge_over_temperature_protection_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* charge_over_temperature_protection_release_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* discharge_over_temperature_alarm_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* discharge_over_temperature_protection_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* discharge_over_temperature_protection_release_number_{ nullptr };

	PaceBmsProtocolV25::ChargeAndDischargeUnderTemperatureConfiguration charge_and_discharge_under_temperature_configuration_;
	bool charge_and_discharge_under_temperature_configuration_seen_{ false };
	pace_bms::PaceBmsNumberImplementation* charge_under_temperature_alarm_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* charge_under_temperature_protection_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* charge_under_temperature_protection_release_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* discharge_under_temperature_alarm_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* discharge_under_temperature_protection_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* discharge_under_temperature_protection_release_number_{ nullptr };

	PaceBmsProtocolV25::MosfetOverTemperatureConfiguration mosfet_over_temperature_configuration_;
	bool mosfet_over_temperature_configuration_seen_{ false };
	pace_bms::PaceBmsNumberImplementation* mosfet_over_temperature_alarm_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* mosfet_over_temperature_protection_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* mosfet_over_temperature_protection_release_number_{ nullptr };

	PaceBmsProtocolV25::EnvironmentOverUnderTemperatureConfiguration environment_over_under_temperature_configuration_;
	bool environment_over_under_temperature_configuration_seen_{ false };
	pace_bms::PaceBmsNumberImplementation* environment_under_temperature_alarm_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* environment_under_temperature_protection_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* environment_under_temperature_protection_release_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* environment_over_temperature_alarm_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* environment_over_temperature_protection_number_{ nullptr };
	pace_bms::PaceBmsNumberImplementation* environment_over_temperature_protection_release_number_{ nullptr };
};

}  // namespace pace_bms
}  // namespace esphome
