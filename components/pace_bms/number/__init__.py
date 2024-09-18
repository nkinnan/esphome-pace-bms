import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.components.number import NUMBER_MODES
from esphome.const import (
    CONF_ID,
    CONF_MODE,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_DURATION,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_TEMPERATURE,
    UNIT_VOLT,
    UNIT_SECOND,
    UNIT_AMPERE,
    UNIT_MILLISECOND,
    UNIT_MINUTE,
    UNIT_PERCENT,
    UNIT_CELSIUS,
    ENTITY_CATEGORY_CONFIG,
)
from .. import pace_bms_ns, CONF_PACE_BMS_ID, PaceBms

CODEOWNERS = ["@nkinnan"]

DEPENDENCIES = ["pace_bms"]

PaceBmsNumber = pace_bms_ns.class_("PaceBmsNumber", cg.Component)
PaceBmsNumberImplementation = pace_bms_ns.class_("PaceBmsNumberImplementation", cg.Component, number.Number)

NUMBER_MODE_BOX = "BOX"

CONF_CELL_OVER_VOLTAGE_ALARM                          = "cell_over_voltage_alarm"
CONF_CELL_OVER_VOLTAGE_PROTECTION                     = "cell_over_voltage_protection"
CONF_CELL_OVER_VOLTAGE_PROTECTION_RELEASE             = "cell_over_voltage_protection_release"
CONF_CELL_OVER_VOLTAGE_PROTECTION_DELAY               = "cell_over_voltage_protection_delay"

CONF_PACK_OVER_VOLTAGE_ALARM                          = "pack_over_voltage_alarm"
CONF_PACK_OVER_VOLTAGE_PROTECTION                     = "pack_over_voltage_protection"
CONF_PACK_OVER_VOLTAGE_PROTECTION_RELEASE             = "pack_over_voltage_protection_release"
CONF_PACK_OVER_VOLTAGE_PROTECTION_DELAY               = "pack_over_voltage_protection_delay"

CONF_CELL_UNDER_VOLTAGE_ALARM                         = "cell_under_voltage_alarm"
CONF_CELL_UNDER_VOLTAGE_PROTECTION                    = "cell_under_voltage_protection"
CONF_CELL_UNDER_VOLTAGE_PROTECTION_RELEASE            = "cell_under_voltage_protection_release"
CONF_CELL_UNDER_VOLTAGE_PROTECTION_DELAY              = "cell_under_voltage_protection_delay"

CONF_PACK_UNDER_VOLTAGE_ALARM                         = "pack_under_voltage_alarm"
CONF_PACK_UNDER_VOLTAGE_PROTECTION                    = "pack_under_voltage_protection"
CONF_PACK_UNDER_VOLTAGE_PROTECTION_RELEASE            = "pack_under_voltage_protection_release"
CONF_PACK_UNDER_VOLTAGE_PROTECTION_DELAY              = "pack_under_voltage_protection_delay"

CONF_CHARGE_OVER_CURRENT_ALARM                        = "charge_over_current_alarm"
CONF_CHARGE_OVER_CURRENT_PROTECTION                   = "charge_over_current_protection"
CONF_CHARGE_OVER_CURRENT_PROTECTION_DELAY             = "charge_over_current_protection_delay"

CONF_DISCHARGE_OVER_CURRENT1_ALARM                    = "discharge_over_current1_alarm"
CONF_DISCHARGE_OVER_CURRENT1_PROTECTION               = "discharge_over_current1_protection"
CONF_DISCHARGE_OVER_CURRENT1_PROTECTION_DELAY         = "discharge_over_current1_protection_delay"

CONF_DISCHARGE_OVER_CURRENT2_PROTECTION               = "discharge_over_current2_protection"
CONF_DISCHARGE_OVER_CURRENT2_PROTECTION_DELAY         = "discharge_over_current2_protection_delay"

CONF_SHORT_CIRCUIT_PROTECTION_DELAY                   = "short_circuit_protection_delay"

CONF_CELL_BALANCING_THRESHOLD                         = "cell_balancing_threshold"
CONF_CELL_BALANCING_DELTA                             = "cell_balancing_delta"

CONF_SLEEP_CELL_VOLTAGE                               = "sleep_cell_voltage"
CONF_SLEEP_DELAY                                      = "sleep_delay"

