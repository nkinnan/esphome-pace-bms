#pragma once

#include <string>
#include <vector>

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

class PaceBmsV25
{
public:
	// dependency injection
	typedef void (*LogFuncPtr)(std::string message);

	// takes pointers to the "real" logging functions
	PaceBmsV25(LogFuncPtr logError, LogFuncPtr logWarning, LogFuncPtr logInfo, LogFuncPtr logVerbose);

private:
	// dependency injection
	LogFuncPtr LogErrorPtr;
	LogFuncPtr LogWarningPtr;
	LogFuncPtr LogInfoPtr;
	LogFuncPtr LogVerbosePtr;

	void LogError(std::string message);
	void LogWarning(std::string message);
	void LogInfo(std::string message);
	void LogVerbose(std::string message);

	enum CID1 : uint8_t
	{
		CID1_LithiumIron = 0x46,
		CID1_LithiumIon = 0x4F,  // not used by PBmsTools 2.4, but reported by someone using a knock-off, rebadged version of it on a 14s 48v pack which also exposes protocol version 0x25
	};

	enum CID2 : uint8_t
	{
		// Main "Realtime Monitoring" tab of PBmsTools 2.4
		// These are the commands sent in a loop to fill out the display
		CID2_ReadAnalogInformation = 0x42,
		CID2_ReadStatusInformation = 0x44,
		CID2_ReadHardwareVersion = 0xC1,
		CID2_ReadSerialNumber = 0xC2,

		// Main "Realtime Monitoring" tab of PBmsTools 2.4
		// These are in the "Switch Control" section
		CID2_WriteSwitchCommand = 0x99, // depending on payload, encompases "Sound Alarm", "LED Alarm", "Charge Limiter", and "Charge Limiter Gear" (which is actually on the "System Configuration" page but logically is grouped with these and uses the same CID2)
		CID2_WriteChargeMosfetSwitchCommand = 0x9A,
		CID2_WriteDischargeMosfetSwitchCommand = 0x9B,
		CID2_WriteRebootCommand = 0x9C,

		// "Memory Information" tab of PBmsTools 2.4
		CID2_ReadDateTime = 0xB1,
		CID2_WriteDateTime = 0xB2,

		// "Parameter Setting" tab of PBmsTools 2.4
		CID2_ReadCellOverVoltageConfiguration = 0xD1,
		CID2_WriteCellOverVoltageConfiguration = 0xD0,
		CID2_ReadPackOverVoltageConfiguration = 0xD5,
		CID2_WritePackOverVoltageConfiguration = 0xD4,
		CID2_ReadCellUnderVoltageConfiguration = 0xD3,
		CID2_WriteCellUnderVoltageConfiguration = 0xD2,
		CID2_ReadPackUnderVoltageConfiguration = 0xD7,
		CID2_WritePackUnderVoltageConfiguration = 0xD6,
		CID2_ReadChargeOverCurrentConfiguration = 0xD9,
		CID2_WriteChargeOverCurrentConfiguration = 0xD8,
		CID2_ReadDischargeSlowOverCurrentConfiguration = 0xDB,
		CID2_WriteDischargeSlowOverCurrentConfiguration = 0xDA,
		CID2_ReadDischargeFastOverCurrentConfiguration = 0xE3,
		CID2_WriteDischargeFastOverCurrentConfiguration = 0xE2,
		CID2_ReadShortCircuitProtectionConfiguration = 0xE5,
		CID2_WriteShortCircuitProtectionConfiguration = 0xE4,
		CID2_ReadCellBalancingConfiguration = 0xB6,
		CID2_WriteCellBalancingConfiguration = 0xB5,
		CID2_ReadSleepConfiguration = 0xA0,
		CID2_WriteSleepConfiguration = 0xA8,
		CID2_ReadFullChargeLowChargeConfiguration = 0xAF,
		CID2_WriteFullChargeLowChargeConfiguration = 0xAE,
		CID2_ReadChargeAndDischargeOverTemperatureConfiguration = 0xDD,
		CID2_WriteChargeAndDischargeOverTemperatureConfiguration = 0xDC,
		CID2_ReadChargeAndDischargeUnderTemperatureConfiguration = 0xDF,
		CID2_WriteChargeAndDischargeUnderTemperatureConfiguration = 0xDE,

