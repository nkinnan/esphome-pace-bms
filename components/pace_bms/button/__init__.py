import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import (
    CONF_ID,
)
from .. import pace_bms_ns, CONF_PACE_BMS_ID, PaceBms

DEPENDENCIES = ["pace_bms"]

PaceBmsButton = pace_bms_ns.class_("PaceBmsButton", cg.Component)
PaceBmsButtonImplementation = pace_bms_ns.class_("PaceBmsButtonImplementation", cg.Component, button.Button)

CONF_SHUTDOWN = "shutdown"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PaceBmsButton),
        cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBms),

        cv.Optional(CONF_SHUTDOWN): button.button_schema(PaceBmsButtonImplementation),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    paren = await cg.get_variable(config[CONF_PACE_BMS_ID])
    cg.add(var.set_parent(paren))

    if shutdown_config := config.get(CONF_SHUTDOWN):
        btn = await button.new_button(shutdown_config)
        cg.add(var.set_shutdown_button(btn))
