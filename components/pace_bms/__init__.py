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
    CONF_NAME,
)
from esphome import pins
import esphome.final_validate as fv

from .pace_bms_globals import (
    save_pace_bms_schema, 
    
    # button
    CONF_SHUTDOWN, 
    
    # datetime
    CONF_SYSTEM_DATE_AND_TIME, 
    
    # select
    CONF_PROTOCOL_CAN, 
    CONF_PROTOCOL_RS485, 
    CONF_PROTOCOL_TYPE,

    # sensor
    CONF_BMS_COUNT, 
    CONF_PAYLOAD_COUNT,

    # switch

    # text_sensor
    CONF_HARDWARE_VERSION,
    CONF_SERIAL_NUMBER,
)

# todo: bizarrely these are not in esphome const.py
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
    save_pace_bms_schema, 
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

def final_validate_master_bms_schema(master_config):

    full_config = fv.full_config.get()
    master_id = master_config.get(CONF_ID)

    # ensure unique addresses across all pace_bms instances
    address_to_id_name = {}
    pace_bms_configs = full_config.get(CONF_PACE_BMS)
    for pace_bms_config in pace_bms_configs:
        id_name = str(pace_bms_config.get(CONF_ID))
        address = pace_bms_config.get(CONF_ADDRESS)
        if(address_to_id_name.get(address) is not None):
            raise cv.Invalid(f"Two pace_bms instances cannot have the same address ({address}). The two ids are '{id_name}' and '{address_to_id_name[address]}'.")
        address_to_id_name[address] = id_name

    # the two counts are only available for v25 master
    sensor_platforms = full_config.get(CONF_SENSOR)
    if(sensor_platforms is not None):
        for sensor_platform in sensor_platforms:
            platform = sensor_platform.get(CONF_PLATFORM)
            parent_bms_id = sensor_platform.get(CONF_PACE_BMS_ID)
            protocol_commandset = master_config.get(CONF_PROTOCOL_COMMANDSET)
            slave_query_mode = master_config.get(CONF_SLAVE_QUERY_MODE)
            if(platform == CONF_PACE_BMS and parent_bms_id == master_id and protocol_commandset != 0x25):
                if CONF_BMS_COUNT in sensor_platform:
                    raise cv.Invalid(f"The '{CONF_BMS_COUNT}' sensor is not available for a BMS with type=MASTER unless protocol_commandset=0x25.")
                if CONF_PAYLOAD_COUNT in sensor_platform:
                    raise cv.Invalid(f"The '{CONF_PAYLOAD_COUNT}' sensor is not available for a BMS with type=MASTER unless protocol_commandset=0x25.")
            # and furthermore the payload count is only available in broadcast mode
            if(platform == CONF_PACE_BMS and parent_bms_id == master_id and slave_query_mode != "BROADCAST"):
                if CONF_PAYLOAD_COUNT in sensor_platform:
                    raise cv.Invalid(f"The '{CONF_PAYLOAD_COUNT}' sensor is not available unless slave_query_mode is BROADCAST.")

    return master_config

