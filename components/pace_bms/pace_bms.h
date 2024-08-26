#pragma once

#include <vector>

#include "esphome/core/component.h"

#include <functional>
#include <queue>

namespace esphome {
namespace pace_bms {

class PaceBms : public PollingComponent {
 public:

  void update() override;
  void loop() override;

 protected:
  struct command_item
  {
	  std::string description_;
  };

  std::queue<command_item*> command_queue_;
};
 
}  // namespace pace_bms
}  // namespace esphome
