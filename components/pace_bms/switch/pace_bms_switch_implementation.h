#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"


namespace esphome {
namespace pace_bms {

	//class PaceBmsSwitch;

// the only purpose of this class is to fill in the pure virtual and call the parent container component on state change
class PaceBmsSwitchImplementation : public switch_::Switch {
 public:
 // void set_parent(PaceBmsSwitch *parent) { parent_ = parent; }

	 void setup() override;
	 float get_setup_priority() const override;
	 void dump_config() override;

 protected:
  //pace_bms::PaceBmsSwitch *parent_;

  void write_state(bool state) override;

};

}  // namespace pace_bms
}  // namespace esphome
