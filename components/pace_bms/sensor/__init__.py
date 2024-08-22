import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (


    UNIT_DEGREES,
    ICON_WEATHER_SUNSET,
    CONF_TYPE,
)
from .. import pace_bms_ns, CONF_PACE_BMS_ID, PaceBms

DEPENDENCIES = ["pace_bms"]

PaceBmsSensor = pace_bms_ns.class_("PaceBmsSensor", sensor.Sensor, cg.PollingComponent)

SensorType = pace_bms_ns.enum("SensorType")
TYPES = {
    "elevation": SensorType.SUN_SENSOR_ELEVATION,
    "azimuth": SensorType.SUN_SENSOR_AZIMUTH,
}

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        PaceBmsSensor,
        unit_of_measurement=UNIT_DEGREES,
        icon=ICON_WEATHER_SUNSET,
        accuracy_decimals=1,
    )
    .extend(
        {
            cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBms),
            cv.Required(CONF_TYPE): cv.enum(TYPES, lower=True),
        }
    )
    .extend(cv.polling_component_schema("60s"))
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)

    cg.add(var.set_type(config[CONF_TYPE]))
    paren = await cg.get_variable(config[CONF_PACE_BMS_ID])
    cg.add(var.set_parent(paren))