		// "System Configuration" tab of PBmsTools 2.4
		CID2_ReadChargeCurrentLimiterStartCurrent = 0xED,
		CID2_WriteChargeCurrentLimiterStartCurrent = 0xEE,
		CID2_ReadRemainingCapacity = 0xA6,
	};

	std::string FormatReturnCode(const uint8_t returnCode);

	// Takes a length value and adds a checksum to the upper nibble, this is "CKLEN" used in command or response headers
	uint16_t CreateChecksummedLength(const uint16_t cklen);

	// Checks if the checksum nibble in a "checksummed length" is valid
	bool ValidateChecksummedLength(const uint16_t cklen);

	// Length is just the lower 12 bits of the checksummed length 
	uint16_t LengthFromChecksummedLength(const uint16_t cklen);

	// Calculates the checksum for an entire request or response "packet" (this is not for the embedded length value)
	uint16_t CalculateRequestOrResponseChecksum(const std::vector<uint8_t>& data);

	// helper for WriteHexEncoded----
	// Works with ASCII encoding, not portable, but that's what the protocol uses
	uint8_t NibbleToHex(const uint8_t nibbleByte);

	// todo: error check input range and log error and/or handle upstream to abort decode
	// helper for ReadHexEncoded----
	// Works with ASCII encoding, not portable, but that's what the protocol uses
	uint8_t HexToNibble(const uint8_t hex);

	// decode a 'real' byte from the stream by reading two ASCII hex encoded bytes
	uint8_t ReadHexEncodedByte(const std::vector<uint8_t>& data, uint16_t* dataOffset);

	// decode a 'real' uint16_t from the stream by reading four ASCII hex encoded bytes
	uint16_t ReadHexEncodedUShort(const std::vector<uint8_t>& data, uint16_t* dataOffset);

	// decode a 'real' int16_t from the stream by reading four ASCII hex encoded bytes
	int16_t ReadHexEncodedSShort(const std::vector<uint8_t>& data, uint16_t* dataOffset);

	// encode a 'real' byte to the stream by writing two ASCII hex encoded bytes
	void WriteHexEncodedByte(std::vector<uint8_t>& data, uint16_t* dataOffset, uint8_t byte);

	// encode a 'real' uint16_t to the stream by writing four ASCII hex encoded bytes
	void WriteHexEncodedUShort(std::vector<uint8_t>& data, uint16_t* dataOffset, uint16_t ushort);

	// encode a 'real' int16_t to the stream by writing four ASCII hex encoded bytes
	void WriteHexEncodedSShort(std::vector<uint8_t>& data, uint16_t* dataOffset, int16_t sshort);

	// create a standard request to the given busId for the given CID2, filling in the payload (if given)
	void CreateRequest(const uint8_t busId, const CID2 cid2, const std::vector<uint8_t> payload, std::vector<uint8_t>& request);

	// validate all fields in the response except the payload data: SOI marker, header values, checksum, EOI marker
	// returns the detected payload length (payload always starts at offset 13), or -1 for error
	int16_t ValidateResponseAndGetPayloadLength(const uint8_t busId, const std::vector<uint8_t> response);

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
		uint8_t cellCount;
		uint16_t cellVoltagesMillivolts[MAX_CELL_COUNT];
		uint8_t tempCount;
		int16_t temperaturesTenthsCelcius[MAX_TEMP_COUNT]; // first 4 are Cell readings, then MOSFET then Environment
		int32_t currentMilliamps; 
		uint16_t totalVoltageMillivolts;
		uint32_t remainingCapacityMilliampHours;
		uint32_t fullCapacityMilliampHours;
		uint16_t cycleCount;
		uint32_t designCapacityMilliampHours;
		float SoC; // in percent
		float SoH; // in percent
		float powerWatts;
		uint16_t minCellVoltageMillivolts;
		uint16_t maxCellVoltageMillivolts;
		uint16_t avgCellVoltageMillivolts;
		uint16_t maxCellDifferentialMillivolts;
	};

	void CreateReadAnalogInformationRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessReadAnalogInformationResponse(const uint8_t busId, const std::vector<uint8_t>& response, AnalogInformation& analogInformation);

	// ==== Read Status Information
	// 0 Responding Bus Id
	// 1 Cell Count (this example has 16 cells)
	// 2 Cell Warning (repeated Cell Count times) see: DecodeWarningValue
	// 3 Temperature Count (this example has 6 temperatures)
	// 4 Temperature Warning (repeated Temperature Count times) see: DecodeWarningValue
	// 5 Charge Current Warning see: DecodeWarningValue
	// 6 Total Voltage Warning see: DecodeWarningValue
	// 7 Discharge Current Warning see: DecodeWarningValue
	// 8 Protection Status 1 see: DecodeProtectionStatus1Value
	// 9 Protection Status 2 see: DecodeProtectionStatus2Value
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

	void CreateReadStatusInformationRequest(const uint8_t busId, std::vector<uint8_t>& request);

