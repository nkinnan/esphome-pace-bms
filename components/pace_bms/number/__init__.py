import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
    CONF_ID,
)
from .. import pace_bms_ns, CONF_PACE_BMS_ID, PaceBms

DEPENDENCIES = ["pace_bms"]

PaceBmsNumber = pace_bms_ns.class_("PaceBmsNumber", cg.Component)
PaceBmsNumberImplementation = pace_bms_ns.class_("PaceBmsNumberImplementation", cg.Component, number.Number)


CONF_TEST           = "test"


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PaceBmsNumber),
        cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBms),

        cv.Optional(CONF_TEST): switch.switch_schema(PaceBmsNumberImplementation),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    paren = await cg.get_variable(config[CONF_PACE_BMS_ID])
    cg.add(var.set_parent(paren))

    if test_config := config.get(CONF_TEST):
        num = await switch.new_number(test_config)
        cg.add(var.set_test_number(num))
