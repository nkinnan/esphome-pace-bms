#pragma once

#include <functional>
#include <queue>
#include <list>
#include <string>
#include <vector>
#include <span>
#include <optional>

#include "esphome/core/component.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pace_bms_base {

class PaceBmsBase {
public:
	// called by the codegen to set our YAML property values
	void set_id_name(std::string id_name) { this->id_name_ = id_name; }
	void set_address(uint8_t address) { this->address_ = address; }
	void set_responding_address(uint8_t responding_address) { this->responding_address_ = responding_address; }

	std::string get_id_name() const { return this->id_name_; }
	uint8_t get_address() const { return this->address_; }

protected:
	// config values set in YAML
	std::string id_name_;
	uint8_t address_{ 0 };
	std::optional<uint8_t> responding_address_;
};

}  // namespace pace_bms_base
}  // namespace esphome
