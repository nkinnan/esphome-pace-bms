#pragma once

#include "pace_bms_protocol_base.h"

class PaceBmsProtocolV25 : public PaceBmsProtocolBase
{
public:
	enum CID1 : uint8_t
	{
		CID1_LithiumIron = 0x46,
		CID1_LithiumIon = 0x4F,  // not used by PBmsTools 2.4, but reported by someone using a rebadged version of it on a 14s 48v pack which also exposes protocol version 0x25
	};

	// dependency injection
	typedef void (*LogFuncPtr)(std::string message);

	// takes pointers to the "real" logging functions
	PaceBmsProtocolV25(
		OPTIONAL_NS::optional<std::string> protocol_variant, OPTIONAL_NS::optional<uint8_t> protocol_version_override, OPTIONAL_NS::optional<uint8_t> batteryChemistry,
		LogFuncPtr logError, LogFuncPtr logWarning, LogFuncPtr logInfo, LogFuncPtr logDebug, LogFuncPtr logVerbose, LogFuncPtr logVeryVerbose);

protected:
	enum CID2 : uint8_t
	{
		// Main "Realtime Monitoring" tab of PBmsTools 2.4
		// These are the commands sent in a loop to fill out the display
		CID2_ReadAnalogInformation                                = 0x42,
		CID2_ReadStatusInformation                                = 0x44,
		CID2_ReadHardwareVersion                                  = 0xC1,
		CID2_ReadSerialNumber                                     = 0xC2,

		// Main "Realtime Monitoring" tab of PBmsTools 2.4
		// These are in the "Switch Control" section
		CID2_WriteSwitchCommand                                   = 0x99, // depending on payload, encompases "Sound Alarm", "LED Alarm", "Charge Limiter", and "Charge Limiter Gear" (which is actually on the "System Configuration" page but logically is grouped with these and uses the same CID2)
		CID2_WriteChargeMosfetSwitchCommand                       = 0x9A,
		CID2_WriteDischargeMosfetSwitchCommand                    = 0x9B,
		CID2_WriteShutdownCommand                                 = 0x9C,

		// "Memory Information" tab of PBmsTools 2.4
		CID2_ReadDateTime                                         = 0xB1,
		CID2_WriteDateTime                                        = 0xB2,

		// "Parameter Setting" tab of PBmsTools 2.4
		CID2_ReadCellOverVoltageConfiguration                     = 0xD1,
		CID2_WriteCellOverVoltageConfiguration                    = 0xD0,
		CID2_ReadPackOverVoltageConfiguration                     = 0xD5,
		CID2_WritePackOverVoltageConfiguration                    = 0xD4,
		CID2_ReadCellUnderVoltageConfiguration                    = 0xD3,
		CID2_WriteCellUnderVoltageConfiguration                   = 0xD2,
		CID2_ReadPackUnderVoltageConfiguration                    = 0xD7,
		CID2_WritePackUnderVoltageConfiguration                   = 0xD6,
		CID2_ReadChargeOverCurrentConfiguration                   = 0xD9,
		CID2_WriteChargeOverCurrentConfiguration                  = 0xD8,
		CID2_ReadDischargeSlowOverCurrentConfiguration            = 0xDB,
		CID2_WriteDischargeSlowOverCurrentConfiguration           = 0xDA,
		CID2_ReadDischargeFastOverCurrentConfiguration            = 0xE3,
		CID2_WriteDischargeFastOverCurrentConfiguration           = 0xE2,
		CID2_ReadShortCircuitProtectionConfiguration              = 0xE5,
		CID2_WriteShortCircuitProtectionConfiguration             = 0xE4,
		CID2_ReadCellBalancingConfiguration                       = 0xB6,
		CID2_WriteCellBalancingConfiguration                      = 0xB5,
		CID2_ReadSleepConfiguration                               = 0xA0,
		CID2_WriteSleepConfiguration                              = 0xA8,
		CID2_ReadFullChargeLowChargeConfiguration                 = 0xAF,
		CID2_WriteFullChargeLowChargeConfiguration                = 0xAE,
		CID2_ReadChargeAndDischargeOverTemperatureConfiguration   = 0xDD,
		CID2_WriteChargeAndDischargeOverTemperatureConfiguration  = 0xDC,
		CID2_ReadChargeAndDischargeUnderTemperatureConfiguration  = 0xDF,
		CID2_WriteChargeAndDischargeUnderTemperatureConfiguration = 0xDE,
		CID2_ReadMosfetOverTemperatureConfiguration               = 0xE1,
		CID2_WriteMosfetOverTemperatureConfiguration              = 0xE0,
		CID2_ReadEnvironmentOverUnderTemperatureConfiguration     = 0xE7,
		CID2_WriteEnvironmentOverUnderTemperatureConfiguration    = 0xE6,

		// "System Configuration" tab of PBmsTools 2.4
		CID2_ReadChargeCurrentLimiterStartCurrent                 = 0xED,
		CID2_WriteChargeCurrentLimiterStartCurrent                = 0xEE,
		CID2_ReadRemainingCapacity                                = 0xA6,

		// I found this in one version of PbmsTools but it seems to be manufacturer specific and I didn't implement it since it 
		// would be confusing alongside the "standard" read/write protocols implementation
		//CID2_WriteOlderVersionOfSetCommunicationProtocol          = 0x99,

		CID2_ReadCommunicationsProtocols                          = 0xEB,
		CID2_WriteCommunicationsProtocols                         = 0xEC,
	};

public:

// ============================================================================
// 
// Main "Realtime Monitoring" tab of PBmsTools 2.4
// These are the commands sent in a loop to fill out the display
// 
// ============================================================================

	// ==== Read Analog Information
	// 0 Responding Bus Id
	// 1 Cell Count (this example has 16 cells)
	// 2 Cell Voltage (repeated Cell Count times) - stored as v * 1000, so 56 is 56000
	// 3 Temperature Count (this example has 6 temperatures)
	// 4 Temperature (repeated Temperature Count times) - stored as (value * 10) + 2730, to decode (value - 2730) / 10.0 = value
	// 5 Current - stored as value * 100
	// 6 Total Voltage - stored as value * 1000
	// 7 Remaining Capacity - stored as value * 100
	// 8 [Constant] = 03
	// 9 Full Capacity - stored as value * 100
	// 0 Cycle Count
	// 1 Design Capacity - stored as value * 100
	// req:   ~25014642E00201FD30.
	// resp:  ~25014600F07A0001100CC70CC80CC70CC70CC70CC50CC60CC70CC70CC60CC70CC60CC60CC70CC60CC7060B9B0B990B990B990BB30BBCFF1FCCCD12D303286A008C2710E1E4.
	//                     00001122222222222222222222222222222222222222222222222222222222222222223344444444444444444444444455556666777788999900001111

	static const uint8_t exampleReadAnalogInformationRequestV25[];
	static const uint8_t exampleReadAnalogInformationResponseV25[];

	static const uint8_t MAX_CELL_COUNT = 16;
	static const uint8_t MAX_TEMP_COUNT = 6;
	struct AnalogInformation
	{
		uint8_t  cellCount{ 0 };
		uint16_t cellVoltagesMillivolts[MAX_CELL_COUNT]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		uint8_t  temperatureCount{ 0 };
		int16_t  temperaturesTenthsCelcius[MAX_TEMP_COUNT]{ 0, 0, 0, 0, 0, 0 }; // first 4 are Cell readings, then MOSFET then Environment
		int32_t  currentMilliamps{ 0 };
		uint16_t totalVoltageMillivolts{ 0 };
		uint32_t remainingCapacityMilliampHours{ 0 };
		uint32_t fullCapacityMilliampHours{ 0 };
		uint16_t cycleCount{ 0 };
		uint32_t designCapacityMilliampHours{ 0 };
		// calculated
		float    SoC{ 0 }; // in percent
		float    SoH{ 0 }; // in percent
		float    powerWatts{ 0 };
		uint16_t minCellVoltageMillivolts{ 0 };
		uint16_t maxCellVoltageMillivolts{ 0 };
		uint16_t avgCellVoltageMillivolts{ 0 };
		uint16_t maxCellDifferentialMillivolts{ 0 };
	};

	bool CreateReadAnalogInformationRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessReadAnalogInformationResponse(const uint8_t busId, const std::vector<uint8_t>& response, AnalogInformation& analogInformation);

	// ==== Read Status Information
	// 0 Responding Bus Id
	// 1 Cell Count (this example has 16 cells)
	// 2 Cell Warning (repeated Cell Count times) see: DecodeWarningValue / enum WarningValue
	// 3 Temperature Count (this example has 6 temperatures)
	// 4 Temperature Warning (repeated Temperature Count times) see: DecodeWarningValue / enum WarningValue
	// 5 Charge Current Warning see: DecodeWarningValue / enum WarningValue
	// 6 Total Voltage Warning see: DecodeWarningValue / enum WarningValue
	// 7 Discharge Current Warning see: DecodeWarningValue / enum WarningValue
	// 8 Protection Status 1 see: DecodeProtectionStatus1Value / enum ProtectionStatus1Flags
	// 9 Protection Status 2 see: DecodeProtectionStatus2Value / enum ProtectionStatus2Flags
	// 0 System Status see: DecodeSystemStatusValue
	// 1 Configuration Status see: DecodeConfigurationStatusValue
	// 2 Fault Status see: DecodeFaultStatusValue
	// 3 Balance Status (high byte) set bits indicate those cells are balancing
	// 4 Balance Status (low byte) set bits indicate those cells are balancing
	// 5 Warning Status 1 see: DecodeWarningStatus1Value
	// 6 Warning Status 2 see: DecodeWarningStatus2Value
	// req:   ~25014644E00201FD2E.
	// resp:  ~25014600004C000110000000000000000000000000000000000600000000000000000000000E000000000000EF3A.
	//                     0000112222222222222222222222222222222233444444444444556677889900112233445566

	static const uint8_t exampleReadStatusInformationRequestV25[];
	static const uint8_t exampleReadStatusInformationResponseV25[];

	// possible values in:
	//     StatusInformation.warning_value_cell[index]
	//     StatusInformation.warning_value_temp[index]
	//     StatusInformation.warning_value_charge_current
	//     StatusInformation.warning_value_total_voltage
	//     StatusInformation.warning_value_discharge_current
	enum StatusInformation_WarningValues
	{
		WV_BelowLowerLimitValue = 1,
		WV_AboveUpperLimitValue = 2,
		WV_UserDefinedFaultRangeStartValue = 0x80,
		WV_UserDefinedFaultRangeEndValue = 0xEF,
		WV_OtherFaultValue = 0xF0,
	};

	// possible flags set in:
	//     StatusInformation.protection_value1
	enum StatusInformation_Protection1Flags
	{
		P1F_UndefinedProtect1Bit = (1 << 7),
		P1F_ShortCircuitProtect1Bit = (1 << 6),
		P1F_DischargeCurrentProtect1Bit = (1 << 5),
		P1F_ChargeCurrentProtect1Bit = (1 << 4),
		P1F_LowTotalVoltageProtect1Bit = (1 << 3),
		P1F_HighTotalVoltageProtect1Bit = (1 << 2),
		P1F_LowCellVoltageProtect1Bit = (1 << 1),
		P1F_HighCellVoltageProtect1Bit = (1 << 0),
	};

	// possible flags set in:
	//     StatusInformation.protection_value2
	enum StatusInformation_Protection2Flags
	{
		P2F_FullyProtect2Bit = (1 << 7),
		P2F_LowEnvironmentalTemperatureProtect2Bit = (1 << 6),
		P2F_HighEnvironmentalTemperatureProtect2Bit = (1 << 5),
		P2F_HighMosfetTemperatureProtect2Bit = (1 << 4),
		P2F_LowDischargeTemperatureProtect2Bit = (1 << 3),
		P2F_LowChargeTemperatureProtect2Bit = (1 << 2),
		P2F_HighDischargeTemperatureProtect2Bit = (1 << 1),
		P2F_HighChargeTemperatureProtect2Bit = (1 << 0),
	};

	// possible flags set in:
	//     StatusInformation.system_value
	enum StatusInformation_SystemFlags
	{
		SF_HeartIndicatorBit = (1 << 7),
		SF_UndefinedStatusBit7 = (1 << 6),
		SF_ChargingBit = (1 << 5),
		SF_PositiveNegativeTerminalsReversedBit = (1 << 4),
		SF_DischargingBit = (1 << 3),
		SF_DischargeMosfetOnBit = (1 << 2),
		SF_ChargeMosfetOnBit = (1 << 1),
		SF_ChargeCurrentLimiterTurnedOffBit = (1 << 0), // this is the inverse of CF_ChargeCurrentLimiterEnabledBit
	};

	// possible flags set in:
	//     StatusInformation.configuration_value
	enum StatusInformation_ConfigurationFlags
	{
		CF_UndefinedConfigurationStatusBit8 = (1 << 7),
		CF_UndefinedConfigurationStatusBit7 = (1 << 6),
		CF_LedAlarmEnabledBit = (1 << 5),
		CF_ChargeCurrentLimiterEnabledBit = (1 << 4),
		CF_ChargeCurrentLimiterLowGearSetBit = (1 << 3),
		CF_DischargeMosfetTurnedOff = (1 << 2), // it is not documented, but in practice I have seen this flag being set to mean "Discharge MOSFET turned OFF" in addition to the SF_DischargeMosfetOnBit flag being cleared
		CF_ChargeMosfetTurnedOff = (1 << 1), // it is not documented, but in practice I have seen this flag being set to mean "Charge MOSFET turned OFF" in addition to the SF_ChargeMosfetOnBit flag being cleared
		CF_BuzzerAlarmEnabledBit = (1 << 0),
	};

	// possible flags set in:
	//     StatusInformation.fault_value
	enum StatusInformation_FaultFlags
	{
		FF_UndefinedFaultStatusBit8 = (1 << 7),
		FF_UndefinedFaultStatusBit7 = (1 << 6),
		FF_SampleBit = (1 << 5),
		FF_CellBit = (1 << 4),
		FF_UndefinedFaultStatusBit4 = (1 << 3),
		FF_NTCBit = (1 << 2),
		FF_DischargeMosfetBit = (1 << 1),
		FF_ChargeMosfetBit = (1 << 0),
	};

	// possible flags set in:
	//     StatusInformation.warning_value1
	enum StatusInformation_Warning1Flags
	{
		W1F_UndefinedWarning1Bit8 = (1 << 7),
		W1F_UndefinedWarning1Bit7 = (1 << 6),
		W1F_DischargeCurrentBit = (1 << 5),
		W1F_ChargeCurrentBit = (1 << 4),
		W1F_LowTotalVoltageBit = (1 << 3),
		W1F_HighTotalVoltageBit = (1 << 2),
		W1F_LowCellVoltageBit = (1 << 1),
		W1F_HighCellVoltageBit = (1 << 0),
	};

	// possible flags set in:
	//     StatusInformation.warning_value2
	enum StatusInformation_Warning2Flags
	{
		W2F_LowPower = (1 << 7),
		W2F_HighMosfetTemperature = (1 << 6),
		W2F_LowEnvironmentalTemperature = (1 << 5),
		W2F_HighEnvironmentalTemperature = (1 << 4),
		W2F_LowDischargeTemperature = (1 << 3),
		W2F_LowChargeTemperature = (1 << 2),
		W2F_HighDischargeTemperature = (1 << 1),
		W2F_HighChargeTemperature = (1 << 0),
	};

