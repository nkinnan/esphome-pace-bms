#pragma once

#include "esphome/core/component.h"
#include "esphome/components/pace_bms/pace_bms.h"
#include "pace_bms_number_implementation.h"

#include "../pace_bms_v25.h"

namespace esphome {
namespace pace_bms {

class PaceBmsNumber : public Component {
 public:
  void set_parent(PaceBms *parent) { parent_ = parent; }

  void set_cell_over_voltage_alarm_number(PaceBmsNumberImplementation* cell_over_voltage_alarm_number) { this->cell_over_voltage_alarm_number_ = cell_over_voltage_alarm_number; request_cell_over_voltage_configuration_callback_ = true; }
  void set_cell_over_voltage_protection_number(PaceBmsNumberImplementation* cell_over_voltage_protection_number) { this->cell_over_voltage_protection_number_ = cell_over_voltage_protection_number; request_cell_over_voltage_configuration_callback_ = true; }
  void set_cell_over_voltage_protection_release_number(PaceBmsNumberImplementation* cell_over_voltage_protection_release_number) { this->cell_over_voltage_protection_release_number_ = cell_over_voltage_protection_release_number; request_cell_over_voltage_configuration_callback_ = true; }
  void set_cell_over_voltage_protection_delay_number(PaceBmsNumberImplementation* cell_over_voltage_protection_delay_number) { this->cell_over_voltage_protection_delay_number_ = cell_over_voltage_protection_delay_number; request_cell_over_voltage_configuration_callback_ = true; }

  void set_pack_over_voltage_alarm_number(PaceBmsNumberImplementation* number) { this->pack_over_voltage_alarm_number_ = number; request_pack_over_voltage_configuration_callback_ = true; }
  void set_pack_over_voltage_protection_number(PaceBmsNumberImplementation* number) { this->pack_over_voltage_protection_number_ = number; request_pack_over_voltage_configuration_callback_ = true; }
  void set_pack_over_voltage_protection_release_number(PaceBmsNumberImplementation* number) { this->pack_over_voltage_protection_release_number_ = number; request_pack_over_voltage_configuration_callback_ = true; }
  void set_pack_over_voltage_protection_delay_number(PaceBmsNumberImplementation* number) { this->pack_over_voltage_protection_delay_number_ = number; request_pack_over_voltage_configuration_callback_ = true; }

  void set_cell_under_voltage_alarm_number(PaceBmsNumberImplementation* cell_under_voltage_alarm_number) { this->cell_under_voltage_alarm_number_ = cell_under_voltage_alarm_number; request_cell_under_voltage_configuration_callback_ = true; }
  void set_cell_under_voltage_protection_number(PaceBmsNumberImplementation* cell_under_voltage_protection_number) { this->cell_under_voltage_protection_number_ = cell_under_voltage_protection_number; request_cell_under_voltage_configuration_callback_ = true; }
  void set_cell_under_voltage_protection_release_number(PaceBmsNumberImplementation* cell_under_voltage_protection_release_number) { this->cell_under_voltage_protection_release_number_ = cell_under_voltage_protection_release_number; request_cell_under_voltage_configuration_callback_ = true; }
  void set_cell_under_voltage_protection_delay_number(PaceBmsNumberImplementation* cell_under_voltage_protection_delay_number) { this->cell_under_voltage_protection_delay_number_ = cell_under_voltage_protection_delay_number; request_cell_under_voltage_configuration_callback_ = true; }

  void set_pack_under_voltage_alarm_number(PaceBmsNumberImplementation* number) { this->pack_under_voltage_alarm_number_ = number; request_pack_under_voltage_configuration_callback_ = true; }
  void set_pack_under_voltage_protection_number(PaceBmsNumberImplementation* number) { this->pack_under_voltage_protection_number_ = number; request_pack_under_voltage_configuration_callback_ = true; }
  void set_pack_under_voltage_protection_release_number(PaceBmsNumberImplementation* number) { this->pack_under_voltage_protection_release_number_ = number; request_pack_under_voltage_configuration_callback_ = true; }
  void set_pack_under_voltage_protection_delay_number(PaceBmsNumberImplementation* number) { this->pack_under_voltage_protection_delay_number_ = number; request_pack_under_voltage_configuration_callback_ = true; }

  void set_charge_over_current_alarm_number(PaceBmsNumberImplementation* number) { this->charge_over_current_alarm_number_ = number; request_charge_over_current_callback_ = true; }
  void set_charge_over_current_protection_number(PaceBmsNumberImplementation* number) { this->charge_over_current_protection_number_ = number; request_charge_over_current_callback_ = true; }
  void set_charge_over_current_protection_delay_number(PaceBmsNumberImplementation* number) { this->charge_over_current_protection_delay_number_ = number; request_charge_over_current_callback_ = true; }

  void set_discharge_over_current1_alarm_number(PaceBmsNumberImplementation* number) { this->discharge_over_current1_alarm_number_ = number; request_discharge_over_current1_callback_ = true; }
  void set_discharge_over_current1_protection_number(PaceBmsNumberImplementation* number) { this->discharge_over_current1_protection_number_ = number; request_discharge_over_current1_callback_ = true; }
  void set_discharge_over_current1_protection_delay_number(PaceBmsNumberImplementation* number) { this->discharge_over_current1_protection_delay_number_ = number; request_discharge_over_current1_callback_ = true; }

