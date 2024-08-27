#include "pace_bms.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include <cinttypes>
#include <iomanip>
#include <sstream>
#include <functional>
#include <list>
#include <queue>
#include <set>
#include <utility>
#include <vector>

namespace esphome {
namespace pace_bms { 

static const char* const TAG = "pace_bms";

uint32_t before = 0;
uint32_t count = 0;
const uint32_t millis_between_logs = 1000/10;

void PaceBms::loop() 
{
    uint32_t now = millis();

    // initialize 'before' on first loop
    if (before == 0)
        before = now;

    // wait until enough time has passed
    if ((now - before) < millis_between_logs)
        return;

    ESP_LOGV(TAG, "Sending iteration %i at %i", count++, now);

    before = now;
}

}  // namespace pace_bms
}  // namespace esphome
