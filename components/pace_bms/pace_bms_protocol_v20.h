#pragma once

#include "pace_bms_protocol_base.h"

/*
General format of requests/responses:
-------------------------------------
note: HexASCII means this is the hex value of a byte or bytes in the form of ASCII hexidecimal characters, so 1 logical byte = 2 HexASCII in the actual stream
	  presumably this is to make the protocol "human readable" but I actually just find it offputting
-------------------------------------
offset 0,        1 byte,   binary    0x7E:   SOI          - start of information '~'
offset 1,        1 byte,   HexASCII, "25":   VER          - protocol version 2.5
offset 3,        1 byte,   HexASCII, "01":   ADR          - device address 0-15
offset 5,        1 bytes,  HexASCII, "46":   CID1         - device identification code / device type description, "46" means "Lithium Iron"
offset 7,        1 bytes   HexASCII, "42":   CID2         - control indication code / data or action description, "42" means "analog pack information"
								*response:   RTN
offset 9,        2 bytes   HexASCII, "E002": CKLEN        - length of bytes (includes LEN and LCHKSUM)
offset 13, LENID bytes,    HexASCII, "01":   COMMAND_INFO - command information / control data information
							   response:     DATA_INFO    - response data information
offset LENID+13, 2 bytes,  HexASCII, "FD2E": CHKSUM       - checksum
offset LENID+17, 1 byte,   binary    0x0D:   EOI          - end of information '\r'
*/

class PaceBmsProtocolV20 : public PaceBmsProtocolBase
{
public:
	enum CID1 : uint8_t
	{
		CID1_LithiumIronEG4 = 0x4A, // undocumented value used by EG4 for lithium iron
		CID1_LithiumIron = 0x46, // documented as lithium iron
	};

	// dependency injection
	typedef void (*LogFuncPtr)(std::string message);

	// takes pointers to the "real" logging functions
	PaceBmsProtocolV20(
		OPTIONAL_NS::optional<std::string> protocol_variant, OPTIONAL_NS::optional<uint8_t> protocol_version_override, OPTIONAL_NS::optional<uint8_t> batteryChemistry,
		LogFuncPtr logError, LogFuncPtr logWarning, LogFuncPtr logInfo, LogFuncPtr logDebug, LogFuncPtr logVerbose, LogFuncPtr logVeryVerbose);

protected:
	enum CID2 : uint8_t
	{
		//// helps to figure out how to address an unknown pack
		//CID2_ReadProtocolVersion = 0x4F,
		//CID2_ReadPackQuantity = 0x90,

		CID2_ReadAnalogInformation = 0x42,
		CID2_ReadStatusInformation = 0x44,
		CID2_ReadHardwareVersion = 0x51,
		CID2_ReadSerialNumber = 0x93,

		CID2_WriteShutdownCommand = 0x95,

		CID2_ReadDateTime = 0x4D,
		CID2_WriteDateTime = 0x4E,
	};

public:

	// ==== Read Analog Information
	// x unknown value, this might be "up to 16 packs supported"
	// 0 Responding Bus Id
	// 1 Cell Count (this example has 16 cells)
	// 2 Cell Voltage (repeated Cell Count times) - stored as v * 1000, so 3.365 is 3365
	// 3 Temperature Count (this example has 6 temperatures but IS LYING and reports only 4)
	// 4 Temperature (repeated Temperature Count times) - stored as (value * 10) + 2730, to decode (value - 2730) / 10.0 = value
	// 5 Current - stored as value * 100
	// 6 Total Voltage - stored as value * 100
	// 7 Remaining Capacity - stored as value * 100
	// 9 Full Capacity - stored as value * 100
	// 0 Cycle Count
	// x the rest of this payload is unknown, it's not documented
	// ------1 Design Capacity - stored as value * 100
	// req:   ~20014A420000FDA2.
	// resp:  ~20014A00A0CA1001100D2F0D2C0D2C0D2D0D2D0D2F0D2F0D2F0D2C0D2D0D2D0D2F0D300D2C0D300D2C040B9B0BA50B9B0B9B0BB90BAF029D151521A9268400540F005700620D300D2C00040BA50B9B000ADAC0000A54550005D473000570A600000680000004CA56897E24D1A5.
	//                     xx001122222222222222222222222222222222222222222222222222222222222222223344444444444444444444444455556666777799990000
	//        

