import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import (
    CONF_ID,
    CONF_DEVICE_ID,
)
from .. import pace_bms_base_ns, CONF_PACE_BMS_ID, PaceBmsBase
from ..pace_bms_globals import inherit_device_id, CONF_SHUTDOWN

CODEOWNERS = ["@nkinnan"]
DEPENDENCIES = ["pace_bms"]

PaceBmsButton = pace_bms_base_ns.class_("PaceBmsButton", cg.Component)
PaceBmsButtonImplementation = pace_bms_base_ns.class_("PaceBmsButtonImplementation", cg.Component, button.Button)

CONFIG_SCHEMA = cv.All(
    inherit_device_id,
    cv.Schema({
        cv.GenerateID(): cv.declare_id(PaceBmsButton),
        cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBmsBase),
        cv.Optional(CONF_DEVICE_ID): cv.sub_device_id,

        cv.Optional(CONF_SHUTDOWN): button.button_schema(PaceBmsButtonImplementation),
    })
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_PACE_BMS_ID])
    cg.add(var.set_parent(parent))

    if shutdown_config := config.get(CONF_SHUTDOWN):
        btn = await button.new_button(shutdown_config)
        cg.add(var.set_shutdown_button(btn))
