import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.cpp_helpers import gpio_pin_expression
from esphome.components import uart
from esphome.const import (
    CONF_ID,
    CONF_DEVICE_ID,
    CONF_FLOW_CONTROL_PIN,
    CONF_ADDRESS,
    CONF_TYPE
)
from esphome import pins


CODEOWNERS = ["@nkinnan"]
DEPENDENCIES = ["uart"]
MULTI_CONF = True

pace_bms_base_ns = cg.esphome_ns.namespace("pace_bms_base")
pace_bms_master_ns = cg.esphome_ns.namespace("pace_bms_master")
pace_bms_slave_ns = cg.esphome_ns.namespace("pace_bms_slave")

PaceBmsBase = pace_bms_base_ns.class_("PaceBmsBase")
PaceBmsMaster = pace_bms_master_ns.class_("PaceBmsMaster", PaceBmsBase, cg.PollingComponent, uart.UARTDevice)
PaceBmsSlave = pace_bms_slave_ns.class_("PaceBmsSlave", PaceBmsBase, cg.Component)


SlaveDiscoveryMode = pace_bms_master_ns.enum("SlaveDiscoveryMode")

SLAVE_DISCOVERY_MODE = {
    "NONE": SlaveDiscoveryMode.SLAVE_DISCOVERY_MODE_NONE,
    "RELAY": SlaveDiscoveryMode.SLAVE_DISCOVERY_MODE_RELAY,
    "BROADCAST": SlaveDiscoveryMode.SLAVE_DISCOVERY_MODE_BROADCAST,
    "RELAY_AND_BROADCAST": SlaveDiscoveryMode.SLAVE_DISCOVERY_MODE_RELAY_AND_BROADCAST,
}


CONF_PACE_BMS    = "pace_bms"
CONF_PACE_BMS_ID = "pace_bms_id"
CONF_MASTER_BMS_ID = "master_bms_id"

CONF_TYPE_MASTER = "master"
CONF_TYPE_SLAVE = "slave"

CONF_TYPE_ENUM = {
    CONF_TYPE_MASTER: 0,
    CONF_TYPE_SLAVE: 1,
}


CONF_RESPONDING_ADDRESS          = "responding_address"

CONF_PROTOCOL_COMMANDSET         = "protocol_commandset"
CONF_PROTOCOL_VARIANT            = "protocol_variant"
CONF_PROTOCOL_VERSION            = "protocol_version"
CONF_CHEMISTRY                   = "battery_chemistry"

CONF_REQUEST_THROTTLE            = "request_throttle"
CONF_RESPONSE_TIMEOUT            = "response_timeout"

CONF_SLAVE_DISCOVERY_MODE        = "slave_discovery_mode"

CONF_RX_BUFFER_SIZE              = "rx_buffer_size"


DEFAULT_CONF_TYPE = "master"

DEFAULT_ADDRESS = 1
#DEFAULT_RESPONDING_ADDRESS = 1

#DEFAULT_FLOW_CONTROL_PIN = 

DEFAULT_PROTOCOL_COMMANDSET = 0x25
#DEFAULT_PROTOCOL_VARIANT = 
#DEFAULT_PROTOCOL_VERSION = 
#DEFAULT_CHEMISTRY = 

DEFAULT_REQUEST_THROTTLE = "50ms"
DEFAULT_RESPONSE_TIMEOUT = "200ms"

DEFAULT_SLAVE_DISCOVERY_MODE = "NONE"

DEFAULT_RX_BUFFER_SIZE = 256


BASE_SCHEMA = cv.Schema({
    # todo why isn't default working for CONF_TYPE??????
    cv.Optional(CONF_TYPE, default=DEFAULT_CONF_TYPE): cv.enum(CONF_TYPE_ENUM, upper=False),
    cv.Optional(CONF_DEVICE_ID): cv.sub_device_id,

    cv.Optional(CONF_ADDRESS, default=DEFAULT_ADDRESS): cv.int_range(min=0, max=15),
    cv.Optional(CONF_RESPONDING_ADDRESS): cv.int_range(min=0, max=15),
})