def final_validate_slave_bms_schema(slave_config):

    full_config = fv.full_config.get()
    slave_id = slave_config.get(CONF_ID)
    master_id = slave_config.get(CONF_MASTER_BMS_ID)
    master_pace_bms_schema = None

    if(master_id is not None): # if it's none the config will fail validation anyway
        # find the parent pace_bms schema 
        pace_bms_schemas = full_config.get(CONF_PACE_BMS)
        for pace_bms_schema_test in pace_bms_schemas: 
            id = pace_bms_schema_test.get(CONF_ID)
            # if not found the config will fail validation anyway
            if(id == master_id):
                # save for later
                master_pace_bms_schema = pace_bms_schema_test
                protocol_commandset = master_pace_bms_schema.get(CONF_PROTOCOL_COMMANDSET)
                if(protocol_commandset != 0x25):
                    raise cv.Invalid(f"BMS with type=SLAVE is only allowed for protocol commandset 0x25.")

    # ensure slave has an address > the master's address
    if(master_pace_bms_schema is not None): # if none then the config will fail validation anyway
        master_address = master_pace_bms_schema.get(CONF_ADDRESS)
        slave_address = slave_config.get(CONF_ADDRESS)
        if(slave_address <= master_address):
            raise cv.Invalid(f"BMS with type=SLAVE must have an address greater than its parent MASTER BMS address.")

    # check for invalid button components when BMS type is slave
    button_platforms = full_config.get(CONF_BUTTON)
    if(button_platforms is not None):
        for button_platform in button_platforms:
            platform = button_platform.get(CONF_PLATFORM)
            parent_bms_id = button_platform.get(CONF_PACE_BMS_ID)
            if(platform == CONF_PACE_BMS and parent_bms_id == slave_id):
                # we now know that this is the platform for the slave we are validating, now check for invalid components when BMS type is slave
                if CONF_SHUTDOWN in button_platform:
                    raise cv.Invalid(f"The '{CONF_SHUTDOWN}' button is not available for a BMS with type=SLAVE.")

    # check for invalid datetime components when BMS type is slave
    datetime_platforms = full_config.get(CONF_DATETIME)
    if(datetime_platforms is not None):
        for datetime_platform in datetime_platforms:
            platform = datetime_platform.get(CONF_PLATFORM)
            parent_bms_id = datetime_platform.get(CONF_PACE_BMS_ID)
            if(platform == CONF_PACE_BMS and parent_bms_id == slave_id):
                # we now know that this is the platform for the slave we are validating, now check for invalid components when BMS type is slave
                if CONF_SYSTEM_DATE_AND_TIME in datetime_platform:
                    raise cv.Invalid(f"The '{CONF_SYSTEM_DATE_AND_TIME}' datetime is not available for a BMS with type=SLAVE.")

    # check for invalid number components when BMS type is slave
    number_platforms = full_config.get(CONF_NUMBER)
    if(number_platforms is not None):
        for number_platform in number_platforms:
            platform = number_platform.get(CONF_PLATFORM)
            parent_bms_id = number_platform.get(CONF_PACE_BMS_ID)
            if(platform == CONF_PACE_BMS and parent_bms_id == slave_id):
                # we now know that this is the platform for the slave we are validating, now check for invalid components when BMS type is slave
                # don't want to check each individual number here, so just check for any child components at all
                for index, (key, value) in enumerate(number_platform.items()):
                    if isinstance(value, dict):
                        name = value.get(CONF_NAME)
                        if(name is not None):
                            raise cv.Invalid(f"The '{name}' number is not available for a BMS with type=SLAVE. No numbers components are valid for SLAVE BMSes.")
                        else:
                            raise cv.Invalid(f"The '<unnamed>' number is not available for a BMS with type=SLAVE. No numbers components are valid for SLAVE BMSes.")

    # check for invalid select components when BMS type is slave
    select_platforms = full_config.get(CONF_SELECT)
    if(select_platforms is not None):
        for select_platform in select_platforms:
            platform = select_platform.get(CONF_PLATFORM)
            parent_bms_id = select_platform.get(CONF_PACE_BMS_ID)
            if(platform == CONF_PACE_BMS and parent_bms_id == slave_id):
                # we now know that this is the platform for the slave we are validating, now check for invalid components when BMS type is slave
                if CONF_PROTOCOL_CAN in select_platform:
                    raise cv.Invalid(f"The '{CONF_PROTOCOL_CAN}' select is not available for a BMS with type=SLAVE.")
                if CONF_PROTOCOL_RS485 in select_platform:
                    raise cv.Invalid(f"The '{CONF_PROTOCOL_RS485}' select is not available for a BMS with type=SLAVE.")
                if CONF_PROTOCOL_TYPE in select_platform:
                    raise cv.Invalid(f"The '{CONF_PROTOCOL_TYPE}' select is not available for a BMS with type=SLAVE.")

    # check for invalid sensor components when BMS type is slave
    sensor_platforms = full_config.get(CONF_SENSOR)
    if(sensor_platforms is not None):
        for sensor_platform in sensor_platforms:
            platform = sensor_platform.get(CONF_PLATFORM)
            parent_bms_id = sensor_platform.get(CONF_PACE_BMS_ID)
            if(platform == CONF_PACE_BMS and parent_bms_id == slave_id):
                # we now know that this is the platform for the slave we are validating, now check for invalid components when BMS type is slave
                if CONF_BMS_COUNT in sensor_platform:
                    raise cv.Invalid(f"The '{CONF_BMS_COUNT}' sensor is not available for a BMS with type=SLAVE.")
                if CONF_PAYLOAD_COUNT in sensor_platform:
                    raise cv.Invalid(f"The '{CONF_PAYLOAD_COUNT}' sensor is not available for a BMS with type=SLAVE.")

    # check for invalid switch components when BMS type is slave
    # the switches just go read-only for slaves, nothing to exclude
    #switch_platforms = full_config.get(CONF_SWITCH)
    #if(switch_platforms is not None):

    # check for invalid text_sensor components when BMS type is slave
    text_sensor_platforms = full_config.get(CONF_TEXT_SENSOR)
    if(text_sensor_platforms is not None):
        for text_sensor_platform in text_sensor_platforms:
            platform = text_sensor_platform.get(CONF_PLATFORM)
            parent_bms_id = text_sensor_platform.get(CONF_PACE_BMS_ID)
            if(platform == CONF_PACE_BMS and parent_bms_id == slave_id):
                # we now know that this is the platform for the slave we are validating, now check for invalid components when BMS type is slave
                if CONF_HARDWARE_VERSION in text_sensor_platform:
                    raise cv.Invalid(f"The '{CONF_HARDWARE_VERSION}' text_sensor is not available for a BMS with type=SLAVE.")
                if CONF_SERIAL_NUMBER in text_sensor_platform:
                    raise cv.Invalid(f"The '{CONF_SERIAL_NUMBER}' text_sensor is not available for a BMS with type=SLAVE.")

    return slave_config


# once the schema is fully constructed, run any final checks, for example on values pulled in from the yaml
FINAL_VALIDATE_SCHEMA = cv.typed_schema({
    CONF_TYPE_MASTER: cv.All(
        cv.Schema(
        # we can enforce a lot here, but the dump_config() override will output a warning if any of the other settings are not as expected,
        # and not enforcing them here leaves the door open to weird BMSes with unusual rates/etc even being possible to specify in the yaml at all
        #uart.final_validate_device_schema(CONF_PACE_BMS, baud_rate=9600, require_rx=True, require_tx=True, data_bits=8, parity="NONE", stop_bits=1),
        uart.final_validate_device_schema(CONF_PACE_BMS, require_rx=True, require_tx=True),
        extra=cv.ALLOW_EXTRA,
        ),
        cv.Schema(
            final_validate_master_bms_schema,
            extra=cv.ALLOW_EXTRA,
        ),
    ),
    CONF_TYPE_SLAVE: cv.Schema(
        final_validate_slave_bms_schema,
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