	struct StatusInformation
	{
		std::string warningText{ "" };
		uint8_t     warning_value_cell[MAX_CELL_COUNT]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // DecodeWarningValue / enum StatusInformation_WarningValues
		uint8_t     warning_value_temp[MAX_TEMP_COUNT]{ 0, 0, 0, 0, 0, 0 }; // DecodeWarningValue / enum StatusInformation_WarningValues
		uint8_t     warning_value_charge_current{ 0 };       // DecodeWarningValue / enum StatusInformation_WarningValues
		uint8_t     warning_value_total_voltage{ 0 };        // DecodeWarningValue / enum StatusInformation_WarningValues
		uint8_t     warning_value_discharge_current{ 0 };    // DecodeWarningValue / enum StatusInformation_WarningValues
		uint8_t     warning_value1{ 0 };                     // DecodeWarningStatus1Value / enum StatusInformation_Warning1Flags
		uint8_t     warning_value2{ 0 };                     // DecodeWarningStatus2Value / enum StatusInformation_Warning2Flags
		std::string balancingText{ "" };
		uint16_t    balancing_value{ 0 };                    // one bit per cell, lowest bit = cell 1
		std::string systemText{ "" };
		uint8_t     system_value{ 0 };                       // DecodeStatusValue / enum StatusInformation_SystemFlags
		std::string configurationText{ "" };
		uint8_t     configuration_value{ 0 };                // DecodeConfigurationStatusValue / enum StatusInformation_ConfigurationFlags
		std::string protectionText{ "" };
		uint8_t     protection_value1{ 0 };                  // DecodeProtectionStatus1Value / enum StatusInformation_Protection1Flags
		uint8_t     protection_value2{ 0 };                  // DecodeProtectionStatus2Value / enum StatusInformation_Protection2Flags
		std::string faultText{ "" };
		uint8_t     fault_value{ 0 };                        // DecodeFaultStatusValue / enum StatusInformation_FaultFlags
	};

	bool CreateReadStatusInformationRequest(const uint8_t busId, std::vector<uint8_t>& request);

protected:
	// helper for: ProcessStatusInformationResponse
	const std::string DecodeWarningValue(const uint8_t val);

	// helper for: ProcessStatusInformationResponse
	const std::string DecodeProtectionStatus1Value(const uint8_t val);

	// helper for: ProcessStatusInformationResponse
	const std::string DecodeProtectionStatus2Value(const uint8_t val);

	// helper for: ProcessStatusInformationResponse
	const std::string DecodeStatusValue(const uint8_t val);

	// helper for: ProcessStatusInformationResponse
	const std::string DecodeConfigurationStatusValue(const uint8_t val);

	// helper for: ProcessStatusInformationResponse
	const std::string DecodeFaultStatusValue(const uint8_t val);

	// helper for: ProcessStatusInformationResponse
	const std::string DecodeWarningStatus1Value(const uint8_t val);

	// helper for: ProcessStatusInformationResponse
	const std::string DecodeWarningStatus2Value(const uint8_t val);

public:
	bool ProcessReadStatusInformationResponse(const uint8_t busId, const std::vector<uint8_t>& response, StatusInformation& statusInformation);

	// ==== Read Hardware Version
	// 1 Hardware Version string (may be ' ' padded at the end), the length header value will tell you how long it is, should be 20 'actual character' bytes (40 ASCII hex chars)
	// req:   ~250146C10000FD9A.
	// resp:  ~25014600602850313653313030412D313831322D312E30302000F58E.
	//                     1111111111111111111111111111111111111111

	static const uint8_t exampleReadHardwareVersionRequestV25[];
	static const uint8_t exampleReadHardwareVersionResponseV25[];

	bool CreateReadHardwareVersionRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessReadHardwareVersionResponse(const uint8_t busId, const std::vector<uint8_t>& response, std::string& hardwareVersion);

	// ==== Read Serial Number
	// 1 Serial Number string (may be ' ' padded at the end), the length header value will tell you how long it is, should be 20 or 40 'actual character' bytes (40 or 80 ASCII hex chars)
	// req:   ~250146C20000FD99.
	// resp:  ~25014600B05031383132313031333830333039442020202020202020202020202020202020202020202020202020EE0F.
	//                     11111111111111111111111111111111111111111111111111111111111111111111111111111111

	static const uint8_t exampleReadSerialNumberRequestV25[];
	static const uint8_t exampleReadSerialNumberResponseV25[];

	bool CreateReadSerialNumberRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessReadSerialNumberResponse(const uint8_t busId, const std::vector<uint8_t>& response, std::string& serialNumber);

	// ============================================================================
	// 
	// Main "Realtime Monitoring" tab of PBmsTools 2.4
	// These are in the "Switch Control" section
	// 
	// ============================================================================

	// ==== Sound Alarm Switch
	// 1: The "on/off" switch command, see: enum SwitchCommand
	// open:  ~25004699E0020DFD12.
	//                     11
	// resp:  ~25004600C0040D01FCC3.
	//                     11??
	// close: ~25004699E0020CFD13.
	//                     11
	// resp:  ~25004600C0040C00FCC5.
	//                     11??

	static const uint8_t exampleWriteDisableBuzzerSwitchCommandRequestV25[];
	static const uint8_t exampleWriteDisableBuzzerSwitchCommandResponseV25[];
	static const uint8_t exampleWriteEnableBuzzerSwitchCommandRequestV25[];
	static const uint8_t exampleWriteEnableBuzzerSwitchCommandResponseV25[];

	// ==== LED Alarm Switch
	// 1: The "on/off" switch command, see: enum SwitchCommand
	// open:  ~25004699E00206FD20.
	//                     11
	// resp:  ~25004600C0040602FCD0.
	//                     11??
	// close: ~25004699E00207FD1F.
	//                     11
	// resp:  ~25004600C0040722FCCD.
	//                     11??

	static const uint8_t exampleWriteDisableLedWarningSwitchCommandRequestV25[];
	static const uint8_t exampleWriteDisableLedWarningSwitchCommandResponseV25[];
	static const uint8_t exampleWriteEnableLedWarningSwitchCommandRequestV25[];
	static const uint8_t exampleWriteEnableLedWarningSwitchCommandResponseV25[];

	// ==== Charge Current Limiter Switch
	// 1: The "on/off" switch command, see: enum SwitchCommand
	// open:  ~25004699E0020AFD15.
	//                     11
	// resp:  ~25004600C0040A22FCC3.
	//                     11??
	// close: ~25004699E0020BFD14.
	//                     11
	// resp:  ~25004600C0040B32FCC1.
	//                     11??

	static const uint8_t exampleWriteDisableChargeCurrentLimiterSwitchCommandRequestV25[];
	static const uint8_t exampleWriteDisableChargeCurrentLimiterSwitchCommandResponseV25[];
	static const uint8_t exampleWriteEnableChargeCurrentLimiterSwitchCommandRequestV25[];
	static const uint8_t exampleWriteEnableChargeCurrentLimiterSwitchCommandResponseV25[];

	// note: this is actually in the "System Configuration" section of PBmsTools 2.4 but logically belongs here and uses the same CID2 as the other switch commands
	// ==== Charge Current Limiter Current Limit Gear Switch
	// 1: The "low/high" switch command, see: enum SwitchCommand
	// low:   ~25004699E00209FD1D.
	//                     11
	// resp:  ~25004600C0040938FCC4.
	//                     11??
	// high:  ~25004699E00208FD1E.
	//                     11
	// resp:  ~25004600C0040830FCCD.
	//                     11??

	static const uint8_t exampleWriteSetChargeCurrentLimiterCurrentLimitLowGearSwitchCommandRequestV25[];
	static const uint8_t exampleWriteSetChargeCurrentLimiterCurrentLimitLowGearSwitchCommandResponseV25[];
	static const uint8_t exampleWriteSetChargeCurrentLimiterCurrentLimitHighGearSwitchCommandRequestV25[];
	static const uint8_t exampleWriteSetChargeCurrentLimiterCurrentLimitHighGearSwitchCommandResponseV25[];

	// note: use caution as there are lot of different brands using the PACE BMS and some may have custom firmware with different commands!
	enum SwitchCommand : uint8_t
	{
		SC_DisableBuzzer = 0x0C,
		SC_EnableBuzzer = 0x0D,

