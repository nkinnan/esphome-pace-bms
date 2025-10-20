
from esphome.const import (
    CONF_ID,
    CONF_DEVICE_ID,
)
import esphome.config_validation as cv
# circular reference
#from .. import CONF_PACE_BMS_ID
CONF_PACE_BMS_ID = "pace_bms_id" # pointer from child component platform (sensor, number, etc.) back to BMS

pace_bms_schemas = []

# called upon intercept of processing of root pace_bms schemas to save the schema to pace_bms_schemas for later use
# note that this means that the definition of the pace_bms node in device yaml MUST come before all platforms are declared / used (see next function)
def save_pace_bms_schema(schema):
    pace_bms_schemas.append(schema)
    return schema

# called upon intercept of processing of all platforms (button, datetime, number, select, sensor, switch, text_sensor)
def inherit_device_id(schema):

    platform_device_id = schema.get(CONF_DEVICE_ID)
    if(platform_device_id is None):
        # get the id of the parent pace_bms that this platform is pointing to
        pace_bms_id = schema.get(CONF_PACE_BMS_ID)
        if(pace_bms_id is None):
            #schema validation will fail later on so just abort
            return schema
        
        # find the parent pace_bms schema (previously saved to a global) by its id and then grab its device_id value
        # (if available)
        pace_bms_device_id = None
        if(pace_bms_schemas is not None):
            for pace_bms_schema in pace_bms_schemas:
                id = pace_bms_schema.get(CONF_ID)
                if(id == pace_bms_id):
                    # found it, save the device_id (if specified)
                    pace_bms_device_id = pace_bms_schema.get(CONF_DEVICE_ID)

        # if the parent pace_bms has a device_id but this platform does not, inherit the parent device_id
        if(platform_device_id is None and pace_bms_device_id is not None):
            schema[CONF_DEVICE_ID] = pace_bms_device_id
            platform_device_id = pace_bms_device_id

        # if after all that, this platform still does not have a device_id, then we can't push it onto the platform's children
        if(platform_device_id is None):
            return schema

    # give all children our device_id
    # note that this will give device_id to all platform schema dictionary values that are themselves dictionaries
    #     I'm not sure if platforms can have children (dictionaries) that are not sensors - this may cause trouble in such a case, 
    #     but I have not seen that in practice
    for index, (key, value) in enumerate(schema.items()):
        if isinstance(value, dict):
            child_device_id = value.get(CONF_DEVICE_ID)
            if(child_device_id is None):
                value[CONF_DEVICE_ID] = platform_device_id

    return schema


# buttons that must be in globals since final_validate needs to check if they are allowed
CONF_SHUTDOWN = "shutdown"

# datetimes that must be in globals since final_validate needs to check if they are allowed
CONF_SYSTEM_DATE_AND_TIME = "system_date_and_time"

# selects that must be in globals since final_validate needs to check if they are allowed
CONF_PROTOCOL_CAN           = "protocol_can"
CONF_PROTOCOL_RS485           = "protocol_rs485"
CONF_PROTOCOL_TYPE           = "protocol_type"

# sensors that must be in globals since final_validate needs to check if they are allowed
CONF_BMS_COUNT = "bms_count"
CONF_PAYLOAD_COUNT = "payload_count"

# switchs that must be in globals since final_validate needs to check if they are allowed


# text_sensors that must be in globals since final_validate needs to check if they are allowed
CONF_HARDWARE_VERSION     = "hardware_version"
CONF_SERIAL_NUMBER        = "serial_number"
