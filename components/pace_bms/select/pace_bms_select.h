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

  // current switch states are actually in the status information response, so subscribe to that in order to update switch state
  void set_buzzer_alarm_select(PaceBmsSelectImplementation* buzzer_select) { this->charge_current_limiter_gear_select_ = buzzer_select; request_status_information_callback_ = true; }

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;

 protected:
  pace_bms::PaceBms *parent_;
  bool request_status_information_callback_ = false;
  void status_information_callback(PaceBmsV25::StatusInformation& status_information);

  pace_bms::PaceBmsSelectImplementation* charge_current_limiter_gear_select_{ nullptr };
};

}  // namespace pace_bms
}  // namespace esphome
