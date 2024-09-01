import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import (
    CONF_ID,
)
from .. import pace_bms_ns, CONF_PACE_BMS_ID, PaceBms

DEPENDENCIES = ["pace_bms"]

PaceBmsSelect = pace_bms_ns.class_("PaceBmsSelect", cg.Component)
PaceBmsSelectImplementation = pace_bms_ns.class_("PaceBmsSelectImplementation", cg.Component, select.Select)


CONF_CHARGE_CURRENT_LIMITER_GEAR           = "charge_current_limiter_gear"
CONF_CHARGE_CURRENT_LIMITER_GEAR_OPTIONS   = {
    "LowGear":  0x08, # SC_SetChargeCurrentLimiterCurrentLimitHighGear
    "HighGear": 0x09, # SC_SetChargeCurrentLimiterCurrentLimitLowGear
}

CONF_PROTOCOL_CAN           = "protocol_can"
protocol_can_options_config = {
	"":                                                                                          0xFF, # 255d <blank entry> I believe this means "turned off"
	"PACE":                                                                                      0x00, # 00d  PACE
	"Pylon / DeYe / CHNT Power / LiVolTek / Megarevo / SunSynk / SunGrow / Sol-Ark / SolarEdge": 0x01, # 01d  Pylon / DeYe / CHNT Power / LiVolTek / Megarevo / SunSynk / SunGrow / Sol-Ark / SolarEdge
	"Growatt / Sacolar":                                                                         0x02, # 02d  Growatt / Sacolar
	"Victron":                                                                                   0x03, # 03d  Victron
	"Schneider / SE / SMA":                                                                      0x04, # 04d  Schneider / SE / SMA
	"LuxPower":                                                                                  0x05, # 05d  LuxPower
	"SoroTec / SRD":                                                                             0x06, # 06d  SoroTec (SRD)
	"SMA / Studer":                                                                              0x07, # 07d  SMA / Studer
	"GoodWe":                                                                                    0x08, # 08d  GoodWe
	"Studer":                                                                                    0x09, # 09d  Studer
	"Sofar":                                                                                     0x0A, # 10d  Sofar
	"Must / PV":                                                                                 0x0B, # 11d  Must / PV
	"Solis / Jinlang":                                                                           0x0C, # 12d  Solis / Jinlang
	"DIDU":                                                                                      0x0D, # 13d  DIDU
	"Senergy":                                                                                   0x0E, # 14d  Senergy
	"TBB":                                                                                       0x0F, # 15d  TBB
	"Pylon_V202":                                                                                0x10, # 16d  Pylon_V202
	"Growatt_V109":                                                                              0x11, # 17d  Growatt_V109
	"Must_V202":                                                                                 0x12, # 18d  Must_V202
	"Afore":                                                                                     0x13, # 19d  Afore
	"INVT / YWT":                                                                                0x14, # 20d  INVT / YWT
	"FUJI":                                                                                      0x15, # 21d  FUJI
	"Sofar V21003":                                                                              0x16, # 22d  Sofar_V21003
}

CONF_PROTOCOL_RS485           = "protocol_rs485"
protocol_rs485_options_config = {
	"":                                     0xFF, # 255d <blank entry> I believe this means "turned off"
	"Pace Modbus":                          0x00, # 00d  Pace Modbus
	"Pylon / DeYe / Bentterson":            0x01, # 01d  Pylon / DeYe / Bentterson
	"Growatt":                              0x02, # 02d  Growatt
	"Voltronic / EA Sun Power / MPP Solar": 0x03, # 03d  Voltronic / EA Sun Power / MPP Solar
	"Schneider / SE":                       0x04, # 04d  Schneider / SE
	"PHOCOS":                               0x05, # 05d  PHOCOS
	"LuxPower":                             0x06, # 06d  LuxPower
	"Solar":                                0x07, # 07d  Solar
	"Lithium":                              0x08, # 08d  Lithium
	"EP":                                   0x09, # 09d  EP
	"RTU04":                                0x0A, # 10d  RTU04
	"LuxPower_V01":                         0x0B, # 11d  LuxPower_V01
	"LuxPower_V03":                         0x0C, # 12d  LuxPower_V03
	"SRNE / WOW":                           0x0D, # 13d  SRNE / WOW
	"LEOCH":                                0x0E, # 14d  LEOCH
	"Pylon_F":                              0x0F, # 15d  Pylon_F
	"Afore":                                0x10, # 16d  Afore
	"UPS_AGXN":                             0x11, # 17d  UPS_AGXN
	"Orex_Sunpolo":                         0x12, # 18d  Orex_Sunpolo
	"XIONGTAO":                             0x13, # 19d  XIONGTAO
	"RONGKE":                               0x14, # 20d  RONGKE
	"XINRUI":                               0x15, # 21d  XINRUI
	"ELTEK":                                0x16, # 22d  ELTEK
	"GT":                                   0x17, # 23d  GT
	"Leoch_V106":                           0x18, # 24d  Leoch_V106
}

CONF_PROTOCOL_TYPE           = "protocol_type"
protocol_type_options_config = {
	""      : 0xFF, # 255d <blank entry>
	"Auto"  : 0x00, # 00d Auto
	"Manual": 0x01, # 01d Manual
}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(PaceBmsSelect),
        cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBms),

        cv.Optional(CONF_CHARGE_CURRENT_LIMITER_GEAR): select.select_schema(PaceBmsSelectImplementation),

        cv.Optional(CONF_PROTOCOL_CAN): select.select_schema(PaceBmsSelectImplementation),
        cv.Optional(CONF_PROTOCOL_RS485): select.select_schema(PaceBmsSelectImplementation),
        cv.Optional(CONF_PROTOCOL_TYPE): select.select_schema(PaceBmsSelectImplementation),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    paren = await cg.get_variable(config[CONF_PACE_BMS_ID])
    cg.add(var.set_parent(paren))

    if charge_current_limiter_gear_config := config.get(CONF_CHARGE_CURRENT_LIMITER_GEAR):
        charge_current_limiter_gear_options_config = config.get(CONF_CHARGE_CURRENT_LIMITER_GEAR_OPTIONS)
        sel = await select.new_select(
            charge_current_limiter_gear_config,
            options=list(charge_current_limiter_gear_options_config.keys()),
        )
        cg.add(var.set_charge_current_limiter_gear_select(sel))
        cg.add(sel.set_protocol_values(charge_current_limiter_gear_options_config.values())

    if protocol_can_config := config.get(CONF_PROTOCOL_CAN):
        sel = await select.new_select(
            protocol_can_config,
            options=list(protocol_can_options_config.keys()),
        )
        cg.add(var.set_protocol_can_select(sel))
        cg.add(sel.set_protocol_values(list(protocol_can_options_config.values())))
    
    if protocol_rs485_config := config.get(CONF_PROTOCOL_RS485):
        sel = await select.new_select(
            protocol_rs485_config,
            options=list(protocol_rs485_options_config.keys()),
        )
        cg.add(var.set_protocol_rs485_select(sel))
        cg.add(sel.set_protocol_values(protocol_rs485_options_config.values()))

    if protocol_type_config := config.get(CONF_PROTOCOL_TYPE):
        sel = await select.new_select(
            protocol_type_config,
            options=list(protocol_type_options_config.keys()),
        )
        cg.add(var.set_protocol_type_select(sel))
        cg.add(sel.set_protocol_values(protocol_type_options_config.values()))
