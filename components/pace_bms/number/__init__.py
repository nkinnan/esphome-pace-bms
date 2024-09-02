import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
    CONF_ID,
    ENTITY_CATEGORY_CONFIG,
    DEVICE_CLASS_VOLTAGE,
    UNIT_VOLT,
    DEVICE_CLASS_DURATION,
    UNIT_SECOND,
    DEVICE_CLASS_CURRENT,
)

from .. import pace_bms_ns, CONF_PACE_BMS_ID, PaceBms

DEPENDENCIES = ["pace_bms"]

PaceBmsNumber = pace_bms_ns.class_("PaceBmsNumber", cg.Component)
PaceBmsNumberImplementation = pace_bms_ns.class_("PaceBmsNumberImplementation", cg.Component, number.Number)

BOX = "BOX" # todo

CONF_CELL_OVER_VOLTAGE_ALARM                        = "cell_over_voltage_alarm"
CONF_CELL_OVER_VOLTAGE_PROTECTION                   = "cell_over_voltage_protection"
CONF_CELL_OVER_VOLTAGE_PROTECTION_RELEASE           = "cell_over_voltage_protection_release"
CONF_CELL_OVER_VOLTAGE_PROTECTION_DELAY             = "cell_over_voltage_protection_delay"

CONF_PACK_OVER_VOLTAGE_ALARM                        = "pack_over_voltage_alarm"
CONF_PACK_OVER_VOLTAGE_PROTECTION                   = "pack_over_voltage_protection"
CONF_PACK_OVER_VOLTAGE_PROTECTION_RELEASE           = "pack_over_voltage_protection_release"
CONF_PACK_OVER_VOLTAGE_PROTECTION_DELAY             = "pack_over_voltage_protection_delay"

CONF_CELL_UNDER_VOLTAGE_ALARM                       = "cell_under_voltage_alarm"
CONF_CELL_UNDER_VOLTAGE_PROTECTION                  = "cell_under_voltage_protection"
CONF_CELL_UNDER_VOLTAGE_PROTECTION_RELEASE          = "cell_under_voltage_protection_release"
CONF_CELL_UNDER_VOLTAGE_PROTECTION_DELAY            = "cell_under_voltage_protection_delay"

CONF_PACK_UNDER_VOLTAGE_ALARM                       = "pack_under_voltage_alarm"
CONF_PACK_UNDER_VOLTAGE_PROTECTION                  = "pack_under_voltage_protection"
CONF_PACK_UNDER_VOLTAGE_PROTECTION_RELEASE          = "pack_under_voltage_protection_release"
CONF_PACK_UNDER_VOLTAGE_PROTECTION_DELAY            = "pack_under_voltage_protection_delay"


CONF_CHARGE_OVER_CURRENT_ALARM                      = "charge_over_current_alarm"
CONF_CHARGE_OVER_CURRENT_PROTECTION                 = "charge_over_current_protection"
CONF_CHARGE_OVER_CURRENT_PROTECTION_DELAY           = "charge_over_current_protection_delay"

CONF_DISCHARGE_OVER_CURRENT1_ALARM                  = "discharge_over_current1_alarm"
CONF_DISCHARGE_OVER_CURRENT1_PROTECTION             = "discharge_over_current1_protection"
CONF_DISCHARGE_OVER_CURRENT1_PROTECTION_DELAY       = "discharge_over_current1_protection_delay"

CONF_DISCHARGE_OVER_CURRENT2_PROTECTION             = "discharge_over_current2_protection"
CONF_DISCHARGE_OVER_CURRENT2_PROTECTION_DELAY       = "discharge_over_current2_protection_delay"

CONF_SHORT_CIRCUIT_PROTECTION_DELAY                 = "short_circuit_protection_delay"







CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PaceBmsNumber),
        cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBms),

        cv.Optional(CONF_CELL_OVER_VOLTAGE_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CELL_OVER_VOLTAGE_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CELL_OVER_VOLTAGE_PROTECTION_RELEASE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CELL_OVER_VOLTAGE_PROTECTION_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),

        cv.Optional(CONF_PACK_OVER_VOLTAGE_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_PACK_OVER_VOLTAGE_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_PACK_OVER_VOLTAGE_PROTECTION_RELEASE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_PACK_OVER_VOLTAGE_PROTECTION_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),

        cv.Optional(CONF_CELL_UNDER_VOLTAGE_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CELL_UNDER_VOLTAGE_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CELL_UNDER_VOLTAGE_PROTECTION_RELEASE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CELL_UNDER_VOLTAGE_PROTECTION_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),

        cv.Optional(CONF_PACK_UNDER_VOLTAGE_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_PACK_UNDER_VOLTAGE_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_PACK_UNDER_VOLTAGE_PROTECTION_RELEASE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_PACK_UNDER_VOLTAGE_PROTECTION_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),

        cv.Optional(CONF_CHARGE_OVER_CURRENT_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_CURRENT,
            unit_of_measurement=UNIT_AMPERE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CHARGE_OVER_CURRENT_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_CURRENT,
            unit_of_measurement=UNIT_AMPERE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CHARGE_OVER_CURRENT_PROTECTION_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),

        cv.Optional(CONF_DISCHARGE_OVER_CURRENT1_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_CURRENT,
            unit_of_measurement=UNIT_AMPERE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_DISCHARGE_OVER_CURRENT1_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_CURRENT,
            unit_of_measurement=UNIT_AMPERE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_DISCHARGE_OVER_CURRENT1_PROTECTION_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),

        cv.Optional(CONF_DISCHARGE_OVER_CURRENT2_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_CURRENT,
            unit_of_measurement=UNIT_AMPERE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_DISCHARGE_OVER_CURRENT2_PROTECTION_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),

        cv.Optional(CONF_SHORT_CIRCUIT_PROTECTION_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),

    }
)







