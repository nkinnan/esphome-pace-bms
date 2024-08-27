#include "pace_bms.h"
#include "esphome/core/log.h"
#include <cinttypes>

namespace esphome {
namespace pace_bms { 

static const char* const TAG = "pace_bms";

uint32_t before = 0;
uint32_t count = 0;
uint32_t millis_between_logs = 1000;

void PaceBms::loop() {
    uint32_t now = millis();

    if (before == 0)
    {
        before = now;
        return;
    }

    if (now - before < millis_between_logs)
        return;

    ESP_LOGV(TAG, "Sending iteration %i at %i", count++, now);
}

}  // namespace pace_bms
}  // namespace esphome
