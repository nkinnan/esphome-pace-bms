#pragma once

#include "esphome/core/component.h"
#include "esphome/components/datetime/datetime_entity.h"

#include "../pace_bms_protocol_v25.h"

namespace esphome {
namespace pace_bms {

class PaceBmsDatetimeImplementation : public Component, public datetime::DateTimeEntity {
public:
	float get_setup_priority() const { return setup_priority::DATA; }

	void add_on_control_callback(std::function<void(const datetime::DateTimeCall&)>&& callback);

	void set_datetime(PaceBmsProtocolV25::DateTime& dt);

protected:
	void control(const datetime::DateTimeCall& call) override;

	CallbackManager<void(const datetime::DateTimeCall&)> control_callbacks_{};
};

}  // namespace pace_bms
}  // namespace esphome
