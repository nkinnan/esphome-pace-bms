import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.cpp_helpers import gpio_pin_expression
from esphome.components import uart
from esphome.const import (
    CONF_ID,
    CONF_FLOW_CONTROL_PIN,
    CONF_ADDRESS,
)
from esphome import pins

CODEOWNERS = ["@nkinnan"]

DEPENDENCIES = ["uart"]

pace_bms_ns = cg.esphome_ns.namespace("pace_bms")
PaceBms = pace_bms_ns.class_("PaceBms", cg.PollingComponent, uart.UARTDevice)

# "this" for pace_bms_sensor/text_sensor/switch/etc. to get parent from
CONF_PACE_BMS_ID = "pace_bms_id"

CONF_PROTOCOL_VERSION = "protocol_version"
CONF_REQUEST_THROTTLE = "request_throttle"
CONF_RESPONSE_TIMEOUT = "response_timeout"

DEFAULT_ADDRESS = 0
DEFAULT_PROTOCOL_VERSION = 0x25
DEFAULT_REQUEST_THROTTLE = "50ms"
DEFAULT_RESPONSE_TIMEOUT = "200ms"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(PaceBms),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "pace_bms", baud_rate=9600, require_rx=True, require_tx=True, 
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    await uart.register_uart_device(var, config)

    if CONF_FLOW_CONTROL_PIN in config:
        pin = await gpio_pin_expression(config[CONF_FLOW_CONTROL_PIN])
        cg.add(var.set_flow_control_pin(pin))
    if CONF_ADDRESS in config:
        cg.add(var.set_address(config[CONF_ADDRESS]))
    if CONF_PROTOCOL_VERSION in config:
        cg.add(var.set_protocol_version(config[CONF_PROTOCOL_VERSION]))
    if CONF_REQUEST_THROTTLE in config:
        cg.add(var.set_request_throttle(config[CONF_REQUEST_THROTTLE]))
    if CONF_RESPONSE_TIMEOUT in config:
        cg.add(var.set_response_timeout(config[CONF_RESPONSE_TIMEOUT]))