		// for LED, the documentation and the "on the wire" data from PBmsTools conflict, this is on the wire, documentation is the reverse of this
		SC_DisableLedWarning = 0x06,
		SC_EnableLedWarning = 0x07,

		// for ChargeCurrentLimiter, the documentation and the "on the wire" data from PBmsTools conflict, this is on the wire, documentation is the reverse of this
		SC_DisableChargeCurrentLimiter = 0x0A,
		SC_EnableChargeCurrentLimiter = 0x0B,

		SC_SetChargeCurrentLimiterCurrentLimitHighGear = 0x08,
		SC_SetChargeCurrentLimiterCurrentLimitLowGear = 0x09,
	};

	bool CreateWriteSwitchCommandRequest(const uint8_t busId, const SwitchCommand command, std::vector<uint8_t>& request);
	bool ProcessWriteSwitchCommandResponse(const uint8_t busId, const SwitchCommand command, const std::vector<uint8_t>& response);

	// ==== Charge MOSFET Switch
	// note: I have seen the BMS enforce that at least one of Charge MOSFET or Discharge MOSFET must always be on, 
	//       you cannot turn both off but you can turn them off individually
	// note: Additionally, I can turn the charge mosfet OFF when the BMS is idle, or already charging, but not while discharging, which is... strange
	// 1: The "on/off" state, see: enum MosfetState
	// open:  ~2500469AE00200FD1E.
	//                     11
	// resp:  ~25004600E00226FD30.
	//                     ??
	// close: ~2500469AE00201FD1D.
	//                     11
	// resp:  ~25004600E00224FD32.
	//                     ??

	static const uint8_t exampleWriteMosfetChargeOpenSwitchCommandRequestV25[];
	static const uint8_t exampleWriteMosfetChargeOpenSwitchCommandResponseV25[];
	static const uint8_t exampleWriteMosfetChargeCloseSwitchCommandRequestV25[];
	static const uint8_t exampleWriteMosfetChargeCloseSwitchCommandResponseV25[];

	// ==== Discharge MOSFET Switch
	// note: I have seen the BMS enforce that at least one of Charge MOSFET or Discharge MOSFET must always be on, 
	//       you cannot turn both off but you can turn them off individually
	// note: Additionally, I can turn the discharge mosfet OFF when the BMS is idle, or already discharging, but not while charging, which is... strange
	// 1: The "on/off" state, see: enum MosfetState
	// open:  ~2500469BE00200FD1D.
	//                     11
	// resp:  ~25004600E00204FD34.
	//                     ??
	// close: ~2500469BE00201FD1C.
	//                     11
	// resp:  ~25004609E00204FD2B.
	//                     ??

	static const uint8_t exampleWriteMosfetDischargeOpenSwitchCommandRequestV25[];
	static const uint8_t exampleWriteMosfetDischargeOpenSwitchCommandResponseV25[];
	static const uint8_t exampleWriteMosfetDischargeCloseSwitchCommandRequestV25[];
	static const uint8_t exampleWriteMosfetDischargeCloseSwitchCommandResponseV25[];

	enum MosfetType : uint8_t
	{
		MT_Charge = CID2_WriteChargeMosfetSwitchCommand,
		MT_Discharge = CID2_WriteDischargeMosfetSwitchCommand
	};

	enum MosfetState : uint8_t
	{
		MS_Open = 0x01,
		MS_Close = 0x00
	};

	bool CreateWriteMosfetSwitchCommandRequest(const uint8_t busId, const MosfetType type, const MosfetState command, std::vector<uint8_t>& request);
	bool ProcessWriteMosfetSwitchCommandResponse(const uint8_t busId, const MosfetType type, const MosfetState command, const std::vector<uint8_t>& response);

	// ==== Shutdown (if the BMS is active charge/discharging it will immediately reboot after shutdown)
	// x: unknown payload, this may be a command code and there may be more but I'm not going to test that due to potentially unknown consequences
	// write: ~2500469CE00201FD1B.
	//                     xx
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleWriteRebootCommandRequestV25[];
	static const uint8_t exampleWriteRebootCommandResponseV25[];

	bool CreateWriteShutdownCommandRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessWriteShutdownCommandResponse(const uint8_t busId, const std::vector<uint8_t>& response);

// ============================================================================
// 
// "Memory Information" tab of PBmsTools 2.4 
// 
// ============================================================================

	// ==== Read Log History
	// This appears to be a "history" table
	// I'm not sure what prompts the battery to create a "history record" entry - the number of entries per day varies from 2-6 at a glance and there is sometimes a week or two missing between records
	// My battery contained 400 records (and it's been on for over a year continuous, so I believe this is the limit)
	// The last 4 (ASCII hex digits) request payload digits are a "count up" starting at 0000 and ending at 0x0190 = 400 dec, record index is zero-based with newest first (lowest payload value)
	// I haven't decoded the response yet, but it contains
	//         Date/Time
	//         Pack Amps (-in/out)
	//         Pack Voltage
	//         Remaing Capacity (Ah)
	//         Full Capacity (Ah)
	//         MaxVolt (cell) (mV)
	//         MinVolt (cell) (mV)
	//         Alarm Type
	//         Protect Type
	//         Fault Type
	//         Cell Voltage 1-16
	//         Temperatures 1-6
	// req:   ~250046A1C004018FFCA7.
	// resp:  ~25004600709018021D020038100D970D990D9A0D990D990D970D990D980D990D980D800D980D980D990D980D98060B740B750B770B760B710B79FF6ED9D7286A286A0000000000060043FFFFFFFFDDE3.
	//            the values in this response:  
	//                2024-2-29 2:00:56 - 1.460	
	//                55.767	
	//                103.460	
	//                103.460	
	//                3482	
	//                3456				
	//                3479	3481	3482	3481	3481	3479	3481	3480	3481	3480	3456	3480	3480	3481	3480	3480	
	//                20.2	20.3	20.5	20.4	19.9	20.7
	// resp:  ~250046000000FDAF.
	//            this means "no more records available"


	// -------- NOT IMPLEMENTED --------


	// ==== System Time
	// 1 Year:   read: 2024 write: 2024 (add 2000) apparently the engineers at pace are sure all of these batteries will be gone by Y2.1K or are too young to remember Y2K :)
	// 2 Month:  read: 08   write: 08
	// 3 Day:    read: 21   write: 20 
	// 4 Hour:   read: 05   write: 14
	// 5 Minute: read: 29   write: 15
	// 6 Second: read: 31   write: 37
	// read:  ~250046B10000FD9C.
	// resp:  ~25004600400C180815051D1FFB10.
	//                     112233445566
	// write: ~250046B2400C1808140E0F25FAFC.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadSystemTimeRequestV25[];
	static const uint8_t exampleReadSystemTimeResponseV25[];
	static const uint8_t exampleWriteSystemTimeRequestV25[];
	static const uint8_t exampleWriteSystemTimeResponseV25[];

	bool CreateReadSystemDateTimeRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessReadSystemDateTimeResponse(const uint8_t busId, const std::vector<uint8_t>& response, DateTime& dateTime);
	bool CreateWriteSystemDateTimeRequest(const uint8_t busId, const DateTime dateTime, std::vector<uint8_t>& request);
	bool ProcessWriteSystemDateTimeResponse(const uint8_t busId, const std::vector<uint8_t>& response);

// ============================================================================
// 
// "Parameter Setting" tab of PBmsTools 2.4
// 
// ============================================================================

