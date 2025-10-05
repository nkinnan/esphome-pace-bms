
from esphome.const import (
    CONF_ID,
    CONF_DEVICE_ID,
)
from .. import CONF_PACE_BMS_ID

pace_bms_schemas = []

# called upon intercept of processing of root pace_bms schemas to save the schema to pace_bms_schemas for later use
# note that this means that the definition of the pace_bms node in device yaml MUST come before all platforms are declared / used (see next function)
def save_pace_bms_schema(schema):
    pace_bms_schemas.append(schema)
    return schema

# called upon intercept of processing of all platforms (button, datetime, number, select, sensor, switch, text_sensor)
def inherit_device_id(schema):


    return schema
