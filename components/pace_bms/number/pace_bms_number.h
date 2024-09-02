#pragma once

#include "esphome/core/component.h"
#include "esphome/components/pace_bms/pace_bms.h"
#include "pace_bms_number_implementation.h"

#include "../pace_bms_v25.h"

namespace esphome {
namespace pace_bms {

class PaceBmsNumber : public Component {
 public:
  void set_parent(PaceBms *parent) { parent_ = parent; }

  // current switch states are actually in the status information response, so subscribe to that in order to update switch state
  void set_test_switch(PaceBmsNumberImplementation* test_number) { this->test_number_ = test_number; request_test_callback_ = true; }

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;

 protected:
  pace_bms::PaceBms *parent_;
  bool request_test_callback_ = false;
  void test_callback(PaceBmsV25::StatusInformation& status_information);

  pace_bms::PaceBmsNumberImplementation* test_number_{ nullptr };
};

}  // namespace pace_bms
}  // namespace esphome

