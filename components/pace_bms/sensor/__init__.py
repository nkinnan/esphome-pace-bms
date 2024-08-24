import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_POWER,
    UNIT_VOLT,
    UNIT_CELSIUS,
    UNIT_AMPERE,
    #UNIT_AMP_HOURS,   <--------- added to const.py but need to check in
    UNIT_WATT,
    UNIT_PERCENT,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_POWER,
    STATE_CLASS_MEASUREMENT,
    DEVICE_CLASS_ENERGY_STORAGE,
)
from .. import pace_bms_ns, CONF_PACE_BMS_ID, PaceBms

DEPENDENCIES = ["pace_bms"]

PaceBmsSensor = pace_bms_ns.class_("PaceBmsSensor", cg.Component)


CONF_CELL_COUNT = "cell_count"
CONF_CELL_VOLTAGE_01 = "cell_voltage_01"
CONF_CELL_VOLTAGE_02 = "cell_voltage_02"
CONF_CELL_VOLTAGE_03 = "cell_voltage_03"
CONF_CELL_VOLTAGE_04 = "cell_voltage_04"
CONF_CELL_VOLTAGE_05 = "cell_voltage_05"
CONF_CELL_VOLTAGE_06 = "cell_voltage_06"
CONF_CELL_VOLTAGE_07 = "cell_voltage_07"
CONF_CELL_VOLTAGE_08 = "cell_voltage_08"
CONF_CELL_VOLTAGE_09 = "cell_voltage_09"
CONF_CELL_VOLTAGE_10 = "cell_voltage_10"
CONF_CELL_VOLTAGE_11 = "cell_voltage_11"
CONF_CELL_VOLTAGE_12 = "cell_voltage_12"
CONF_CELL_VOLTAGE_13 = "cell_voltage_13"
CONF_CELL_VOLTAGE_14 = "cell_voltage_14"
CONF_CELL_VOLTAGE_15 = "cell_voltage_15"
CONF_CELL_VOLTAGE_16 = "cell_voltage_16"
CELL_VOLTAGES = [
    CONF_CELL_VOLTAGE_01,
    CONF_CELL_VOLTAGE_02,
    CONF_CELL_VOLTAGE_03,
    CONF_CELL_VOLTAGE_04,
    CONF_CELL_VOLTAGE_05,
    CONF_CELL_VOLTAGE_06,
    CONF_CELL_VOLTAGE_07,
    CONF_CELL_VOLTAGE_08,
    CONF_CELL_VOLTAGE_09,
    CONF_CELL_VOLTAGE_10,
    CONF_CELL_VOLTAGE_11,
    CONF_CELL_VOLTAGE_12,
    CONF_CELL_VOLTAGE_13,
    CONF_CELL_VOLTAGE_14,
    CONF_CELL_VOLTAGE_15,
    CONF_CELL_VOLTAGE_16,
]
CONF_TEMPERATURE_COUNT = "temperature_count"
CONF_TEMPERATURE_01 = "temperature_01"
CONF_TEMPERATURE_02 = "temperature_02"
CONF_TEMPERATURE_03 = "temperature_03"
CONF_TEMPERATURE_04 = "temperature_04"
CONF_TEMPERATURE_05 = "temperature_05"
CONF_TEMPERATURE_06 = "temperature_06"
TEMPERATURES = [
    CONF_TEMPERATURE_01,
    CONF_TEMPERATURE_02,
    CONF_TEMPERATURE_03,
    CONF_TEMPERATURE_04,
    CONF_TEMPERATURE_05,
    CONF_TEMPERATURE_06,
]
CONF_CURRENT = "current"
CONF_TOTAL_VOLTAGE = "total_voltage"
CONF_REMAINING_CAPACITY = "remaining_capacity"
CONF_FULL_CAPACITY = "full_capacity"
CONF_DESIGN_CAPACITY = "design_capacity"
CONF_CYCLE_COUNT = "cycle_count"
CONF_STATE_OF_CHARGE = "state_of_charge"
CONF_STATE_OF_HEALTH = "state_of_health"
#CONF_POWER = "power"
CONF_MIN_CELL_VOLTAGE = "min_cell_voltage"
CONF_MAX_CELL_VOLTAGE = "max_cell_voltage"
CONF_AVG_CELL_VOLTAGE = "avg_cell_voltage"
CONF_MAX_CELL_DIFFERENTIAL = "max_cell_differential"


