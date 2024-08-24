import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import (
    CONF_ID,
)
from .. import pace_bms_ns
#from ..switch import CONF_PACE_BMS_SWITCH_ID, PaceBmsSwitch

#DEPENDENCIES = ["pace_bms_switch"]

PaceBmsSwitchImplementation = pace_bms_ns.class_("PaceBmsSwitchImplementation", cg.Component, switch.Switch)

CONFIG_SCHEMA = cv.All(
    switch.switch_schema(default_restore_mode="DISABLED")
    .extend(
        {
            cv.GenerateID(): cv.declare_id(PaceBmsSwitchImplementation),
            #cv.GenerateID(CONF_PACE_BMS_SWITCH_ID): cv.use_id(PaceBmsSwitch),
        }
    )
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    #paren = await cg.get_variable(config[CONF_PACE_BMS_SWITCH_ID])
    #cg.add(var.set_parent(paren))