CONF_FULL_CHARGE_VOLTAGE                              = "full_charge_voltage"
CONF_FULL_CHARGE_AMPS                                 = "full_charge_amps"
CONF_LOW_CHARGE_ALARM_PERCENT                         = "low_charge_alarm_percent"

CONF_CHARGE_OVER_TEMPERATURE_ALARM                    = "charge_over_temperature_alarm"
CONF_CHARGE_OVER_TEMPERATURE_PROTECTION               = "charge_over_temperature_protection"
CONF_CHARGE_OVER_TEMPERATURE_PROTECTION_RELEASE       = "charge_over_temperature_protection_release"

CONF_DISCHARGE_OVER_TEMPERATURE_ALARM                 = "discharge_over_temperature_alarm"
CONF_DISCHARGE_OVER_TEMPERATURE_PROTECTION            = "discharge_over_temperature_protection"
CONF_DISCHARGE_OVER_TEMPERATURE_PROTECTION_RELEASE    = "discharge_over_temperature_protection_release"

CONF_CHARGE_UNDER_TEMPERATURE_ALARM                   = "charge_under_temperature_alarm"
CONF_CHARGE_UNDER_TEMPERATURE_PROTECTION              = "charge_under_temperature_protection"
CONF_CHARGE_UNDER_TEMPERATURE_PROTECTION_RELEASE      = "charge_under_temperature_protection_release"

CONF_DISCHARGE_UNDER_TEMPERATURE_ALARM                = "discharge_under_temperature_alarm"
CONF_DISCHARGE_UNDER_TEMPERATURE_PROTECTION           = "discharge_under_temperature_protection"
CONF_DISCHARGE_UNDER_TEMPERATURE_PROTECTION_RELEASE   = "discharge_under_temperature_protection_release"

CONF_MOSFET_OVER_TEMPERATURE_ALARM                    = "mosfet_over_temperature_alarm"
CONF_MOSFET_OVER_TEMPERATURE_PROTECTION               = "mosfet_over_temperature_protection"
CONF_MOSFET_OVER_TEMPERATURE_PROTECTION_RELEASE       = "mosfet_over_temperature_protection_release"