CONF_WARNING_STATUS_VALUE_CELL_01           = "warning_status_value_cell_01"
CONF_WARNING_STATUS_VALUE_CELL_02           = "warning_status_value_cell_02"
CONF_WARNING_STATUS_VALUE_CELL_03           = "warning_status_value_cell_03"
CONF_WARNING_STATUS_VALUE_CELL_04           = "warning_status_value_cell_04"
CONF_WARNING_STATUS_VALUE_CELL_05           = "warning_status_value_cell_05"
CONF_WARNING_STATUS_VALUE_CELL_06           = "warning_status_value_cell_06"
CONF_WARNING_STATUS_VALUE_CELL_07           = "warning_status_value_cell_07"
CONF_WARNING_STATUS_VALUE_CELL_08           = "warning_status_value_cell_08"
CONF_WARNING_STATUS_VALUE_CELL_09           = "warning_status_value_cell_09"
CONF_WARNING_STATUS_VALUE_CELL_10           = "warning_status_value_cell_10"
CONF_WARNING_STATUS_VALUE_CELL_11           = "warning_status_value_cell_11"
CONF_WARNING_STATUS_VALUE_CELL_12           = "warning_status_value_cell_12"
CONF_WARNING_STATUS_VALUE_CELL_13           = "warning_status_value_cell_13"
CONF_WARNING_STATUS_VALUE_CELL_14           = "warning_status_value_cell_14"
CONF_WARNING_STATUS_VALUE_CELL_15           = "warning_status_value_cell_15"
CONF_WARNING_STATUS_VALUE_CELL_16           = "warning_status_value_cell_16"
CONF_WARNING_STATUS_VALUE_CELLS = [
    CONF_WARNING_STATUS_VALUE_CELL_01,
    CONF_WARNING_STATUS_VALUE_CELL_02,
    CONF_WARNING_STATUS_VALUE_CELL_03,
    CONF_WARNING_STATUS_VALUE_CELL_04,
    CONF_WARNING_STATUS_VALUE_CELL_05,
    CONF_WARNING_STATUS_VALUE_CELL_06,
    CONF_WARNING_STATUS_VALUE_CELL_07,
    CONF_WARNING_STATUS_VALUE_CELL_08,
    CONF_WARNING_STATUS_VALUE_CELL_09,
    CONF_WARNING_STATUS_VALUE_CELL_10,
    CONF_WARNING_STATUS_VALUE_CELL_11,
    CONF_WARNING_STATUS_VALUE_CELL_12,
    CONF_WARNING_STATUS_VALUE_CELL_13,
    CONF_WARNING_STATUS_VALUE_CELL_14,
    CONF_WARNING_STATUS_VALUE_CELL_15,
    CONF_WARNING_STATUS_VALUE_CELL_16,
]
CONF_WARNING_STATUS_VALUE_TEMP_01           = "warning_status_value_temperature_01"
CONF_WARNING_STATUS_VALUE_TEMP_02           = "warning_status_value_temperature_02"
CONF_WARNING_STATUS_VALUE_TEMP_03           = "warning_status_value_temperature_03"
CONF_WARNING_STATUS_VALUE_TEMP_04           = "warning_status_value_temperature_04"
CONF_WARNING_STATUS_VALUE_TEMP_05           = "warning_status_value_temperature_05"
CONF_WARNING_STATUS_VALUE_TEMP_06           = "warning_status_value_temperature_06"
CONF_WARNING_STATUS_VALUE_TEMPS = [
    CONF_WARNING_STATUS_VALUE_TEMP_01,
    CONF_WARNING_STATUS_VALUE_TEMP_02,
    CONF_WARNING_STATUS_VALUE_TEMP_03,
    CONF_WARNING_STATUS_VALUE_TEMP_04,
    CONF_WARNING_STATUS_VALUE_TEMP_05,
    CONF_WARNING_STATUS_VALUE_TEMP_06,
]
CONF_WARNING_STATUS_VALUE_CHARGE_CURRENT    = "warning_status_value_charge_current"
CONF_WARNING_STATUS_VALUE_TOTAL_VOLTAGE     = "warning_status_value_total_voltage"
CONF_WARNING_STATUS_VALUE_DISCHARGE_CURRENT = "warning_status_value_discharge_current"
CONF_WARNING_STATUS_VALUE_1                 = "warning_status_value_1"
CONF_WARNING_STATUS_VALUE_2                 = "warning_status_value_2"

