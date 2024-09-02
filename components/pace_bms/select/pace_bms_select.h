#pragma once

#include "esphome/core/component.h"
#include "esphome/components/pace_bms/pace_bms.h"
#include "pace_bms_select_implementation.h"

#include "../pace_bms_v25.h"

namespace esphome {
namespace pace_bms {

class PaceBmsSelect : public Component {
 public:
  void set_parent(PaceBms *parent) { parent_ = parent; }

  void set_charge_current_limiter_gear_select(PaceBmsSelectImplementation* charge_current_limiter_gear_select) { this->charge_current_limiter_gear_select_ = charge_current_limiter_gear_select; request_status_information_callback_ = true; }

  void set_protocol_can_select(PaceBmsSelectImplementation* protocol_can_select) { this->protocol_can_select_ = protocol_can_select; request_protocols_callback_ = true; }
  void set_protocol_rs485_select(PaceBmsSelectImplementation* protocol_rs485_select) { this->protocol_rs485_select_ = protocol_rs485_select; request_protocols_callback_ = true; }
  void set_protocol_type_select(PaceBmsSelectImplementation* protocol_type_select) { this->protocol_type_select_ = protocol_type_select; request_protocols_callback_ = true; }

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;

 protected:
  pace_bms::PaceBms *parent_;

  bool request_status_information_callback_ = false;
  void status_information_callback(PaceBmsV25::StatusInformation& status_information);

  pace_bms::PaceBmsSelectImplementation* charge_current_limiter_gear_select_{ nullptr };

  bool request_protocols_callback_ = false;
  void protocols_callback(PaceBmsV25::Protocols& protocols);
  PaceBmsV25::Protocols last_seen_protocols;
  bool protocols_seen = false;

  pace_bms::PaceBmsSelectImplementation* protocol_can_select_{ nullptr };
  pace_bms::PaceBmsSelectImplementation* protocol_rs485_select_{ nullptr };
  pace_bms::PaceBmsSelectImplementation* protocol_type_select_{ nullptr };
};

}  // namespace pace_bms
}  // namespace esphome

