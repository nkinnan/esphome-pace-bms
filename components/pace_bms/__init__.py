import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.cpp_helpers import gpio_pin_expression
from esphome.components import sensor, uart
from esphome.const import (
    CONF_FLOW_CONTROL_PIN,
    CONF_ID,
)
from esphome import pins

DEPENDENCIES = ["uart"]

pace_bms_ns = cg.esphome_ns.namespace("pace_bms")
PaceBms = pace_bms_ns.class_("PaceBmsComponent", cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(PaceBms),
            cv.Optional(CONF_FLOW_CONTROL_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.polling_component_schema("5s"))
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
