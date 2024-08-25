#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace pace_bms {

class PaceBmsSwitchImplementation : public Component, public switch_::Switch {
 public:
  float get_setup_priority() const override;

  void add_on_write_state_callback(std::function<void(bool)>&& callback);

  // copied from Switch::publish_state this sets the internal state without saving or publishing since round-trip to the device is pending, 
  // the parent will wait for the device to inform of the true state after the state change write request, and make a full publish call at that point
  // but without recording the "presumed" state in the meantime, the UX doesn't work correctly 
  void record_state_without_publish(bool state);

 protected:
  // the main purpose of this class is to simply fill in this pure virtual and call the parent container component on user initiated state change request
  void write_state(bool state) override;
  CallbackManager<void(bool)> write_state_callback_{};
};

}  // namespace pace_bms
}  // namespace esphome