	static const uint8_t exampleReadAnalogInformationRequestV20[];
	static const uint8_t exampleReadAnalogInformationResponseV20[];

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

		float    SoH{ 0 }; // in percent
		float    SoC{ 0 }; // in percent
		float    powerWatts{ 0 };
		uint16_t minCellVoltageMillivolts{ 0 };
		uint16_t maxCellVoltageMillivolts{ 0 };
		uint16_t avgCellVoltageMillivolts{ 0 };
		uint16_t maxCellDifferentialMillivolts{ 0 };
	};

	bool CreateReadAnalogInformationRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessReadAnalogInformationResponse(const uint8_t busId, const std::vector<uint8_t>& response, AnalogInformation& analogInformation);

protected:
	// protocol variants
	bool ProcessReadAnalogInformationResponse_PYLON(const uint8_t busId, const std::vector<uint8_t>& response, AnalogInformation& analogInformation);
	bool ProcessReadAnalogInformationResponse_SEPLOS(const uint8_t busId, const std::vector<uint8_t>& response, AnalogInformation& analogInformation);
	bool ProcessReadAnalogInformationResponse_EG4(const uint8_t busId, const std::vector<uint8_t>& response, AnalogInformation& analogInformation);

public:
	// ==== Read Status Information
	// x unknown value, this might be "up to 16 packs supported"
	// 0 Responding Bus Id
	// 1 Cell Count (this example has 16 cells)
	// 2 Cell Warning (repeated Cell Count times) see: DecodeWarningValue / enum WarningValue
	// 3 Temperature Count (this example has 6 temperatures but IS LYING and reports only 4)
	// 4 Temperature Warning (repeated Temperature Count times) see: DecodeWarningValue / enum WarningValue
	// 5 Charge Current Warning see: DecodeWarningValue / enum WarningValue
	// 6 Total Voltage Warning see: DecodeWarningValue / enum WarningValue
	// 7 Discharge Current Warning see: DecodeWarningValue / enum WarningValue
	// 8 Status 1 see: enum Status1
	// 9 Status 2 see: enum Status2
	// 0 Status 3 see: enum Status3
	// 1 Status 4 see: enum Status4
	// 2 Status 5 see: enum Status5
	// req:   ~20014A440000FDA0.
	// resp:  ~20014A007054100110000000000000000000000000000000000400000000000000000900000000000003020000000000EDC3.
	//                     xx001122222222222222222222222222222222334444444444445566778899001122

	static const uint8_t exampleReadStatusInformationRequestV20[];
	static const uint8_t exampleReadStatusInformationResponseV20[];

	// possible values in:
	//     StatusInformation.warning_value_cell[index]
	//     StatusInformation.warning_value_temp[index]
	//     StatusInformation.warning_value_charge_current
	//     StatusInformation.warning_value_total_voltage
	//     StatusInformation.warning_value_discharge_current
	enum StatusInformation_WarningValues
	{
		WV_Normal = 0,
		WV_BelowLowerLimitValue = 1,
		WV_AboveUpperLimitValue = 2,
		WV_OtherFaultValue = 0xF0,
	};

	// UD value <not present>
	class StatusDecode_PYLON
	{
	public:
		enum StatusInformation_Status1
		{
			S1_PackUnderVoltage = (1 << 7),
			S1_ChargeTemperatureProtection = (1 << 6),
			S1_DischargeTemperatureProtection = (1 << 5),
			S1_DischargeOverCurrent = (1 << 4),
			S1_UndefinedStatus1Bit4 = (1 << 3),
			S1_ChargeOverCurrent = (1 << 2),
			S1_CellUnderVoltage = (1 << 1),
			S1_PackOverVoltage = (1 << 0),
		};
		enum StatusInformation_Status2
		{
			S2_UndefinedStatus2Bit8 = (1 << 7),
			S2_UndefinedStatus2Bit7 = (1 << 6),
			S2_UndefinedStatus2Bit6 = (1 << 5),
			S2_UndefinedStatus2Bit5 = (1 << 4),
			S2_UsingBatteryPower = (1 << 3),
			S2_DischargeMosfetOn = (1 << 2),
			S2_ChargeMosfetOn = (1 << 1),
			S2_PrechargeMosfetOn = (1 << 0),
		};
		enum StatusInformation_Status3
		{
			S3_Charging = (1 << 7),
			S3_Discharging = (1 << 6),
			S3_HeaterOn = (1 << 5),
			S3_UndefinedStatus3Bit5 = (1 << 4),
			S3_FullyCharged = (1 << 3),
			S3_UndefinedStatus3Bit3 = (1 << 2),
			S3_UndefinedStatus3Bit2 = (1 << 1),
			S3_Buzzer = (1 << 0),
		};
		enum StatusInformation_Status4
		{
			S4_Cell08Fault = (1 << 7),
			S4_Cell07Fault = (1 << 6),
			S4_Cell06Fault = (1 << 5),
			S4_Cell05Fault = (1 << 4),
			S4_Cell04Fault = (1 << 3),
			S4_Cell03Fault = (1 << 2),
			S4_Cell02Fault = (1 << 1),
			S4_Cell01Fault = (1 << 0),
		};
		enum StatusInformation_Status5
		{
			S5_Cell16Fault = (1 << 7),
			S5_Cell15Fault = (1 << 6),
			S5_Cell14Fault = (1 << 5),
			S5_Cell13Fault = (1 << 4),
			S5_Cell12Fault = (1 << 3),
			S5_Cell11Fault = (1 << 2),
			S5_Cell10Fault = (1 << 1),
			S5_Cell09Fault = (1 << 0),
		};

		static void DecodeStatus1Value(const uint8_t val, std::string& protectionText);
		static void DecodeStatus2Value(const uint8_t val, std::string& configurationText);
		static void DecodeStatus3Value(const uint8_t val, std::string& systemText);
		static void DecodeStatus4Value(const uint8_t val, std::string& faultText);
		static void DecodeStatus5Value(const uint8_t val, std::string& faultText);
	};

	// UD value 20 at offset 28
	class StatusDecode_SEPLOS
	{
	public:
		enum StatusInformation_Warning1
		{
			// pretty sure these three mean MOSFET when they say "Switch" in the doc...
			W1_CurrentLimitSwitchFailure = (1 << 7), // fault
			W1_DischaringSwitchFailure = (1 << 6), // fault
			W1_ChargingSwitchFailure = (1 << 5), // fault
			W1_CellVoltageDifferenceSensingFailure = (1 << 4), // fault
			W1_PowerSwitchFailure = (1 << 3), // fault
			W1_CurrentSensingFailure = (1 << 2), // fault
			W1_TemperatureSensingFailure = (1 << 1), // fault
			W1_VoltageSensingFailure = (1 << 0), // fault
		};
		enum StatusInformation_Warning2
		{
			W2_PackLowVoltageProtection = (1 << 7), // protection 
			W2_PackLowVoltageWarning = (1 << 6), // warning 
			W2_PackOverVoltageProtection = (1 << 5), // protection 
			W2_PackOverVoltageWarning = (1 << 4), // warning 
			W2_CellLowVoltageProtection = (1 << 3), // protection 
			W2_CellLowVoltageWarning = (1 << 2), // warning 
			W2_CellOverVoltageProtection = (1 << 1), // protection 
			W2_CellOverVoltageWarning = (1 << 0), // warning 
		};
		enum StatusInformation_Warning3
		{
			W3_DischargingLowTemperatureProtection = (1 << 7), // protection 
			W3_DischargingLowTemperatureWarning = (1 << 6), // warning
			W3_DischargingHighTemperatureProtection = (1 << 5), // protection 
			W3_DischargingHighTemperatureWarning = (1 << 4), // warning
			W3_ChargingLowTemperatureProtection = (1 << 3), // protection 
			W3_ChargingLowTemperatureWarning = (1 << 2), // warning
			W3_ChargingHighTemperatureProtection = (1 << 1), // protection 
			W3_ChargingHighTemperatureWarning = (1 << 0), // warning
		};
		enum StatusInformation_Warning4
		{
			W4_ReservedWarning4Bit8 = (1 << 7), // warning
			W4_Heating = (1 << 6), // system
			W4_ComponentHighTemperatureProtection = (1 << 5), // protection
			W4_ComponentHighTemperatureWarning = (1 << 4), // warning
			W4_AmbientLowTemperatureProtection  = (1 << 3), // protection
			W4_AmbientLowTemperatureWarning = (1 << 2), // warning
			W4_AmbientHighTemperatureProtection = (1 << 1), // protection
			W4_AmbientHighTemperatureWarning = (1 << 0), // warning
		};
		enum StatusInformation_Warning5
		{
			W5_OutputShortCircuitLock = (1 << 7), // fault 
			W5_TransientOverCurrentLock = (1 << 6), // fault
			W5_OutputShortCircuitProtection = (1 << 5), // protection 
			W5_TansientOverCurrentProtection = (1 << 4), // protection 
			W5_DischargeOverCurrentProtection = (1 << 3), // protection 
			W5_DischargeOverCurrentWarning = (1 << 2), // warning
			W5_ChargeOverCurrentProtection = (1 << 1), // protection 
			W5_ChargeOverCurrentWarning = (1 << 0), // warning
		};
		enum StatusInformation_Warning6
		{
			W6_InternalWarning6Bit8 = (1 << 7), // warning
			W6_OutputConnectionFailure = (1 << 6), // fault
			W6_OutputReverseConnectionProtection = (1 << 5), // protection
			W6_CellLowVoltageChargingForbidden = (1 << 4), // fault
			W6_RemaingCapacityProtection = (1 << 3), // protection
			W6_RemaingCapacityWarning = (1 << 2), // warning
			W6_IntermittentPowerSupplementWaiting = (1 << 1), // warning
			W6_ChargingHighVoltageProtection = (1 << 0), // protection
		};
		enum StatusInformation_PowerStatus
		{
			PS_ReservedPowerStatusBit8 = (1 << 7),
			PS_ReservedPowerStatusBit7 = (1 << 6),
			PS_ReservedPowerStatusBit6 = (1 << 5),
			PS_ReservedPowerStatusBit5 = (1 << 4),
			PS_HeatingSwitchStatus = (1 << 3),
			PS_CurrentLimitSwitchStatus = (1 << 2),
			PS_ChargeSwitchStatus = (1 << 1),
			PS_DischargeSwitchStatus = (1 << 0),
		};
		enum StatusInformation_SystemStatus
		{
			SS_ReservedSystemStatusBit8 = (1 << 7),
			SS_ReservedSystemStatusBit7 = (1 << 6),
			SS_PowerOff = (1 << 5),
			SS_Standby = (1 << 4),
			SS_ReservedSystemStatusBit4 = (1 << 3),
			SS_FloatingCharge = (1 << 2),
			SS_Charging = (1 << 1),
			SS_Discharging = (1 << 0),
		};
		enum StatusInformation_Warning7
		{
			W7_Warning7InternalBit8 = (1 << 7),
			W7_Warning7InternalBit7 = (1 << 6),
			W7_ManualChargingWait = (1 << 5),
			W7_AutoChargingWait = (1 << 4),
			W7_Warning7InternalBit4 = (1 << 3),
			W7_Warning7InternalBit3 = (1 << 2),
			W7_Warning7InternalBit2 = (1 << 1),
			W7_Warning7InternalBit1 = (1 << 0),
		};
		enum StatusInformation_Warning8
		{
			W8_Warning8InternalBit8 = (1 << 7),
			W8_Warning8InternalBit7 = (1 << 6),
			W8_Warning8InternalBit6 = (1 << 5),
			W8_NoNullPointCalibration = (1 << 4),
			W8_NoCurrentCalibration = (1 << 3),
			W8_NoVoltageCalibration = (1 << 2),
			W8_RTCFailure = (1 << 1),
			W8_EEPStorageFailure = (1 << 0),
		};

		static void DecodeWarning1Value(const uint8_t val, std::string& faultText);
		static void DecodeWarning2Value(const uint8_t val, std::string& warningText, std::string& protectionText);
		static void DecodeWarning3Value(const uint8_t val, std::string& warningText, std::string& protectionText);
		static void DecodeWarning4Value(const uint8_t val, std::string& warningText, std::string& protectionText, std::string& systemText);
		static void DecodeWarning5Value(const uint8_t val, std::string& warningText, std::string& protectionText, std::string& faultText);
		static void DecodeWarning6Value(const uint8_t val, std::string& warningText, std::string& protectionText, std::string& faultText);
		static void DecodePowerStatusValue(const uint8_t val, std::string& configurationText);
		static void DecodeSystemStatusValue(const uint8_t val, std::string& systemText);
		static void DecodeWarning7Value(const uint8_t val, std::string& warningText);
		static void DecodeWarning8Value(const uint8_t val, std::string& faultText);
	};

	// UD value 9 at offset 29
	class StatusDecode_EG4
	{
	public:
		enum StatusInformation_BalanceEvent
		{
			BE_BalanceEventReservedBit8 = (1 << 7), // warning
			BE_DischargeMosFaultAlarm = (1 << 6), // fault
			BE_ChargeMosFaultAlarm = (1 << 5), // fault
			BE_CellVoltageDifferenceAlarm = (1 << 4), // warning
			BE_BalanceEventReservedBit4 = (1 << 3), // warning
			BE_BalanceEventReservedBit3 = (1 << 2), // warning
			BE_BalanceEventReservedBit2 = (1 << 1), // warning
			BE_BalanceEventBalancingActive = (1 << 0), // warning
		};
		enum StatusInformation_VoltageEvent
		{
			VE_PackUnderVoltageProtect = (1 << 7), // protection
			VE_PackUnderVoltageAlarm = (1 << 6), // warning
			VE_PackOverVoltageProtect = (1 << 5), // protection
			VE_PackOverVoltageAlarm = (1 << 4), // warning
			VE_CellUnderVoltageProtect = (1 << 3), // protection
			VE_CellUnderVoltageAlarm = (1 << 2), // warning
			VE_CellOverVoltageProtect = (1 << 1), // protection
			VE_CellOverVoltageAlarm = (1 << 0), // warning
		};
		enum StatusInformation_TemperatureEvent
		{
			TE_TemperatureEventReservedBit16 = (1 << 15), // warning
			TE_TemperatureEventReservedBit15 = (1 << 14), // warning
			TE_FireAlarm = (1 << 13), // fault
			TE_MosfetHighTemperatureProtect = (1 << 12), // protection
			TE_EnvironmentLowTemperatureProtect = (1 << 11), // protection
			TE_EnvironmentLowTemperatureAlarm = (1 << 10), // warning
			TE_EnvironmentHighTemperatureProtect = (1 << 9), // protection
			TE_EnvironmentHighTemperatureAlarm = (1 << 8), // warning
			TE_DischargeLowTemperatureProtect = (1 << 7), // protection
			TE_DischargeLowTemperatureAlarm = (1 << 6), // warning
			TE_DischargeHighTemperatureProtect = (1 << 5), // protection
			TE_DischargeHighTemperatureAlarm = (1 << 4), // warning
			TE_ChargeLowTemperatureProtect = (1 << 3), // protection
			TE_ChargeLowTemperatureAlarm = (1 << 2), // warning
			TE_ChargeHighTemperatureProtect = (1 << 1), // protection
			TE_ChargeHighTemperatureAlarm = (1 << 0), // warning
		};
		enum StatusInformation_CurrentEvent
		{
			CE_OutputShortCircuitLockout = (1 << 7), // fault
			CE_DischargeLevel2OverCurrentLockout = (1 << 6), // fault
			CE_OutputShortCircuitProtect = (1 << 5), // protection
			CE_DischargeLevel2OverCurrentProtect = (1 << 4), // protection
			CE_DischargeOverCurrentProtect = (1 << 3), // protection
			CE_DischargeOverCurrentAlarm = (1 << 2), // warning
			CE_ChargeOverCurrentProtect = (1 << 1), // protection
			CE_ChargeOverCurrentAlarm = (1 << 0), // warning
		};
		enum StatusInformation_RemainingCapacity
		{
			RC_RemainingCapacityReservedBit8 = (1 << 7), // warning
			RC_RemainingCapacityReservedBit7 = (1 << 6), // warning
			RC_RemainingCapacityReservedBit6 = (1 << 5), // warning
			RC_RemainingCapacityReservedBit5 = (1 << 4), // warning
			RC_RemainingCapacityReservedBit4 = (1 << 3), // warning
			RC_RemainingCapacityReservedBit3 = (1 << 2), // warning
			RC_RemainingCapacityReservedBit2 = (1 << 1), // warning
			RC_StateOfChargeLow = (1 << 0), // warning
		};
		enum StatusInformation_FetStatus
		{
			FS_FetStatusReservedBit8 = (1 << 7), // configuration
			FS_FetStatusReservedBit7 = (1 << 6), // configuration
			FS_FetStatusReservedBit6 = (1 << 5), // configuration
			FS_FetStatusReservedBit5 = (1 << 4), // configuration
			FS_HeaterOn = (1 << 3), // configuration
			FS_ChargeCurrentLimiterOn = (1 << 2), // configuration
			FS_ChargeMosfetOn = (1 << 1), // configuration
			FS_DischargeMosfetOn = (1 << 0), // configuration
		};
		enum StatusInformation_SystemStatus
		{
			SS_SystemStatusReservedBit8 = (1 << 7), // system
			SS_SystemStatusReservedBit7 = (1 << 6), // system
			SS_SystemStatusReservedBit6 = (1 << 5), // system
			SS_SystemStatusReservedBit5 = (1 << 4), // system
			SS_Standby = (1 << 3), // system
			SS_SystemStatusReservedBit3 = (1 << 2), // system
			SS_Charging = (1 << 1), // system
			SS_Discharging = (1 << 0), // system
		};

		static void DecodeBalanceEvent(const uint8_t val, std::string& warningText, std::string& faultText);
		static void DecodeVoltageEvent(const uint8_t val, std::string& warningText, std::string& protectionText);
		static void DecodeTemperatureEvent(const uint16_t val, std::string& warningText, std::string& protectionText, std::string& faultText);
		static void DecodeCurrentEvent(const uint8_t val, std::string& warningText, std::string& protectionText, std::string& faultText);
		static void DecodeRemainingCapacity(const uint8_t val, std::string& warningText);
		static void DecodeFetStatus(const uint8_t val, std::string& configurationText);
		static void DecodeSystemStatus(const uint8_t val, std::string& systemText);
	};

	struct StatusInformation
	{
		std::string warningText;
		uint8_t     warning_value_cell[MAX_CELL_COUNT]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // DecodeWarningValue / enum StatusInformation_WarningValues
		uint8_t     warning_value_temp[MAX_TEMP_COUNT]{ 0, 0, 0, 0, 0, 0 }; // DecodeWarningValue / enum StatusInformation_WarningValues
		uint8_t     warning_value_charge_current{ 0 };       // DecodeWarningValue / enum StatusInformation_WarningValues
		uint8_t     warning_value_total_voltage{ 0 };        // DecodeWarningValue / enum StatusInformation_WarningValues
		uint8_t     warning_value_discharge_current{ 0 };    // DecodeWarningValue / enum StatusInformation_WarningValues

		// also used in 0x25
		uint16_t    balancing_value{ 0 };     // one bit per cell, lowest bit = cell 1
		uint8_t     system_value{ 0 };

		// PYLON
		uint8_t     status1_value{ 0 };
		uint8_t     status2_value{ 0 };
		uint8_t     status3_value{ 0 };
		uint8_t     status4_value{ 0 };
		uint8_t     status5_value{ 0 };

		// SEPLOS
		uint8_t     warning1_value{ 0 };
		uint8_t     warning2_value{ 0 };
		uint8_t     warning3_value{ 0 };
		uint8_t     warning4_value{ 0 };
		uint8_t     warning5_value{ 0 };
		uint8_t     warning6_value{ 0 };
		uint8_t     power_value{ 0 };
		uint16_t    disconnection_value{ 0 };   // one bit per cell, lowest bit = cell 1
		uint8_t     warning7_value{ 0 };
		uint8_t     warning8_value{ 0 };
		//uint16_t    balancing_value{ 0 };     // one bit per cell, lowest bit = cell 1
		//uint8_t     system_value{ 0 };

		// EG4
		uint8_t     balance_event_value;
		uint8_t     voltage_event_value;
		uint16_t    temperature_event_value;
		uint8_t     current_event_value;
		uint8_t     remaining_capacity_value;
		uint8_t     fet_status_value;
		//uint16_t    balancing_value{ 0 };     // one bit per cell, lowest bit = cell 1
        //uint8_t     system_value{ 0 };

		std::string balancingText{ "" };
		std::string systemText{ "" };
		std::string configurationText{ "" };
		std::string protectionText{ "" };
		std::string faultText{ "" };
	};

	bool CreateReadStatusInformationRequest(const uint8_t busId, std::vector<uint8_t>& request);

	// helper for: ProcessStatusInformationResponse
	const std::string DecodeWarningValue(const uint8_t val);

	bool ProcessReadStatusInformationResponse(const uint8_t busId, const std::vector<uint8_t>& response, StatusInformation& statusInformation);

