#pragma once

#include "esphome/core/component.h"

namespace esphome {
namespace pace_bms {

class PaceBms : public Component {
 public:
  void loop() override;
};
 
}  // namespace pace_bms
}  // namespace esphome
