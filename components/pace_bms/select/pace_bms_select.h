#pragma once

#include "esphome/core/component.h"

#include "esphome/components/pace_bms/pace_bms_component.h"

#include "pace_bms_select_implementation.h"

#include "../pace_bms_protocol_v25.h"

namespace esphome {
namespace pace_bms {

class PaceBmsSelect : public Component {
public:
	void set_parent(PaceBms* parent) { parent_ = parent; }

	void set_charge_current_limiter_gear_select(PaceBmsSelectImplementation* select) { this->charge_current_limiter_gear_select_ = select; }

	void set_protocol_can_select(PaceBmsSelectImplementation* select) { this->protocol_can_select_ = select; }
	void set_protocol_rs485_select(PaceBmsSelectImplementation* select) { this->protocol_rs485_select_ = select; }
	void set_protocol_type_select(PaceBmsSelectImplementation* select) { this->protocol_type_select_ = select; }

	void setup() override;
	float get_setup_priority() const override { return setup_priority::DATA; };
	void dump_config() override;

protected:
	pace_bms::PaceBms* parent_;

	pace_bms::PaceBmsSelectImplementation* charge_current_limiter_gear_select_{ nullptr };

	PaceBmsProtocolV25::Protocols protocols_;
	bool protocols_seen_ = false;
	pace_bms::PaceBmsSelectImplementation* protocol_can_select_{ nullptr };
	pace_bms::PaceBmsSelectImplementation* protocol_rs485_select_{ nullptr };
	pace_bms::PaceBmsSelectImplementation* protocol_type_select_{ nullptr };
};

}  // namespace pace_bms
}  // namespace esphome