CONF_BALANCING_STATUS_VALUE     = "balancing_status_value"
CONF_SYSTEM_STATUS_VALUE        = "system_status_value"
CONF_CONFIGURATION_STATUS_VALUE = "configuration_status_value"
CONF_PROTECTION_STATUS_VALUE_1  = "protection_status_value_1"
CONF_PROTECTION_STATUS_VALUE_2  = "protection_status_value_2"
CONF_FAULT_STATUS_VALUE         = "fault_status_value"




UNIT_AMP_HOURS = "Ah" # todo: use existing



CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PaceBmsSensor),
        cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBms),
        cv.Optional(CONF_CELL_COUNT): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_01): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_02): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_03): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_04): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_05): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_06): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_07): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_08): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_09): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_10): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_11): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_12): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_13): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_14): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_15): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_16): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE_COUNT): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE_01): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE_02): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE_03): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE_04): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE_05): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE_06): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TOTAL_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_REMAINING_CAPACITY): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMP_HOURS,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_ENERGY_STORAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_FULL_CAPACITY): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMP_HOURS,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_ENERGY_STORAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DESIGN_CAPACITY): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMP_HOURS,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_ENERGY_STORAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CYCLE_COUNT): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_STATE_OF_CHARGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=2,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_STATE_OF_HEALTH): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=2,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MIN_CELL_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MAX_CELL_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_AVG_CELL_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MAX_CELL_DIFFERENTIAL): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),

        
        cv.Optional(CONF_WARNING_STATUS_VALUE_CELL_01): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_CELL_02): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_CELL_03): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_CELL_04): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_CELL_05): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_CELL_06): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_CELL_07): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_CELL_08): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_CELL_09): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_CELL_10): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_CELL_11): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_CELL_12): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_CELL_13): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_CELL_14): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_CELL_15): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_CELL_16): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),

        
        cv.Optional(CONF_WARNING_STATUS_VALUE_TEMP_01): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_TEMP_02): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_TEMP_03): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_TEMP_04): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_TEMP_05): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_TEMP_06): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),


        cv.Optional(CONF_WARNING_STATUS_VALUE_CHARGE_CURRENT): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_TOTAL_VOLTAGE): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_DISCHARGE_CURRENT): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_1): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WARNING_STATUS_VALUE_2): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),


        cv.Optional(CONF_BALANCING_STATUS_VALUE): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_SYSTEM_STATUS_VALUE): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CONFIGURATION_STATUS_VALUE): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_PROTECTION_STATUS_VALUE_1): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_PROTECTION_STATUS_VALUE_2): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_FAULT_STATUS_VALUE): sensor.sensor_schema(
            #unit_of_measurement=,
            accuracy_decimals=0,
            #device_class=,
            state_class=STATE_CLASS_MEASUREMENT,
        ),

    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    paren = await cg.get_variable(config[CONF_PACE_BMS_ID])
    cg.add(var.set_parent(paren))

    if cell_count_config := config.get(CONF_CELL_COUNT):
        sens = await sensor.new_sensor(cell_count_config)
        cg.add(var.set_cell_count_sensor(sens))

    for i, key in enumerate(CELL_VOLTAGES):
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(var.set_cell_voltage_sensor(i, sens))

    if temperature_count_config := config.get(CONF_TEMPERATURE_COUNT):
        sens = await sensor.new_sensor(temperature_count_config)
        cg.add(var.set_temperature_count_sensor(sens))

    for i, key in enumerate(TEMPERATURES):
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(var.set_temperature_sensor(i, sens))

    if current_config := config.get(CONF_CURRENT):
        sens = await sensor.new_sensor(current_config)
        cg.add(var.set_current_sensor(sens))

    if voltage_config := config.get(CONF_TOTAL_VOLTAGE):
        sens = await sensor.new_sensor(voltage_config)
        cg.add(var.set_total_voltage_sensor(sens))

    if remaining_capacity := config.get(CONF_REMAINING_CAPACITY):
        sens = await sensor.new_sensor(remaining_capacity)
        cg.add(var.set_remaining_capacity_sensor(sens))

    if full_capacity := config.get(CONF_FULL_CAPACITY):
        sens = await sensor.new_sensor(full_capacity)
        cg.add(var.set_full_capacity_sensor(sens))

    if design_capacity := config.get(CONF_DESIGN_CAPACITY):
        sens = await sensor.new_sensor(design_capacity)
        cg.add(var.set_design_capacity_sensor(sens))

    if cycle_count := config.get(CONF_CYCLE_COUNT):
        sens = await sensor.new_sensor(cycle_count)
        cg.add(var.set_cycle_count_sensor(sens))

    if state_of_charge := config.get(CONF_STATE_OF_CHARGE):
        sens = await sensor.new_sensor(state_of_charge)
        cg.add(var.set_state_of_charge_sensor(sens))

    if state_of_health := config.get(CONF_STATE_OF_HEALTH):
        sens = await sensor.new_sensor(state_of_health)
        cg.add(var.set_state_of_health_sensor(sens))

    if power := config.get(CONF_POWER):
        sens = await sensor.new_sensor(power)
        cg.add(var.set_power_sensor(sens))

    if min_cell_voltage := config.get(CONF_MIN_CELL_VOLTAGE):
        sens = await sensor.new_sensor(min_cell_voltage)
        cg.add(var.set_min_cell_voltage_sensor(sens))

    if max_cell_voltage := config.get(CONF_MAX_CELL_VOLTAGE):
        sens = await sensor.new_sensor(max_cell_voltage)
        cg.add(var.set_max_cell_voltage_sensor(sens))

    if avg_cell_voltage := config.get(CONF_AVG_CELL_VOLTAGE):
        sens = await sensor.new_sensor(avg_cell_voltage)
        cg.add(var.set_avg_cell_voltage_sensor(sens))

    if max_cell_differential := config.get(CONF_MAX_CELL_DIFFERENTIAL):
        sens = await sensor.new_sensor(max_cell_differential)
        cg.add(var.set_max_cell_differential_sensor(sens))

    for i, key in enumerate(CONF_WARNING_STATUS_VALUE_CELLS):
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(var.set_warning_status_value_cells_sensor(i, sens))

    for i, key in enumerate(CONF_WARNING_STATUS_VALUE_TEMPS):
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(var.set_warning_status_value_temps_sensor(i, sens))

    if warning_status_value_charge_current := config.get(CONF_WARNING_STATUS_VALUE_CHARGE_CURRENT):
        sens = await sensor.new_sensor(warning_status_value_charge_current)
        cg.add(var.set_warning_status_value_charge_current_sensor(sens))

    if warning_status_value_total_voltage := config.get(CONF_WARNING_STATUS_VALUE_TOTAL_VOLTAGE):
        sens = await sensor.new_sensor(warning_status_value_total_voltage)
        cg.add(var.set_warning_status_value_total_voltage_sensor(sens))

    if warning_status_value_discharge_current := config.get(CONF_WARNING_STATUS_VALUE_DISCHARGE_CURRENT):
        sens = await sensor.new_sensor(warning_status_value_discharge_current)
        cg.add(var.set_warning_status_value_discharge_current_sensor(sens))

    if warning_status_value_1 := config.get(CONF_WARNING_STATUS_VALUE_1):
        sens = await sensor.new_sensor(warning_status_value_1)
        cg.add(var.set_warning_status_value_1_sensor(sens))

    if warning_status_value_2 := config.get(CONF_WARNING_STATUS_VALUE_2):
        sens = await sensor.new_sensor(warning_status_value_2)
        cg.add(var.set_warning_status_value_2_sensor(sens))

    if balancing_status_value := config.get(CONF_BALANCING_STATUS_VALUE):
        sens = await sensor.new_sensor(balancing_status_value)
        cg.add(var.set_balancing_status_value_sensor(sens))

    if system_status_value := config.get(CONF_SYSTEM_STATUS_VALUE):
        sens = await sensor.new_sensor(system_status_value)
        cg.add(var.set_system_status_value_sensor(sens))

    if configuration_status_value := config.get(CONF_CONFIGURATION_STATUS_VALUE):
        sens = await sensor.new_sensor(configuration_status_value)
        cg.add(var.set_configuration_status_value_sensor(sens))

    if protection_status_value_1 := config.get(CONF_PROTECTION_STATUS_VALUE_1):
        sens = await sensor.new_sensor(protection_status_value_1)
        cg.add(var.set_protection_status_value_1_sensor(sens))

    if protection_status_value_2 := config.get(CONF_PROTECTION_STATUS_VALUE_2):
        sens = await sensor.new_sensor(protection_status_value_2)
        cg.add(var.set_protection_status_value_2_sensor(sens))

    if fault_status_value := config.get(CONF_FAULT_STATUS_VALUE):
        sens = await sensor.new_sensor(fault_status_value)
        cg.add(var.set_fault_status_value_sensor(sens))