CONF_ENVIRONMENT_UNDER_TEMPERATURE_ALARM              = "environment_under_temperature_alarm"
CONF_ENVIRONMENT_UNDER_TEMPERATURE_PROTECTION         = "environment_under_temperature_protection"
CONF_ENVIRONMENT_UNDER_TEMPERATURE_PROTECTION_RELEASE = "environment_under_temperature_protection_release"
CONF_ENVIRONMENT_OVER_TEMPERATURE_ALARM               = "environment_over_temperature_alarm"
CONF_ENVIRONMENT_OVER_TEMPERATURE_PROTECTION          = "environment_over_temperature_protection"
CONF_ENVIRONMENT_OVER_TEMPERATURE_PROTECTION_RELEASE  = "environment_over_temperature_protection_release"


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PaceBmsNumber),
        cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBms),

        cv.Optional(CONF_CELL_OVER_VOLTAGE_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_CELL_OVER_VOLTAGE_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_CELL_OVER_VOLTAGE_PROTECTION_RELEASE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_CELL_OVER_VOLTAGE_PROTECTION_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),

        cv.Optional(CONF_PACK_OVER_VOLTAGE_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_PACK_OVER_VOLTAGE_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_PACK_OVER_VOLTAGE_PROTECTION_RELEASE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_PACK_OVER_VOLTAGE_PROTECTION_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),

        cv.Optional(CONF_CELL_UNDER_VOLTAGE_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_CELL_UNDER_VOLTAGE_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_CELL_UNDER_VOLTAGE_PROTECTION_RELEASE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_CELL_UNDER_VOLTAGE_PROTECTION_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),

        cv.Optional(CONF_PACK_UNDER_VOLTAGE_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_PACK_UNDER_VOLTAGE_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_PACK_UNDER_VOLTAGE_PROTECTION_RELEASE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_PACK_UNDER_VOLTAGE_PROTECTION_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),

        cv.Optional(CONF_CHARGE_OVER_CURRENT_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_CURRENT,
            unit_of_measurement=UNIT_AMPERE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_CHARGE_OVER_CURRENT_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_CURRENT,
            unit_of_measurement=UNIT_AMPERE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_CHARGE_OVER_CURRENT_PROTECTION_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),

        cv.Optional(CONF_DISCHARGE_OVER_CURRENT1_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_CURRENT,
            unit_of_measurement=UNIT_AMPERE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_DISCHARGE_OVER_CURRENT1_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_CURRENT,
            unit_of_measurement=UNIT_AMPERE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_DISCHARGE_OVER_CURRENT1_PROTECTION_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),

        cv.Optional(CONF_DISCHARGE_OVER_CURRENT2_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_CURRENT,
            unit_of_measurement=UNIT_AMPERE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_DISCHARGE_OVER_CURRENT2_PROTECTION_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),

        cv.Optional(CONF_SHORT_CIRCUIT_PROTECTION_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_MILLISECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),

        cv.Optional(CONF_CELL_BALANCING_THRESHOLD): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_CELL_BALANCING_DELTA): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),

        cv.Optional(CONF_SLEEP_CELL_VOLTAGE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_SLEEP_DELAY): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_MINUTE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),

         cv.Optional(CONF_FULL_CHARGE_VOLTAGE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_VOLTAGE,
            unit_of_measurement=UNIT_VOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_FULL_CHARGE_AMPS): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_CURRENT,
            unit_of_measurement=UNIT_AMPERE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_LOW_CHARGE_ALARM_PERCENT): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_BATTERY,
            unit_of_measurement=UNIT_PERCENT,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        
         cv.Optional(CONF_CHARGE_OVER_TEMPERATURE_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_CHARGE_OVER_TEMPERATURE_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_CHARGE_OVER_TEMPERATURE_PROTECTION_RELEASE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        
         cv.Optional(CONF_DISCHARGE_OVER_TEMPERATURE_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_DISCHARGE_OVER_TEMPERATURE_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_DISCHARGE_OVER_TEMPERATURE_PROTECTION_RELEASE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        
         cv.Optional(CONF_CHARGE_UNDER_TEMPERATURE_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_CHARGE_UNDER_TEMPERATURE_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_CHARGE_UNDER_TEMPERATURE_PROTECTION_RELEASE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        
         cv.Optional(CONF_DISCHARGE_UNDER_TEMPERATURE_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_DISCHARGE_UNDER_TEMPERATURE_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_DISCHARGE_UNDER_TEMPERATURE_PROTECTION_RELEASE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        
         cv.Optional(CONF_MOSFET_OVER_TEMPERATURE_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_MOSFET_OVER_TEMPERATURE_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_MOSFET_OVER_TEMPERATURE_PROTECTION_RELEASE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),        
        
         cv.Optional(CONF_ENVIRONMENT_OVER_TEMPERATURE_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_ENVIRONMENT_OVER_TEMPERATURE_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_ENVIRONMENT_OVER_TEMPERATURE_PROTECTION_RELEASE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),        
         cv.Optional(CONF_ENVIRONMENT_UNDER_TEMPERATURE_ALARM): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_ENVIRONMENT_UNDER_TEMPERATURE_PROTECTION): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),
        cv.Optional(CONF_ENVIRONMENT_UNDER_TEMPERATURE_PROTECTION_RELEASE): number.number_schema(
            PaceBmsNumberImplementation,
            device_class=DEVICE_CLASS_TEMPERATURE,
            unit_of_measurement=UNIT_CELSIUS,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend({ cv.Optional(CONF_MODE, default=NUMBER_MODE_BOX): cv.enum(NUMBER_MODES, upper=True), }),        
        
   }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_PACE_BMS_ID])
    cg.add(var.set_parent(parent))

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
            min_value=0.1, 
            max_value=0.5, 
            step=0.05)
        cg.add(var.set_short_circuit_protection_delay_number(num))

    if cell_balancing_threshold_config := config.get(CONF_CELL_BALANCING_THRESHOLD):
        num = await number.new_number(
            cell_balancing_threshold_config, 
            min_value=3.3, 
            max_value=4.5, 
            step=0.01)
        cg.add(var.set_cell_balancing_threshold_number(num))
    if cell_balancing_delta_config := config.get(CONF_CELL_BALANCING_DELTA):
        num = await number.new_number(
            cell_balancing_delta_config, 
            min_value=0.020, 
            max_value=0.500, 
            step=0.005)
        cg.add(var.set_cell_balancing_delta_number(num))

    if sleep_cell_voltage_config := config.get(CONF_SLEEP_CELL_VOLTAGE):
        num = await number.new_number(
            sleep_cell_voltage_config, 
            min_value=2, 
            max_value=4, 
            step=0.01)
        cg.add(var.set_sleep_cell_voltage_number(num))
    if sleep_delay_config := config.get(CONF_SLEEP_DELAY):
        num = await number.new_number(
            sleep_delay_config, 
            min_value=1, 
            max_value=120, 
            step=1)
        cg.add(var.set_sleep_delay_number(num))

    if full_charge_voltage_config := config.get(CONF_FULL_CHARGE_VOLTAGE):
        num = await number.new_number(
            full_charge_voltage_config, 
            min_value=20, 
            max_value=65, 
            step=0.01)
        cg.add(var.set_full_charge_voltage_number(num))
    if full_charge_amps_config := config.get(CONF_FULL_CHARGE_AMPS):
        num = await number.new_number(
            full_charge_amps_config, 
            min_value=0.5, 
            max_value=5, 
            step=0.5)
        cg.add(var.set_full_charge_amps_number(num))
    if low_charge_alarm_percent_config := config.get(CONF_LOW_CHARGE_ALARM_PERCENT):
        num = await number.new_number(
            low_charge_alarm_percent_config, 
            min_value=0, 
            max_value=100, 
            step=1)
        cg.add(var.set_low_charge_alarm_percent_number(num))

    if charge_over_temperature_alarm_config := config.get(CONF_CHARGE_OVER_TEMPERATURE_ALARM):
        num = await number.new_number(
            charge_over_temperature_alarm_config, 
            min_value=20, 
            max_value=100, 
            step=1)
        cg.add(var.set_charge_over_temperature_alarm_number(num))
    if charge_over_temperature_protection_config := config.get(CONF_CHARGE_OVER_TEMPERATURE_PROTECTION):
        num = await number.new_number(
            charge_over_temperature_protection_config, 
            min_value=20, 
            max_value=100, 
            step=1)
        cg.add(var.set_charge_over_temperature_protection_number(num))
    if charge_over_temperature_protection_release_config := config.get(CONF_CHARGE_OVER_TEMPERATURE_PROTECTION_RELEASE):
        num = await number.new_number(
            charge_over_temperature_protection_release_config, 
            min_value=20, 
            max_value=100, 
            step=1)
        cg.add(var.set_charge_over_temperature_protection_release_number(num))

    if discharge_over_temperature_alarm_config := config.get(CONF_DISCHARGE_OVER_TEMPERATURE_ALARM):
        num = await number.new_number(
            discharge_over_temperature_alarm_config, 
            min_value=20, 
            max_value=100, 
            step=1)
        cg.add(var.set_discharge_over_temperature_alarm_number(num))
    if discharge_over_temperature_protection_config := config.get(CONF_DISCHARGE_OVER_TEMPERATURE_PROTECTION):
        num = await number.new_number(
            discharge_over_temperature_protection_config, 
            min_value=20, 
            max_value=100, 
            step=1)
        cg.add(var.set_discharge_over_temperature_protection_number(num))
    if discharge_over_temperature_protection_release_config := config.get(CONF_DISCHARGE_OVER_TEMPERATURE_PROTECTION_RELEASE):
        num = await number.new_number(
            discharge_over_temperature_protection_release_config, 
            min_value=20, 
            max_value=100, 
            step=1)
        cg.add(var.set_discharge_over_temperature_protection_release_number(num))

    if charge_under_temperature_alarm_config := config.get(CONF_CHARGE_UNDER_TEMPERATURE_ALARM):
        num = await number.new_number(
            charge_under_temperature_alarm_config, 
            min_value=-35, 
            max_value=30, 
            step=1)
        cg.add(var.set_charge_under_temperature_alarm_number(num))
    if charge_under_temperature_protection_config := config.get(CONF_CHARGE_UNDER_TEMPERATURE_PROTECTION):
        num = await number.new_number(
            charge_under_temperature_protection_config, 
            min_value=-35, 
            max_value=30, 
            step=1)
        cg.add(var.set_charge_under_temperature_protection_number(num))
    if charge_under_temperature_protection_release_config := config.get(CONF_CHARGE_UNDER_TEMPERATURE_PROTECTION_RELEASE):
        num = await number.new_number(
            charge_under_temperature_protection_release_config, 
            min_value=-35, 
            max_value=30, 
            step=1)
        cg.add(var.set_charge_under_temperature_protection_release_number(num))

    if discharge_under_temperature_alarm_config := config.get(CONF_DISCHARGE_UNDER_TEMPERATURE_ALARM):
        num = await number.new_number(
            discharge_under_temperature_alarm_config, 
            min_value=-35, 
            max_value=30, 
            step=1)
        cg.add(var.set_discharge_under_temperature_alarm_number(num))
    if discharge_under_temperature_protection_config := config.get(CONF_DISCHARGE_UNDER_TEMPERATURE_PROTECTION):
        num = await number.new_number(
            discharge_under_temperature_protection_config, 
            min_value=-35, 
            max_value=30, 
            step=1)
        cg.add(var.set_discharge_under_temperature_protection_number(num))
    if discharge_under_temperature_protection_release_config := config.get(CONF_DISCHARGE_UNDER_TEMPERATURE_PROTECTION_RELEASE):
        num = await number.new_number(
            discharge_under_temperature_protection_release_config, 
            min_value=-35, 
            max_value=30, 
            step=1)
        cg.add(var.set_discharge_under_temperature_protection_release_number(num))

    if mosfet_over_temperature_alarm_config := config.get(CONF_MOSFET_OVER_TEMPERATURE_ALARM):
        num = await number.new_number(
            mosfet_over_temperature_alarm_config, 
            min_value=30, 
            max_value=120, 
            step=1)
        cg.add(var.set_mosfet_over_temperature_alarm_number(num))
    if mosfet_over_temperature_protection_config := config.get(CONF_MOSFET_OVER_TEMPERATURE_PROTECTION):
        num = await number.new_number(
            mosfet_over_temperature_protection_config, 
            min_value=30, 
            max_value=120, 
            step=1)
        cg.add(var.set_mosfet_over_temperature_protection_number(num))
    if mosfet_over_temperature_protection_release_config := config.get(CONF_MOSFET_OVER_TEMPERATURE_PROTECTION_RELEASE):
        num = await number.new_number(
            mosfet_over_temperature_protection_release_config, 
            min_value=30, 
            max_value=120, 
            step=1)
        cg.add(var.set_mosfet_over_temperature_protection_release_number(num))

    if environment_under_temperature_alarm_config := config.get(CONF_ENVIRONMENT_UNDER_TEMPERATURE_ALARM):
        num = await number.new_number(
            environment_under_temperature_alarm_config, 
            min_value=-35, 
            max_value=30, 
            step=1)
        cg.add(var.set_environment_under_temperature_alarm_number(num))
    if environment_under_temperature_protection_config := config.get(CONF_ENVIRONMENT_UNDER_TEMPERATURE_PROTECTION):
        num = await number.new_number(
            environment_under_temperature_protection_config, 
            min_value=-35, 
            max_value=30, 
            step=1)
        cg.add(var.set_environment_under_temperature_protection_number(num))
    if environment_under_temperature_protection_release_config := config.get(CONF_ENVIRONMENT_UNDER_TEMPERATURE_PROTECTION_RELEASE):
        num = await number.new_number(
            environment_under_temperature_protection_release_config, 
            min_value=-35, 
            max_value=30, 
            step=1)
        cg.add(var.set_environment_under_temperature_protection_release_number(num))
    if environment_over_temperature_alarm_config := config.get(CONF_ENVIRONMENT_OVER_TEMPERATURE_ALARM):
        num = await number.new_number(
            environment_over_temperature_alarm_config, 
            min_value=20, 
            max_value=100, 
            step=1)
        cg.add(var.set_environment_over_temperature_alarm_number(num))
    if environment_over_temperature_protection_config := config.get(CONF_ENVIRONMENT_OVER_TEMPERATURE_PROTECTION):
        num = await number.new_number(
            environment_over_temperature_protection_config, 
            min_value=20, 
            max_value=100, 
            step=1)
        cg.add(var.set_environment_over_temperature_protection_number(num))
    if environment_over_temperature_protection_release_config := config.get(CONF_ENVIRONMENT_OVER_TEMPERATURE_PROTECTION_RELEASE):
        num = await number.new_number(
            environment_over_temperature_protection_release_config, 
            min_value=20, 
            max_value=100, 
            step=1)
        cg.add(var.set_environment_over_temperature_protection_release_number(num))
