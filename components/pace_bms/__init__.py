import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.cpp_helpers import gpio_pin_expression
from esphome.components import uart
from esphome.const import (
    CONF_ID,
    CONF_DEVICE_ID,
    CONF_FLOW_CONTROL_PIN,
    CONF_ADDRESS,
    CONF_TYPE,

    CONF_BUTTON,
    CONF_DATETIME,
    CONF_NUMBER,
    CONF_SENSOR,
    CONF_SWITCH,

    CONF_PLATFORM,
)
from esphome import pins
import esphome.final_validate as fv

from esphome.components.pace_bms import pace_bms_globals


# bizarrely these are not in esphome const.py
CONF_SELECT = "select"
CONF_TEXT_SENSOR = "text_sensor"


CODEOWNERS = ["@nkinnan"]
DEPENDENCIES = ["uart"]
MULTI_CONF = True

pace_bms_base_ns = cg.esphome_ns.namespace("pace_bms_base")
pace_bms_master_ns = cg.esphome_ns.namespace("pace_bms_master")
pace_bms_slave_ns = cg.esphome_ns.namespace("pace_bms_slave")

PaceBmsBase = pace_bms_base_ns.class_("PaceBmsBase")
PaceBmsMaster = pace_bms_master_ns.class_("PaceBmsMaster", PaceBmsBase, cg.PollingComponent, uart.UARTDevice)
PaceBmsSlave = pace_bms_slave_ns.class_("PaceBmsSlave", PaceBmsBase, cg.Component)


CONF_PACE_BMS    = "pace_bms"
CONF_PACE_BMS_ID = "pace_bms_id" # pointer from child component platform (sensor, number, etc.) back to BMS (master or slave)
CONF_MASTER_BMS_ID = "master_bms_id" # pointer from slave BMS back to master BMS


BmsType = pace_bms_base_ns.enum("BmsType")

BMS_TYPE = {
    "MASTER": BmsType.BMS_TYPE_MASTER,
    "SLAVE": BmsType.BMS_TYPE_SLAVE,
}

CONF_TYPE_MASTER = "MASTER"
CONF_TYPE_SLAVE = "SLAVE"


SlaveDiscoveryMode = pace_bms_master_ns.enum("SlaveDiscoveryMode")

SLAVE_DISCOVERY_MODE = {
    "NONE": SlaveDiscoveryMode.SLAVE_DISCOVERY_MODE_NONE,
    "RELAY": SlaveDiscoveryMode.SLAVE_DISCOVERY_MODE_RELAY,
    "BROADCAST": SlaveDiscoveryMode.SLAVE_DISCOVERY_MODE_BROADCAST,
    "RELAY_AND_BROADCAST": SlaveDiscoveryMode.SLAVE_DISCOVERY_MODE_RELAY_AND_BROADCAST,
}


BmsType = pace_bms_master_ns.enum("SlaveQueryMode")

SLAVE_QUERY_MODE = {
    "BROADCAST": BmsType.SLAVE_QUERY_MODE_BROADCAST,
    "RELAY": BmsType.SLAVE_QUERY_MODE_RELAY,
}


CONF_RESPONDING_ADDRESS          = "responding_address"

CONF_PROTOCOL_COMMANDSET         = "protocol_commandset"
CONF_PROTOCOL_VARIANT            = "protocol_variant"
CONF_PROTOCOL_VERSION            = "protocol_version"
CONF_CHEMISTRY                   = "battery_chemistry"

CONF_REQUEST_THROTTLE            = "request_throttle"
CONF_RESPONSE_TIMEOUT            = "response_timeout"

CONF_SLAVE_DISCOVERY_MODE        = "slave_discovery_mode"
CONF_SLAVE_QUERY_MODE            = "slave_query_mode"

CONF_RX_BUFFER_SIZE              = "rx_buffer_size"


DEFAULT_BMS_TYPE = "MASTER"

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
DEFAULT_SLAVE_QUERY_MODE = "BROADCAST"

DEFAULT_RX_BUFFER_SIZE = 256


BASE_SCHEMA = cv.Schema({
    cv.Optional(CONF_TYPE, default=DEFAULT_BMS_TYPE): cv.enum(BMS_TYPE, upper=True),
    cv.Optional(CONF_DEVICE_ID): cv.sub_device_id,

    cv.Optional(CONF_ADDRESS, default=DEFAULT_ADDRESS): cv.int_range(min=0, max=15),
})

