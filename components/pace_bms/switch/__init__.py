import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import (
    CONF_ID,
)
from .. import pace_bms_ns, CONF_PACE_BMS_ID, PaceBms

DEPENDENCIES = ["pace_bms"]

PaceBmsSwitch = pace_bms_ns.class_("PaceBmsSwitch", cg.Component)
PaceBmsSwitchImplementation = pace_bms_ns.class_("PaceBmsSwitchImplementation", cg.Component, switch.Switch)


CONF_BUZZER_ALARM           = "buzzer_alarm"
CONF_LED_ALARM              = "led_alarm"
CONF_CHARGE_CURRENT_LIMITER = "charge_current_limiter"
CONF_CHARGE_MOSFET          = "charge_mosfet"
CONF_DISCHARGE_MOSFET       = "discharge_mosfet"


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PaceBmsSwitch),
        cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBms),

        cv.Optional(CONF_BUZZER): switch.switch_schema(PaceBmsSwitchImplementation, default_restore_mode="DISABLED"),
        cv.Optional(CONF_LED): switch.switch_schema(PaceBmsSwitchImplementation, default_restore_mode="DISABLED"),

    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    paren = await cg.get_variable(config[CONF_PACE_BMS_ID])
    cg.add(var.set_parent(paren))

    if buzzer_alarm_config := config.get(CONF_BUZZER_ALARM):
        sens = await switch.new_switch(buzzer_alarm_config)
        cg.add(var.set_buzzer_alarm_switch(sens))

    if led_alarm_config := config.get(CONF_LED_ALARM):
        sens = await switch.new_switch(led_alarm_config)
        cg.add(var.set_led_alarm_switch(sens))

    if charge_current_limiter_config := config.get(CONF_CHARGE_CURRENT_LIMITER):
        sens = await switch.new_switch(charge_current_limiter_config)
        cg.add(var.set_charge_current_limiter_switch(sens))

    if charge_mosfet_config := config.get(CONF_CHARGE_MOSFET):
        sens = await switch.new_switch(charge_mosfet_config)
        cg.add(var.set_charge_mosfet_switch(sens))

    if discharge_mosfet_config := config.get(CONF_DISCHARGE_MOSFET):
        sens = await switch.new_switch(discharge_mosfet_config)
        cg.add(var.set_discharge_mosfet_switch(sens))

