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

CONF_CHEMISTRY           = "chemistry"
CONF_CELL_COUNT          = "v20_cell_count"
CONF_TEMPERATURE_COUNT   = "v20_temperature_count"
CONF_SKIP_UD2            = "v20_skip_ud2"
CONF_SKIP_SOC_DC         = "v20_skip_soc_dc"
CONF_SKIP_SOH_PV         = "v20_skip_soh_pv"
CONF_DESIGN_CAPACITY_MAH = "v20_design_capacity_mah"
CONF_SKIP_STATUS_FLAGS   = "v20_skip_status_flags"
CONF_PROTOCOL_VERSION    = "protocol_version"
CONF_REQUEST_THROTTLE    = "request_throttle"
CONF_RESPONSE_TIMEOUT    = "response_timeout"

DEFAULT_ADDRESS = 1
DEFAULT_CHEMISTRY = 0x46
DEFAULT_CELL_COUNT = 0
DEFAULT_TEMPERATURE_COUNT = 0
DEFAULT_SKIP_UD2 = False
DEFAULT_SKIP_SOC_DC = True
DEFAULT_SKIP_SOH_PV = True
DEFAULT_DESIGN_CAPACITY_MAH = 0
DEFAULT_SKIP_STATUS_FLAGS = False
DEFAULT_PROTOCOL_VERSION = 0x25
DEFAULT_REQUEST_THROTTLE = "50ms"
DEFAULT_RESPONSE_TIMEOUT = "200ms"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(PaceBms),
            cv.Optional(CONF_FLOW_CONTROL_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_ADDRESS, default=DEFAULT_ADDRESS): cv.int_range(min=0, max=15),
            cv.Optional(CONF_CHEMISTRY, default=DEFAULT_CHEMISTRY): cv.int_range(min=0, max=255),
            cv.Optional(CONF_CELL_COUNT, default=DEFAULT_CELL_COUNT): cv.int_range(min=0, max=16),
            cv.Optional(CONF_TEMPERATURE_COUNT, default=DEFAULT_TEMPERATURE_COUNT): cv.int_range(min=0, max=6),
            cv.Optional(CONF_SKIP_UD2, default=DEFAULT_SKIP_UD2): cv.boolean,
            cv.Optional(CONF_SKIP_SOC_DC, default=DEFAULT_SKIP_SOC_DC): cv.boolean,
            cv.Optional(CONF_SKIP_SOH_PV, default=DEFAULT_SKIP_SOH_PV): cv.boolean,
            cv.Optional(CONF_DESIGN_CAPACITY_MAH, default=DEFAULT_DESIGN_CAPACITY_MAH): cv.int_range(min=0, max=1000000),
            cv.Optional(CONF_SKIP_STATUS_FLAGS, default=DEFAULT_SKIP_STATUS_FLAGS): cv.boolean,
            cv.Optional(CONF_PROTOCOL_VERSION, default=DEFAULT_PROTOCOL_VERSION): cv.int_range(min=0x20, max=0x25),
            cv.Optional(CONF_REQUEST_THROTTLE, default=DEFAULT_REQUEST_THROTTLE): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_RESPONSE_TIMEOUT, default=DEFAULT_RESPONSE_TIMEOUT): cv.positive_time_period_milliseconds,
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
    if CONF_CHEMISTRY in config:
        cg.add(var.set_chemistry(config[CONF_CHEMISTRY]))
    if CONF_CELL_COUNT in config:
        cg.add(var.set_cell_count(config[CONF_CELL_COUNT]))
    if CONF_TEMPERATURE_COUNT in config:
        cg.add(var.set_temperature_count(config[CONF_TEMPERATURE_COUNT]))
    if CONF_SKIP_UD2 in config:
        cg.add(var.set_skip_ud2(config[CONF_SKIP_UD2]))
    if CONF_SKIP_SOC_DC in config:
        cg.add(var.set_skip_soc_dc(config[CONF_SKIP_SOC_DC]))
    if CONF_SKIP_SOH_PV in config:
        cg.add(var.set_skip_soh_pv(config[CONF_SKIP_SOH_PV]))
    if CONF_DESIGN_CAPACITY_MAH in config:
        cg.add(var.set_design_capacity_mah(config[CONF_DESIGN_CAPACITY_MAH]))
    if CONF_SKIP_STATUS_FLAGS in config:
        cg.add(var.set_skip_status_flags(config[CONF_SKIP_STATUS_FLAGS]))
    if CONF_PROTOCOL_VERSION in config:
        cg.add(var.set_protocol_version(config[CONF_PROTOCOL_VERSION]))
    if CONF_REQUEST_THROTTLE in config:
        cg.add(var.set_request_throttle(config[CONF_REQUEST_THROTTLE]))
    if CONF_RESPONSE_TIMEOUT in config:
        cg.add(var.set_response_timeout(config[CONF_RESPONSE_TIMEOUT]))

