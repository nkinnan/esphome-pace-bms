
#include "pace_bms_protocol_v20.h"

// takes pointers to the "real" logging functions
PaceBmsProtocolV20::PaceBmsProtocolV20(
	OPTIONAL_NS::optional<std::string> protocol_variant, OPTIONAL_NS::optional<uint8_t> protocol_version_override, OPTIONAL_NS::optional<uint8_t> batteryChemistry,
	LogFuncPtr logError, LogFuncPtr logWarning, LogFuncPtr logInfo, LogFuncPtr logDebug, LogFuncPtr logVerbose, LogFuncPtr logVeryVerbose) :
	PaceBmsProtocolBase(
		0x20, protocol_variant, protocol_version_override, batteryChemistry,
		logError, logWarning, logInfo, logDebug, logVerbose, logVeryVerbose)
{
}

// EG4 variant
const unsigned char PaceBmsProtocolV20::exampleReadAnalogInformationRequestV20[] = "~20014A420000FDA2\r";
const unsigned char PaceBmsProtocolV20::exampleReadAnalogInformationResponseV20[] = "~20014A00A0CA1001100D2F0D2C0D2C0D2D0D2D0D2F0D2F0D2F0D2C0D2D0D2D0D2F0D300D2C0D300D2C040B9B0BA50B9B0B9B0BB90BAF029D151521A9268400540F005700620D300D2C00040BA50B9B000ADAC0000A54550005D473000570A600000680000004CA56897E24D1A5\r";

// seplos?
// 0x200046001096 0001100CD70CE90CF40CD60CEF0CE50CE10CDC0CE90CF00CE80CEF0CEA0CDA0CDE0CD8060BA60BA00B970BA60BA50BA2FD5C14A0344E0A426803134650004603E8149F0000000000000000
// pylon?
// 0x260046003076 00011000000000000000000000000000000000000000000000000000000000000000000608530853085308530BAC0B9000000000002D0213880001E6B8

bool PaceBmsProtocolV20::CreateReadAnalogInformationRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	// the payload is the requested busId (could be FF for "get all" when speaking to a set of daisy-chained units but this code doesn't support that)
	if (protocol_variant.has_value() && 
		protocol_variant.value() != "EG4") 
	{
		const uint16_t payloadLen = 2;
		std::vector<uint8_t> payload(payloadLen);
		uint16_t payloadOffset = 0;
		WriteHexEncodedByte(payload, payloadOffset, busId);

		CreateRequest(busId, CID2_ReadAnalogInformation, payload, request);
	}
	else
		// EG4 variant does not send address in payload
		CreateRequest(busId, CID2_ReadAnalogInformation, std::vector<uint8_t>(), request);

	return true;
}
bool PaceBmsProtocolV20::ProcessReadAnalogInformationResponse(const uint8_t busId, const std::vector<uint8_t>& response, AnalogInformation& analogInformation)
{
	// save in order compare against what ProcessReadStatusInformationResponse sussed out
	OPTIONAL_NS::optional<std::string> previously_detected_variant = OPTIONAL_NS::optional<std::string>(detected_variant.value());

	// try to auto-detect the protocol variant
	//if (!detected_variant.has_value()) 
	{
		uint16_t byteOffset;

		bool isEG4 = false;
		byteOffset = 13 + 116;
		if (response.size() - byteOffset > 2)
		{
			uint8_t byte = ReadHexEncodedByte(response, byteOffset);
			if (byte == 15)
				isEG4 = true;
		}

		bool isPylon = false;
		byteOffset = 13 + 106;
		if (response.size() - byteOffset > 2)
		{
			uint8_t byte = ReadHexEncodedByte(response, byteOffset);
			if (byte == 02)
				isEG4 = true;
		}

		bool isSeplos = false;
		byteOffset = 13 + 106;
		if (response.size() - byteOffset > 2)
		{
			uint8_t byte = ReadHexEncodedByte(response, byteOffset);
			if (byte == 10)
				isSeplos = true;
		}

		if (isPylon)
		{
			detected_variant = OPTIONAL_NS::optional<std::string>("PYLON");
			LogInfo("Detected protocol variant: PYLON");
		}
		if (isSeplos)
		{
			detected_variant = OPTIONAL_NS::optional<std::string>("SEPLOS");
			LogInfo("Detected protocol variant: SEPLOS");
		}
		if (isEG4)
		{
			detected_variant = OPTIONAL_NS::optional<std::string>("EG4");
			LogInfo("Detected protocol variant: EG4");
		}

		int detected_count = 0;
		if (isPylon) detected_count++;
		if (isSeplos) detected_count++;
		if (isEG4) detected_count++;

		if (detected_count > 1)
		{
			// conflict, unable to decide
			LogWarning("Multiple matches on protocol variant auto-detect, unable to narrow down");
			detected_variant = {};
		}
	}

	// does detected variant conflict with configured variant?
	if (protocol_variant.has_value() && detected_variant.has_value() &&
		protocol_variant.value() != detected_variant.value())
	{
		LogWarning("Auto-detected protocol variant '" + detected_variant.value() + "' does not match configured protocol variant '" + protocol_variant.value() + "', using configured value.");
	}

	// does detected variant conflict with what ProcessReadAnalogInformationResponse detected?
	if (previously_detected_variant.has_value() && detected_variant.has_value() &&
		previously_detected_variant.value() != detected_variant.value())
	{
		LogWarning("Auto-detected protocol variant '" + detected_variant.value() + "' does not match previously detected protocol variant '" + protocol_variant.value() + "' determined via a different method, using newly detected value.");
	}

	// decide what variant to use
	std::string variant_to_use;
	if (protocol_variant.has_value())
		variant_to_use = protocol_variant.value();
	else if (detected_variant.has_value())
		variant_to_use = detected_variant.value();
	else
	{
		// have to fallback to something
		LogWarning("Protocol variant not configured, unable to auto-detect, defaulting to EG4");
		variant_to_use = "EG4";
	}

	// fan-out to variant handlers
	if (variant_to_use == "PYLON")
	{
		return ProcessReadAnalogInformationResponse_PYLON(busId, response, analogInformation);
	}
	else if (variant_to_use == "SEPLOS")
	{
		return ProcessReadAnalogInformationResponse_SEPLOS(busId, response, analogInformation);
	}
	else if (variant_to_use == "EG4")
	{
		return ProcessReadAnalogInformationResponse_EG4(busId, response, analogInformation);
	}
	else
	{
		LogError("Invalid protocol variant '" + variant_to_use + "'");
		return false;
	}
}

