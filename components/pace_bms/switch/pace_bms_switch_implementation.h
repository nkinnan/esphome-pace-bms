#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace pace_bms {

// the only purpose of this class is to fill in the pure virtual and call the parent container component on state change via publish_state executing callbacks
class PaceBmsSwitchImplementation : public Component, public switch_::Switch {
 public:
  float get_setup_priority() const override;

  void add_on_write_state_callback(std::function<void(bool)>&& callback);

 protected:
  void write_state(bool state) override;
  CallbackManager<void(bool)> write_state_callback_{};
};

}  // namespace pace_bms
}  // namespace esphome
