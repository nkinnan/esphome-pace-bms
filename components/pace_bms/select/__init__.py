import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import (
    CONF_ID,
)
from .. import pace_bms_ns, CONF_PACE_BMS_ID, PaceBms

DEPENDENCIES = ["pace_bms"]

PaceBmsSelect = pace_bms_ns.class_("PaceBmsSelect", cg.Component)
PaceBmsSelectImplementation = pace_bms_ns.class_("PaceBmsSelectImplementation", cg.Component, select.Select)


CONF_CHARGE_CURRENT_LIMITER_GEAR           = "charge_current_limiter_gear"
CONF_CHARGE_CURRENT_LIMITER_GEAR_OPTIONS   = [
    "LowGear",
    "HighGear",
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PaceBmsSelect),
        cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBms),

        cv.Optional(CONF_CHARGE_CURRENT_LIMITER_GEAR): select.select_schema(PaceBmsSelectImplementation)
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    paren = await cg.get_variable(config[CONF_PACE_BMS_ID])
    cg.add(var.set_parent(paren))

    if charge_current_limiter_gear_config := config.get(CONF_CHARGE_CURRENT_LIMITER_GEAR):
        sel = await select.new_select(
            gear_config,
            options=[CONF_CHARGE_CURRENT_LIMITER_GEAR_OPTIONS],
        )
        cg.add(var.set_charge_current_limiter_gear_select(sens))
