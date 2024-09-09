#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace pace_bms {

class PaceBmsSwitchImplementation : public Component, public switch_::Switch {
 public:
  float get_setup_priority() const override { return setup_priority::DATA; };

  void add_on_write_state_callback(std::function<void(bool)>&& callback) { this->write_state_callback_.add(std::move(callback)); }

 protected:
  // the only purpose of this class is to simply fill in this pure virtual and call the parent container component on user initiated state change request
  void write_state(bool state) override;

  CallbackManager<void(bool)> write_state_callback_{};
};

}  // namespace pace_bms
}  // namespace esphome