  void set_discharge_over_current2_protection_number(PaceBmsNumberImplementation* number) { this->discharge_over_current2_protection_number_ = number; request_discharge_over_current2_callback_ = true; }
  void set_discharge_over_current2_protection_delay_number(PaceBmsNumberImplementation* number) { this->discharge_over_current2_protection_delay_number_ = number; request_discharge_over_current2_callback_ = true; }

  void set_short_circuit_protection_delay_number(PaceBmsNumberImplementation* number) { this->short_circuit_protection_delay_number_ = number; request_short_circuit_callback_ = true; }

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;

 protected:
  pace_bms::PaceBms *parent_;

  void cell_over_voltage_configuration_callback(PaceBmsV25::CellOverVoltageConfiguration& configuration);
  bool request_cell_over_voltage_configuration_callback_{ false };
  PaceBmsV25::CellOverVoltageConfiguration cell_over_voltage_configuration_;
  bool cell_over_voltage_configuration_seen_{ false };
  pace_bms::PaceBmsNumberImplementation* cell_over_voltage_alarm_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* cell_over_voltage_protection_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* cell_over_voltage_protection_release_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* cell_over_voltage_protection_delay_number_{ nullptr };

  void pack_over_voltage_configuration_callback(PaceBmsV25::PackOverVoltageConfiguration& configuration);
  bool request_pack_over_voltage_configuration_callback_{ false };
  PaceBmsV25::PackOverVoltageConfiguration pack_over_voltage_configuration_;
  bool pack_over_voltage_configuration_seen_{ false };
  pace_bms::PaceBmsNumberImplementation* pack_over_voltage_alarm_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* pack_over_voltage_protection_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* pack_over_voltage_protection_release_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* pack_over_voltage_protection_delay_number_{ nullptr };

  void cell_under_voltage_configuration_callback(PaceBmsV25::CellUnderVoltageConfiguration& configuration);
  bool request_cell_under_voltage_configuration_callback_{ false };
  PaceBmsV25::CellUnderVoltageConfiguration cell_under_voltage_configuration_;
  bool cell_under_voltage_configuration_seen_{ false };
  pace_bms::PaceBmsNumberImplementation* cell_under_voltage_alarm_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* cell_under_voltage_protection_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* cell_under_voltage_protection_release_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* cell_under_voltage_protection_delay_number_{ nullptr };

  void pack_under_voltage_configuration_callback(PaceBmsV25::PackUnderVoltageConfiguration& configuration);
  bool request_pack_under_voltage_configuration_callback_{ false };
  PaceBmsV25::PackUnderVoltageConfiguration pack_under_voltage_configuration_;
  bool pack_under_voltage_configuration_seen_{ false };
  pace_bms::PaceBmsNumberImplementation* pack_under_voltage_alarm_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* pack_under_voltage_protection_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* pack_under_voltage_protection_release_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* pack_under_voltage_protection_delay_number_{ nullptr };

  void charge_over_current_configuration_callback(PaceBmsV25::ChargeOverCurrentConfiguration& configuration);
  bool request_charge_over_current_configuration_callback_{ false };
  PaceBmsV25::ChargeOverCurrentConfiguration charge_over_current_configuration_;
  bool charge_over_current_configuration_seen_{ false };
  pace_bms::PaceBmsNumberImplementation* charge_over_current_alarm_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* charge_over_current_protection_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* charge_over_current_protection_delay_number_{ nullptr };

  void discharge_over_current1_configuration_callback(PaceBmsV25::DischargeOverCurrent1Configuration& configuration);
  bool request_discharge_over_current1_configuration_callback_{ false };
  PaceBmsV25::DischargeOverCurrent1Configuration discharge_over_current1_configuration_;
  bool discharge_over_current1_configuration_seen_{ false };
  pace_bms::PaceBmsNumberImplementation* discharge_over_current1_alarm_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* discharge_over_current1_protection_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* discharge_over_current1_protection_delay_number_{ nullptr };

  void discharge_over_current2_configuration_callback(PaceBmsV25::DischargeOverCurrent2Configuration& configuration);
  bool request_discharge_over_current2_configuration_callback_{ false };
  PaceBmsV25::DischargeOverCurrent2Configuration discharge_over_current2_configuration_;
  bool discharge_over_current2_configuration_seen_{ false };
  pace_bms::PaceBmsNumberImplementation* discharge_over_current2_protection_number_{ nullptr };
  pace_bms::PaceBmsNumberImplementation* discharge_over_current2_protection_delay_number_{ nullptr };

  void short_circuit_configuration_callback(PaceBmsV25::ShortCircuitProtectionConfiguration& configuration);
  bool request_short_circuit_configuration_callback_{ false };
  PaceBmsV25::ShortCircuitProtectionConfiguration short_circuit_configuration_;
  bool short_circuit_configuration_seen_{ false };
  pace_bms::PaceBmsNumberImplementation* short_circuit_protection_delay_number_{ nullptr };
};

}  // namespace pace_bms
}  // namespace esphome