	enum ReadConfigurationType {
		RC_CellOverVoltage                    = CID2_ReadCellOverVoltageConfiguration,
		RC_PackOverVoltage                    = CID2_ReadPackOverVoltageConfiguration,
		RC_CellUnderVoltage                   = CID2_ReadCellUnderVoltageConfiguration,
		RC_PackUnderVoltage                   = CID2_ReadPackUnderVoltageConfiguration,
		RC_ChargeOverCurrent                  = CID2_ReadChargeOverCurrentConfiguration,
		RC_DischargeOverCurrent1              = CID2_ReadDischargeSlowOverCurrentConfiguration,
		RC_DischargeOverCurrent2              = CID2_ReadDischargeFastOverCurrentConfiguration,
		RC_ShortCircuitProtection             = CID2_ReadShortCircuitProtectionConfiguration,
		RC_CellBalancing                      = CID2_ReadCellBalancingConfiguration,
		RC_Sleep                              = CID2_ReadSleepConfiguration,
		RC_FullChargeLowCharge                = CID2_ReadFullChargeLowChargeConfiguration,
		RC_ChargeAndDischargeOverTemperature  = CID2_ReadChargeAndDischargeOverTemperatureConfiguration,
		RC_ChargeAndDischargeUnderTemperature = CID2_ReadChargeAndDischargeUnderTemperatureConfiguration,
		RC_MosfetOverTemperature              = CID2_ReadMosfetOverTemperatureConfiguration,
		RC_EnvironmentOverUnderTemperature    = CID2_ReadEnvironmentOverUnderTemperatureConfiguration,
	};

