#pragma once

#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace pace_bms {

class PaceBmsNumberImplementation : public Component, public number::Number {
 public:
  float get_setup_priority() const override;

 protected:
  void control(float value) override;
};

}  // namespace pace_bms
}  // namespace esphome
