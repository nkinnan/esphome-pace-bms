
#include "pace_bms_slave_component.h"

// had to do a forard declaration in the header to avoid circular dependency
#include "pace_bms_master_component.h"

namespace esphome {
namespace pace_bms_slave {

static const char* const TAG = "pace_bms_slave";

/*
* log configuration
*/

void PaceBmsSlave::dump_config() {
	ESP_LOGCONFIG(TAG, "pace_bms_slave:");
	ESP_LOGCONFIG(TAG, "  pace_bms master yaml id: %s", this->parent_->get_id_name().c_str());
	ESP_LOGCONFIG(TAG, "  pace_bms master address: %i", this->parent_->get_address());
}

/*
* setup this component
*/

void PaceBmsSlave::setup() {
	this->parent_->register_slave(this);
}

/*
* main processing loop
*/

void PaceBmsSlave::loop() {
}

}  // namespace pace_bms_slave
}  // namespace esphome