	// these are used for all of the individual configurations, "book-ending" them, while individual configuration's 
	// process response / create write request are differentiated via parameter overload, taking or returning one of 
	// the configuration structs
	bool CreateReadConfigurationRequest(const uint8_t busId, const ReadConfigurationType configType, std::vector<uint8_t>& request);
	bool ProcessWriteConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response);

	// ==== Cell Over Voltage Configuration
	// 1 Cell OV Alarm (V): 3.60 - stored as v * 1000, so 3.6 is 3600 - valid range reported by PBmsTools as 2.5-4.5 in steps of 0.01
	// 2 Cell OV Protect (V): 3.70 - stored as v * 1000, so 3.7 is 3700 - valid range reported by PBmsTools as 2.5-4.5 in steps of 0.01
	// 3 Cell OVP Release (V): 3.38  - stored as v * 1000, so 3.38 is 3380 - valid range reported by PBmsTools as 2.5-4.5 in steps of 0.01
	// 4 Cell OVP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 1000 to 20000 in steps of 500
	// read:  ~250046D10000FD9A.
	// resp:  ~25004600F010010E100E740D340AFA35.
	//                     ??11112222333344 
	// write: ~250046D0F010010E100E740D340AFA21.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadCellOverVoltageConfigurationRequestV25[];
	static const uint8_t exampleReadCellOverVoltageConfigurationResponseV25[];
	static const uint8_t exampleWriteCellOverVoltageConfigurationRequestV25[];
	static const uint8_t exampleWriteCellOverVoltageConfigurationResponseV25[];

	struct CellOverVoltageConfiguration
	{
		uint16_t AlarmMillivolts;
		uint16_t ProtectionMillivolts;
		uint16_t ProtectionReleaseMillivolts;
		uint16_t ProtectionDelayMilliseconds;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, CellOverVoltageConfiguration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const CellOverVoltageConfiguration& config, std::vector<uint8_t>& request);

	// ==== Pack Over Voltage Configuration
	// 1 Pack OV Alarm (V): 57.6 (write: 57.61) - stored as v * 100, so 57.6 is 57600 - valid range reported by PBmsTools as 20-65 in steps of 0.01
	// 2 Pack OV Protect (V): 59.2 - stored as v * 1000, so 59.2 is 59200 - valid range reported by PBmsTools as 20-65 in steps of 0.01
	// 3 Pack OVP Release (V): 54.0 - stored as v * 1000, so 54.0 is 54000 - valid range reported by PBmsTools as 20-65 in steps of 0.01
	// 4 Pack OVP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 1000 to 20000 in steps of 500
	// read:  ~250046D50000FD96.
	// resp:  ~25004600F01001E100E740D2F00AFA24.
	//                     ??11112222333344
	// write: ~250046D4F01001E10AE740D2F00AF9FB.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadPackOverVoltageConfigurationRequestV25[];
	static const uint8_t exampleReadPackOverVoltageConfigurationResponseV25[];
	static const uint8_t exampleWritePackOverVoltageConfigurationRequestV25[];
	static const uint8_t exampleWritePackOverVoltageConfigurationResponseV25[];

	struct PackOverVoltageConfiguration
	{
		uint16_t AlarmMillivolts;
		uint16_t ProtectionMillivolts;
		uint16_t ProtectionReleaseMillivolts;
		uint16_t ProtectionDelayMilliseconds;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t> response, PackOverVoltageConfiguration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const PackOverVoltageConfiguration& config, std::vector<uint8_t>& request);

	// ==== Cell Under Voltage Configuration
	// 1 Cell UV Alarm (V): 2.8 - stored as v * 1000, so 2.8 is 2800 - valid range reported by PBmsTools as 2-3.5 in steps of 0.01
	// 2 Cell UV Protect (V): 2.5 - stored as v * 1000, so 2.5 is 2500 - valid range reported by PBmsTools as 2-3.5 in steps of 0.01
	// 3 Cell UVP Release (V): 2.9 - stored as v * 1000, so 2.9 is 2900 - valid range reported by PBmsTools as 2-3.5 in steps of 0.01
	// 4 Cell UVP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 1000 to 20000 in steps of 500
	// read:  ~250046D30000FD98.
	// resp:  ~25004600F010010AF009C40B540AFA24.
	//                     ??11112222333344
	// write: ~250046D2F010010AF009C40B540AFA0E.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadCellUnderVoltageConfigurationRequestV25[];
	static const uint8_t exampleReadCellUnderVoltageConfigurationResponseV25[];
	static const uint8_t exampleWriteCellUnderVoltageConfigurationRequestV25[];
	static const uint8_t exampleWriteCellUnderVoltageConfigurationResponseV25[];

	struct CellUnderVoltageConfiguration
	{
		uint16_t AlarmMillivolts;
		uint16_t ProtectionMillivolts;
		uint16_t ProtectionReleaseMillivolts;
		uint16_t ProtectionDelayMilliseconds;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, CellUnderVoltageConfiguration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const CellUnderVoltageConfiguration& config, std::vector<uint8_t>& request);

	// ==== Pack Under Voltage Configuration
	// 1 Pack UV Alarm (V): 44.8 - stored as v * 1000, so 44.8 is 44800 - valid range reported by PBmsTools as 15-50 in steps of 0.01
	// 2 Pack UV Protect (V): 40.0 - stored as v * 1000, so 40.0 is 40000 - valid range reported by PBmsTools as 15-50 in steps of 0.01
	// 3 Pack UVP Release (V): 46.4 - stored as v * 1000, so 46.4 is 46400 - valid range reported by PBmsTools as 15-50 in steps of 0.01
	// 4 Pack UVP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 1000 to 20000 in steps of 500
	// read:  ~250046D70000FD94.
	// resp:  ~25004600F01001AF009C40B5400AFA24.
	//                     ??11112222333344
	// write: ~250046D6F01001AF009C40B5400AFA0A.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadPackUnderVoltageConfigurationRequestV25[];
	static const uint8_t exampleReadPackUnderVoltageConfigurationResponseV25[];
	static const uint8_t exampleWritePackUnderVoltageConfigurationRequestV25[];
	static const uint8_t exampleWritePackUnderVoltageConfigurationResponseV25[];

	struct PackUnderVoltageConfiguration
	{
		uint16_t AlarmMillivolts;
		uint16_t ProtectionMillivolts;
		uint16_t ProtectionReleaseMillivolts;
		uint16_t ProtectionDelayMilliseconds;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, PackUnderVoltageConfiguration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const PackUnderVoltageConfiguration& config, std::vector<uint8_t>& request);

	// ==== Charge Over Current Configuration
	// 1 Charge OC Alarm (A): 104 - stored directly in amps - valid range reported by PBmsTools as 1-220
	// 2 Charge OC Protect (A): 110 - stored directly in amps - valid range reported by PBmsTools as 1-220
	// 3 Charge OCP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 500 to 25000 in steps of 500
	// read:  ~250046D90000FD92.
	// resp:  ~25004600400C010068006E0AFB1D.
	//                     ??1111222233
	// write: ~250046D8400C010068006E0AFB01.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadChargeOverCurrentConfigurationRequestV25[];
	static const uint8_t exampleReadChargeOverCurrentConfigurationResponseV25[];
	static const uint8_t exampleWriteChargeOverCurrentConfigurationRequestV25[];
	static const uint8_t exampleWriteChargeOverCurrentConfigurationResponseV25[];

	struct ChargeOverCurrentConfiguration
	{
		uint16_t AlarmAmperage;
		uint16_t ProtectionAmperage;
		uint16_t ProtectionDelayMilliseconds;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, ChargeOverCurrentConfiguration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const ChargeOverCurrentConfiguration& config, std::vector<uint8_t>& request);

	// ==== Discharge Over Current 1 Configuration
	// 1 Discharge OC Alarm (A): 105 - stored as negative two's complement in amps***, -105 is FF97 - valid range reported by PBmsTools as 1-220
	// 2 Discharge OC 1 Protect (A): 110 - stored as negative two's complement in amps***, -110 is FF92 - valid range reported by PBmsTools as 1-220
	// 3 Discharge OC 1 Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 500 to 25000 in steps of 500
	// ********* important *********: this is returned as the negative two's complement, but is STORED (written back) as the normal positive value!
	// read:  ~250046DB0000FD89.
	// resp:  ~25004600400C01FF97FF920AFAD3.
	//                     ??1111222233
	// write: ~250046DA400C010069006E0AFAF7.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadDishargeOverCurrent1ConfigurationRequestV25[];
	static const uint8_t exampleReadDishargeOverCurrent1ConfigurationResponseV25[];
	static const uint8_t exampleWriteDishargeOverCurrent1ConfigurationRequestV25[];
	static const uint8_t exampleWriteDishargeOverCurrent1ConfigurationResponseV25[];

	struct DischargeOverCurrent1Configuration
	{
		uint16_t AlarmAmperage;
		uint16_t ProtectionAmperage;
		uint16_t ProtectionDelayMilliseconds;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, DischargeOverCurrent1Configuration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const DischargeOverCurrent1Configuration& config, std::vector<uint8_t>& request);

	// ==== Dicharge Over Current 2 Configuration
	// 1 Discharge OC 2 Protect: 150 - stored directly in amps - valid range reported by PBmsTools as 5-300 in steps of 5, but since this is an 8 bit store location, the actual max is 255????????
	// 2 Discharge OC 2 Delay Time (ms): 100 - stored in 25ms steps, so 100 is 4 (4x25=100), 400 is 16 (16x25=400) - valid range reported by PBmsTools as 100-2000 in steps of 100
	// x = apparently, garbage written by the firmware - it's not included in the PBmsTools write
	// read:  ~250046E30000FD97.
	// resp:  ~25004600400C009604009604FB32.
	//                     ??1122xxxxxx
	// write: ~250046E2A006009604FC4E.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadDishargeOverCurrent2ConfigurationRequestV25[];
	static const uint8_t exampleReadDishargeOverCurrent2ConfigurationResponseV25[];
	static const uint8_t exampleWriteDishargeOverCurrent2ConfigurationRequestV25[];
	static const uint8_t exampleWriteDishargeOverCurrent2ConfigurationResponseV25[];

	struct DischargeOverCurrent2Configuration
	{
		uint8_t ProtectionAmperage;
		uint16_t ProtectionDelayMilliseconds;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, DischargeOverCurrent2Configuration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const DischargeOverCurrent2Configuration& config, std::vector<uint8_t>& request);

	// ==== Short Circuit Protection Configuration
	// 1 Delay Time (us): 300 - stored in 25 microsecond steps, 300 is 12 - valid range reported by PBmsTools as as 100-500 in steps of 50
	// read:  ~250046E50000FD95.
	// resp:  ~25004600E0020CFD25.
	//                     11
	// write: ~250046E4E0020CFD0C.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadShortCircuitProtectionConfigurationRequestV25[];
	static const uint8_t exampleReadShortCircuitProtectionConfigurationResponseV25[];
	static const uint8_t exampleWriteShortCircuitProtectionConfigurationRequestV25[];
	static const uint8_t exampleWriteShortCircuitProtectionConfigurationResponseV25[];

	struct ShortCircuitProtectionConfiguration
	{
		uint16_t ProtectionDelayMicroseconds;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, ShortCircuitProtectionConfiguration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const ShortCircuitProtectionConfiguration& config, std::vector<uint8_t>& request);

	// ==== Cell Balancing Configuration
	// 1 Balance Threshold (V): 3.4 - stored as v * 1000, so 3.4 is 3400 - valid range reported by PBmsTools as 3.3-4.5 in steps of 0.01
	// 2 Balance Delta Cell (mv): 30 - stored directly, so 30 is 30 - valid range reported by PBmsTools as 20-500 in steps of 5
	// read:  ~250046B60000FD97.
	// resp:  ~2500460080080D48001EFBE9.
	//                     11112222
	// write: ~250046B580080D48001EFBD2.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadCellBalancingConfigurationRequestV25[];
	static const uint8_t exampleReadCellBalancingConfigurationResponseV25[];
	static const uint8_t exampleWriteCellBalancingConfigurationRequestV25[];
	static const uint8_t exampleWriteCellBalancingConfigurationResponseV25[];

	struct CellBalancingConfiguration
	{
		uint16_t ThresholdMillivolts;
		uint16_t DeltaCellMillivolts;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, CellBalancingConfiguration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const CellBalancingConfiguration& config, std::vector<uint8_t>& request);

	// ==== Sleep Configuration
	// 1 Sleep v-cell: 3.1 - stored as v * 1000, so 3.1 is 3100 - valid range reported by PBmsTools as 2-4 in steps of 0.01
	// 2 Delay Time (minute): 5 - stored directly - valid range reported by PBmsTools as 1-120
	// read:  ~250046A00000FD9E.
	// resp:  ~2500460080080C1C0005FBF3.
	//                     1111??22
	// write: ~250046A880080C1C0005FBDA.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadSleepConfigurationRequestV25[];
	static const uint8_t exampleReadSleepConfigurationResponseV25[];
	static const uint8_t exampleWriteSleepConfigurationRequestV25[];
	static const uint8_t exampleWriteSleepConfigurationResponseV25[];

	struct SleepConfiguration
	{
		uint16_t CellMillivolts;
		uint8_t DelayMinutes;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, SleepConfiguration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const SleepConfiguration& config, std::vector<uint8_t>& request);

	// ==== Full Charge and Low Charge
	// 1 Pack Full Charge Voltage: 56.0 - stored as v * 1000, so 56 is 56000 - valid range reported by PBmsTools as 20-65 in steps of 0.01
	// 2 Pack Full Charge Current (ma): 2000 - stored directly in ma - valid range reported by PBmsTools as 500-5000 in steps of 500
	// 3 State of Charge Low Alarm (%): 5 - stored directly - valid range reported by PBmsTools as 0-100
	// read:  ~250046AF0000FD88.
	// resp:  ~25004600600ADAC007D005FB60.
	//                     1111222233
	// write: ~250046AE600ADAC007D005FB3A.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadFullChargeLowChargeConfigurationRequestV25[];
	static const uint8_t exampleReadFullChargeLowChargeConfigurationResponseV25[];
	static const uint8_t exampleWriteFullChargeLowChargeConfigurationRequestV25[];
	static const uint8_t exampleWriteFullChargeLowChargeConfigurationResponseV25[];

	struct FullChargeLowChargeConfiguration
	{
		uint16_t FullChargeMillivolts;
		uint16_t FullChargeMilliamps;
		uint8_t LowChargeAlarmPercent;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, FullChargeLowChargeConfiguration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const FullChargeLowChargeConfiguration& config, std::vector<uint8_t>& request);

	// ==== Charge / Discharge Over Temperature Protection Configuration
	// 1 Charge Over Temperature Alarm: 51 - stored as (value * 10) + 2730 = 3240, to decode (value - 2730) / 10.0 = 51 - valid range reported by PBmsTools as 20-100
	// 2 Charge Over Temperature Protect: 55 - stored as (value * 10) + 2730 = 3280, to decode (value - 2730) / 10.0 = 55 - valid range reported by PBmsTools as 20-100
	// 3 Charge Over Temperature Protection Release: 50 - stored as (value * 10) + 2730 = 3230, to decode (value - 2730) / 10.0 = 50 - valid range reported by PBmsTools as 20-100
	// 4 Discharge Over Temperature Alarm: 56 - stored as (value * 10) + 2730 = 3290, to decode (value - 2730) / 10.0 = 56 - valid range reported by PBmsTools as 20-100
	// 5 Discharge Over Temperature Protect: 60 - stored as (value * 10) + 2730 = 3330, to decode (value - 2730) / 10.0 = 60 - valid range reported by PBmsTools as 20-100
	// 6 Discharge Over Temperature Protect Release: 55 - stored as (value * 10) + 2730 = 3280, to decode (value - 2730) / 10.0 = 55 - valid range reported by PBmsTools as 20-100
	// read:  ~250046DD0000FD87.
	// resp:  ~25004600501A010CA80CD00C9E0CDA0D020CD0F7BE.
	//                     ??111122223333444455556666
	// write: ~250046DC501A010CA80CD00C9E0CDA0D020CD0F797.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadChargeAndDischargeOverTemperatureConfigurationRequestV25[];
	static const uint8_t exampleReadChargeAndDischargeOverTemperatureConfigurationResponseV25[];
	static const uint8_t exampleWriteChargeAndDischargeOverTemperatureConfigurationRequestV25[];
	static const uint8_t exampleWriteChargeAndDischargeOverTemperatureConfigurationResponseV25[];

	struct ChargeAndDischargeOverTemperatureConfiguration
	{
		uint8_t ChargeAlarm;
		uint8_t ChargeProtection;
		uint8_t ChargeProtectionRelease;
		uint8_t DischargeAlarm;
		uint8_t DischargeProtection;
		uint8_t DischargeProtectionRelease;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, ChargeAndDischargeOverTemperatureConfiguration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const ChargeAndDischargeOverTemperatureConfiguration& config, std::vector<uint8_t>& request);

	// ==== Charge / Discharge Under Temperature Protection Configuration   
	// 1 Charge Under Temperature Alarm: 0 - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as (-35)-30
	// 2 Charge Under Temperature Protection: (-5) - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as (-35)-30
	// 3 Charge Under Temperature Release: 0 - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as (-35)-30
	// 4 Discharge Under Temperature Alarm: (-15) - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as (-35)-30
	// 5 Discharge Under Temperature Protect: (-20) - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as (-35)-30
	// 5 Discharge Under Temperature Release: (-15) - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as (-35)-30
	// read:  ~250046DF0000FD85.
	// resp:  ~25004600501A010AAA0A780AAA0A1409E20A14F7E5.
	//                     ??111122223333444455556666
	// write: ~250046DE501A010AAA0A780AAA0A1409E20A14F7BC.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadChargeAndDischargeUnderTemperatureConfigurationRequestV25[];
	static const uint8_t exampleReadChargeAndDischargeUnderTemperatureConfigurationResponseV25[];
	static const uint8_t exampleWriteChargeAndDischargeUnderTemperatureConfigurationRequestV25[];
	static const uint8_t exampleWriteChargeAndDischargeUnderTemperatureConfigurationResponseV25[];

	struct ChargeAndDischargeUnderTemperatureConfiguration
	{
		int8_t ChargeAlarm;
		int8_t ChargeProtection;
		int8_t ChargeProtectionRelease;
		int8_t DischargeAlarm;
		int8_t DischargeProtection;
		int8_t DischargeProtectionRelease;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, ChargeAndDischargeUnderTemperatureConfiguration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const ChargeAndDischargeUnderTemperatureConfiguration& config, std::vector<uint8_t>& request);

	// ==== Mosfet Over Temperature Protection Configuration
	// 1 Mosfet Over Temperature Alarm: 90 - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as 30-120
	// 2 Mosfet Over Temperature Protection: 110 - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as 30-120
	// 3 Mosfet Over Temperature Release: 85 - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as 30-120
	// read:  ~250046E10000FD99.
	// resp:  ~25004600200E010E2E0EF60DFCFA5D.
	//                     ??111122223333
	// write: ~250046E0200E010E2E0EF60DFCFA48.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadMosfetOverTemperatureConfigurationRequestV25[];
	static const uint8_t exampleReadMosfetOverTemperatureConfigurationResponseV25[];
	static const uint8_t exampleWriteMosfetOverTemperatureConfigurationRequestV25[];
	static const uint8_t exampleWriteMosfetOverTemperatureConfigurationResponseV25[];

	struct MosfetOverTemperatureConfiguration
	{
		int8_t Alarm;
		int8_t Protection;
		int8_t ProtectionRelease;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, MosfetOverTemperatureConfiguration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const MosfetOverTemperatureConfiguration& config, std::vector<uint8_t>& request);

	// ==== Environment Over/Under Temperature Protection Configuration
	// 1 Environment Under Temperature Alarm: (-20) - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as (-35)-30
	// 2 Environment Under Temperature Protection: (-25) - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as (-35)-30
	// 3 Environment Under Temperature Release: (-20) - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as (-35)-30
	// 4 Environment Over Temperature Alarm: 65 - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as 20-100
	// 5 Environment Over Temperature Protection: 70 - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as 20-100
	// 6 Environment Over Temperature Release: 65 - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as 20-100
	// read:  ~250046E70000FD93.
	// resp:  ~25004600501A0109E209B009E20D340D660D34F806.
	//                     ??111122223333444455556666
	// write: ~250046E6501A0109E209B009E20D340D660D34F7EB.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadEnvironmentOverUnderTemperatureConfigurationRequestV25[];
	static const uint8_t exampleReadEnvironmentOverUnderTemperatureConfigurationResponseV25[];
	static const uint8_t exampleWriteEnvironmentOverUnderTemperatureConfigurationRequestV25[];
	static const uint8_t exampleWriteEnvironmentOverUnderTemperatureConfigurationResponseV25[];

	struct EnvironmentOverUnderTemperatureConfiguration
	{
		int8_t UnderAlarm;
		int8_t UnderProtection;
		int8_t UnderProtectionRelease;
		int8_t OverAlarm;
		int8_t OverProtection;
		int8_t OverProtectionRelease;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, EnvironmentOverUnderTemperatureConfiguration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const EnvironmentOverUnderTemperatureConfiguration& config, std::vector<uint8_t>& request);