CONFIG_SCHEMA = cv.typed_schema({
    CONF_TYPE_MASTER: BASE_SCHEMA.extend({
        cv.GenerateID(): cv.declare_id(PaceBmsMaster),

        cv.Optional(CONF_FLOW_CONTROL_PIN): pins.gpio_output_pin_schema,

        cv.Optional(CONF_PROTOCOL_COMMANDSET, default=DEFAULT_PROTOCOL_COMMANDSET): cv.int_range(min=0x20, max=0x25),
        cv.Optional(CONF_PROTOCOL_VARIANT): cv.string_strict,
        cv.Optional(CONF_PROTOCOL_VERSION): cv.int_range(min=0, max=255),
        cv.Optional(CONF_CHEMISTRY): cv.int_range(min=0, max=255),

        cv.Optional(CONF_REQUEST_THROTTLE, default=DEFAULT_REQUEST_THROTTLE): cv.positive_time_period_milliseconds,
        cv.Optional(CONF_RESPONSE_TIMEOUT, default=DEFAULT_RESPONSE_TIMEOUT): cv.positive_time_period_milliseconds,

        cv.Optional(CONF_SLAVE_DISCOVERY_MODE, default=DEFAULT_SLAVE_DISCOVERY_MODE): cv.enum(SLAVE_DISCOVERY_MODE, upper=True),

        cv.Optional(CONF_RX_BUFFER_SIZE, default=DEFAULT_RX_BUFFER_SIZE): cv.int_range(min=256, max=4096),
    })
    .extend(cv.polling_component_schema("60s"))
    .extend(uart.UART_DEVICE_SCHEMA),

    CONF_TYPE_SLAVE: BASE_SCHEMA.extend({
        cv.GenerateID(): cv.declare_id(PaceBmsSlave),

        # point back to master
        cv.GenerateID(CONF_MASTER_BMS_ID): cv.use_id(PaceBmsMaster),
    }).extend(cv.COMPONENT_SCHEMA)
},lower=True)


# todo why doesn't this work???????
# once the schema is fully constructed, run any final checks, for example on values pulled in from the yaml
#FINAL_VALIDATE_SCHEMA = cv.typed_schema({
#    CONF_TYPE_MASTER: cv.Schema({
#        uart.final_validate_device_schema(CONF_PACE_BMS, baud_rate=9600, require_rx=True, require_tx=True, data_bits=8, parity="NONE", stop_bits=1),
#    }),
#
#    CONF_TYPE_SLAVE: cv.Schema({
#    })
#},lower=True)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_id_name(str(config[CONF_ID])))

    cg.add(var.set_address(config[CONF_ADDRESS]))
    if CONF_RESPONDING_ADDRESS in config:
        cg.add(var.set_responding_address(config[CONF_RESPONDING_ADDRESS]))

    bms_type = config[CONF_TYPE]

    if bms_type == CONF_TYPE_MASTER:

        await uart.register_uart_device(var, config)

        if CONF_FLOW_CONTROL_PIN in config:
            pin = await gpio_pin_expression(config[CONF_FLOW_CONTROL_PIN])
            cg.add(var.set_flow_control_pin(pin))

        cg.add(var.set_protocol_commandset(config[CONF_PROTOCOL_COMMANDSET]))
        if CONF_PROTOCOL_VARIANT in config:
            cg.add(var.set_protocol_variant(config[CONF_PROTOCOL_VARIANT]))
        if CONF_PROTOCOL_VERSION in config:
            cg.add(var.set_protocol_version(config[CONF_PROTOCOL_VERSION]))
        if CONF_CHEMISTRY in config:
            cg.add(var.set_chemistry(config[CONF_CHEMISTRY]))

        cg.add(var.set_request_throttle(config[CONF_REQUEST_THROTTLE]))
        cg.add(var.set_response_timeout(config[CONF_RESPONSE_TIMEOUT]))

        cg.add(var.set_slave_discovery_mode(SLAVE_DISCOVERY_MODE[config.get(CONF_SLAVE_DISCOVERY_MODE)]))

        cg.add(var.set_rx_buffer_size(config[CONF_RX_BUFFER_SIZE]))

    if bms_type == CONF_TYPE_SLAVE:

        paren = await cg.get_variable(config[CONF_MASTER_BMS_ID])
        cg.add(var.set_parent(paren))
