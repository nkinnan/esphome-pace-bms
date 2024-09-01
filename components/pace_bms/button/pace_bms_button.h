#pragma once

#include "esphome/core/component.h"
#include "esphome/components/pace_bms/pace_bms.h"
#include "esphome/components/sensor/sensor.h"

#include "../pace_bms_v25.h"

namespace esphome {
namespace pace_bms {

class PaceBmsButton : public Component {
 public:
  void set_parent(PaceBms *parent) { parent_ = parent; }

  void set_cell_count_sensor(button::Button* shutdown_button) { this->shutdown_button_ = shutdown_button;  }

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;

 protected:
  pace_bms::PaceBms *parent_;

  // analog info
  button::Button* shutdown_button_{ nullptr };
};

}  // namespace pace_bms
}  // namespace esphome