bool PaceBmsProtocolV20::ProcessReadAnalogInformationResponse_PYLON(const uint8_t busId, const std::vector<uint8_t>& response, AnalogInformation& analogInformation)
{
	//std::memset(&analogInformation, 0, sizeof(AnalogInformation));

	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	// spec says "infoflag" preceeds the response data but doesn't explain what that is, I'm assuming the example above is pylon, which means this is present
	uint8_t data_flag = ReadHexEncodedByte(response, byteOffset);
	//if (data_flag != 0)
	//	LogWarning("data_flag unexpected value");

	uint8_t pack_count = ReadHexEncodedByte(response, byteOffset);
	if (pack_count != 01)
		LogWarning("response contains data from more than one pack");

	analogInformation.cellCount = ReadHexEncodedByte(response, byteOffset);
	if (analogInformation.cellCount > MAX_CELL_COUNT)
		LogWarning("Response contains more cell voltage readings than are supported, results will be truncated");
	for (int i = 0; i < analogInformation.cellCount; i++)
	{
		uint16_t cellVoltage = ReadHexEncodedUShort(response, byteOffset);
		if (i > MAX_CELL_COUNT - 1)
			continue;
		analogInformation.cellVoltagesMillivolts[i] = cellVoltage;
	}

	analogInformation.temperatureCount = ReadHexEncodedByte(response, byteOffset);
	if (analogInformation.temperatureCount > MAX_TEMP_COUNT)
		LogWarning("Response contains more temperature readings than are supported, results will be truncated");
	for (int i = 0; i < analogInformation.temperatureCount; i++)
	{
		uint16_t temperature = ReadHexEncodedUShort(response, byteOffset);
		if (i > MAX_TEMP_COUNT - 1)
			continue;
		analogInformation.temperaturesTenthsCelcius[i] = (temperature - 2730);
	}

	analogInformation.currentMilliamps = ReadHexEncodedSShort(response, byteOffset) * 10;
	analogInformation.totalVoltageMillivolts = ReadHexEncodedUShort(response, byteOffset) * 10;
	analogInformation.remainingCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset) * 10;

	uint8_t UD2 = ReadHexEncodedByte(response, byteOffset);
	if (UD2 != 2)
		LogWarning("Response contains a constant with an unexpected value, this may be an incorrect protocol variant");

	analogInformation.fullCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset) * 10;
	analogInformation.cycleCount = ReadHexEncodedUShort(response, byteOffset);

	if (byteOffset != payloadLen + 13)
		LogWarning("Length mismatch reading analog information response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off");

	// calculate some "extras"
	analogInformation.SoC = ((float)analogInformation.remainingCapacityMilliampHours / (float)analogInformation.fullCapacityMilliampHours);
	// SoH not possible without design capacity information
	// todo: allow user specified design capacity override?
	analogInformation.powerWatts = ((float)analogInformation.totalVoltageMillivolts * (float)analogInformation.currentMilliamps) / 1000000.0f;
	analogInformation.minCellVoltageMillivolts = 65535;
	analogInformation.maxCellVoltageMillivolts = 0;
	analogInformation.avgCellVoltageMillivolts = 0;
	for (int i = 0; i < analogInformation.cellCount; i++)
	{
		if (analogInformation.cellVoltagesMillivolts[i] > analogInformation.maxCellVoltageMillivolts)
			analogInformation.maxCellVoltageMillivolts = analogInformation.cellVoltagesMillivolts[i];
		if (analogInformation.cellVoltagesMillivolts[i] < analogInformation.minCellVoltageMillivolts)
			analogInformation.minCellVoltageMillivolts = analogInformation.cellVoltagesMillivolts[i];
		analogInformation.avgCellVoltageMillivolts += analogInformation.cellVoltagesMillivolts[i];
	}
	analogInformation.avgCellVoltageMillivolts /= analogInformation.cellCount;
	analogInformation.maxCellDifferentialMillivolts = analogInformation.maxCellVoltageMillivolts - analogInformation.minCellVoltageMillivolts;

	return true;
}
bool PaceBmsProtocolV20::ProcessReadAnalogInformationResponse_SEPLOS(const uint8_t busId, const std::vector<uint8_t>& response, AnalogInformation& analogInformation)
{
	//std::memset(&analogInformation, 0, sizeof(AnalogInformation));

	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	uint8_t data_flag = ReadHexEncodedByte(response, byteOffset);
	//if (data_flag != 0)
	//	LogWarning("data_flag unexpected value");

	uint8_t pack_group = ReadHexEncodedByte(response, byteOffset);
	if (pack_group != busId)
		LogWarning("response from wrong bus id");

	analogInformation.cellCount = ReadHexEncodedByte(response, byteOffset);
	if (analogInformation.cellCount > MAX_CELL_COUNT)
		LogWarning("Response contains more cell voltage readings than are supported, results will be truncated");
	for (int i = 0; i < analogInformation.cellCount; i++)
	{
		uint16_t cellVoltage = ReadHexEncodedUShort(response, byteOffset);
		if (i > MAX_CELL_COUNT - 1)
			continue;
		analogInformation.cellVoltagesMillivolts[i] = cellVoltage;
	}

	analogInformation.temperatureCount = ReadHexEncodedByte(response, byteOffset);
	if (analogInformation.temperatureCount > MAX_TEMP_COUNT)
		LogWarning("Response contains more temperature readings than are supported, results will be truncated");
	for (int i = 0; i < analogInformation.temperatureCount; i++)
	{
		uint16_t temperature = ReadHexEncodedUShort(response, byteOffset);
		if (i > MAX_TEMP_COUNT - 1)
			continue;
		analogInformation.temperaturesTenthsCelcius[i] = (temperature - 2730);
	}

	analogInformation.currentMilliamps = ReadHexEncodedSShort(response, byteOffset) * 10;
	analogInformation.totalVoltageMillivolts = ReadHexEncodedUShort(response, byteOffset) * 10;
	analogInformation.remainingCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset) * 10;

	uint8_t UD10 = ReadHexEncodedByte(response, byteOffset);
	if (UD10 != 10)
		LogWarning("Response contains a constant with an unexpected value, this may be an incorrect protocol variant");

	analogInformation.fullCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset) * 10;
	analogInformation.SoC = ReadHexEncodedUShort(response, byteOffset);
	analogInformation.designCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset);
	analogInformation.cycleCount = ReadHexEncodedUShort(response, byteOffset);
	analogInformation.SoH = ReadHexEncodedUShort(response, byteOffset);

	//todo: expose? what even is it?
	uint16_t portVoltageMillivolts = ReadHexEncodedUShort(response, byteOffset) * 10;

	// reserved 1-4 (16 bit each, two hex per byte encoding)
	byteOffset += 16;

	if (byteOffset != payloadLen + 13)
		LogWarning("Length mismatch reading analog information response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off");

	// calculate some "extras"
	analogInformation.powerWatts = ((float)analogInformation.totalVoltageMillivolts * (float)analogInformation.currentMilliamps) / 1000000.0f;
	analogInformation.minCellVoltageMillivolts = 65535;
	analogInformation.maxCellVoltageMillivolts = 0;
	analogInformation.avgCellVoltageMillivolts = 0;
	for (int i = 0; i < analogInformation.cellCount; i++)
	{
		if (analogInformation.cellVoltagesMillivolts[i] > analogInformation.maxCellVoltageMillivolts)
			analogInformation.maxCellVoltageMillivolts = analogInformation.cellVoltagesMillivolts[i];
		if (analogInformation.cellVoltagesMillivolts[i] < analogInformation.minCellVoltageMillivolts)
			analogInformation.minCellVoltageMillivolts = analogInformation.cellVoltagesMillivolts[i];
		analogInformation.avgCellVoltageMillivolts += analogInformation.cellVoltagesMillivolts[i];
	}
	analogInformation.avgCellVoltageMillivolts /= analogInformation.cellCount;
	analogInformation.maxCellDifferentialMillivolts = analogInformation.maxCellVoltageMillivolts - analogInformation.minCellVoltageMillivolts;

	return true;
}
bool PaceBmsProtocolV20::ProcessReadAnalogInformationResponse_EG4(const uint8_t busId, const std::vector<uint8_t>& response, AnalogInformation& analogInformation)
{
	//std::memset(&analogInformation, 0, sizeof(AnalogInformation));

	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	uint8_t data_flag = ReadHexEncodedByte(response, byteOffset);
	//if (data_flag != 16)
	//	LogWarning("data_flag unexpected value");

	uint8_t pack_group = ReadHexEncodedByte(response, byteOffset);
	if (pack_group != busId)
		LogWarning("response from wrong bus id");

	analogInformation.cellCount = ReadHexEncodedByte(response, byteOffset);
	if (analogInformation.cellCount > MAX_CELL_COUNT)
		LogWarning("Response contains more cell voltage readings than are supported, results will be truncated");
	for (int i = 0; i < analogInformation.cellCount; i++)
	{
		uint16_t cellVoltage = ReadHexEncodedUShort(response, byteOffset);
		if (i > MAX_CELL_COUNT - 1)
			continue;
		analogInformation.cellVoltagesMillivolts[i] = cellVoltage;
	}

	analogInformation.temperatureCount = ReadHexEncodedByte(response, byteOffset);
	if (analogInformation.temperatureCount > MAX_TEMP_COUNT)
		LogWarning("Response contains more temperature readings than are supported, results will be truncated");
	// EG4 protocol specifies 4 temperatures and then environment and mosfet temps come next, which is the same thing as 6 temperatures
	if (analogInformation.temperatureCount == 4)
		analogInformation.temperatureCount = 6;
	for (int i = 0; i < analogInformation.temperatureCount; i++)
	{
		uint16_t temperature = ReadHexEncodedUShort(response, byteOffset);
		if (i > MAX_TEMP_COUNT - 1)
			continue;
		analogInformation.temperaturesTenthsCelcius[i] = (temperature - 2730);
	}

	analogInformation.currentMilliamps = ReadHexEncodedSShort(response, byteOffset) * 10;
	analogInformation.totalVoltageMillivolts = ReadHexEncodedUShort(response, byteOffset) * 10;
	analogInformation.remainingCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset) * 10;
	analogInformation.fullCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset) * 10;
	analogInformation.cycleCount = ReadHexEncodedUShort(response, byteOffset);

	uint8_t UD15 = ReadHexEncodedByte(response, byteOffset);
	if (UD15 != 15)
		LogWarning("Response contains a constant with an unexpected value '" + std::to_string(UD15) + "', this may be an incorrect protocol variant");

	analogInformation.SoC = ReadHexEncodedUShort(response, byteOffset);
	analogInformation.SoH = ReadHexEncodedUShort(response, byteOffset);

	analogInformation.maxCellVoltageMillivolts = ReadHexEncodedUShort(response, byteOffset);
	analogInformation.minCellVoltageMillivolts = ReadHexEncodedUShort(response, byteOffset);
	analogInformation.maxCellDifferentialMillivolts = ReadHexEncodedUShort(response, byteOffset);

	// todo: expose this extra information
	uint16_t maxCellTempTenthsCelcius = ReadHexEncodedUShort(response, byteOffset) - 2730;
	uint16_t minCellTempTenthsCelcius = ReadHexEncodedUShort(response, byteOffset) - 2730;

	uint32_t cumulativeChargedMilliampHours = ReadHexEncodedULong(response, byteOffset) * 10;
	uint32_t cumulativeDischargedMilliampHours = ReadHexEncodedULong(response, byteOffset) * 10;

	uint32_t cumulativeChargedWattHours = ReadHexEncodedULong(response, byteOffset);
	uint32_t cumulativeDischargedWattHours = ReadHexEncodedULong(response, byteOffset);

	uint32_t cumulativeChargeTimeHours = ReadHexEncodedULong(response, byteOffset);
	uint32_t cumulativeDischargeTimeHours = ReadHexEncodedULong(response, byteOffset);

	uint16_t cumulativeChargeOccurences = ReadHexEncodedUShort(response, byteOffset);
	uint16_t cumulativeDischargeOccurences = ReadHexEncodedUShort(response, byteOffset);

	if (byteOffset != payloadLen + 13)
		LogWarning("Length mismatch reading analog information response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off");

	// calculate some "extras"
	analogInformation.powerWatts = ((float)analogInformation.totalVoltageMillivolts * (float)analogInformation.currentMilliamps) / 1000000.0f;
	for (int i = 0; i < analogInformation.cellCount; i++)
		analogInformation.avgCellVoltageMillivolts += analogInformation.cellVoltagesMillivolts[i];
	analogInformation.avgCellVoltageMillivolts /= analogInformation.cellCount;

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadStatusInformationRequestV20[] = "~20014A440000FDA0\r";
const unsigned char PaceBmsProtocolV20::exampleReadStatusInformationResponseV20[] = "~20014A007054100110000000000000000000000000000000000400000000000000000900000000000003020000000000EDC3\r";

