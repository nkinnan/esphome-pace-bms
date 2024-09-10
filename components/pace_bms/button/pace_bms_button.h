#pragma once

#include "esphome/core/component.h"
#include "esphome/components/pace_bms/pace_bms.h"
#include "esphome/components/button/button.h"

namespace esphome {
namespace pace_bms {

class PaceBmsButton : public Component {
public:
	void set_parent(PaceBms* parent) { parent_ = parent; }

	void set_shutdown_button(button::Button* button) { this->shutdown_button_ = button; }

	void setup() override;
	float get_setup_priority() const { return setup_priority::DATA; }
	void dump_config() override;

protected:
	pace_bms::PaceBms* parent_;

	// analog info
	button::Button* shutdown_button_{ nullptr };
};

}  // namespace pace_bms
}  // namespace esphome
