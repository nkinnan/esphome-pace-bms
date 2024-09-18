#pragma once

#include <vector>
#include <string>

#include "esphome/core/component.h"
#include "esphome/components/select/select.h"

namespace esphome {
namespace pace_bms {

class PaceBmsSelectImplementation : public Component, public select::Select {
public:
	void set_values(std::vector<uint8_t> values) { this->values_ = std::move(values); }

	float get_setup_priority() const override { return setup_priority::DATA; };

	void add_on_control_callback(std::function<void(const std::string&, const uint8_t)>&& callback) { this->control_callback_.add(std::move(callback)); }

	uint8_t value_from_option(std::string str);
	std::string option_from_value(uint8_t number);

protected:
	// the primary purpose of this class is to simply fill in this pure virtual and call the parent container component on user initiated state change request
	void control(const std::string& text) override;

	CallbackManager<void(const std::string&, uint8_t value)> control_callback_{};

	std::vector<uint8_t> values_;
};

}  // namespace pace_bms
}  // namespace esphome
