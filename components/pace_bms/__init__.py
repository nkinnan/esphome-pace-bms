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

CONF_CHEMISTRY                   = "chemistry"
CONF_SKIP_ADDRESS_PAYLOAD        = "v20_skip_address_payload"
CONF_ANALOG_CELL_COUNT           = "analog_cell_count_override"
CONF_ANALOG_TEMPERATURE_COUNT    = "analog_temperature_count_override"
CONF_STATUS_CELL_COUNT           = "status_cell_count_override"
CONF_STATUS_TEMPERATURE_COUNT    = "status_temperature_count_override"
CONF_SKIP_UD2                    = "v20_skip_ud2"
CONF_SKIP_SOC                    = "v20_skip_soc"
CONF_SKIP_DC                     = "v20_skip_dc"
CONF_SKIP_SOH                    = "v20_skip_soh"
CONF_SKIP_PV                     = "v20_skip_pv"
CONF_DESIGN_CAPACITY_MAH         = "design_capacity_mah_override"
CONF_SKIP_STATUS_FLAGS           = "v20_skip_status_flags"
CONF_PROTOCOL_VERSION            = "protocol_version"
CONF_REQUEST_THROTTLE            = "request_throttle"
CONF_RESPONSE_TIMEOUT            = "response_timeout"

DEFAULT_ADDRESS = 1
DEFAULT_CHEMISTRY = 0x46
DEFAULT_SKIP_ADDRESS_PAYLOAD = False
#DEFAULT_ANALOG_CELL_COUNT = 0
#DEFAULT_ANALOG_TEMPERATURE_COUNT = 0
#DEFAULT_STATUS_CELL_COUNT = 0
#DEFAULT_STATUS_TEMPERATURE_COUNT = 0
DEFAULT_SKIP_UD2 = False
DEFAULT_SKIP_SOC = True
DEFAULT_SKIP_DC = True
DEFAULT_SKIP_SOH = True
DEFAULT_SKIP_PV = True
DEFAULT_DESIGN_CAPACITY_MAH = 0
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
            cv.Optional(CONF_SKIP_ADDRESS_PAYLOAD, default=DEFAULT_SKIP_ADDRESS_PAYLOAD): cv.boolean,
            cv.Optional(CONF_ANALOG_CELL_COUNT): cv.int_range(min=0, max=16),
            cv.Optional(CONF_ANALOG_TEMPERATURE_COUNT): cv.int_range(min=0, max=6),
            cv.Optional(CONF_STATUS_CELL_COUNT): cv.int_range(min=0, max=16),
            cv.Optional(CONF_STATUS_TEMPERATURE_COUNT): cv.int_range(min=0, max=6),
            cv.Optional(CONF_SKIP_UD2, default=DEFAULT_SKIP_UD2): cv.boolean,
            cv.Optional(CONF_SKIP_SOC, default=DEFAULT_SKIP_SOC): cv.boolean,
            cv.Optional(CONF_SKIP_DC, default=DEFAULT_SKIP_DC): cv.boolean,
            cv.Optional(CONF_SKIP_SOH, default=DEFAULT_SKIP_SOH): cv.boolean,
            cv.Optional(CONF_SKIP_PV, default=DEFAULT_SKIP_PV): cv.boolean,
            cv.Optional(CONF_DESIGN_CAPACITY_MAH, default=DEFAULT_DESIGN_CAPACITY_MAH): cv.int_range(min=0, max=1000000),
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
    if CONF_SKIP_ADDRESS_PAYLOAD in config:
        cg.add(var.set_skip_address_payload(config[CONF_SKIP_ADDRESS_PAYLOAD]))
    if CONF_ANALOG_CELL_COUNT in config:
        cg.add(var.set_analog_cell_count(config[CONF_ANALOG_CELL_COUNT]))
    if CONF_ANALOG_TEMPERATURE_COUNT in config:
        cg.add(var.set_analog_temperature_count(config[CONF_ANALOG_TEMPERATURE_COUNT]))
    if CONF_STATUS_CELL_COUNT in config:
        cg.add(var.set_status_cell_count(config[CONF_STATUS_CELL_COUNT]))
    if CONF_STATUS_TEMPERATURE_COUNT in config:
        cg.add(var.set_status_temperature_count(config[CONF_STATUS_TEMPERATURE_COUNT]))
    if CONF_SKIP_UD2 in config:
        cg.add(var.set_skip_ud2(config[CONF_SKIP_UD2]))
    if CONF_SKIP_SOC in config:
        cg.add(var.set_skip_soc(config[CONF_SKIP_SOC]))
    if CONF_SKIP_DC in config:
        cg.add(var.set_skip_dc(config[CONF_SKIP_DC]))
    if CONF_SKIP_SOH in config:
        cg.add(var.set_skip_soh(config[CONF_SKIP_SOH]))
    if CONF_SKIP_PV in config:
        cg.add(var.set_skip_pv(config[CONF_SKIP_PV]))
    if CONF_DESIGN_CAPACITY_MAH in config:
        cg.add(var.set_design_capacity_mah(config[CONF_DESIGN_CAPACITY_MAH]))
    if CONF_PROTOCOL_VERSION in config:
        cg.add(var.set_protocol_version(config[CONF_PROTOCOL_VERSION]))
    if CONF_REQUEST_THROTTLE in config:
        cg.add(var.set_request_throttle(config[CONF_REQUEST_THROTTLE]))
    if CONF_RESPONSE_TIMEOUT in config:
        cg.add(var.set_response_timeout(config[CONF_RESPONSE_TIMEOUT]))

