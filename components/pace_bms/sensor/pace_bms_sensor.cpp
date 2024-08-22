#include "pace_bms_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pace_bms {

static const char* const TAG = "pace_bms.sensor";

void PaceBmsSensor::dump_config() { LOG_SENSOR("", "Sun Sensor", this); }

}  // namespace pace_bms
}  // namespace esphome
