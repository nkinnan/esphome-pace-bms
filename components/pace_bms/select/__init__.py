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

CONF_PROTOCOL_CAN           = "protocol_can"
CONF_PROTOCOL_CAN_OPTIONS   = {
    "can1": 1,
    "can2": 2,
}
CONF_PROTOCOL_RS485           = "protocol_rs485"
CONF_PROTOCOL_RS485_OPTIONS   = {
    "rs485-1": 1,
    "rs485-2": 2,
}
CONF_PROTOCOL_TYPE           = "protocol_type"
CONF_PROTOCOL_TYPE_OPTIONS   = {
    "type1": 1,
    "type2": 2,
}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PaceBmsSelect),
        cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBms),

        cv.Optional(CONF_CHARGE_CURRENT_LIMITER_GEAR): select.select_schema(PaceBmsSelectImplementation),

        cv.Optional(CONF_PROTOCOL_CAN): select.select_schema(PaceBmsSelectImplementation),
        cv.Optional(CONF_PROTOCOL_RS485): select.select_schema(PaceBmsSelectImplementation),
        cv.Optional(CONF_PROTOCOL_TYPE): select.select_schema(PaceBmsSelectImplementation),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    paren = await cg.get_variable(config[CONF_PACE_BMS_ID])
    cg.add(var.set_parent(paren))

    if charge_current_limiter_gear_config := config.get(CONF_CHARGE_CURRENT_LIMITER_GEAR):
        sel = await select.new_select(
            charge_current_limiter_gear_config,
            options=CONF_CHARGE_CURRENT_LIMITER_GEAR_OPTIONS,
        )
        cg.add(var.set_charge_current_limiter_gear_select(sel))

    if protocol_can_config := config.get(CONF_PROTOCOL_CAN):
        protocol_can_options_config = config.get(CONF_PROTOCOL_CAN_OPTIONS)
        sel = await select.new_select(
            protocol_can_config,
            options=list(protocol_can_options_config.keys()),
        )
        cg.add(var.set_protocol_can_select(sel))
        cg.add(sel.set_protocol_values(protocol_can_options_config.values())
    
    if protocol_rs485_config := config.get(CONF_PROTOCOL_RS485):
        protocol_rs485_options_config = config.get(CONF_PROTOCOL_RS485_OPTIONS)
        sel = await select.new_select(
            protocol_rs485_config,
            options=list(protocol_rs485_options_config.keys()),
        )
        cg.add(var.set_protocol_rs485_select(sel))
        cg.add(sel.set_protocol_values(protocol_rs485_options_config.values())

    if protocol_type_config := config.get(CONF_PROTOCOL_TYPE):
        protocol_type_options_config = config.get(CONF_PROTOCOL_TYPE_OPTIONS)
        sel = await select.new_select(
            protocol_type_config,
            options=list(protocol_type_options_config.keys()),
        )
        cg.add(var.set_protocol_type_select(sel))
        cg.add(sel.set_protocol_values(protocol_type_options_config.values())