CONFIG_SCHEMA = cv.All(
    pace_bms_globals.save_pace_bms_schema, 
    cv.typed_schema({
        CONF_TYPE_MASTER: BASE_SCHEMA.extend({
            cv.GenerateID(): cv.declare_id(PaceBmsMaster),

            cv.Optional(CONF_RESPONDING_ADDRESS): cv.int_range(min=0, max=15),

            cv.Optional(CONF_FLOW_CONTROL_PIN): pins.gpio_output_pin_schema,

            cv.Optional(CONF_PROTOCOL_COMMANDSET, default=DEFAULT_PROTOCOL_COMMANDSET): cv.int_range(min=0x20, max=0x25),
            cv.Optional(CONF_PROTOCOL_VARIANT): cv.string_strict,
            cv.Optional(CONF_PROTOCOL_VERSION): cv.int_range(min=0, max=255),
            cv.Optional(CONF_CHEMISTRY): cv.int_range(min=0, max=255),

            cv.Optional(CONF_REQUEST_THROTTLE, default=DEFAULT_REQUEST_THROTTLE): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_RESPONSE_TIMEOUT, default=DEFAULT_RESPONSE_TIMEOUT): cv.positive_time_period_milliseconds,

            cv.Optional(CONF_SLAVE_DISCOVERY_MODE, default=DEFAULT_SLAVE_DISCOVERY_MODE): cv.enum(SLAVE_DISCOVERY_MODE, upper=True),
            cv.Optional(CONF_SLAVE_QUERY_MODE, default=DEFAULT_SLAVE_QUERY_MODE): cv.enum(SLAVE_QUERY_MODE, upper=True),

            cv.Optional(CONF_RX_BUFFER_SIZE, default=DEFAULT_RX_BUFFER_SIZE): cv.int_range(min=256, max=4096),
        })
        .extend(cv.polling_component_schema("60s"))
        .extend(uart.UART_DEVICE_SCHEMA),

        CONF_TYPE_SLAVE: BASE_SCHEMA.extend({
            cv.GenerateID(): cv.declare_id(PaceBmsSlave),

            # point back to master
            cv.GenerateID(CONF_MASTER_BMS_ID): cv.use_id(PaceBmsMaster),
        }).extend(cv.COMPONENT_SCHEMA),
    },lower=False, default_type=DEFAULT_BMS_TYPE)
)

def final_validate_slave_bms_schema():

    full_config = fv.full_config.get()
    print(f"final validate full_config: {full_config}")

    #def find_pace_bms_config(pace_bms_id):
    #    fixme
    
    #def is_pace_bms_platform(platforms):
    #    platform = platforms.get(CONF_PLATFORM)
    #    if(platform)

    button_platform = full_config.get(CONF_BUTTON)
    if(button_platform is not None):
        print(f"button_platform: {button_platform}")

    datetime_platform = full_config.get(CONF_DATETIME)
    if(datetime_platform is not None):
        print(f"datetime_platform: {datetime_platform}")

    number_platform = full_config.get(CONF_NUMBER)
    if(number_platform is not None):
        print(f"number_platform: {number_platform}")

    sensor_platform = full_config.get(CONF_SENSOR)
    if(sensor_platform is not None):
        print(f"sensor_platform: {sensor_platform}")

    switch_platform = full_config.get(CONF_SWITCH)
    if(switch_platform is not None):
        print(f"switch_platform: {switch_platform}")

    select_platform = full_config.get(CONF_SELECT)
    if(select_platform is not None):
        print(f"select_platform: {select_platform}")

    text_sensor_platform = full_config.get(CONF_TEXT_SENSOR)
    if(text_sensor_platform is not None):
        print(f"text_sensor_platform: {text_sensor_platform}")



# once the schema is fully constructed, run any final checks, for example on values pulled in from the yaml
FINAL_VALIDATE_SCHEMA = cv.typed_schema({
    CONF_TYPE_MASTER: cv.Schema(
        # we can enforce a lot here, but the dump_config() override will output a warning if any of the other settings are not as expected,
        # and not enforcing them here leaves the door open to weird BMSes with unusual rates even being possible to specify in the yaml at all
        #uart.final_validate_device_schema(CONF_PACE_BMS, baud_rate=9600, require_rx=True, require_tx=True, data_bits=8, parity="NONE", stop_bits=1),
        uart.final_validate_device_schema(CONF_PACE_BMS, require_rx=True, require_tx=True),
        extra=cv.ALLOW_EXTRA,
    ),

    CONF_TYPE_SLAVE: cv.Schema(
        final_validate_slave_bms_schema(),
        extra=cv.ALLOW_EXTRA,
    )
},lower=False, default_type=DEFAULT_BMS_TYPE)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_id_name(str(config[CONF_ID])))

    cg.add(var.set_address(config[CONF_ADDRESS]))

    if CONF_RESPONDING_ADDRESS in config:
        cg.add(var.set_responding_address(config[CONF_RESPONDING_ADDRESS]))

    bms_type = config[CONF_TYPE]
    cg.add(var.set_bms_type(BMS_TYPE[bms_type]))

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
        cg.add(var.set_slave_query_mode(SLAVE_QUERY_MODE[config.get(CONF_SLAVE_QUERY_MODE)]))

        cg.add(var.set_rx_buffer_size(config[CONF_RX_BUFFER_SIZE]))

    if bms_type == CONF_TYPE_SLAVE:

        parent = await cg.get_variable(config[CONF_MASTER_BMS_ID])
        cg.add(var.set_parent(parent))
