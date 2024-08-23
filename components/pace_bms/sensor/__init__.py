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

#		uint16_t minCellVoltageMillivolts;
#		uint16_t maxCellVoltageMillivolts;
#		uint16_t avgCellVoltageMillivolts;
#		uint16_t maxCellDifferentialMillivolts;



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

