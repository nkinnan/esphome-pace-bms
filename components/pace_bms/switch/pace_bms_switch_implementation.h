#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/pace_bms/switch/pace_bms_switch.h"


namespace esphome {
namespace pace_bms {

// the only purpose of this class is to fill in the pure virtual and call the parent container component on state change
class PaceBmsSwitchImplementation : public switch_::Switch {
 public:
  void set_parent(PaceBmsSwitch *parent) { parent_ = parent; }

 protected:
  pace_bms::PaceBmsSwitch *parent_;

  void write_state(bool state) override;

};

}  // namespace pace_bms
}  // namespace esphome
