import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    UNIT_VOLT,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
)
from .. import pace_bms_ns, CONF_PACE_BMS_ID, PaceBms

DEPENDENCIES = ["pace_bms"]

PaceBmsSensor = pace_bms_ns.class_("PaceBmsSensor", cg.Component)


CONF_CELL_COUNT = "cell_count"
#		uint16_t cellVoltagesMillivolts[MAX_CELL_COUNT];
#		uint8_t tempCount;
#		int16_t temperaturesTenthsCelcius[MAX_TEMP_COUNT]; // first 4 are Cell readings, then MOSFET then Environment
#		int32_t currentMilliamps; 
CONF_TOTAL_VOLTAGE = "total_voltage"
#		uint32_t remainingCapacityMilliampHours;
#		uint32_t fullCapacityMilliampHours;
#		uint16_t cycleCount;
#		uint32_t designCapacityMilliampHours;
#		float SoC; // in percent
#		float SoH; // in percent
#		float powerWatts;
#		uint16_t minCellVoltageMillivolts;
#		uint16_t maxCellVoltageMillivolts;
#		uint16_t avgCellVoltageMillivolts;
#		uint16_t maxCellDifferentialMillivolts;





CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PaceBmsSensor),
        cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBms),
        cv.Optional(CONF_CELL_COUNT): sensor.sensor_schema(
            #unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=0,
            #device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),





        cv.Optional(CONF_TOTAL_VOLTAGE): sensor.sensor_schema(
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

    if cell_count := config.get(CONF_CELL_COUNT):
        sens = await sensor.new_sensor(cell_count)
        cg.add(var.set_cell_count_sensor(sens))




    if voltage_config := config.get(CONF_TOTAL_VOLTAGE):
        sens = await sensor.new_sensor(voltage_config)
        cg.add(var.set_total_voltage_sensor(sens))

