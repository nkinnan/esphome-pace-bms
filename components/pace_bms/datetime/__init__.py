import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import datetime
from esphome.const import (
    CONF_ID,
    CONF_TYPE,
)
from .. import pace_bms_ns, CONF_PACE_BMS_ID, PaceBms

CODEOWNERS = ["@nkinnan"]

DEPENDENCIES = ["pace_bms"]

PaceBmsDatetime = pace_bms_ns.class_("PaceBmsDatetime", cg.Component)
PaceBmsDatetimeImplementation = pace_bms_ns.class_("PaceBmsDatetimeImplementation", cg.Component, datetime.DateTimeEntity)

CONF_SYSTEM_DATE_AND_TIME = "system_date_and_time"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PaceBmsDatetime),
        cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBms),

        cv.Optional(CONF_SYSTEM_DATE_AND_TIME): datetime.datetime_schema(
            PaceBmsDatetimeImplementation,
        ).extend({ 
            cv.Optional(CONF_TYPE, default="DATETIME"): cv.one_of("DATETIME", upper=True),
        }),
   }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_PACE_BMS_ID])
    cg.add(var.set_parent(parent))

    if system_date_and_time_config := config.get(CONF_SYSTEM_DATE_AND_TIME):
        dt = await datetime.new_datetime(system_date_and_time_config)
        cg.add(var.set_system_date_and_time_datetime(dt))


