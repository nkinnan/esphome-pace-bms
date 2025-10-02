#pragma once

#include "pace_bms_component_base.h"

// forward declaration to avoid circular dependency, code using PaceBmsMaster is in the .cpp where the actual header is included
//#include "../pace_bms/pace_bms_master_component.h"
namespace esphome {
	namespace pace_bms_master {
		class PaceBmsMaster;
	}
}

namespace esphome {
namespace pace_bms_slave {

// this class encapsulates an instance of PaceBmsProtocolV25 (which handles protocol version 0x25) and injects the logging dependencies into it
//     in the future, other protocol versions may be supported
class PaceBmsSlave : public pace_bms_base::PaceBmsBase, public Component {
public:

	void set_parent(pace_bms_master::PaceBmsMaster* parent) { parent_ = parent; }

	// standard overrides to implement component behavior, update() queues periodic commands to request updates from the BMS
	void dump_config() override;
	void setup() override;
	void loop() override;

	// preferably we'll be setup after all child sensors have registered their callbacks via their own setup(), but
	//     this class still handles the case where they register late gracefully, a single update cycle will simply 
	//     be missed in that case
	float get_setup_priority() const { return setup_priority::LATE; }



	int get_protocol_commandset() override { return this->parent_->get_protocol_commandset(); }

protected:
	pace_bms_master::PaceBmsMaster* parent_{ nullptr };
};

}  // namespace pace_bms_slave
}  // namespace esphome