private:
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
	bool ProcessReadStatusInformationResponse(const uint8_t busId, const std::vector<uint8_t>& response, std::string& warningText, std::string& balancingText, std::string& systemText, std::string& configurationText, std::string& protectionText, std::string& faultText);

	// ==== Read Hardware Version
	// 1 Hardware Version string (may be ' ' padded at the end), the length header value will tell you how long it is, should be 20 'actual character' bytes (40 ASCII hex chars)
	// req:   ~250146C10000FD9A.
	// resp:  ~25014600602850313653313030412D313831322D312E30302000F58E.
	//                     1111111111111111111111111111111111111111

	static const uint8_t exampleReadHardwareVersionRequestV25[];
	static const uint8_t exampleReadHardwareVersionResponseV25[];

	void CreateReadHardwareVersionRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessReadHardwareVersionResponse(const uint8_t busId, const std::vector<uint8_t>& response, std::string& hardwareVersion);

	// ==== Read Serial Number
	// 1 Serial Number string (may be ' ' padded at the end), the length header value will tell you how long it is, should be 20 or 40 'actual character' bytes (40 or 80 ASCII hex chars)
	// req:   ~250146C20000FD99.
	// resp:  ~25014600B05031383132313031333830333039442020202020202020202020202020202020202020202020202020EE0F.
	//                     11111111111111111111111111111111111111111111111111111111111111111111111111111111

	static const uint8_t exampleReadSerialNumberRequestV25[];
	static const uint8_t exampleReadSerialNumberResponseV25[];

	void CreateReadSerialNumberRequest(const uint8_t busId, std::vector<uint8_t>& request);
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
		SC_DisableBuzzer = 0x0D,
		SC_EnableBuzzer = 0x0C,

		// for LED, the documentation and the "on the wire" data from PBmsTools conflict, this is on the wire, documentation is the reverse of this
		SC_DisableLedWarning = 0x06,
		SC_EnableLedWarning = 0x07,

		// for ChargeCurrentLimiter, the documentation and the "on the wire" data from PBmsTools conflict, this is on the wire, documentation is the reverse of this
		SC_DisableChargeCurrentLimiter = 0x0A,
		SC_EnableChargeCurrentLimiter = 0x0B,

		SC_SetChargeCurrentLimiterCurrentLimitHighGear = 0x08,
		SC_SetChargeCurrentLimiterCurrentLimitLowGear = 0x09,
	};

	void CreateWriteSwitchCommandRequest(const uint8_t busId, const SwitchCommand command, std::vector<uint8_t>& request);
	bool ProcessWriteSwitchCommandResponse(const uint8_t busId, const SwitchCommand command, const std::vector<uint8_t>& response);

	// ==== Charge MOSFET Switch
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
		MS_Open = 0x00,
		MS_Close = 0x01
	};

	void CreateWriteMosfetSwitchCommandRequest(const uint8_t busId, const MosfetType type, const MosfetState command, std::vector<uint8_t>& request);
	bool ProcessWriteMosfetSwitchCommandResponse(const uint8_t busId, const MosfetType type, const MosfetState command, const std::vector<uint8_t>& response);

	// ==== Reboot (labeled as "Shutdown" in PBmsTools, but it actually causes a reboot in my experience)
	// x: unknown payload, this may be a command code and there may be more but I'm not going to test that due to potentially unknown consequences
	// write: ~2500469CE00201FD1B.
	//                     xx
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleWriteRebootCommandRequestV25[];
	static const uint8_t exampleWriteRebootCommandResponseV25[];

	void CreateWriteRebootCommandRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessWriteRebootCommandResponse(const uint8_t busId, const std::vector<uint8_t>& response);


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
	// 1 Year:   read: 2024 write: 2024 (add 2000)
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

	struct DateTime
	{
		uint16_t Year;
		uint8_t Month;
		uint8_t Day;
		uint8_t Hour;
		uint8_t Minute;
		uint8_t Second;
	};

	void CreateReadSystemTimeRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessReadSystemTimeResponse(const uint8_t busId, const std::vector<uint8_t>& response, DateTime& dateTime);
	bool CreateWriteSystemTimeRequest(const uint8_t busId, const DateTime dateTime, std::vector<uint8_t>& request);
	bool ProcessWriteSystemTimeResponse(const uint8_t busId, const std::vector<uint8_t>& response);



	// ============================================================================
	// 
	// "Parameter Setting" tab of PBmsTools 2.4
	// 
	// ============================================================================

	enum ReadConfigurationType {
		RC_CellOverVoltage = CID2_ReadCellOverVoltageConfiguration,
		RC_PackOverVoltage = CID2_ReadPackOverVoltageConfiguration,
		RC_CellUnderVoltage = CID2_ReadCellUnderVoltageConfiguration,
		RC_PackUnderVoltage = CID2_ReadPackUnderVoltageConfiguration,
		RC_ChargeOverCurrent = CID2_ReadChargeOverCurrentConfiguration,
		RC_DischargeSlowOverCurrent = CID2_ReadDischargeSlowOverCurrentConfiguration,
		RC_DischargeFastOverCurrent = CID2_ReadDischargeFastOverCurrentConfiguration,
		RC_ShortCircuitProtection = CID2_ReadShortCircuitProtectionConfiguration,
		RC_CellBalancing = CID2_ReadCellBalancingConfiguration,
		RC_Sleep = CID2_ReadSleepConfiguration,
		RC_FullChargeLowCharge = CID2_ReadFullChargeLowChargeConfiguration,
		RC_ChargeAndDischargeOverTemperature = CID2_ReadChargeAndDischargeOverTemperatureConfiguration,
		RC_ChargeAndDischargeUnderTemperature = CID2_ReadChargeAndDischargeUnderTemperatureConfiguration,
	};

	void CreateReadConfigurationRequest(const uint8_t busId, const ReadConfigurationType configType, std::vector<uint8_t>& request);
	bool ProcessWriteConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response);

	// ==== Cell Over Voltage Configuration
	// 1 Cell OV Alarm (V): 3.60 - stored as v * 1000, so 3.6 is 3600 - valid range reported by PBmsTools as 2.5-4.5 in steps of 0.01
	// 2 Cell OV Protect (V): 3.70 - stored as v * 1000, so 3.7 is 3700 - valid range reported by PBmsTools as 2.5-4.5 in steps of 0.01
	// 3 Cell OVP Release (V): 3.38  - stored as v * 1000, so 3.38 is 3380 - valid range reported by PBmsTools as 2.5-4.5 in steps of 0.01
	// 4 Cell OVP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 1000 to 5000 in steps of 500
	// read:  ~250046D10000FD9A.
	// resp:  ~25004600F010010E100E740D340AFA35.
	//                     ??11112222333344 
	// write: ~250046D0F010010E100E740D340AFA21.
	// resp:  ~250046000000FDAF.

	// todo: everywhere ?? = busid in comments
	// todo: everywhere readResponse and writeRequest in parameters list

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
	// 4 Pack OVP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 1000 to 5000 in steps of 500
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
	// 4 Cell UVP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 1000 to 5000 in steps of 500
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
	// 4 Pack UVP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 1000 to 5000 in steps of 500
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
	// 1 Charge OC Alarm (A): 104 - stored directly in amps - valid range reported by PBmsTools as 1-150
	// 2 Charge OC Protect (A): 110 - stored directly in amps - valid range reported by PBmsTools as 1-150
	// 3 Charge OCP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 500 to 10000 in steps of 500
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

	// ==== Discharge SLOW Over Current Configuration
	// 1 Discharge OC Alarm (A): 105 - stored as negative two's complement in amps***, -105 is FF97 - valid range reported by PBmsTools as 1-150
	// 2 Discharge OC 1 Protect (A): 110 - stored as negative two's complement in amps***, -110 is FF92 - valid range reported by PBmsTools as 1-150
	// 3 Discharge OC 1 Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 500 to 10000 in steps of 500
	// ********* important *********: this is returned as the negative two's complement, but is STORED (written back) as the normal positive value!
	// read:  ~250046DB0000FD89.
	// resp:  ~25004600400C01FF97FF920AFAD3.
	//                     ??1111222233
	// write: ~250046DA400C010069006E0AFAF7.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadDishargeSlowOverCurrentConfigurationRequestV25[];
	static const uint8_t exampleReadDishargeSlowOverCurrentConfigurationResponseV25[];
	static const uint8_t exampleWriteDishargeSlowOverCurrentConfigurationRequestV25[];
	static const uint8_t exampleWriteDishargeSlowOverCurrentConfigurationResponseV25[];

	struct DishargeSlowOverCurrentConfiguration
	{
		uint16_t AlarmAmperage;
		uint16_t ProtectionAmperage;
		uint16_t ProtectionDelayMilliseconds;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, DishargeSlowOverCurrentConfiguration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const DishargeSlowOverCurrentConfiguration& config, std::vector<uint8_t>& request);

	// ==== Dicharge FAST Over Current Configuration
	// 1 Discharge OC 2 Protect: 150 - stored directly in amps - valid range reported by PBmsTools as 5-300 in steps of 5, but since this is an 8 bit store location, the actual max is 255????????
	// 2 Discharge OC 2 Delay Time (ms): 100 - stored in 25ms steps, so 100 is 4 (4x25=100), 400 is 16 (16x25=400) - valid range reported by PBmsTools as 100-2000 in steps of 100
	// x = apparently, garbage written by the firmware - it's not included in the PBmsTools write
	// read:  ~250046E30000FD97.
	// resp:  ~25004600400C009604009604FB32.
	//                     ??1122xxxxxx
	// write: ~250046E2A006009604FC4E.
	// resp:  ~250046000000FDAF.

	static const uint8_t exampleReadDishargeFastOverCurrentConfigurationRequestV25[];
	static const uint8_t exampleReadDishargeFastOverCurrentConfigurationResponseV25[];
	static const uint8_t exampleWriteDishargeFastOverCurrentConfigurationRequestV25[];
	static const uint8_t exampleWriteDishargeFastOverCurrentConfigurationResponseV25[];

	struct DishargeFastOverCurrentConfiguration
	{
		uint8_t ProtectionAmperage;
		uint16_t ProtectionDelayMilliseconds;
	};

	bool ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, DishargeFastOverCurrentConfiguration& config);
	bool CreateWriteConfigurationRequest(const uint8_t busId, const DishargeFastOverCurrentConfiguration& config, std::vector<uint8_t>& request);

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
	// 3 State of Charge Low Alarm (%): 5 - stored directly - valid range reported by PBmsTools as 1-100
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

	// ????????? mystery ????????? this is sent during "Parameter Setting" tab Read/Write All but does not correspond to ANY value that I can find (and certainly not in the Parameter Setting tab) displayed in PBmsTools
	// read:  ~250046E10000FD99.
	// resp:  ~25004600200E010E2E0EF60DFCFA5D.
	//                     ??????????????
	// write: ~250046E0200E010E2E0EF60DFCFA48.
	// resp:  ~250046000000FDAF.

	// ????????? mystery ????????? this is sent during "Parameter Setting" tab Read/Write All but does not correspond to ANY value that I can find (and certainly not in the Parameter Setting tab) displayed in PBmsTools
	// read:  ~250046E70000FD93.
	// resp:  ~25004600501A0109E209B009E20D340D660D34F806.
	//                     ??????????????????????????
	// write: ~250046E6501A0109E209B009E20D340D660D34F7EB.
	// resp:  ~250046000000FDAF.



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

	void CreateReadChargeCurrentLimiterStartCurrentRequest(const uint8_t busId, std::vector<uint8_t>& request);
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

	void CreateReadRemainingCapacityRequest(const uint8_t busId, std::vector<uint8_t>& request);
	bool ProcessReadRemainingCapacityResponse(const uint8_t busId, const std::vector<uint8_t>& response, uint32_t& remainingCapacityMilliampHours, uint32_t& actualCapacityMilliampHours, uint32_t& designCapacityMilliampHours);


	// There are many other settings in "System Configuration" that can be written and/or calibrated here, 
	// none of which I am exposing because it would be a Very Bad Idea to mess with them


};

