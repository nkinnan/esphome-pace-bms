#pragma once

#include "esphome/core/component.h"
#include "esphome/components/select/select.h"

namespace esphome {
namespace pace_bms {

class PaceBmsSelectImplementation : public Component, public select::Select {
 public:
  float get_setup_priority() const override;

  void add_on_control_callback(std::function<void(const std::string)>&& callback);

 protected:
  // the only purpose of this class is to simply fill in this pure virtual and call the parent container component on user initiated state change request
  void control(const std::string& value) override;

  CallbackManager<void(const std::string&)> control_callback_{};
};

}  // namespace pace_bms
}  // namespace esphome
