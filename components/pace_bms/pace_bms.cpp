#include "pace_bms.h"
#include "esphome/core/log.h"
#include <cinttypes>
#include <iomanip>
#include <sstream>
#include <functional>

namespace esphome {
namespace pace_bms { 

static const char* const TAG = "pace_bms";

static const int items_per_update = 50;

void PaceBms::update() {
 if (!command_queue_.empty()) {
    ESP_LOGW(TAG, "%i commands still in queue on update(), skipping cycle", command_queue_.size());
  }
  else {
    for(int i = 0; i < items_per_update; i++)
    {
      command_item* item = new command_item;
      // ==================================================================================================
      // ============== comment out the following line or not, to repro the memory leak ===================
      // ============== you can also increase items_per_update to make it OOM faster    ===================
      // ==================================================================================================
      item->description_ = std::string("some std::string");
      command_queue_.push(item);
    }
    ESP_LOGV(TAG, "Update commands queued: %i", command_queue_.size());
  }
}

void PaceBms::loop() {
    int deleted_count = 0;
    while(!command_queue_.empty()) {
      PaceBms::command_item* command = command_queue_.front();
      command_queue_.pop();
      free(command);
      deleted_count++;
    }
    if(deleted_count > 0)
      ESP_LOGV(TAG, "Update commands dequeued and deleted: %i", deleted_count);
}

}  // namespace pace_bms
}  // namespace esphome
