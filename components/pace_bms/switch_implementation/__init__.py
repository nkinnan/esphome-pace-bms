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

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(PaceBmsSwitchImplementation),
            cv.Optional(CONF_FLOW_CONTROL_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_ADDRESS, default=DEFAULT_ADDRESS): cv.int_range(min=0, max=15),
            cv.Optional(CONF_PROTOCOL_VERSION, default=DEFAULT_PROTOCOL_VERSION): cv.int_range(min=0x25, max=0x25),
            cv.Optional(CONF_REQUEST_THROTTLE, default=DEFAULT_REQUEST_THROTTLE): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_RESPONSE_TIMEOUT, default=DEFAULT_RESPONSE_TIMEOUT): cv.positive_time_period_milliseconds,
        }
    )
    .extend(switch.switch_schema(default_restore_mode="DISABLED"))
)

#FINAL_VALIDATE_SCHEMA = CONFIG_SCHEMA

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    #paren = await cg.get_variable(config[CONF_PACE_BMS_SWITCH_ID])
    #cg.add(var.set_parent(paren))
