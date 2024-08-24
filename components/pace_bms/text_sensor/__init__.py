import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_EMPTY,
    STATE_CLASS_NONE,
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

CONF_HARDWARE_VERSION     = "hardware_version"
CONF_SERIAL_NUMBER        = "serial_number"



CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PaceBmsTextSensor),
        cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBms),
        cv.Optional(CONF_WARNING_STATUS): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_BALANCING_STATUS): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_SYSTEM_STATUS): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_CONFIGURATION_STATUS): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_PROTECTION_STATUS): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_FAULT_STATUS): text_sensor.text_sensor_schema(),

        cv.Optional(CONF_HARDWARE_VERSION): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_SERIAL_NUMBER): text_sensor.text_sensor_schema(),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    paren = await cg.get_variable(config[CONF_PACE_BMS_ID])
    cg.add(var.set_parent(paren))

    if warning_status_config := config.get(CONF_WARNING_STATUS):
        sens = await text_sensor.new_text_sensor(warning_status_config)
        cg.add(var.set_warning_status_sensor(sens))

    if balancing_status_config := config.get(CONF_BALANCING_STATUS):
        sens = await text_sensor.new_text_sensor(balancing_status_config)
        cg.add(var.set_balancing_status_sensor(sens))

    if system_status_config := config.get(CONF_SYSTEM_STATUS):
        sens = await text_sensor.new_text_sensor(system_status_config)
        cg.add(var.set_system_status_sensor(sens))

    if configuration_status_config := config.get(CONF_CONFIGURATION_STATUS):
        sens = await text_sensor.new_text_sensor(configuration_status_config)
        cg.add(var.set_configuration_status_sensor(sens))

    if protection_status_config := config.get(CONF_PROTECTION_STATUS):
        sens = await text_sensor.new_text_sensor(protection_status_config)
        cg.add(var.set_protection_status_sensor(sens))

    if fault_status_config := config.get(CONF_FAULT_STATUS):
        sens = await text_sensor.new_text_sensor(fault_status_config)
        cg.add(var.set_fault_status_sensor(sens))

    if hardware_version_config := config.get(CONF_HARDWARE_VERSION):
        sens = await text_sensor.new_text_sensor(hardware_version_config)
        cg.add(var.set_hardware_version_sensor(sens))

    if serial_number_config := config.get(CONF_SERIAL_NUMBER):
        sens = await text_sensor.new_text_sensor(serial_number_config)
        cg.add(var.set_serial_number_sensor(sens))

