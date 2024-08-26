#include "pace_bms.h"
#include "esphome/core/log.h"
#include <cinttypes>
#include <iomanip>
#include <sstream>
#include <functional>

namespace esphome {
namespace pace_bms { 

static const char* const TAG = "pace_bms";

void PaceBms::update() {
 if (!command_queue_.empty()) {
    ESP_LOGW(TAG, "%i commands still in queue on update(), skipping cycle", command_queue_.size());
  }
  else {
    for(int i = 0; i < 100; i++)
    {
      command_item* item = new command_item;
      // ==================================================================================================
      // ============== comment out the following line or not, to repro the memory leak ===================
      // ==================================================================================================
      item->description_ = std::string("some std::string");
      command_queue_.push(item);
    }

    ESP_LOGV(TAG, "Update commands queued: %i", command_queue_.size());
  }
}

void PaceBms::loop() {
    if (command_queue_.empty()) {
      ESP_LOGV(TAG, "command queue empty on loop()");
      return;
    }
    while(!command_queue_.empty()) {
      PaceBms::command_item* command = command_queue_.front();
      command_queue_.pop();
      free(command);
    }
}

}  // namespace pace_bms
}  // namespace esphome