async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    paren = await cg.get_variable(config[CONF_PACE_BMS_ID])
    cg.add(var.set_parent(paren))

    if cell_over_voltage_alarm_config := config.get(CONF_CELL_OVER_VOLTAGE_ALARM):
        num = await number.new_number(
            cell_over_voltage_alarm_config, 
            min_value=2.5, 
            max_value=4.5, 
            step=0.01)
        cg.add(var.set_cell_over_voltage_alarm_number(num))
    if cell_over_voltage_protection_config := config.get(CONF_CELL_OVER_VOLTAGE_PROTECTION):
        num = await number.new_number(
            cell_over_voltage_protection_config, 
            min_value=2.5, 
            max_value=4.5, 
            step=0.01)
        cg.add(var.set_cell_over_voltage_protection_number(num))
    if cell_over_voltage_protection_release_config := config.get(CONF_CELL_OVER_VOLTAGE_PROTECTION_RELEASE):
        num = await number.new_number(
            cell_over_voltage_protection_release_config, 
            min_value=2.5, 
            max_value=4.5, 
            step=0.01)
        cg.add(var.set_cell_over_voltage_protection_release_number(num))
    if cell_over_voltage_protection_delay_config := config.get(CONF_CELL_OVER_VOLTAGE_PROTECTION_DELAY):
        num = await number.new_number(
            cell_over_voltage_protection_delay_config, 
            min_value=1, 
            max_value=20, 
            step=0.5)
        cg.add(var.set_cell_over_voltage_protection_delay_number(num))

    if pack_over_voltage_alarm_config := config.get(CONF_PACK_OVER_VOLTAGE_ALARM):
        num = await number.new_number(
            pack_over_voltage_alarm_config, 
            min_value=20, 
            max_value=65, 
            step=0.01)
        cg.add(var.set_pack_over_voltage_alarm_number(num))
    if pack_over_voltage_protection_config := config.get(CONF_PACK_OVER_VOLTAGE_PROTECTION):
        num = await number.new_number(
            pack_over_voltage_protection_config, 
            min_value=20, 
            max_value=65, 
            step=0.01)
        cg.add(var.set_pack_over_voltage_protection_number(num))
    if pack_over_voltage_protection_release_config := config.get(CONF_PACK_OVER_VOLTAGE_PROTECTION_RELEASE):
        num = await number.new_number(
            pack_over_voltage_protection_release_config, 
            min_value=20, 
            max_value=65, 
            step=0.01)
        cg.add(var.set_pack_over_voltage_protection_release_number(num))
    if pack_over_voltage_protection_delay_config := config.get(CONF_PACK_OVER_VOLTAGE_PROTECTION_DELAY):
        num = await number.new_number(
            pack_over_voltage_protection_delay_config, 
            min_value=1, 
            max_value=20, 
            step=0.5)
        cg.add(var.set_pack_over_voltage_protection_delay_number(num))

    if cell_under_voltage_alarm_config := config.get(CONF_CELL_UNDER_VOLTAGE_ALARM):
        num = await number.new_number(
            cell_under_voltage_alarm_config, 
            min_value=2, 
            max_value=3.5, 
            step=0.01)
        cg.add(var.set_cell_under_voltage_alarm_number(num))
    if cell_under_voltage_protection_config := config.get(CONF_CELL_UNDER_VOLTAGE_PROTECTION):
        num = await number.new_number(
            cell_under_voltage_protection_config, 
            min_value=2, 
            max_value=3.5, 
            step=0.01)
        cg.add(var.set_cell_under_voltage_protection_number(num))
    if cell_under_voltage_protection_release_config := config.get(CONF_CELL_UNDER_VOLTAGE_PROTECTION_RELEASE):
        num = await number.new_number(
            cell_under_voltage_protection_release_config, 
            min_value=2, 
            max_value=3.5, 
            step=0.01)
        cg.add(var.set_cell_under_voltage_protection_release_number(num))
    if cell_under_voltage_protection_delay_config := config.get(CONF_CELL_UNDER_VOLTAGE_PROTECTION_DELAY):
        num = await number.new_number(
            cell_under_voltage_protection_delay_config, 
            min_value=1, 
            max_value=20, 
            step=0.5)
        cg.add(var.set_cell_under_voltage_protection_delay_number(num))

    if pack_under_voltage_alarm_config := config.get(CONF_PACK_UNDER_VOLTAGE_ALARM):
        num = await number.new_number(
            pack_under_voltage_alarm_config, 
            min_value=15, 
            max_value=50, 
            step=0.01)
        cg.add(var.set_pack_under_voltage_alarm_number(num))
    if pack_under_voltage_protection_config := config.get(CONF_PACK_UNDER_VOLTAGE_PROTECTION):
        num = await number.new_number(
            pack_under_voltage_protection_config, 
            min_value=15, 
            max_value=50, 
            step=0.01)
        cg.add(var.set_pack_under_voltage_protection_number(num))
    if pack_under_voltage_protection_release_config := config.get(CONF_PACK_UNDER_VOLTAGE_PROTECTION_RELEASE):
        num = await number.new_number(
            pack_under_voltage_protection_release_config, 
            min_value=15, 
            max_value=50, 
            step=0.01)
        cg.add(var.set_pack_under_voltage_protection_release_number(num))
    if pack_under_voltage_protection_delay_config := config.get(CONF_PACK_UNDER_VOLTAGE_PROTECTION_DELAY):
        num = await number.new_number(
            pack_under_voltage_protection_delay_config, 
            min_value=1, 
            max_value=20, 
            step=0.5)
        cg.add(var.set_pack_under_voltage_protection_delay_number(num))

    if charge_over_current_alarm_config := config.get(CONF_CHARGE_OVER_CURRENT_ALARM):
        num = await number.new_number(
            charge_over_current_alarm_config, 
            min_value=1, 
            max_value=220, 
            step=1)
        cg.add(var.set_charge_over_current_alarm_number(num))
    if charge_over_current_protection_config := config.get(CONF_CHARGE_OVER_CURRENT_PROTECTION):
        num = await number.new_number(
            charge_over_current_protection_config, 
            min_value=1, 
            max_value=220, 
            step=1)
        cg.add(var.set_charge_over_current_protection_number(num))
    if charge_over_current_protection_delay_config := config.get(CONF_CHARGE_OVER_CURRENT_PROTECTION_DELAY):
        num = await number.new_number(
            charge_over_current_protection_delay_config, 
            min_value=0.5, 
            max_value=25, 
            step=0.5)
        cg.add(var.set_charge_over_current_protection_delay_number(num))

    if discharge_over_current1_alarm_config := config.get(CONF_DISCHARGE_OVER_CURRENT1_ALARM):
        num = await number.new_number(
            discharge_over_current1_alarm_config, 
            min_value=1, 
            max_value=220, 
            step=1)
        cg.add(var.set_discharge_over_current1_alarm_number(num))
    if discharge_over_current1_protection_config := config.get(CONF_DISCHARGE_OVER_CURRENT1_PROTECTION):
        num = await number.new_number(
            discharge_over_current1_protection_config, 
            min_value=1, 
            max_value=220, 
            step=1)
        cg.add(var.set_discharge_over_current1_protection_number(num))
    if discharge_over_current1_protection_delay_config := config.get(CONF_DISCHARGE_OVER_CURRENT1_PROTECTION_DELAY):
        num = await number.new_number(
            discharge_over_current1_protection_delay_config, 
            min_value=0.5, 
            max_value=25, 
            step=0.5)
        cg.add(var.set_discharge_over_current1_protection_delay_number(num))

    if discharge_over_current2_protection_config := config.get(CONF_DISCHARGE_OVER_CURRENT2_PROTECTION):
        num = await number.new_number(
            discharge_over_current2_protection_config, 
            min_value=5, 
            max_value=255, 
            step=5)
        cg.add(var.set_discharge_over_current2_protection_number(num))
    if discharge_over_current2_protection_delay_config := config.get(CONF_DISCHARGE_OVER_CURRENT2_PROTECTION_DELAY):
        num = await number.new_number(
            discharge_over_current2_protection_delay_config, 
            min_value=0.1, 
            max_value=2, 
            step=0.1)
        cg.add(var.set_discharge_over_current2_protection_delay_number(num))

    if short_circuit_protection_delay_config := config.get(CONF_SHORT_CIRCUIT_PROTECTION_DELAY):
        num = await number.new_number(
            short_circuit_protection_delay_config, 
            min_value=0.0001, 
            max_value=0.0005, 
            step=0.00005)
        cg.add(var.set_short_circuit_protection_delay_number(num))
