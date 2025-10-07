#pragma once

#include "esphome/core/component.h"

#include "pace_bms_datetime_implementation.h"
#include "esphome/components/pace_bms/pace_bms_component_base.h"

namespace esphome {
namespace pace_bms_base {

class PaceBmsDatetime : public Component {
public:
	void set_parent(PaceBmsBase* parent) { parent_ = parent; }

	void set_system_date_and_time_datetime(PaceBmsDatetimeImplementation* datetime) { this->system_date_and_time_datetime_ = datetime; }

	void setup() override;
	float get_setup_priority() const { return setup_priority::DATA; }
	void dump_config() override;

protected:
	pace_bms_base::PaceBmsBase* parent_;

	PaceBmsProtocolV25::DateTime system_date_and_time_;
	bool system_date_and_time_seen_{ false };
	pace_bms_base::PaceBmsDatetimeImplementation* system_date_and_time_datetime_{ nullptr };
};

}  // namespace pace_bms_base
}  // namespace esphome



