#pragma once

#include "esphome/core/component.h"
#include "esphome/components/pace_bms/pace_bms.h"
//#include "esphome/components/switch/switch.h"
#include "esphome/components/pace_bms/switch_implementation/pace_bms_switch_implementation.h"

#include "../pace_bms_v25.h"

namespace esphome {
namespace pace_bms {

// forward declaration, we only need the pointer
// PaceBmsSwitchImplementation calls our switch_state_changed() method but we only call it's base switch::Switch methods
class PaceBmsSwitchImplementation;

class PaceBmsSwitch : public Component {
 public:
  void set_parent(PaceBms *parent) { parent_ = parent; }

  // switch states are actually in the status information response, so subscribe to that to update switch state
  void set_buzzer_switch(switch::Switch* buzzer_switch) { this->buzzer_switch_ = buzzer_switch; request_status_information_callback_ = true; }

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;

  void status_information_callback(PaceBmsV25::StatusInformation& status_information);

  void child_switch_state_changed(PaceBmsSwitchImplementation* child_switch, bool state);

 protected:
  pace_bms::PaceBms *parent_;

  pace_bms::PaceBmsSwitchImplementation* buzzer_switch_{ nullptr };

  bool request_status_information_callback_ = false;
};

}  // namespace pace_bms
}  // namespace esphome