protected:
	// protocol variants
	bool ProcessReadStatusInformationResponse_PYLON(const uint8_t busId, const std::vector<uint8_t>& response, StatusInformation& statusInformation);
	bool ProcessReadStatusInformationResponse_SEPLOS(const uint8_t busId, const std::vector<uint8_t>& response, StatusInformation& statusInformation);
	bool ProcessReadStatusInformationResponse_EG4(const uint8_t busId, const std::vector<uint8_t>& response, StatusInformation& statusInformation);

public:
	// ==== Read Hardware Version
	// 1 Hardware Version string with a bunch of garbage in it (spaces and non-printable) on the BMS I have
	// req:   ~20014A510000FDA2.
	// resp:  ~20014A00F05C202020202020202020202020202020202020202000005154484E2020202020202020202020202020202030640306EBA8.
	//                     11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111

	static const uint8_t exampleReadHardwareVersionRequestV20[];
	static const uint8_t exampleReadHardwareVersionResponseV20[];

	bool CreateReadHardwareVersionRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessReadHardwareVersionResponse(const uint8_t busId, const std::vector<uint8_t>& response, std::string& hardwareVersion);

	// ==== Read Serial Number
	// 1 Serial Number string (?)
	// req:   ~20014A930000FD9C.
	// resp:  (I don't have a BMS that responds to this)

	static const uint8_t exampleReadSerialNumberRequestV20[];
	static const uint8_t exampleReadSerialNumberResponseV20[];

	bool CreateReadSerialNumberRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessReadSerialNumberResponse(const uint8_t busId, const std::vector<uint8_t>& response, std::string& serialNumber);

	// ==== Shutdown (if the BMS is active charge/discharging it will immediately reboot after shutdown)
	// x: unknown payload, this may be a command code and there may be more but I'm not going to test that due to potentially unknown consequences
	// write: ~2500469CE00201FD1B.
	//                     xx
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleWriteRebootCommandRequestV20[];
	static const uint8_t exampleWriteRebootCommandResponseV20[];

	bool CreateWriteShutdownCommandRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessWriteShutdownCommandResponse(const uint8_t busId, const std::vector<uint8_t>& response);

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

	static const uint8_t exampleReadSystemTimeRequestV20[];
	static const uint8_t exampleReadSystemTimeResponseV20[];
	static const uint8_t exampleWriteSystemTimeRequestV20[];
	static const uint8_t exampleWriteSystemTimeResponseV20[];

	bool CreateReadSystemDateTimeRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessReadSystemDateTimeResponse(const uint8_t busId, const std::vector<uint8_t>& response, DateTime& dateTime);
	bool CreateWriteSystemDateTimeRequest(const uint8_t busId, const DateTime dateTime, std::vector<uint8_t>& request);
	bool ProcessWriteSystemDateTimeResponse(const uint8_t busId, const std::vector<uint8_t>& response);
};

