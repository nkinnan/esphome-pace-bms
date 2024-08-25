#pragma once

#include "esphome/core/component.h"
#include "esphome/components/pace_bms/pace_bms.h"
#include "pace_bms_switch_implementation.h"

#include "../pace_bms_v25.h"

namespace esphome {
namespace pace_bms {

class PaceBmsSwitch : public Component {
 public:
  void set_parent(PaceBms *parent) { parent_ = parent; }

  // current switch states are actually in the status information response, so subscribe to that in order to update switch state
  void set_buzzer_alarm_switch(PaceBmsSwitchImplementation* buzzer_switch) { this->buzzer_alarm_switch_ = buzzer_switch; request_status_information_callback_ = true; }
  void set_led_alarm_switch(PaceBmsSwitchImplementation* led_switch) { this->led_alarm_switch_ = led_switch; request_status_information_callback_ = true; }
  void set_charge_current_limiter_switch(PaceBmsSwitchImplementation* charge_current_limiter_switch) { this->charge_current_limiter_switch_ = charge_current_limiter_switch; request_status_information_callback_ = true; }
  void set_charge_mosfet_switch(PaceBmsSwitchImplementation* charge_mosfet_switch) { this->charge_mosfet_switch_ = charge_mosfet_switch; request_status_information_callback_ = true; }
  void set_discharge_mosfet_switch(PaceBmsSwitchImplementation* discharge_mosfet_switch) { this->discharge_mosfet_switch_ = discharge_mosfet_switch; request_status_information_callback_ = true; }

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;

 protected:
  pace_bms::PaceBms *parent_;
  bool request_status_information_callback_ = false;
  void status_information_callback(PaceBmsV25::StatusInformation& status_information);

  pace_bms::PaceBmsSwitchImplementation* buzzer_alarm_switch_{ nullptr };
  pace_bms::PaceBmsSwitchImplementation* led_alarm_switch_{ nullptr };
  pace_bms::PaceBmsSwitchImplementation* charge_current_limiter_switch_{ nullptr };
  pace_bms::PaceBmsSwitchImplementation* charge_mosfet_switch_{ nullptr };
  pace_bms::PaceBmsSwitchImplementation* discharge_mosfet_switch_{ nullptr };
};

}  // namespace pace_bms
}  // namespace esphome