// ============================================================================
// 
// "System Configuration" tab of PBmsTools 2.4
// 
// ============================================================================

	// note: "Charge Current Limiter Current Limit Gear Switch" is in this page in PBmsTools but I moved it to the SwitchCommand section above because it uses the same CID2 and fits in nicely with that code

	// "The charge limiter limits the charge current if it goes > 100A to 10A, this is useful for a multi-pack setup where the packs are not balanced"

	// ==== Charge Current Limiter Start Current 
	// 1 Charge Current Limiter Start Current: 100 - stored directly - valid range reported by PBmsTools as 5-150 
	// read:  ~250046ED0000FD86.
	// resp:  ~25004600C0040064FCCE.
	//                     ??11
	// write: ~250046EEC0040064FCA4.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadChargeCurrentLimiterStartCurrentRequestV25[];
	static const uint8_t exampleReadChargeCurrentLimiterStartCurrentResponseV25[];
	static const uint8_t exampleWriteChargeCurrentLimiterStartCurrentRequestV25[];
	static const uint8_t exampleWriteChargeCurrentLimiterStartCurrentResponseV25[];

	bool CreateReadChargeCurrentLimiterStartCurrentRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessReadChargeCurrentLimiterStartCurrentResponse(const uint8_t busId, const std::vector<uint8_t>& response, uint8_t& current);
	bool CreateWriteChargeCurrentLimiterStartCurrentRequest(const uint8_t busId, const uint8_t current, std::vector<uint8_t>& request);
	bool ProcessWriteChargeCurrentLimiterStartCurrentResponse(const uint8_t busId, const std::vector<uint8_t>& response);

	// ==== Read Remaining Capacity
	// 1 Remaining Capacity (mAh): 62040 - stored in 10mAh hours, so 62040 is 6204
	// 2 Actual Capacity (mAh): 103460 - stored in 10mAh hours, so 103460 is 10346
	// 3 Design Capacity (mAh): 100000 - stored in 10mAh hours, so 100000 is 10000
	// read:  ~250046A60000FD98.
	// resp:  ~25004600400C183C286A2710FB0E.
	//                     111122223333

	static const uint8_t exampleReadRemainingCapacityRequestV25[];
	static const uint8_t exampleReadRemainingCapacityResponseV25[];

	bool CreateReadRemainingCapacityRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessReadRemainingCapacityResponse(const uint8_t busId, const std::vector<uint8_t>& response, uint32_t& remainingCapacityMilliampHours, uint32_t& actualCapacityMilliampHours, uint32_t& designCapacityMilliampHours);

	// ==== Protocol
	// 1 - CAN protocol, see enum, this example is "AFORE"
	// 2 - RS485 protocol, see enum, this example is "RONGKE"
	// 3 - "Type", see enum, not sure what this means exactly, I'd go with "Auto" which is in this example
	// read:  ~250046EB0000FD88.
	// resp:  ~25004600A006131400FC6F.
	//                     112233
	// write: ~250046ECA006131400FC47.
	// resp:  ~250046000000FDAF.

	enum ProtocolList_CAN : uint8_t
	{
		can_empty = 0xFF,        // 255d <blank entry> I believe this means "turned off"
		can_Pace = 0x00,         // 00d PACE
		can_Pylon = 0x01,        // 01d Pylon / DeYe / CHNT Power / LiVolTek / Megarevo / SunSynk / SunGrow / Sol-Ark / SolarEdge
		can_Growatt = 0x02,      // 02d Growatt / Sacolar
		can_Victron = 0x03,      // 03d Victron
		can_Schneider = 0x04,    // 04d Schneider / SE / SMA
		can_LuxPower = 0x05,     // 05d LuxPower
		can_SoroTec = 0x06,      // 06d SoroTec / SRD
		can_SMA = 0x07,          // 07d SMA / Studer
		can_GoodWe = 0x08,       // 08d GoodWe
		can_Studer = 0x09,       // 09d Studer
		can_Sofar = 0x0A,        // 10d Sofar
		can_Must = 0x0B,         // 11d Must / PV
		can_Solis = 0x0C,        // 12d Solis / Jinlang / JL
		can_DIDU = 0x0D,         // 13d DIDU / TBB
		can_Senergy = 0x0E,      // 14d Senergy / Aifu
		can_TBB = 0x0F,          // 15d TBB
		can_Pylon_V202 = 0x10,   // 16d Pylon_V202
		can_Growatt_V109 = 0x11, // 17d Growatt_V109
		can_Must_V202 = 0x12,    // 18d Must_V202
		can_Afore = 0x13,        // 19d Afore
		can_INVT = 0x14,         // 20d INVT / YWT
		can_FUJI = 0x15,         // 21d FUJI
		can_Sofar_V21003 = 0x16, // 22d Sofar_V21003
	};

	enum ProtocolList_RS485 : uint8_t
	{
		rs485_empty = 0xFF,        // 255d <blank entry> I believe this means "turned off"
		rs485_PaceModbus = 0x00,   // 00d Pace Modbus
		rs485_Pylon = 0x01,        // 01d Pylon / DeYe / Bentterson
		rs485_Growatt = 0x02,      // 02d Growatt
		rs485_Voltronic = 0x03,    // 03d Voltronic / EA Sun Power / MPP Solar
		rs485_Schneider = 0x04,    // 04d Schneider / SE
		rs485_PHOCOS = 0x05,       // 05d PHOCOS
		rs485_LuxPower = 0x06,     // 06d LuxPower
		rs485_Solar = 0x07,        // 07d Solar
		rs485_Lithium = 0x08,      // 08d Lithium / SMARK
		rs485_EP = 0x09,           // 09d EP / MSL
		rs485_RTU04 = 0x0A,        // 10d RTU04
		rs485_LuxPower_V01 = 0x0B, // 11d LuxPower_V01
		rs485_LuxPower_V03 = 0x0C, // 12d LuxPower_V03
		rs485_SRNE = 0x0D,         // 13d SRNE / WOW
		rs485_LEOCH = 0x0E,        // 14d LEOCH
		rs485_Pylon_F = 0x0F,      // 15d Pylon_F
		rs485_Afore = 0x10,        // 16d Afore
		rs485_UPS_AGXN = 0x11,     // 17d UPS_AGXN
		rs485_Orex_Sunpolo = 0x12, // 18d Orex_Sunpolo
		rs485_XIONGTAO = 0x13,     // 19d XIONGTAO
		rs485_RONGKE = 0x14,       // 20d RONGKE
		rs485_XINRUI = 0x15,       // 21d XINRUI
		rs485_ELTEK = 0x16,        // 22d ELTEK
		rs485_GT = 0x17,           // 23d GT
		rs485_Leoch_V106 = 0x18,   // 24d Leoch_V106
	};

	enum ProtocolList_Type : uint8_t
	{
		empty = 0xFF,  // 255d <blank entry>
		Auto = 0x00,   // 00d Auto
		Manual = 0x01, // 01d Manual
	};

	static const uint8_t exampleReadProtocolsRequestV25[];
	static const uint8_t exampleReadProtocolsResponseV25[];
	static const uint8_t exampleWriteProtocolsRequestV25[];
	static const uint8_t exampleWriteProtocolsResponseV25[];

	struct Protocols
	{
		ProtocolList_CAN   CAN;
		ProtocolList_RS485 RS485;
		ProtocolList_Type  Type;
	};

	bool CreateReadProtocolsRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessReadProtocolsResponse(const uint8_t busId, const std::vector<uint8_t>& response, Protocols& protocols);
	bool CreateWriteProtocolsRequest(const uint8_t busId, const Protocols& protocols, std::vector<uint8_t>& request);
	bool ProcessWriteProtocolsResponse(const uint8_t busId, const std::vector<uint8_t>& response);


	// There are many other settings in "System Configuration" that can be written and/or calibrated here, 
	// none of which I am exposing because it would be a Very Bad Idea to mess with them


	/*
	* SOK Protocol Edit (pbms tools)
	-------------------------------
	note:  unable to get responses since my BMS ignores this command BUT faking responses via Pace BMS Emulator causes
			   the software to say "OK" when I send a response identical to that which works on the "new" set protocol command
			   I believe this one is manufacturer specific for SOK firmwares

	It's unclear if these commands would only work on rs232 but that's my guess.

	There does not appear to be a "get current inverter protocol" command for this older custom method

	set paceic (unclear if CAN/485)
	req: ~25004699E0020EFD11.
					  xx

	x = protocol to set
		0E = paceic version 25
		0F = Pylon (DeYe) CAN
		10 = Growatt CAN
		13 = Pylon 485
		12 = Growatt 485
		11 = LuxPower 485
	*/

	/*
	enum OldStyleProtocolList
	{
		old_paceic      = 0x0E,
		old_CAN_Pylon   = 0x0F,
		old_CAN_Growatt = 0x10,
		old_Pylon       = 0x13,
		old_Growatt     = 0x12,
		old_LuxPower    = 0x11,
	};
	*/

	// -------- NOT IMPLEMENTED --------
};

