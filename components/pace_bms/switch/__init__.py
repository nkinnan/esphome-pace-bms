import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import (
    CONF_ID,
)
from .. import pace_bms_ns, CONF_PACE_BMS_ID, PaceBms

DEPENDENCIES = ["pace_bms"]

PaceBmsSwitch = pace_bms_ns.class_("PaceBmsSwitch", cg.Component)

# "this" for pace_bms_switch_implementation to get parent from
CONF_PACE_BMS_SWITCH_ID = "pace_bms_switch_id"


CONF_BUZZER       = "buzzer"



CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PaceBmsSwitch),
        cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBms),

        cv.Optional(CONF_BUZZER): switch.switch_schema(default_restore_mode="DISABLED"),

    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    paren = await cg.get_variable(config[CONF_PACE_BMS_ID])
    cg.add(var.set_parent(paren))

    if buzzer_config := config.get(CONF_BUZZER):
        sens = await switch.new_switch(buzzer_config)
        cg.add(var.set_buzzer_switch(sens))

