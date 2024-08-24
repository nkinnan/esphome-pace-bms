import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
)
from .. import pace_bms_ns, CONF_PACE_BMS_ID, PaceBms

DEPENDENCIES = ["pace_bms"]

PaceBmsTextSensor = pace_bms_ns.class_("PaceBmsTextSensor", cg.Component)


CONF_WARNING_STATUS       = "warning_status"
CONF_BALANCING_STATUS     = "balancing_status"
CONF_SYSTEM_STATUS        = "system_status"
CONF_CONFIGURATION_STATUS = "configuration_status"
CONF_PROTECTION_STATUS    = "protection_status"
CONF_FAULT_STATUS         = "fault_status"



CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PaceBmsTextSensor),
        cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBms),
        cv.Optional(CONF_WARNING_STATUS): sensor.sensor_schema(
            #unit_of_measurement=,
            #accuracy_decimals=,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_NONE,
        ),
        cv.Optional(CONF_BALANCING_STATUS): sensor.sensor_schema(
            #unit_of_measurement=,
            #accuracy_decimals=,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_NONE,
        ),
        cv.Optional(CONF_SYSTEM_STATUS): sensor.sensor_schema(
            #unit_of_measurement=,
            #accuracy_decimals=,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_NONE,
        ),
        cv.Optional(CONF_CONFIGURATION_STATUS): sensor.sensor_schema(
            #unit_of_measurement=,
            #accuracy_decimals=,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_NONE,
        ),
        cv.Optional(CONF_PROTECTION_STATUS): sensor.sensor_schema(
            #unit_of_measurement=,
            #accuracy_decimals=,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_NONE,
        ),
        cv.Optional(CONF_FAULT_STATUS): sensor.sensor_schema(
            #unit_of_measurement=,
            #accuracy_decimals=,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_NONE,
        ),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    paren = await cg.get_variable(config[CONF_PACE_BMS_ID])
    cg.add(var.set_parent(paren))

    if warning_status_config := config.get(CONF_WARNING_STATUS):
        sens = await sensor.new_sensor(warning_status_config)
        cg.add(var.set_warning_status_sensor(sens))

    if balancing_status_config := config.get(CONF_BALANCING_STATUS):
        sens = await sensor.new_sensor(balancing_status_config)
        cg.add(var.set_balancing_status_sensor(sens))

    if system_status_config := config.get(CONF_SYSTEM_STATUS):
        sens = await sensor.new_sensor(system_status_config)
        cg.add(var.set_system_status_sensor(sens))

    if configuration_status_config := config.get(CONF_CONFIGURATION_STATUS):
        sens = await sensor.new_sensor(configuration_status_config)
        cg.add(var.set_configuration_status_sensor(sens))

    if protection_status_config := config.get(CONF_PROTECTION_STATUS):
        sens = await sensor.new_sensor(protection_status_config)
        cg.add(var.set_protection_status_sensor(sens))

    if fault_status_config := config.get(CONF_FAULT_STATUS):
        sens = await sensor.new_sensor(fault_status_config)
        cg.add(var.set_fault_status_sensor(sens))