bool PaceBmsProtocolV20::CreateReadStatusInformationRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	// the payload is the requested busId (could be FF for "get all" when speaking to a set of daisy-chained units but this code doesn't support that)
	if (protocol_variant.has_value() &&
		protocol_variant.value() != "EG4") // EG4 variant does not send address in payload
	{
		const uint16_t payloadLen = 2;
		std::vector<uint8_t> payload(payloadLen);
		uint16_t payloadOffset = 0;
		WriteHexEncodedByte(payload, payloadOffset, busId);

		CreateRequest(busId, CID2_ReadStatusInformation, payload, request);
	}
	else
		CreateRequest(busId, CID2_ReadStatusInformation, std::vector<uint8_t>(), request);

	return true;
}

// helper for: ProcessStatusInformationResponse
const std::string PaceBmsProtocolV20::DecodeWarningValue(const uint8_t val)
{
	if (val == 0)
	{
		// calling code error
		return "(no warning)";
	}
	if (val == WV_BelowLowerLimitValue)
	{
		return std::string("Below Lower Limit");
	}
	if (val == WV_AboveUpperLimitValue)
	{
		return std::string("Above Upper Limit");
	}
	if (val == WV_OtherFaultValue)
	{
		return std::string("Other Fault");
	}

	return std::string("Unknown Fault Value");
}

