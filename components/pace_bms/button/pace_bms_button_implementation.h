#pragma once

#include "esphome/core/component.h"
#include "esphome/components/button/button.h"

namespace esphome {
namespace pace_bms {

class PaceBmsButtonImplementation : public Component, public button::Button {
 public:
	 float get_setup_priority() const { return setup_priority::DATA; }

 protected:
  // the only purpose of this class is to fill in this pure virtual so the class is instantiable
	 void press_action() override;
};

}  // namespace pace_bms
}  // namespace esphome