// helper for: ProcessStatusInformationResponse
// note: in docs some of these are labeled protection and the rest are unlabeled as to "category" so I'm assuming protection also since it's in the same flags register?
void PaceBmsProtocolV20::StatusDecode_PYLON::DecodeStatus1Value(const uint8_t val, std::string& protectionText)
{
	if ((val & S1_PackUnderVoltage) != 0)
	{
		protectionText.append("Pack Under Voltage; ");
	}
	if ((val & S1_ChargeTemperatureProtection) != 0)
	{
		protectionText.append("Charge Temperature; ");
	}
	if ((val & S1_DischargeTemperatureProtection) != 0)
	{
		protectionText.append("Discharge Temperature; ");
	}
	if ((val & S1_DischargeOverCurrent) != 0)
	{
		protectionText.append("Discharge Over Current; ");
	}
	if ((val & S1_UndefinedStatus1Bit4) != 0)
	{
		protectionText.append("Undefined Status1 Bit4; ");
	}
	if ((val & S1_ChargeOverCurrent) != 0)
	{
		protectionText.append("Charge Over Current; ");
	}
	if ((val & S1_CellUnderVoltage) != 0)
	{
		protectionText.append("Cell Under Voltage; ");
	}
	if ((val & S1_PackOverVoltage) != 0)
	{
		protectionText.append("Pack Over Voltage; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_PYLON::DecodeStatus2Value(const uint8_t val, std::string& configurationText)
{
	if ((val & S2_UndefinedStatus2Bit8) != 0)
	{
		configurationText.append("Undefined Status2 Bit8; ");
	}
	if ((val & S2_UndefinedStatus2Bit7) != 0)
	{
		configurationText.append("Undefined Status2 Bit7; ");
	}
	if ((val & S2_UndefinedStatus2Bit6) != 0)
	{
		configurationText.append("Undefined Status2 Bit6; ");
	}
	if ((val & S2_UndefinedStatus2Bit5) != 0)
	{
		configurationText.append("Undefined Status2 Bit5; ");
	}
	if ((val & S2_UsingBatteryPower) != 0)
	{
		configurationText.append("Using Battery Power; ");
	}
	if ((val & S2_DischargeMosfetOn) != 0)
	{
		configurationText.append("Discharge Mosfet On; ");
	}
	if ((val & S2_ChargeMosfetOn) != 0)
	{
		configurationText.append("Charge Mosfet On; ");
	}
	if ((val & S2_PrechargeMosfetOn) != 0)
	{
		configurationText.append("Precharge Mosfet On; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_PYLON::DecodeStatus3Value(const uint8_t val, std::string& systemText)
{
	if ((val & S3_Charging) != 0)
	{
		systemText.append("Charging; ");
	}
	if ((val & S3_Discharging) != 0)
	{
		systemText.append("Discharging; ");
	}
	if ((val & S3_HeaterOn) != 0)
	{
		systemText.append("Heater On; ");
	}
	if ((val & S3_UndefinedStatus3Bit5) != 0)
	{
		systemText.append("Undefined Status3 Bit5; ");
	}
	if ((val & S3_FullyCharged) != 0)
	{
		systemText.append("Fully Charged; ");
	}
	if ((val & S3_UndefinedStatus3Bit3) != 0)
	{
		systemText.append("Undefined Status3 Bit3; ");
	}
	if ((val & S3_UndefinedStatus3Bit2) != 0)
	{
		systemText.append("Undefined Status3 Bit2; ");
	}
	if ((val & S3_Buzzer) != 0)
	{
		systemText.append("Buzzer On; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_PYLON::DecodeStatus4Value(const uint8_t val, std::string& faultText)
{
	if ((val & S4_Cell08Fault) != 0)
	{
		faultText.append("Cell 08 Fault (cell > 4.2v or cell < 1.0v); ");
	}
	if ((val & S4_Cell07Fault) != 0)
	{
		faultText.append("Cell 07 Fault (cell > 4.2v or cell < 1.0v); ");
	}
	if ((val & S4_Cell06Fault) != 0)
	{
		faultText.append("Cell 06 Fault (cell > 4.2v or cell < 1.0v); ");
	}
	if ((val & S4_Cell05Fault) != 0)
	{
		faultText.append("Cell 05 Fault (cell > 4.2v or cell < 1.0v); ");
	}
	if ((val & S4_Cell04Fault) != 0)
	{
		faultText.append("Cell 04 Fault (cell > 4.2v or cell < 1.0v); ");
	}
	if ((val & S4_Cell03Fault) != 0)
	{
		faultText.append("Cell 03 Fault (cell > 4.2v or cell < 1.0v); ");
	}
	if ((val & S4_Cell02Fault) != 0)
	{
		faultText.append("Cell 02 Fault (cell > 4.2v or cell < 1.0v); ");
	}
	if ((val & S4_Cell01Fault) != 0)
	{
		faultText.append("Cell 01 Fault (cell > 4.2v or cell < 1.0v); ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_PYLON::DecodeStatus5Value(const uint8_t val, std::string& faultText)
{
	if ((val & S5_Cell16Fault) != 0)
	{
		faultText.append("Cell 16 Fault (cell > 4.2v or cell < 1.0v); ");
	}
	if ((val & S5_Cell15Fault) != 0)
	{
		faultText.append("Cell 15 Fault (cell > 4.2v or cell < 1.0v); ");
	}
	if ((val & S5_Cell14Fault) != 0)
	{
		faultText.append("Cell 14 Fault (cell > 4.2v or cell < 1.0v); ");
	}
	if ((val & S5_Cell13Fault) != 0)
	{
		faultText.append("Cell 13 Fault (cell > 4.2v or cell < 1.0v); ");
	}
	if ((val & S5_Cell12Fault) != 0)
	{
		faultText.append("Cell 12 Fault (cell > 4.2v or cell < 1.0v); ");
	}
	if ((val & S5_Cell11Fault) != 0)
	{
		faultText.append("Cell 11 Fault (cell > 4.2v or cell < 1.0v); ");
	}
	if ((val & S5_Cell10Fault) != 0)
	{
		faultText.append("Cell 10 Fault (cell > 4.2v or cell < 1.0v); ");
	}
	if ((val & S5_Cell09Fault) != 0)
	{
		faultText.append("Cell 09 Fault (cell > 4.2v or cell < 1.0v); ");
	}
}

// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_SEPLOS::DecodeWarning1Value(const uint8_t val, std::string& faultText)
{
	if ((val & W1_CurrentLimitSwitchFailure) != 0)
	{
		faultText.append("Current Limit Switch; ");
	}
	if ((val & W1_DischaringSwitchFailure) != 0)
	{
		faultText.append("Discharge Switch; ");
	}
	if ((val & W1_ChargingSwitchFailure) != 0)
	{
		faultText.append("Charge Switch; ");
	}
	if ((val & W1_CellVoltageDifferenceSensingFailure) != 0)
	{
		faultText.append("Cell Voltage Difference Sense; ");
	}
	if ((val & W1_PowerSwitchFailure) != 0)
	{
		faultText.append("Power Switch; ");
	}
	if ((val & W1_CurrentSensingFailure) != 0)
	{
		faultText.append("Current Sense; ");
	}
	if ((val & W1_TemperatureSensingFailure) != 0)
	{
		faultText.append("Temperature Sense; ");
	}
	if ((val & W1_VoltageSensingFailure) != 0)
	{
		faultText.append("Voltage Sense; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_SEPLOS::DecodeWarning2Value(const uint8_t val, std::string& warningText, std::string& protectionText)
{
	if ((val & W2_PackLowVoltageProtection) != 0)
	{
		protectionText.append("Pack Low Voltage; ");
	}
	if ((val & W2_PackLowVoltageWarning) != 0)
	{
		warningText.append("Pack Low Voltage; ");
	}
	if ((val & W2_PackOverVoltageProtection) != 0)
	{
		protectionText.append("Pack Over Voltage; ");
	}
	if ((val & W2_PackOverVoltageWarning) != 0)
	{
		warningText.append("Pack Over Voltage; ");
	}
	if ((val & W2_CellLowVoltageProtection) != 0)
	{
		protectionText.append("Cell Low Voltage; ");
	}
	if ((val & W2_CellLowVoltageWarning) != 0)
	{
		warningText.append("Cell Low Voltage; ");
	}
	if ((val & W2_CellOverVoltageProtection) != 0)
	{
		protectionText.append("Cell Over Voltage; ");
	}
	if ((val & W2_CellOverVoltageWarning) != 0)
	{
		warningText.append("Cell Over Voltage; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_SEPLOS::DecodeWarning3Value(const uint8_t val, std::string& warningText, std::string& protectionText)
{
	if ((val & W3_DischargingLowTemperatureProtection) != 0)
	{
		protectionText.append("Discharge Low Temperature; ");
	}
	if ((val & W3_DischargingLowTemperatureWarning) != 0)
	{
		warningText.append("Discharge Low Temperature; ");
	}
	if ((val & W3_DischargingHighTemperatureProtection) != 0)
	{
		protectionText.append("Discharge High Temperature; ");
	}
	if ((val & W3_DischargingHighTemperatureWarning) != 0)
	{
		warningText.append("Discharge High Temperature; ");
	}
	if ((val & W3_ChargingLowTemperatureProtection) != 0)
	{
		protectionText.append("Charge Low Temperature; ");
	}
	if ((val & W3_ChargingLowTemperatureWarning) != 0)
	{
		warningText.append("Charge Low Temperature; ");
	}
	if ((val & W3_ChargingHighTemperatureProtection) != 0)
	{
		protectionText.append("Charge High Temperature; ");
	}
	if ((val & W3_ChargingHighTemperatureWarning) != 0)
	{
		warningText.append("Charge High Temperature; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_SEPLOS::DecodeWarning4Value(const uint8_t val, std::string& warningText, std::string& protectionText, std::string& systemText)
{
	if ((val & W4_ReservedWarning4Bit8) != 0)
	{
		warningText.append("Reserved Warning4 Bit8; ");
	}
	if ((val & W4_Heating) != 0)
	{
		systemText.append("Heating; ");
	}
	if ((val & W4_ComponentHighTemperatureProtection) != 0)
	{
		protectionText.append("Component High Temperature; ");
	}
	if ((val & W4_ComponentHighTemperatureWarning) != 0)
	{
		warningText.append("Component High Temperature; ");
	}
	if ((val & W4_AmbientLowTemperatureProtection) != 0)
	{
		protectionText.append("Ambient Low Temperature; ");
	}
	if ((val & W4_AmbientLowTemperatureWarning) != 0)
	{
		warningText.append("Ambient Low Temperature; ");
	}
	if ((val & W4_AmbientHighTemperatureProtection) != 0)
	{
		protectionText.append("Ambient High Temperature; ");
	}
	if ((val & W4_AmbientHighTemperatureWarning) != 0)
	{
		warningText.append("Ambient High Temperature; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_SEPLOS::DecodeWarning5Value(const uint8_t val, std::string& warningText, std::string& protectionText, std::string& faultText)
{
	if ((val & W5_OutputShortCircuitLock) != 0)
	{
		faultText.append("Output Short Circuit; ");
	}
	if ((val & W5_TransientOverCurrentLock) != 0)
	{
		faultText.append("Transient Over Current; ");
	}
	if ((val & W5_OutputShortCircuitProtection) != 0)
	{
		protectionText.append("Output Short Circuit; ");
	}
	if ((val & W5_TansientOverCurrentProtection) != 0)
	{
		protectionText.append("Transient Over Current; ");
	}
	if ((val & W5_DischargeOverCurrentProtection) != 0)
	{
		protectionText.append("Discharge Over Current; ");
	}
	if ((val & W5_DischargeOverCurrentWarning) != 0)
	{
		warningText.append("Discharge Over Current; ");
	}
	if ((val & W5_ChargeOverCurrentProtection) != 0)
	{
		protectionText.append("Charge Over Current; ");
	}
	if ((val & W5_ChargeOverCurrentWarning) != 0)
	{
		warningText.append("Charge Over Current; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_SEPLOS::DecodeWarning6Value(const uint8_t val, std::string& warningText, std::string& protectionText, std::string& faultText)
{
	if ((val & W6_InternalWarning6Bit8) != 0)
	{
		warningText.append("Internal Warning6 Bit8; ");
	}
	if ((val & W6_OutputConnectionFailure) != 0)
	{
		faultText.append("Output Connection; ");
	}
	if ((val & W6_OutputReverseConnectionProtection) != 0)
	{
		protectionText.append("Output Reverse Connection; ");
	}
	if ((val & W6_CellLowVoltageChargingForbidden) != 0)
	{
		faultText.append("Cell Low Voltage Charging Forbidden; ");
	}
	if ((val & W6_RemaingCapacityProtection) != 0)
	{
		protectionText.append("Remaining Capacity; ");
	}
	if ((val & W6_RemaingCapacityWarning) != 0)
	{
		warningText.append("Remaining Capacity; ");
	}
	if ((val & W6_IntermittentPowerSupplementWaiting) != 0)
	{
		warningText.append("Intermittent Power Supply Waiting; ");
	}
	if ((val & W6_ChargingHighVoltageProtection) != 0)
	{
		protectionText.append("Charging High Voltage; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_SEPLOS::DecodePowerStatusValue(const uint8_t val, std::string& configurationText)
{
	if ((val & PS_ReservedPowerStatusBit8) != 0)
	{
		configurationText.append("Reserved Power Status Bit8; ");
	}
	if ((val & PS_ReservedPowerStatusBit7) != 0)
	{
		configurationText.append("Reserved Power Status Bit8; ");
	}
	if ((val & PS_ReservedPowerStatusBit6) != 0)
	{
		configurationText.append("Reserved Power Status Bit8; ");
	}
	if ((val & PS_ReservedPowerStatusBit5) != 0)
	{
		configurationText.append("Reserved Power Status Bit8; ");
	}
	if ((val & PS_HeatingSwitchStatus) != 0)
	{
		configurationText.append("Heating Switch On; ");
	}
	if ((val & PS_CurrentLimitSwitchStatus) != 0)
	{
		configurationText.append("Current Limit Switch On; ");
	}
	if ((val & PS_ChargeSwitchStatus) != 0)
	{
		configurationText.append("Charge Switch On; ");
	}
	if ((val & PS_DischargeSwitchStatus) != 0)
	{
		configurationText.append("Discharge Switch On; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_SEPLOS::DecodeSystemStatusValue(const uint8_t val, std::string& systemText)
{
	if ((val & SS_ReservedSystemStatusBit8) != 0)
	{
		systemText.append("Reserved System Status Bit8; ");
	}
	if ((val & SS_ReservedSystemStatusBit7) != 0)
	{
		systemText.append("Reserved System Status Bit8; ");
	}
	if ((val & SS_PowerOff) != 0)
	{
		systemText.append("Power Off; ");
	}
	if ((val & SS_Standby) != 0)
	{
		systemText.append("Standby; ");
	}
	if ((val & SS_ReservedSystemStatusBit4) != 0)
	{
		systemText.append("Reserved System Status Bit4; ");
	}
	if ((val & SS_FloatingCharge) != 0)
	{
		systemText.append("Floating Charge; ");
	}
	if ((val & SS_Charging) != 0)
	{
		systemText.append("Charging; ");
	}
	if ((val & SS_Discharging) != 0)
	{
		systemText.append("Discharging; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_SEPLOS::DecodeWarning7Value(const uint8_t val, std::string& warningText)
{
	if ((val & W7_Warning7InternalBit8) != 0)
	{
		warningText.append("Warning7 Internal Bit8; ");
	}
	if ((val & W7_Warning7InternalBit7) != 0)
	{
		warningText.append("Warning7 Internal Bit7; ");
	}
	if ((val & W7_ManualChargingWait) != 0)
	{
		warningText.append("Manual Charging Wait; ");
	}
	if ((val & W7_AutoChargingWait) != 0)
	{
		warningText.append("Auto Charging Wait; ");
	}
	if ((val & W7_Warning7InternalBit4) != 0)
	{
		warningText.append("Warning7 Internal Bit4; ");
	}
	if ((val & W7_Warning7InternalBit3) != 0)
	{
		warningText.append("Warning7 Internal Bit3; ");
	}
	if ((val & W7_Warning7InternalBit2) != 0)
	{
		warningText.append("Warning7 Internal Bit2; ");
	}
	if ((val & W7_Warning7InternalBit1) != 0)
	{
		warningText.append("Warning7 Internal Bit1; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_SEPLOS::DecodeWarning8Value(const uint8_t val, std::string& faultText)
{
	if ((val & W8_Warning8InternalBit8) != 0)
	{
		faultText.append("Warning8 Internal Bit8; ");
	}
	if ((val & W8_Warning8InternalBit7) != 0)
	{
		faultText.append("Warning8 Internal Bit7; ");
	}
	if ((val & W8_Warning8InternalBit6) != 0)
	{
		faultText.append("Warning8 Internal Bit6; ");
	}
	if ((val & W8_NoNullPointCalibration) != 0)
	{
		faultText.append("No Null Point Calibration; ");
	}
	if ((val & W8_NoCurrentCalibration) != 0)
	{
		faultText.append("No Current Calibration; ");
	}
	if ((val & W8_NoVoltageCalibration) != 0)
	{
		faultText.append("No Voltage Calibration; ");
	}
	if ((val & W8_RTCFailure) != 0)
	{
		faultText.append("Real-Time Clock Failure; ");
	}
	if ((val & W8_EEPStorageFailure) != 0)
	{
		faultText.append("EEP Storage Failure; ");
	}
}

// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_EG4::DecodeBalanceEvent(const uint8_t val, std::string& warningText, std::string& faultText)
{
	if ((val & BE_BalanceEventReservedBit8) != 0)
	{
		warningText.append("BalanceEventReservedBit8; ");
	}
	if ((val & BE_DischargeMosFaultAlarm) != 0)
	{
		faultText.append("Discharge MOSFET; ");
	}
	if ((val & BE_ChargeMosFaultAlarm) != 0)
	{
		faultText.append("Charge MOSFET; ");
	}
	if ((val & BE_CellVoltageDifferenceAlarm) != 0)
	{
		warningText.append("Cell Voltage Difference; ");
	}
	if ((val & BE_BalanceEventReservedBit4) != 0)
	{
		warningText.append("BalanceEventReservedBit4; ");
	}
	if ((val & BE_BalanceEventReservedBit3) != 0)
	{
		warningText.append("BalanceEventReservedBit3; ");
	}
	if ((val & BE_BalanceEventReservedBit2) != 0)
	{
		warningText.append("BalanceEventReservedBit2; ");
	}
	if ((val & BE_BalanceEventBalancingActive) != 0)
	{
		warningText.append("BalanceEventReservedBit1; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_EG4::DecodeVoltageEvent(const uint8_t val, std::string& warningText, std::string& protectionText)
{
	if ((val & VE_PackUnderVoltageProtect) != 0)
	{
		protectionText.append("Pack Under Voltage; ");
	}
	if ((val & VE_PackUnderVoltageAlarm) != 0)
	{
		warningText.append("Pack Under Voltage; ");
	}
	if ((val & VE_PackOverVoltageProtect) != 0)
	{
		protectionText.append("Pack Over Voltage; ");
	}
	if ((val & VE_PackOverVoltageAlarm) != 0)
	{
		warningText.append("Pack Over Voltage; ");
	}
	if ((val & VE_CellUnderVoltageProtect) != 0)
	{
		protectionText.append("Cell Under Voltage; ");
	}
	if ((val & VE_CellUnderVoltageAlarm) != 0)
	{
		warningText.append("Cell Under Voltage; ");
	}
	if ((val & VE_CellOverVoltageProtect) != 0)
	{
		protectionText.append("Cell Over Voltage; ");
	}
	if ((val & VE_CellOverVoltageAlarm) != 0)
	{
		warningText.append("Cell Over Voltage; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_EG4::DecodeTemperatureEvent(const uint16_t val, std::string& warningText, std::string& protectionText, std::string& faultText)
{
	if ((val & TE_TemperatureEventReservedBit16) != 0)
	{
		warningText.append("TemperatureEventReservedBit16; ");
	}
	if ((val & TE_TemperatureEventReservedBit15) != 0)
	{
		warningText.append("TemperatureEventReservedBit15; ");
	}
	if ((val & TE_FireAlarm) != 0)
	{
		faultText.append("Fire Alarm; ");
	}
	if ((val & TE_MosfetHighTemperatureProtect) != 0)
	{
		protectionText.append("MOSFET High Temperature; ");
	}
	if ((val & TE_EnvironmentLowTemperatureProtect) != 0)
	{
		protectionText.append("Environment Low Temperature; ");
	}
	if ((val & TE_EnvironmentLowTemperatureAlarm) != 0)
	{
		warningText.append("Environment Low Temperature; ");
	}
	if ((val & TE_EnvironmentHighTemperatureProtect) != 0)
	{
		protectionText.append("Environment High Temperature; ");
	}
	if ((val & TE_EnvironmentHighTemperatureAlarm) != 0)
	{
		warningText.append("Environment High Temperature; ");
	}
	if ((val & TE_DischargeLowTemperatureProtect) != 0)
	{
		protectionText.append("Discharge Low Temperature; ");
	}
	if ((val & TE_DischargeLowTemperatureAlarm) != 0)
	{
		warningText.append("Discharge Low Temperature; ");
	}
	if ((val & TE_DischargeHighTemperatureProtect) != 0)
	{
		protectionText.append("Discharge High Temperature; ");
	}
	if ((val & TE_DischargeHighTemperatureAlarm) != 0)
	{
		warningText.append("Discharge High Temperature; ");
	}
	if ((val & TE_ChargeLowTemperatureProtect) != 0)
	{
		protectionText.append("Charge Low Temperature; ");
	}
	if ((val & TE_ChargeLowTemperatureAlarm) != 0)
	{
		warningText.append("Charge Low Temperature; ");
	}
	if ((val & TE_ChargeHighTemperatureProtect) != 0)
	{
		protectionText.append("Charge High Temperature; ");
	}
	if ((val & TE_ChargeHighTemperatureAlarm) != 0)
	{
		warningText.append("Charge High Temperature; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_EG4::DecodeCurrentEvent(const uint8_t val, std::string& warningText, std::string& protectionText, std::string& faultText)
{
	if ((val & CE_OutputShortCircuitLockout) != 0)
	{
		faultText.append("Output Short Circuit Lockout; ");
	}
	if ((val & CE_DischargeLevel2OverCurrentLockout) != 0)
	{
		faultText.append("Discharge Level 2 Over Current Lockout; ");
	}
	if ((val & CE_OutputShortCircuitProtect) != 0)
	{
		protectionText.append("Output Short Circuit; ");
	}
	if ((val & CE_DischargeLevel2OverCurrentProtect) != 0)
	{
		protectionText.append("Discharge Level 2 Over Current; ");
	}
	if ((val & CE_DischargeOverCurrentProtect) != 0)
	{
		protectionText.append("Discharge Over Current; ");
	}
	if ((val & CE_DischargeOverCurrentAlarm) != 0)
	{
		warningText.append("Discharge Over Current; ");
	}
	if ((val & CE_ChargeOverCurrentProtect) != 0)
	{
		protectionText.append("Charge Over Current; ");
	}
	if ((val & CE_ChargeOverCurrentAlarm) != 0)
	{
		warningText.append("Charge Over Current; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_EG4::DecodeRemainingCapacity(const uint8_t val, std::string& warningText)
{
	if ((val & RC_RemainingCapacityReservedBit8) != 0)
	{
		warningText.append("RemainingCapacityReservedBit8; ");
	}
	if ((val & RC_RemainingCapacityReservedBit7) != 0)
	{
		warningText.append("RemainingCapacityReservedBit7; ");
	}
	if ((val & RC_RemainingCapacityReservedBit6) != 0)
	{
		warningText.append("RemainingCapacityReservedBit6; ");
	}
	if ((val & RC_RemainingCapacityReservedBit5) != 0)
	{
		warningText.append("RemainingCapacityReservedBit5; ");
	}
	if ((val & RC_RemainingCapacityReservedBit4) != 0)
	{
		warningText.append("RemainingCapacityReservedBit4; ");
	}
	if ((val & RC_RemainingCapacityReservedBit3) != 0)
	{
		warningText.append("RemainingCapacityReservedBit3; ");
	}
	if ((val & RC_RemainingCapacityReservedBit2) != 0)
	{
		warningText.append("RemainingCapacityReservedBit2; ");
	}
	if ((val & RC_StateOfChargeLow) != 0)
	{
		warningText.append("State of Charge Low; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_EG4::DecodeFetStatus(const uint8_t val, std::string& configurationText)
{
	if ((val & FS_FetStatusReservedBit8) != 0)
	{
		configurationText.append("FetStatusReservedBit8; ");
	}
	if ((val & FS_FetStatusReservedBit7) != 0)
	{
		configurationText.append("FetStatusReservedBit7; ");
	}
	if ((val & FS_FetStatusReservedBit6) != 0)
	{
		configurationText.append("FetStatusReservedBit6; ");
	}
	if ((val & FS_FetStatusReservedBit5) != 0)
	{
		configurationText.append("FetStatusReservedBit5; ");
	}
	if ((val & FS_HeaterOn) != 0)
	{
		configurationText.append("Heater On; ");
	}
	if ((val & FS_ChargeCurrentLimiterOn) != 0)
	{
		configurationText.append("Charge Current Limiter On; ");
	}
	if ((val & FS_ChargeMosfetOn) != 0)
	{
		configurationText.append("Charge MOSFET On; ");
	}
	if ((val & FS_DischargeMosfetOn) != 0)
	{
		configurationText.append("Discharge MOSFET On; ");
	}
}
// helper for: ProcessStatusInformationResponse
void PaceBmsProtocolV20::StatusDecode_EG4::DecodeSystemStatus(const uint8_t val, std::string& systemText)
{
	if ((val & SS_SystemStatusReservedBit8) != 0)
	{
		systemText.append("SystemStatusReservedBit8; ");
	}
	if ((val & SS_SystemStatusReservedBit7) != 0)
	{
		systemText.append("SystemStatusReservedBit7; ");
	}
	if ((val & SS_SystemStatusReservedBit6) != 0)
	{
		systemText.append("SystemStatusReservedBit6; ");
	}
	if ((val & SS_SystemStatusReservedBit5) != 0)
	{
		systemText.append("SystemStatusReservedBit5; ");
	}
	if ((val & SS_Standby) != 0)
	{
		systemText.append("Standby; ");
	}
	if ((val & SS_SystemStatusReservedBit3) != 0)
	{
		systemText.append("SystemStatusReservedBit3; ");
	}
	if ((val & SS_Charging) != 0)
	{
		systemText.append("Charging; ");
	}
	if ((val & SS_Discharging) != 0)
	{
		systemText.append("Discharging; ");
	}
}

bool PaceBmsProtocolV20::ProcessReadStatusInformationResponse(const uint8_t busId, const std::vector<uint8_t>& response, StatusInformation& statusInformation)
{
	// save in order compare against what ProcessReadAnalogInformationResponse sussed out
	OPTIONAL_NS::optional<std::string> previously_detected_variant = OPTIONAL_NS::optional<std::string>(detected_variant.value());

	// try to auto-detect the protocol variant
	//if (!detected_variant.has_value())
	{
		uint16_t byteOffset;

		bool isEG4 = false;
		byteOffset = 13 + 56;
		if (response.size() - byteOffset > 2)
		{
			uint8_t byte = ReadHexEncodedByte(response, byteOffset);
			if (byte == 9)
				isEG4 = true;
		}

		// I considered sniffing this out via payload length, but pylon doesn't contain any UD value and length may overlap between variants, plus I don't have a confirmed example!
		bool isPylon = false;
		//byteOffset = 13 + 56;
		//if (response.size() - byteOffset > 2)
		//{
		//	uint8_t byte = ReadHexEncodedByte(response, byteOffset);
		//	if (byte == 02)
		//		isEG4 = true;
		//}

		bool isSeplos = false;
		byteOffset = 13 + 56;
		if (response.size() - byteOffset > 2)
		{
			uint8_t byte = ReadHexEncodedByte(response, byteOffset);
			if (byte == 20)
				isSeplos = true;
		}

		if (isPylon)
		{
			detected_variant = OPTIONAL_NS::optional<std::string>("PYLON");
			LogInfo("Detected protocol variant: PYLON");
		}
		if (isSeplos)
		{
			detected_variant = OPTIONAL_NS::optional<std::string>("SEPLOS");
			LogInfo("Detected protocol variant: SEPLOS");
		}
		if (isEG4)
		{
			detected_variant = OPTIONAL_NS::optional<std::string>("EG4");
			LogInfo("Detected protocol variant: EG4");
		}

		int detected_count = 0;
		if (isPylon) detected_count++;
		if (isSeplos) detected_count++;
		if (isEG4) detected_count++;

		if (detected_count > 1)
		{
			// conflict, unable to decide
			LogWarning("Multiple matches on protocol variant auto-detect, unable to narrow down");
			detected_variant = {};
		}
	}

	// does detected variant conflict with configured variant?
	if (protocol_variant.has_value() && detected_variant.has_value() &&
		protocol_variant.value() != detected_variant.value())
	{
		LogWarning("Auto-detected protocol variant '" + detected_variant.value() + "' does not match configured protocol variant '" + protocol_variant.value() + "', using configured value.");
	}

	// does detected variant conflict with what ProcessReadAnalogInformationResponse detected?
	if (previously_detected_variant.has_value() && detected_variant.has_value() &&
		previously_detected_variant.value() != detected_variant.value())
	{
		LogWarning("Auto-detected protocol variant '" + detected_variant.value() + "' does not match previously detected protocol variant '" + protocol_variant.value() + "' determined via a different method, using newly detected value.");
	}

	// decide what variant to use
	std::string variant_to_use;
	if (protocol_variant.has_value())
		variant_to_use = protocol_variant.value();
	else if (detected_variant.has_value())
		variant_to_use = detected_variant.value();
	else
	{
		// have to fallback to something
		LogWarning("Protocol variant not configured, unable to auto-detect, defaulting to EG4");
		variant_to_use = "EG4";
	}

	// fan-out to variant handlers
	if (variant_to_use == "PYLON")
	{
		return ProcessReadStatusInformationResponse_PYLON(busId, response, statusInformation);
	}
	else if (variant_to_use == "SEPLOS")
	{
		return ProcessReadStatusInformationResponse_SEPLOS(busId, response, statusInformation);
	}
	else if (variant_to_use == "EG4")
	{
		return ProcessReadStatusInformationResponse_EG4(busId, response, statusInformation);
	}
	else
	{
		LogError("Invalid protocol variant '" + variant_to_use + "'");
		return false;
	}
}

bool PaceBmsProtocolV20::ProcessReadStatusInformationResponse_PYLON(const uint8_t busId, const std::vector<uint8_t>& response, StatusInformation& statusInformation)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
		// failed to validate, the call would have done it's own logging
		return false;

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	// spec says "dataflag" preceeds the response data but doesn't explain what that is
	uint8_t data_flag = ReadHexEncodedByte(response, byteOffset);
	//if (data_flag != 0)
	//	LogWarning("data_flag unexpected value");

	uint8_t pack_count = ReadHexEncodedByte(response, byteOffset);
	if (pack_count != 01)
		LogWarning("response contains data for more than one pack");

	// ========================== Warning / Alarm Status ==========================
	uint8_t cellCount = ReadHexEncodedByte(response, byteOffset);
	if (cellCount > MAX_CELL_COUNT)
		LogWarning("Response contains more cell warnings than are supported, results will be truncated");
	for (int i = 0; i < cellCount; i++)
	{
		uint8_t cw = ReadHexEncodedByte(response, byteOffset);
		statusInformation.warning_value_cell[i] = cw;
		if (i > MAX_CELL_COUNT - 1)
			continue;
		if (cw == 0)
			continue;
		// below/above limit
		statusInformation.warningText.append(std::string("Cell ") + std::to_string(i + 1) + std::string(": ") + DecodeWarningValue(cw) + std::string("; "));
	}

	uint8_t tempCount = ReadHexEncodedByte(response, byteOffset);
	if (tempCount > MAX_TEMP_COUNT)
		LogWarning("Response contains more temperature warnings than are supported, results will be truncated");
	for (int i = 0; i < tempCount; i++)
	{
		uint8_t tw = ReadHexEncodedByte(response, byteOffset);
		statusInformation.warning_value_temp[i] = tw;
		if (i > MAX_TEMP_COUNT - 1)
			continue;
		if (tw == 0)
			continue;
		// below/above limit
		statusInformation.warningText.append(std::string("Temperature ") + std::to_string(i + 1) + ": " + DecodeWarningValue(tw) + std::string("; "));
	}

	uint8_t chargeCurrentWarn = ReadHexEncodedByte(response, byteOffset);
	statusInformation.warning_value_charge_current = chargeCurrentWarn;
	if (chargeCurrentWarn != 0)
		// below/above limit
		statusInformation.warningText.append(std::string("Charge current: ") + DecodeWarningValue(chargeCurrentWarn) + std::string("; "));

	uint8_t totalVoltageWarn = ReadHexEncodedByte(response, byteOffset);
	statusInformation.warning_value_total_voltage = totalVoltageWarn;
	if (totalVoltageWarn != 0)
		// below/above limit
		statusInformation.warningText.append(std::string("Total voltage: ") + DecodeWarningValue(totalVoltageWarn) + std::string("; "));

	uint8_t dischargeCurrentWarn = ReadHexEncodedByte(response, byteOffset);
	statusInformation.warning_value_discharge_current = dischargeCurrentWarn;
	if (dischargeCurrentWarn != 0)
		// below/above limit
		statusInformation.warningText.append(std::string("Discharge current: ") + DecodeWarningValue(dischargeCurrentWarn) + std::string("; "));

	// ========================== Status 1-5 Flags ==========================
	statusInformation.status1_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.status1_value != 0)
		StatusDecode_PYLON::DecodeStatus1Value(statusInformation.status1_value, statusInformation.protectionText);

	statusInformation.status2_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.status2_value != 0)
		StatusDecode_PYLON::DecodeStatus2Value(statusInformation.status2_value, statusInformation.configurationText);

	statusInformation.status3_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.status3_value != 0)
		StatusDecode_PYLON::DecodeStatus3Value(statusInformation.status3_value, statusInformation.systemText);

	statusInformation.status4_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.status4_value != 0)
		StatusDecode_PYLON::DecodeStatus4Value(statusInformation.status4_value, statusInformation.faultText);

	statusInformation.status5_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.status5_value != 0)
		StatusDecode_PYLON::DecodeStatus5Value(statusInformation.status5_value, statusInformation.faultText);

	if (byteOffset != payloadLen + 13)
		LogWarning("Length mismatch reading status information response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off");

	// pop off any trailing "; " separator
	if (statusInformation.warningText.length() > 2)
	{
		statusInformation.warningText.pop_back();
		statusInformation.warningText.pop_back();
	}
	//if (statusInformation.balancingText.length() > 2)
	//{
	//	statusInformation.balancingText.pop_back();
	//	statusInformation.balancingText.pop_back();
	//}
	if (statusInformation.systemText.length() > 2)
	{
		statusInformation.systemText.pop_back();
		statusInformation.systemText.pop_back();
	}
	if (statusInformation.configurationText.length() > 2)
	{
		statusInformation.configurationText.pop_back();
		statusInformation.configurationText.pop_back();
	}
	if (statusInformation.protectionText.length() > 2)
	{
		statusInformation.protectionText.pop_back();
		statusInformation.protectionText.pop_back();
	}
	if (statusInformation.faultText.length() > 2)
	{
		statusInformation.faultText.pop_back();
		statusInformation.faultText.pop_back();
	}

	return true;
}
bool PaceBmsProtocolV20::ProcessReadStatusInformationResponse_SEPLOS(const uint8_t busId, const std::vector<uint8_t>& response, StatusInformation& statusInformation)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
		// failed to validate, the call would have done it's own logging
		return false;

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	// spec says "dataflag" preceeds the response data but doesn't explain what that is
	uint8_t data_flag = ReadHexEncodedByte(response, byteOffset);
	//if (data_flag != 0)
	//	LogWarning("data_flag unexpected value");

	// spec doesn't say this is here but just by comparing implementations I'm pretty sure it is, can't test it though :(
	uint8_t pack_group = ReadHexEncodedByte(response, byteOffset);
	if (pack_group != busId)
		LogWarning("response from wrong bus id");

	// ========================== Warning / Alarm Status ==========================
	uint8_t cellCount = ReadHexEncodedByte(response, byteOffset);
	if (cellCount > MAX_CELL_COUNT)
		LogWarning("Response contains more cell warnings than are supported, results will be truncated");
	for (int i = 0; i < cellCount; i++)
	{
		uint8_t cw = ReadHexEncodedByte(response, byteOffset);
		statusInformation.warning_value_cell[i] = cw;
		if (i > MAX_CELL_COUNT - 1)
			continue;
		if (cw == 0)
			continue;
		// below/above limit
		statusInformation.warningText.append(std::string("Cell ") + std::to_string(i + 1) + std::string(": ") + DecodeWarningValue(cw) + std::string("; "));
	}

	uint8_t tempCount = ReadHexEncodedByte(response, byteOffset);
	if (tempCount > MAX_TEMP_COUNT)
		LogWarning("Response contains more temperature warnings than are supported, results will be truncated");
	for (int i = 0; i < tempCount; i++)
	{
		uint8_t tw = ReadHexEncodedByte(response, byteOffset);
		statusInformation.warning_value_temp[i] = tw;
		if (i > MAX_TEMP_COUNT - 1)
			continue;
		if (tw == 0)
			continue;
		// below/above limit
		statusInformation.warningText.append(std::string("Temperature ") + std::to_string(i + 1) + ": " + DecodeWarningValue(tw) + std::string("; "));
	}

	uint8_t currentWarn = ReadHexEncodedByte(response, byteOffset);
	// SEPLOS combines these two into a single value, so setting both
	statusInformation.warning_value_charge_current = currentWarn;
	statusInformation.warning_value_discharge_current = currentWarn;
	if (currentWarn != 0)
		// below/above limit
		// SEPLOS combines these two into a single value, so adjusting text
		statusInformation.warningText.append(std::string("Current: ") + DecodeWarningValue(currentWarn) + std::string("; "));

	uint8_t totalVoltageWarn = ReadHexEncodedByte(response, byteOffset);
	statusInformation.warning_value_total_voltage = totalVoltageWarn;
	if (totalVoltageWarn != 0)
		// below/above limit
		statusInformation.warningText.append(std::string("Total voltage: ") + DecodeWarningValue(totalVoltageWarn) + std::string("; "));

	//uint8_t dischargeCurrentWarn = ReadHexEncodedByte(response, byteOffset);
	//statusInformation.warning_value_discharge_current = dischargeCurrentWarn;
	//if (dischargeCurrentWarn != 0)
	//	// below/above limit
	//	statusInformation.warningText.append(std::string("Discharge current: ") + DecodeWarningValue(dischargeCurrentWarn) + std::string("; "));

	uint8_t UD20 = ReadHexEncodedByte(response, byteOffset);
	if (UD20 != 20)
		LogWarning("Response contains a constant with an unexpected value, this may be an incorrect protocol variant");

	// ========================== Status Flags ==========================
	statusInformation.warning1_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.warning1_value != 0)
		StatusDecode_SEPLOS::DecodeWarning1Value(statusInformation.warning1_value, statusInformation.faultText);

	statusInformation.warning2_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.warning2_value != 0)
		StatusDecode_SEPLOS::DecodeWarning2Value(statusInformation.warning2_value, statusInformation.warningText, statusInformation.protectionText);

	statusInformation.warning3_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.warning3_value != 0)
		StatusDecode_SEPLOS::DecodeWarning3Value(statusInformation.warning3_value, statusInformation.warningText, statusInformation.protectionText);

	statusInformation.warning4_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.warning4_value != 0)
		StatusDecode_SEPLOS::DecodeWarning4Value(statusInformation.warning4_value, statusInformation.warningText, statusInformation.protectionText, statusInformation.systemText);

	statusInformation.warning5_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.warning5_value != 0)
		StatusDecode_SEPLOS::DecodeWarning5Value(statusInformation.warning5_value, statusInformation.warningText, statusInformation.protectionText, statusInformation.faultText);

	statusInformation.warning6_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.warning6_value != 0)
		StatusDecode_SEPLOS::DecodeWarning6Value(statusInformation.warning6_value, statusInformation.warningText, statusInformation.protectionText, statusInformation.faultText);

	statusInformation.power_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.power_value != 0)
		StatusDecode_SEPLOS::DecodePowerStatusValue(statusInformation.power_value, statusInformation.configurationText);

	statusInformation.balancing_value = ReadHexEncodedUShort(response, byteOffset);
	for (int i = 0; i < 16; i++)
	{
		if ((statusInformation.balancing_value & (1 << i)) != 0)
		{
			statusInformation.balancingText.append(std::string("Cell ") + std::to_string(i + 1) + " is balancing; ");
		}
	}

	statusInformation.system_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.system_value != 0)
		StatusDecode_SEPLOS::DecodeWarning6Value(statusInformation.system_value, statusInformation.warningText, statusInformation.protectionText, statusInformation.faultText);

	statusInformation.disconnection_value = ReadHexEncodedUShort(response, byteOffset);
	for (int i = 0; i < 16; i++)
	{
		if ((statusInformation.disconnection_value & (1 << i)) != 0)
		{
			statusInformation.faultText.append(std::string("Cell ") + std::to_string(i + 1) + " is disconnected; ");
		}
	}

	statusInformation.warning7_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.warning7_value != 0)
		StatusDecode_SEPLOS::DecodeWarning7Value(statusInformation.warning7_value, statusInformation.warningText);

	statusInformation.warning8_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.warning8_value != 0)
		StatusDecode_SEPLOS::DecodeWarning8Value(statusInformation.warning8_value, statusInformation.faultText);

	// reserved 1-6
	byteOffset += 12; // 6 one byte values as two byte hexascii

	if (byteOffset != payloadLen + 13)
		LogWarning("Length mismatch reading status information response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off");

	// pop off any trailing "; " separator
	if (statusInformation.warningText.length() > 2)
	{
		statusInformation.warningText.pop_back();
		statusInformation.warningText.pop_back();
	}
	if (statusInformation.balancingText.length() > 2)
	{
		statusInformation.balancingText.pop_back();
		statusInformation.balancingText.pop_back();
	}
	if (statusInformation.systemText.length() > 2)
	{
		statusInformation.systemText.pop_back();
		statusInformation.systemText.pop_back();
	}
	if (statusInformation.configurationText.length() > 2)
	{
		statusInformation.configurationText.pop_back();
		statusInformation.configurationText.pop_back();
	}
	if (statusInformation.protectionText.length() > 2)
	{
		statusInformation.protectionText.pop_back();
		statusInformation.protectionText.pop_back();
	}
	if (statusInformation.faultText.length() > 2)
	{
		statusInformation.faultText.pop_back();
		statusInformation.faultText.pop_back();
	}

	return true;
}
bool PaceBmsProtocolV20::ProcessReadStatusInformationResponse_EG4(const uint8_t busId, const std::vector<uint8_t>& response, StatusInformation& statusInformation)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	uint8_t data_flag = ReadHexEncodedByte(response, byteOffset);
	//if (data_flag != 0)
	//	LogWarning("data_flag unexpected value");

	uint8_t pack_group = ReadHexEncodedByte(response, byteOffset);
	if (pack_group != busId)
		LogWarning("response from wrong bus id");

	// ========================== Warning / Alarm Status ==========================
	uint8_t cellCount = ReadHexEncodedByte(response, byteOffset);
	if (cellCount > MAX_CELL_COUNT)
		LogWarning("Response contains more cell warnings than are supported, results will be truncated");
	for (int i = 0; i < cellCount; i++)
	{
		uint8_t cw = ReadHexEncodedByte(response, byteOffset);
		statusInformation.warning_value_cell[i] = cw;
		if (i > MAX_CELL_COUNT - 1)
			continue;
		if (cw == 0)
			continue;
		// below/above limit
		statusInformation.warningText.append(std::string("Cell ") + std::to_string(i + 1) + std::string(": ") + DecodeWarningValue(cw) + std::string("; "));
	}

	uint8_t tempCount = ReadHexEncodedByte(response, byteOffset);
	if (tempCount > MAX_TEMP_COUNT)
		LogWarning("Response contains more temperature warnings than are supported, results will be truncated");
	// EG4 protocol specifies 4 temperatures and then environment and mosfet temps come next, which is the same thing as 6 temperatures
	if (tempCount == 4)
		tempCount = 6;
	for (int i = 0; i < tempCount; i++)
	{
		uint8_t tw = ReadHexEncodedByte(response, byteOffset);
		statusInformation.warning_value_temp[i] = tw;
		if (i > MAX_TEMP_COUNT - 1)
			continue;
		if (tw == 0)
			continue;
		// below/above limit
		statusInformation.warningText.append(std::string("Temperature ") + std::to_string(i + 1) + ": " + DecodeWarningValue(tw) + std::string("; "));
	}

	uint8_t currentWarn = ReadHexEncodedByte(response, byteOffset);
	// EG4 combines these two into a single value, so setting both
	statusInformation.warning_value_charge_current = currentWarn;
	statusInformation.warning_value_discharge_current = currentWarn;
	if (currentWarn != 0)
		// below/above limit
		// EG4 combines these two into a single value, so adjusting text
		statusInformation.warningText.append(std::string("Current: ") + DecodeWarningValue(currentWarn) + std::string("; "));

	uint8_t totalVoltageWarn = ReadHexEncodedByte(response, byteOffset);
	statusInformation.warning_value_total_voltage = totalVoltageWarn;
	if (totalVoltageWarn != 0)
		// below/above limit
		statusInformation.warningText.append(std::string("Total voltage: ") + DecodeWarningValue(totalVoltageWarn) + std::string("; "));

	//uint8_t dischargeCurrentWarn = ReadHexEncodedByte(response, byteOffset);
	//statusInformation.warning_value_discharge_current = dischargeCurrentWarn;
	//if (dischargeCurrentWarn != 0)
	//	// below/above limit
	//	statusInformation.warningText.append(std::string("Discharge current: ") + DecodeWarningValue(dischargeCurrentWarn) + std::string("; "));

	uint8_t UD9 = ReadHexEncodedByte(response, byteOffset);
	if (UD9 != 9)
		LogWarning("Response contains a constant with an unexpected value, this may be an incorrect protocol variant");

	// ========================== Status Flags ==========================
	statusInformation.balance_event_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.balance_event_value != 0)
		StatusDecode_EG4::DecodeBalanceEvent(statusInformation.balance_event_value, statusInformation.warningText, statusInformation.faultText);

	statusInformation.voltage_event_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.voltage_event_value != 0)
		StatusDecode_EG4::DecodeVoltageEvent(statusInformation.voltage_event_value, statusInformation.warningText, statusInformation.protectionText);

	statusInformation.temperature_event_value = ReadHexEncodedUShort(response, byteOffset);
	if (statusInformation.temperature_event_value != 0)
		StatusDecode_EG4::DecodeTemperatureEvent(statusInformation.temperature_event_value, statusInformation.warningText, statusInformation.protectionText, statusInformation.faultText);

	statusInformation.current_event_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.current_event_value != 0)
		StatusDecode_EG4::DecodeCurrentEvent(statusInformation.current_event_value, statusInformation.warningText, statusInformation.protectionText, statusInformation.faultText);

	statusInformation.remaining_capacity_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.remaining_capacity_value != 0)
		StatusDecode_EG4::DecodeRemainingCapacity(statusInformation.remaining_capacity_value, statusInformation.warningText);

	statusInformation.fet_status_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.fet_status_value != 0)
		StatusDecode_EG4::DecodeFetStatus(statusInformation.fet_status_value, statusInformation.configurationText);

	statusInformation.system_value = ReadHexEncodedByte(response, byteOffset);
	if (statusInformation.system_value != 0)
		StatusDecode_EG4::DecodeSystemStatus(statusInformation.system_value, statusInformation.systemText);

	statusInformation.balancing_value = ReadHexEncodedULong(response, byteOffset);
	for (int i = 0; i < 16; i++)
	{
		if ((statusInformation.balancing_value & (1 << i)) != 0)
		{
			statusInformation.balancingText.append(std::string("Cell ") + std::to_string(i + 1) + " is balancing; ");
		}
	}

	// "reserved"
	byteOffset += 2;

	if (byteOffset != payloadLen + 13)
		LogWarning("Length mismatch reading status information response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off");

	// pop off any trailing "; " separator
	if (statusInformation.warningText.length() > 2)
	{
		statusInformation.warningText.pop_back();
		statusInformation.warningText.pop_back();
	}
	if (statusInformation.balancingText.length() > 2)
	{
		statusInformation.balancingText.pop_back();
		statusInformation.balancingText.pop_back();
	}
	if (statusInformation.systemText.length() > 2)
	{
		statusInformation.systemText.pop_back();
		statusInformation.systemText.pop_back();
	}
	if (statusInformation.configurationText.length() > 2)
	{
		statusInformation.configurationText.pop_back();
		statusInformation.configurationText.pop_back();
	}
	if (statusInformation.protectionText.length() > 2)
	{
		statusInformation.protectionText.pop_back();
		statusInformation.protectionText.pop_back();
	}
	if (statusInformation.faultText.length() > 2)
	{
		statusInformation.faultText.pop_back();
		statusInformation.faultText.pop_back();
	}

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadHardwareVersionRequestV20[] = "~20014A510000FDA2\r";
const unsigned char PaceBmsProtocolV20::exampleReadHardwareVersionResponseV20[] = "~20014A00F05C202020202020202020202020202020202020202000005154484E2020202020202020202020202020202030640306EBA8\r";

bool PaceBmsProtocolV20::CreateReadHardwareVersionRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadHardwareVersion, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsProtocolV20::ProcessReadHardwareVersionResponse(const uint8_t busId, const std::vector<uint8_t>& response, std::string& hardwareVersion)
{
	hardwareVersion.clear();

	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	if (payloadLen != 64)
	{
		//std::string message = std::string("Documentation indicates a hardware version request should return a 64 byte payload in the response, but this response's payload length is ") + std::to_string(payloadLen);
		//LogWarning(message);
	}

	// attempt to format the garbage that off-brand BMSes return into something legible
	// leading and trailing spaces eliminated
	// multiple spaces are collapsed to a single space
	// non-printable chars converted to [decimal_value] 
	bool in_space = false;
	bool encountered_data = false;
	for (int i = 0; i < payloadLen/2; i++)
	{
		uint8_t byte = ReadHexEncodedByte(response, byteOffset);
		if (byte == ' ')
		{
			if (!in_space)
			{
				if(encountered_data)
					hardwareVersion.append(" ");
				in_space = true;
			}
			continue;
		}
		in_space = false;
		encountered_data = true;
		if (byte > 32 && byte <= 127)
			hardwareVersion.append(1, (char)byte);
		else
			hardwareVersion.append("[" + std::to_string(byte) + "]");
	}
	if (hardwareVersion[hardwareVersion.length() - 1] == ' ')
		hardwareVersion.pop_back();

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadSerialNumberRequestV20[] = "~20014A930000FD9C\r";
const unsigned char PaceBmsProtocolV20::exampleReadSerialNumberResponseV20[] = ""; // I don't have a BMS that responds to this

bool PaceBmsProtocolV20::CreateReadSerialNumberRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadSerialNumber, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsProtocolV20::ProcessReadSerialNumberResponse(const uint8_t busId, const std::vector<uint8_t>& response, std::string& serialNumber)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	if (payloadLen != 80 && payloadLen != 32)
	{
		std::string message = std::string("Documentation indicates a serial number information request should return either a 32 byte payload in the response, but this response's payload length is ") + std::to_string(payloadLen);
		LogWarning(message);
	}

	//// throwaway -- I'm torn whether to do this or not, the spec says there's a byte we don't care about 
	////     here, but that's not true of 0x25 plus who knows what off-brand BMSes do?
	//uint8_t command_value = ReadHexEncodedByte(response, byteOffset);

	// attempt to format the garbage that off-brand BMSes return into something legible
	// leading and trailing spaces eliminated
	// multiple spaces are collapsed to a single space
	// non-printable chars converted to [decimal_value] 
	bool in_space = false;
	bool encountered_data = false;
	for (int i = 0; i < payloadLen / 2; i++)
	{
		uint8_t byte = ReadHexEncodedByte(response, byteOffset);
		if (byte == ' ')
		{
			if (!in_space)
			{
				if (encountered_data)
					serialNumber.append(" ");
				in_space = true;
			}
			continue;
		}
		in_space = false;
		encountered_data = true;
		if (byte > 32 && byte <= 127)
			serialNumber.append(1, (char)byte);
		else
			serialNumber.append("[" + std::to_string(byte) + "]");
	}
	if (serialNumber[serialNumber.length() - 1] == ' ')
		serialNumber.pop_back();

	return true;
}

                                                         // "system reboot" EG4: ~20014A45C004060FFCAC\r
const unsigned char PaceBmsProtocolV20::exampleWriteRebootCommandRequestV20[] = "";
const unsigned char PaceBmsProtocolV20::exampleWriteRebootCommandResponseV20[] = "";

bool PaceBmsProtocolV20::CreateWriteShutdownCommandRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	// the payload is the mosfet state to set
	const uint16_t payloadLen = 2;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, busId);

	CreateRequest(busId, CID2_WriteShutdownCommand, payload, request);

	return true;
}
bool PaceBmsProtocolV20::ProcessWriteShutdownCommandResponse(const uint8_t busId, const std::vector<uint8_t>& response)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	if (payloadLen != 0)
	{
		std::string message = std::string("Documentation indicates a shutdown command should return no payload, but this response's payload length is ") + std::to_string(payloadLen);
		LogError(message);
		return false;
	}

	// according to documentation, if the RTN code is 0 (this is checked by ValidateResponseAndGetPayloadLength) then it worked, no need to check response payload

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadSystemTimeRequestV20[]   = "~20014A4D0000FD90\r";
const unsigned char PaceBmsProtocolV20::exampleReadSystemTimeResponseV20[]  = "~20014A00200E07E809110B3B1FFA84\r";
const unsigned char PaceBmsProtocolV20::exampleWriteSystemTimeRequestV20[]  = "~20014A4E200E07E809110C0402FA90\r";
const unsigned char PaceBmsProtocolV20::exampleWriteSystemTimeResponseV20[] = "~20014A000000FDA8\r";

bool PaceBmsProtocolV20::CreateReadSystemDateTimeRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadDateTime, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsProtocolV20::ProcessReadSystemDateTimeResponse(const uint8_t busId, const std::vector<uint8_t>& response, DateTime& dateTime)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	dateTime.Year = ReadHexEncodedUShort(response, byteOffset);
	dateTime.Month = ReadHexEncodedByte(response, byteOffset);
	dateTime.Day = ReadHexEncodedByte(response, byteOffset);
	dateTime.Hour = ReadHexEncodedByte(response, byteOffset);
	dateTime.Minute = ReadHexEncodedByte(response, byteOffset);
	dateTime.Second = ReadHexEncodedByte(response, byteOffset);

	return true;
}
bool PaceBmsProtocolV20::CreateWriteSystemDateTimeRequest(const uint8_t busId, const DateTime dateTime, std::vector<uint8_t>& request)
{
	const uint16_t payloadLen = 14;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedUShort(payload, payloadOffset, dateTime.Year);
	WriteHexEncodedByte(payload, payloadOffset, dateTime.Month);
	WriteHexEncodedByte(payload, payloadOffset, dateTime.Day);
	WriteHexEncodedByte(payload, payloadOffset, dateTime.Hour);
	WriteHexEncodedByte(payload, payloadOffset, dateTime.Minute);
	WriteHexEncodedByte(payload, payloadOffset, dateTime.Second);

	CreateRequest(busId, CID2_WriteDateTime, payload, request);

	return true;
}
bool PaceBmsProtocolV20::ProcessWriteSystemDateTimeResponse(const uint8_t busId, const std::vector<uint8_t>& response)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	if (payloadLen != 0)
	{
		std::string message = std::string("Documentation indicates a write system time response should return no payload, but this response's payload length is ") + std::to_string(payloadLen);
		LogError(message);
		return false;
	}

	return true;
}
