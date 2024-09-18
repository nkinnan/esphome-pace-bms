
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

/*
// ============================================================================
// 
// Main "Realtime Monitoring" tab of PBmsTools 2.4
// These are in the "Switch Control" section
// 
// ============================================================================

const unsigned char PaceBmsProtocolV20::exampleWriteDisableBuzzerSwitchCommandRequestV20[] = "~25004699E0020DFD12\r";
const unsigned char PaceBmsProtocolV20::exampleWriteDisableBuzzerSwitchCommandResponseV20[] = "~25004600C0040D01FCC3\r";
const unsigned char PaceBmsProtocolV20::exampleWriteEnableBuzzerSwitchCommandRequestV20[] = "~25004699E0020CFD13\r";
const unsigned char PaceBmsProtocolV20::exampleWriteEnableBuzzerSwitchCommandResponseV20[] = "~25004600C0040C00FCC5\r";

const unsigned char PaceBmsProtocolV20::exampleWriteDisableLedWarningSwitchCommandRequestV20[] = "~25004699E00206FD20\r";
const unsigned char PaceBmsProtocolV20::exampleWriteDisableLedWarningSwitchCommandResponseV20[] = "~25004600C0040602FCD0\r";
const unsigned char PaceBmsProtocolV20::exampleWriteEnableLedWarningSwitchCommandRequestV20[] = "~25004699E00207FD1F\r";
const unsigned char PaceBmsProtocolV20::exampleWriteEnableLedWarningSwitchCommandResponseV20[] = "~25004600C0040722FCCD\r";

const unsigned char PaceBmsProtocolV20::exampleWriteDisableChargeCurrentLimiterSwitchCommandRequestV20[] = "~25004699E0020AFD15\r";
const unsigned char PaceBmsProtocolV20::exampleWriteDisableChargeCurrentLimiterSwitchCommandResponseV20[] = "~25004600C0040A22FCC3\r";
const unsigned char PaceBmsProtocolV20::exampleWriteEnableChargeCurrentLimiterSwitchCommandRequestV20[] = "~25004699E0020BFD14\r";
const unsigned char PaceBmsProtocolV20::exampleWriteEnableChargeCurrentLimiterSwitchCommandResponseV20[] = "~25004600C0040B32FCC1\r";

const unsigned char PaceBmsProtocolV20::exampleWriteSetChargeCurrentLimiterCurrentLimitLowGearSwitchCommandRequestV20[] = "~25004699E00209FD1D\r";
const unsigned char PaceBmsProtocolV20::exampleWriteSetChargeCurrentLimiterCurrentLimitLowGearSwitchCommandResponseV20[] = "~25004600C0040938FCC4\r";
const unsigned char PaceBmsProtocolV20::exampleWriteSetChargeCurrentLimiterCurrentLimitHighGearSwitchCommandRequestV20[] = "~25004699E00208FD1E\r";
const unsigned char PaceBmsProtocolV20::exampleWriteSetChargeCurrentLimiterCurrentLimitHighGearSwitchCommandResponseV20[] = "~25004600C0040830FCCD\r";

bool PaceBmsProtocolV20::CreateWriteSwitchCommandRequest(const uint8_t busId, const SwitchCommand command, std::vector<uint8_t>& request)
{
	// the payload is the control command code
	const uint16_t payloadLen = 2;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, command);

	CreateRequest(busId, CID2_WriteSwitchCommand, payload, request);

	return true;
}
bool PaceBmsProtocolV20::ProcessWriteSwitchCommandResponse(const uint8_t busId, const SwitchCommand command, const std::vector<uint8_t>& response)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	// documented as 2 (decoded) bytes but the meaning of them is undocumented
	// in any case this is the only thing I can be certain enough about to elevate to error status and return failure
	if (payloadLen != 4)
	{
		std::string message = std::string("Documentation indicates a switch command should return a 4 byte payload in the response, but this response's payload length is ") + std::to_string(payloadLen);
		LogError(message);
		return false;
	}

	uint8_t commandEcho = ReadHexEncodedByte(response, byteOffset);

	// this is behavior I have observed but is not documented
	if (commandEcho != command)
	{
		LogWarning("Switch command response did not echo back the switch command");
		//return false;
	}

	// this is behavior I have observed but is not documented
	uint8_t unknown = ReadHexEncodedByte(response, byteOffset);
	switch (command)
	{
	case SC_DisableBuzzer:
		if (unknown != 0x01)
		{
			LogWarning("Undocumented payload byte does not match reverse engineering observation");
		}
		break;
	case SC_EnableBuzzer:
		if (unknown != 0x00)
		{
			LogWarning("Undocumented payload byte does not match reverse engineering observation");
		}
		break;
	case SC_DisableLedWarning:
		if (unknown != 0x02)
		{
			LogWarning("Undocumented payload byte does not match reverse engineering observation");
		}
		break;
	case SC_EnableLedWarning:
		if (unknown != 0x22)
		{
			LogWarning("Undocumented payload byte does not match reverse engineering observation");
		}
		break;
	case SC_SetChargeCurrentLimiterCurrentLimitHighGear:
		if (unknown != 0x30)
		{
			LogWarning("Undocumented payload byte does not match reverse engineering observation");
		}
		break;
	case SC_SetChargeCurrentLimiterCurrentLimitLowGear:
		if (unknown != 0x38)
		{
			LogWarning("Undocumented payload byte does not match reverse engineering observation");
		}
		break;
	case SC_DisableChargeCurrentLimiter:
		if (unknown != 0x22)
		{
			LogWarning("Undocumented payload byte does not match reverse engineering observation");
		}
		break;
	case SC_EnableChargeCurrentLimiter:
		if (unknown != 0x32)
		{
			LogWarning("Undocumented payload byte does not match reverse engineering observation");
		}
		break;
	}

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleWriteMosfetChargeOpenSwitchCommandRequestV20[] = "~2500469AE00200FD1E\r";
const unsigned char PaceBmsProtocolV20::exampleWriteMosfetChargeOpenSwitchCommandResponseV20[] = "~25004600E00226FD30\r";
const unsigned char PaceBmsProtocolV20::exampleWriteMosfetChargeCloseSwitchCommandRequestV20[] = "~2500469AE00201FD1D\r";
const unsigned char PaceBmsProtocolV20::exampleWriteMosfetChargeCloseSwitchCommandResponseV20[] = "~25004600E00224FD32\r";

const unsigned char PaceBmsProtocolV20::exampleWriteMosfetDischargeOpenSwitchCommandRequestV20[] = "~2500469BE00200FD1D\r";
const unsigned char PaceBmsProtocolV20::exampleWriteMosfetDischargeOpenSwitchCommandResponseV20[] = "~25004600E00204FD34\r";
const unsigned char PaceBmsProtocolV20::exampleWriteMosfetDischargeCloseSwitchCommandRequestV20[] = "~2500469BE00201FD1C\r";
const unsigned char PaceBmsProtocolV20::exampleWriteMosfetDischargeCloseSwitchCommandResponseV20[] = "~25004609E00204FD2B\r";

bool PaceBmsProtocolV20::CreateWriteMosfetSwitchCommandRequest(const uint8_t busId, const MosfetType type, const MosfetState command, std::vector<uint8_t>& request)
{
	// the payload is the mosfet state to set
	const uint16_t payloadLen = 2;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, command);

	CreateRequest(busId, (CID2)type, payload, request);

	return true;
}
bool PaceBmsProtocolV20::ProcessWriteMosfetSwitchCommandResponse(const uint8_t busId, const MosfetType type, const MosfetState command, const std::vector<uint8_t>& response)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	// documented as 1 (decoded) byte but the meaning of it is undocumented
	// in any case this is the only thing I can be certain enough about to elevate to error status and return failure
	if (payloadLen != 2)
	{
		std::string message = std::string("Documentation indicates a MOSFET command should return a 2 byte payload in the response, but this response's payload length is ") + std::to_string(payloadLen);
		LogError(message);
		return false;
	}

	// this is behavior I have observed but is not documented
	uint8_t unknown = ReadHexEncodedByte(response, byteOffset);
	if (type == MT_Charge && command == MS_Open)
	{
		if (unknown != 0x26)
		{
			LogWarning("Undocumented payload byte does not match reverse engineering observation");
		}
	}
	if (type == MT_Charge && command == MS_Close)
	{
		if (unknown != 0x24)
		{
			LogWarning("Undocumented payload byte does not match reverse engineering observation");
		}
	}
	if (type == MT_Discharge && command == MS_Open)
	{
		if (unknown != 0x04)
		{
			LogWarning("Undocumented payload byte does not match reverse engineering observation");
		}
	}
	if (type == MT_Discharge && command == MS_Close)
	{
		if (unknown != 0x04)
		{
			LogWarning("Undocumented payload byte does not match reverse engineering observation");
		}
	}

	return true;
}
*/
const unsigned char PaceBmsProtocolV20::exampleWriteRebootCommandRequestV20[] = "~2500469CE00201FD1B\r";
const unsigned char PaceBmsProtocolV20::exampleWriteRebootCommandResponseV20[] = "~250046000000FDAF\r";

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

// ============================================================================
// 
// "Memory Information" tab of PBmsTools 2.4 
// 
// ============================================================================

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
/*
// ============================================================================
// 
// "Parameter Setting" tab of PBmsTools 2.4 with DIP set to address 00 
// PBmsTools 2.4 is broken and can't address packs other than 00 for configuration
// 
// ============================================================================

bool PaceBmsProtocolV20::CreateReadConfigurationRequest(const uint8_t busId, const ReadConfigurationType configType, std::vector<uint8_t>& request)
{
	CreateRequest(busId, (CID2)configType, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsProtocolV20::ProcessWriteConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	if (payloadLen != 0)
	{
		std::string message = std::string("Documentation indicates a write configuration response should return no payload, but this response's payload length is ") + std::to_string(payloadLen);
		LogError(message);
		return false;
	}

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadCellOverVoltageConfigurationRequestV20[] = "~250046D10000FD9A\r";
const unsigned char PaceBmsProtocolV20::exampleReadCellOverVoltageConfigurationResponseV20[] = "~25004600F010010E100E740D340AFA35\r";
const unsigned char PaceBmsProtocolV20::exampleWriteCellOverVoltageConfigurationRequestV20[] = "~250046D0F010010E100E740D340AFA21\r";
const unsigned char PaceBmsProtocolV20::exampleWriteCellOverVoltageConfigurationResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, CellOverVoltageConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	// todo: everywhere check payload length is what's expected

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	uint16_t unknown = ReadHexEncodedByte(response, byteOffset);
	if (unknown != 01)
	{
		LogWarning("Unknown payload byte does not match previously observed value");
		return false;
	}

	config.AlarmMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionReleaseMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionDelayMilliseconds = ReadHexEncodedByte(response, byteOffset) * 100;

	return true;
}
bool PaceBmsProtocolV20::CreateWriteConfigurationRequest(const uint8_t busId, const CellOverVoltageConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.AlarmMillivolts < 2500 || config.AlarmMillivolts > 4500)
	{
		LogError("AlarmVoltage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.AlarmMillivolts % 10 != 0)
	{
		LogError("AlarmVoltage should be in steps of 0.01 volts");
		return false;
	}
	if (config.ProtectionMillivolts < 2500 || config.ProtectionMillivolts > 4500)
	{
		LogError("ProtectionVoltage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionMillivolts % 10 != 0)
	{
		LogError("ProtectionVoltage should be in steps of 0.01 volts");
		return false;
	}
	if (config.ProtectionReleaseMillivolts < 2500 || config.ProtectionReleaseMillivolts > 4500)
	{
		LogError("ProtectionReleaseVoltage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionReleaseMillivolts % 10 != 0)
	{
		LogError("ProtectionReleaseVoltage should be in steps of 0.01 volts");
		return false;
	}
	if (config.ProtectionDelayMilliseconds < 1000 || config.ProtectionDelayMilliseconds > 20000)
	{
		LogError("ProtectionDelaySeconds is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionDelayMilliseconds % 500 != 0)
	{
		LogError("ProtectionDelaySeconds should be in steps of 0.5 seconds");
		return false;
	}

	const uint16_t payloadLen = 16;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	// unknown value
	WriteHexEncodedByte(payload, payloadOffset, 0x01);
	WriteHexEncodedUShort(payload, payloadOffset, config.AlarmMillivolts);
	WriteHexEncodedUShort(payload, payloadOffset, config.ProtectionMillivolts);
	WriteHexEncodedUShort(payload, payloadOffset, config.ProtectionReleaseMillivolts);
	WriteHexEncodedByte(payload, payloadOffset, config.ProtectionDelayMilliseconds / 100);

	CreateRequest(busId, CID2_WriteCellOverVoltageConfiguration, payload, request);

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadPackOverVoltageConfigurationRequestV20[] = "~250046D50000FD96\r";
const unsigned char PaceBmsProtocolV20::exampleReadPackOverVoltageConfigurationResponseV20[] = "~25004600F01001E100E740D2F00AFA24\r";
const unsigned char PaceBmsProtocolV20::exampleWritePackOverVoltageConfigurationRequestV20[] = "~250046D4F01001E10AE740D2F00AF9FB\r";
const unsigned char PaceBmsProtocolV20::exampleWritePackOverVoltageConfigurationResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t> response, PackOverVoltageConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	uint16_t unknown = ReadHexEncodedByte(response, byteOffset);
	if (unknown != 01)
	{
		LogWarning("Unknown payload byte does not match previously observed value");
		return false;
	}

	config.AlarmMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionReleaseMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionDelayMilliseconds = ReadHexEncodedByte(response, byteOffset) * 100;

	return true;
}
bool PaceBmsProtocolV20::CreateWriteConfigurationRequest(const uint8_t busId, const PackOverVoltageConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.AlarmMillivolts < 20000 || config.AlarmMillivolts > 65000)
	{
		LogError("AlarmVoltage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.AlarmMillivolts % 10)
	{
		LogError("AlarmVoltage should be in steps of 0.01");
		return false;
	}
	if (config.ProtectionMillivolts < 20000 || config.ProtectionMillivolts > 65000)
	{
		LogError("ProtectionVoltage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionMillivolts % 10 != 0)
	{
		LogError("ProtectionVoltage should be in steps of 0.01");
		return false;
	}
	if (config.ProtectionReleaseMillivolts < 20000 || config.ProtectionReleaseMillivolts > 65000)
	{
		LogError("ProtectionReleaseVoltage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionReleaseMillivolts % 10 != 0)
	{
		LogError("ProtectionReleaseVoltage should be in steps of 0.01");
		return false;
	}
	if (config.ProtectionDelayMilliseconds < 1000 || config.ProtectionDelayMilliseconds > 20000)
	{
		LogError("ProtectionDelayMilliseconds is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionDelayMilliseconds % 500 != 0)
	{
		LogError("ProtectionDelayMilliseconds should be in steps of 500");
		return false;
	}

	const uint16_t payloadLen = 16;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	// unknown value
	WriteHexEncodedByte(payload, payloadOffset, 0x01);
	WriteHexEncodedUShort(payload, payloadOffset, config.AlarmMillivolts);
	WriteHexEncodedUShort(payload, payloadOffset, config.ProtectionMillivolts);
	WriteHexEncodedUShort(payload, payloadOffset, config.ProtectionReleaseMillivolts);
	WriteHexEncodedByte(payload, payloadOffset, config.ProtectionDelayMilliseconds / 100);

	CreateRequest(busId, CID2_WritePackOverVoltageConfiguration, payload, request);

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadCellUnderVoltageConfigurationRequestV20[] = "~250046D30000FD98\r";
const unsigned char PaceBmsProtocolV20::exampleReadCellUnderVoltageConfigurationResponseV20[] = "~25004600F010010AF009C40B540AFA24\r";
const unsigned char PaceBmsProtocolV20::exampleWriteCellUnderVoltageConfigurationRequestV20[] = "~250046D2F010010AF009C40B540AFA0E\r";
const unsigned char PaceBmsProtocolV20::exampleWriteCellUnderVoltageConfigurationResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, CellUnderVoltageConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	uint16_t unknown = ReadHexEncodedByte(response, byteOffset);
	if (unknown != 01)
	{
		LogWarning("Unknown payload byte does not match previously observed value");
		return false;
	}

	config.AlarmMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionReleaseMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionDelayMilliseconds = ReadHexEncodedByte(response, byteOffset) * 100;

	return true;
}
bool PaceBmsProtocolV20::CreateWriteConfigurationRequest(const uint8_t busId, const CellUnderVoltageConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.AlarmMillivolts < 2000 || config.AlarmMillivolts > 3500)
	{
		LogError("AlarmVoltage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.AlarmMillivolts % 10 != 0)
	{
		LogError("AlarmVoltage should be in steps of 0.01");
		return false;
	}
	if (config.ProtectionMillivolts < 2000 || config.ProtectionMillivolts > 3500)
	{
		LogError("ProtectionVoltage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionMillivolts % 10 != 0)
	{
		LogError("ProtectionVoltage should be in steps of 0.01");
		return false;
	}
	if (config.ProtectionReleaseMillivolts < 2000 || config.ProtectionReleaseMillivolts > 3500)
	{
		LogError("ProtectionReleaseVoltage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionReleaseMillivolts % 10 != 0)
	{
		LogError("ProtectionReleaseVoltage should be in steps of 0.01");
		return false;
	}
	if (config.ProtectionDelayMilliseconds < 1000 || config.ProtectionDelayMilliseconds > 20000)
	{
		LogError("ProtectionDelayMilliseconds is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionDelayMilliseconds % 500 != 0)
	{
		LogError("ProtectionDelayMilliseconds should be in steps of 500");
		return false;
	}

	const uint16_t payloadLen = 16;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	// unknown value
	WriteHexEncodedByte(payload, payloadOffset, 0x01);
	WriteHexEncodedUShort(payload, payloadOffset, config.AlarmMillivolts);
	WriteHexEncodedUShort(payload, payloadOffset, config.ProtectionMillivolts);
	WriteHexEncodedUShort(payload, payloadOffset, config.ProtectionReleaseMillivolts);
	WriteHexEncodedByte(payload, payloadOffset, config.ProtectionDelayMilliseconds / 100);

	CreateRequest(busId, CID2_WriteCellUnderVoltageConfiguration, payload, request);

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadPackUnderVoltageConfigurationRequestV20[] = "~250046D70000FD94\r";
const unsigned char PaceBmsProtocolV20::exampleReadPackUnderVoltageConfigurationResponseV20[] = "~25004600F01001AF009C40B5400AFA24\r";
const unsigned char PaceBmsProtocolV20::exampleWritePackUnderVoltageConfigurationRequestV20[] = "~250046D6F01001AF009C40B5400AFA0A\r";
const unsigned char PaceBmsProtocolV20::exampleWritePackUnderVoltageConfigurationResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, PackUnderVoltageConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	uint16_t unknown = ReadHexEncodedByte(response, byteOffset);
	if (unknown != 01)
	{
		LogWarning("Unknown payload byte does not match previously observed value");
		return false;
	}

	config.AlarmMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionReleaseMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionDelayMilliseconds = ReadHexEncodedByte(response, byteOffset) * 100;

	return true;
}
bool PaceBmsProtocolV20::CreateWriteConfigurationRequest(const uint8_t busId, const PackUnderVoltageConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.AlarmMillivolts < 15000 || config.AlarmMillivolts > 50000)
	{
		LogError("AlarmVoltage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.AlarmMillivolts % 10 != 0)
	{
		LogError("AlarmVoltage should be in steps of 0.01");
		return false;
	}
	if (config.ProtectionMillivolts < 15000 || config.ProtectionMillivolts > 50000)
	{
		LogError("ProtectionVoltage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionMillivolts % 10 != 0)
	{
		LogError("ProtectionVoltage should be in steps of 0.01");
		return false;
	}
	if (config.ProtectionReleaseMillivolts < 15000 || config.ProtectionReleaseMillivolts > 50000)
	{
		LogError("ProtectionReleaseVoltage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionReleaseMillivolts % 10 != 0)
	{
		LogError("ProtectionReleaseVoltage should be in steps of 0.01");
		return false;
	}
	if (config.ProtectionDelayMilliseconds < 1000 || config.ProtectionDelayMilliseconds > 20000)
	{
		LogError("ProtectionDelayMilliseconds is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionDelayMilliseconds % 500 != 0)
	{
		LogError("ProtectionDelayMilliseconds should be in steps of 500");
		return false;
	}

	const uint16_t payloadLen = 16;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	// unknown value
	WriteHexEncodedByte(payload, payloadOffset, 0x01);
	WriteHexEncodedUShort(payload, payloadOffset, config.AlarmMillivolts);
	WriteHexEncodedUShort(payload, payloadOffset, config.ProtectionMillivolts);
	WriteHexEncodedUShort(payload, payloadOffset, config.ProtectionReleaseMillivolts);
	WriteHexEncodedByte(payload, payloadOffset, config.ProtectionDelayMilliseconds / 100);

	CreateRequest(busId, CID2_WritePackUnderVoltageConfiguration, payload, request);

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadChargeOverCurrentConfigurationRequestV20[] = "~250046D90000FD92\r";
const unsigned char PaceBmsProtocolV20::exampleReadChargeOverCurrentConfigurationResponseV20[] = "~25004600400C010068006E0AFB1D\r";
const unsigned char PaceBmsProtocolV20::exampleWriteChargeOverCurrentConfigurationRequestV20[] = "~250046D8400C010068006E0AFB01\r";
const unsigned char PaceBmsProtocolV20::exampleWriteChargeOverCurrentConfigurationResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, ChargeOverCurrentConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	uint16_t unknown = ReadHexEncodedByte(response, byteOffset);
	if (unknown != 01)
	{
		LogWarning("Unknown payload byte does not match previously observed value");
		return false;
	}

	config.AlarmAmperage = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionAmperage = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionDelayMilliseconds = ReadHexEncodedByte(response, byteOffset) * 100;

	return true;
}
bool PaceBmsProtocolV20::CreateWriteConfigurationRequest(const uint8_t busId, const ChargeOverCurrentConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.AlarmAmperage < 1 || config.AlarmAmperage > 220)
	{
		LogError("AlarmAmperage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionAmperage < 1 || config.ProtectionAmperage > 220)
	{
		LogError("ProtectionAmperage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionDelayMilliseconds < 500 || config.ProtectionDelayMilliseconds > 25000)
	{
		LogError("ProtectionDelayMilliseconds is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionDelayMilliseconds % 500 != 0)
	{
		LogError("ProtectionDelayMilliseconds should be in steps of 500");
		return false;
	}

	const uint16_t payloadLen = 12;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	// unknown value
	WriteHexEncodedByte(payload, payloadOffset, 0x01);
	WriteHexEncodedUShort(payload, payloadOffset, config.AlarmAmperage);
	WriteHexEncodedUShort(payload, payloadOffset, config.ProtectionAmperage);
	WriteHexEncodedByte(payload, payloadOffset, config.ProtectionDelayMilliseconds / 100);

	CreateRequest(busId, CID2_WriteChargeOverCurrentConfiguration, payload, request);

	// todo: (everywhere) check alignment of current byteOffset and payloadLen

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadDishargeOverCurrent1ConfigurationRequestV20[] = "~250046DB0000FD89\r";
const unsigned char PaceBmsProtocolV20::exampleReadDishargeOverCurrent1ConfigurationResponseV20[] = "~25004600400C01FF97FF920AFAD3\r";
const unsigned char PaceBmsProtocolV20::exampleWriteDishargeOverCurrent1ConfigurationRequestV20[] = "~250046DA400C010069006E0AFAF7\r";
const unsigned char PaceBmsProtocolV20::exampleWriteDishargeOverCurrent1ConfigurationResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, DischargeOverCurrent1Configuration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	uint16_t unknown = ReadHexEncodedByte(response, byteOffset);
	if (unknown != 01)
	{
		LogWarning("Unknown payload byte does not match previously observed value");
		return false;
	}

	config.AlarmAmperage = ReadHexEncodedSShort(response, byteOffset) * -1;
	config.ProtectionAmperage = ReadHexEncodedSShort(response, byteOffset) * -1;
	config.ProtectionDelayMilliseconds = ReadHexEncodedByte(response, byteOffset) * 100;

	return true;
}
bool PaceBmsProtocolV20::CreateWriteConfigurationRequest(const uint8_t busId, const DischargeOverCurrent1Configuration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.AlarmAmperage < 1 || config.AlarmAmperage > 220)
	{
		LogError("AlarmAmperage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionAmperage < 1 || config.ProtectionAmperage > 220)
	{
		LogError("ProtectionAmperage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionDelayMilliseconds < 500 || config.ProtectionDelayMilliseconds > 25000)
	{
		LogError("ProtectionDelayMilliseconds is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionDelayMilliseconds % 500 != 0)
	{
		LogError("ProtectionDelayMilliseconds should be in steps of 500");
		return false;
	}

	const uint16_t payloadLen = 12;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	// unknown value
	WriteHexEncodedByte(payload, payloadOffset, 0x01);
	WriteHexEncodedUShort(payload, payloadOffset, config.AlarmAmperage);
	WriteHexEncodedUShort(payload, payloadOffset, config.ProtectionAmperage);
	WriteHexEncodedByte(payload, payloadOffset, config.ProtectionDelayMilliseconds / 100);

	CreateRequest(busId, CID2_WriteDischargeSlowOverCurrentConfiguration, payload, request);

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadDishargeOverCurrent2ConfigurationRequestV20[] = "~250046E30000FD97\r";
const unsigned char PaceBmsProtocolV20::exampleReadDishargeOverCurrent2ConfigurationResponseV20[] = "~25004600400C009604009604FB32\r";
const unsigned char PaceBmsProtocolV20::exampleWriteDishargeOverCurrent2ConfigurationRequestV20[] = "~250046E2A006009604FC4E\r";
const unsigned char PaceBmsProtocolV20::exampleWriteDishargeOverCurrent2ConfigurationResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, DischargeOverCurrent2Configuration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	uint16_t unknown = ReadHexEncodedByte(response, byteOffset);
	if (unknown != 0)
	{
		LogWarning("Unknown payload byte does not match previously observed value");
		return false;
	}

	config.ProtectionAmperage = ReadHexEncodedByte(response, byteOffset);
	config.ProtectionDelayMilliseconds = ReadHexEncodedByte(response, byteOffset) * 25;

	// ignore the garbage tail, likely firmware bug since it's not sent on the write

	return true;
}
bool PaceBmsProtocolV20::CreateWriteConfigurationRequest(const uint8_t busId, const DischargeOverCurrent2Configuration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.ProtectionAmperage < 5 || config.ProtectionAmperage > 255)
	{
		LogError("ProtectionAmperage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionAmperage % 5 != 0)
	{
		LogError("ProtectionAmperage should be in steps of 5");
		return false;
	}
	if (config.ProtectionDelayMilliseconds < 100 || config.ProtectionDelayMilliseconds > 2000)
	{
		LogError("ProtectionDelayMilliseconds is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionDelayMilliseconds % 100 != 0)
	{
		LogError("ProtectionDelayMilliseconds should be in steps of 100");
		return false;
	}

	const uint16_t payloadLen = 6;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	// unknown value
	WriteHexEncodedByte(payload, payloadOffset, 0x00);
	WriteHexEncodedByte(payload, payloadOffset, config.ProtectionAmperage);
	WriteHexEncodedByte(payload, payloadOffset, config.ProtectionDelayMilliseconds / 25);

	CreateRequest(busId, CID2_WriteDischargeFastOverCurrentConfiguration, payload, request);

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadShortCircuitProtectionConfigurationRequestV20[] = "~250046E50000FD95\r";
const unsigned char PaceBmsProtocolV20::exampleReadShortCircuitProtectionConfigurationResponseV20[] = "~25004600E0020CFD25\r";
const unsigned char PaceBmsProtocolV20::exampleWriteShortCircuitProtectionConfigurationRequestV20[] = "~250046E4E0020CFD0C\r";
const unsigned char PaceBmsProtocolV20::exampleWriteShortCircuitProtectionConfigurationResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, ShortCircuitProtectionConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	config.ProtectionDelayMicroseconds = ReadHexEncodedByte(response, byteOffset) * 25;

	return true;
}
bool PaceBmsProtocolV20::CreateWriteConfigurationRequest(const uint8_t busId, const ShortCircuitProtectionConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.ProtectionDelayMicroseconds < 100 || config.ProtectionDelayMicroseconds > 500)
	{
		LogError("ProtectionDelayMicroseconds is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionDelayMicroseconds % 50 != 0)
	{
		LogError("ProtectionDelayMicroseconds should be in steps of 50");
		return false;
	}

	const uint16_t payloadLen = 2;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, config.ProtectionDelayMicroseconds / 25);

	CreateRequest(busId, CID2_WriteShortCircuitProtectionConfiguration, payload, request);

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadCellBalancingConfigurationRequestV20[] = "~250046B60000FD97\r";
const unsigned char PaceBmsProtocolV20::exampleReadCellBalancingConfigurationResponseV20[] = "~2500460080080D48001EFBE9\r";
const unsigned char PaceBmsProtocolV20::exampleWriteCellBalancingConfigurationRequestV20[] = "~250046B580080D48001EFBD2\r";
const unsigned char PaceBmsProtocolV20::exampleWriteCellBalancingConfigurationResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, CellBalancingConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	config.ThresholdMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.DeltaCellMillivolts = ReadHexEncodedUShort(response, byteOffset);

	return true;
}
bool PaceBmsProtocolV20::CreateWriteConfigurationRequest(const uint8_t busId, const CellBalancingConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.ThresholdMillivolts < 3300 || config.ThresholdMillivolts > 4500)
	{
		LogError("ThresholdVolts is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ThresholdMillivolts % 10 != 0)
	{
		LogError("ThresholdVolts should be in steps of 0.01");
		return false;
	}
	if (config.DeltaCellMillivolts < 20 || config.DeltaCellMillivolts > 500)
	{
		LogError("DeltaCellMillivolts is not in the range that PBmsTools would send (or expect back)");
		return false;
	}

	const uint16_t payloadLen = 8;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedUShort(payload, payloadOffset, config.ThresholdMillivolts);
	WriteHexEncodedUShort(payload, payloadOffset, config.DeltaCellMillivolts);

	CreateRequest(busId, CID2_WriteCellBalancingConfiguration, payload, request);

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadSleepConfigurationRequestV20[] = "~250046A00000FD9E\r";
const unsigned char PaceBmsProtocolV20::exampleReadSleepConfigurationResponseV20[] = "~2500460080080C1C0005FBF3\r";
const unsigned char PaceBmsProtocolV20::exampleWriteSleepConfigurationRequestV20[] = "~250046A880080C1C0005FBDA\r";
const unsigned char PaceBmsProtocolV20::exampleWriteSleepConfigurationResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, SleepConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	config.CellMillivolts = ReadHexEncodedUShort(response, byteOffset);
	uint8_t unknown2 = ReadHexEncodedByte(response, byteOffset);
	config.DelayMinutes = ReadHexEncodedByte(response, byteOffset);

	if (unknown2 != 0)
	{
		LogError("Unknown2 value in payload is not zero");
		return false;
	}

	return true;
}
bool PaceBmsProtocolV20::CreateWriteConfigurationRequest(const uint8_t busId, const SleepConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.CellMillivolts < 2000 || config.CellMillivolts > 4000)
	{
		LogError("CellVoltage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.CellMillivolts % 10 != 0)
	{
		LogError("CellVoltage should be in steps of 0.01");
		return false;
	}
	if (config.DelayMinutes < 1.0f || config.DelayMinutes > 120.0f)
	{
		LogError("DelayMinutes is not in the range that PBmsTools would send (or expect back)");
		return false;
	}

	const uint16_t payloadLen = 8;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedUShort(payload, payloadOffset, config.CellMillivolts);
	WriteHexEncodedByte(payload, payloadOffset, 0x00);
	WriteHexEncodedByte(payload, payloadOffset, config.DelayMinutes);

	CreateRequest(busId, CID2_WriteSleepConfiguration, payload, request);

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadFullChargeLowChargeConfigurationRequestV20[] = "~250046AF0000FD88\r";
const unsigned char PaceBmsProtocolV20::exampleReadFullChargeLowChargeConfigurationResponseV20[] = "~25004600600ADAC007D005FB60\r";
const unsigned char PaceBmsProtocolV20::exampleWriteFullChargeLowChargeConfigurationRequestV20[] = "~250046AE600ADAC007D005FB3A\r";
const unsigned char PaceBmsProtocolV20::exampleWriteFullChargeLowChargeConfigurationResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, FullChargeLowChargeConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	config.FullChargeMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.FullChargeMilliamps = ReadHexEncodedUShort(response, byteOffset);
	config.LowChargeAlarmPercent = ReadHexEncodedByte(response, byteOffset);

	return true;
}
bool PaceBmsProtocolV20::CreateWriteConfigurationRequest(const uint8_t busId, const FullChargeLowChargeConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.FullChargeMillivolts < 20000 || config.FullChargeMillivolts > 65000)
	{
		LogError("FullChargeVoltage is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.FullChargeMillivolts % 10 != 0)
	{
		LogError("FullChargeVoltage should be in steps of 0.001");
		return false;
	}
	if (config.FullChargeMilliamps < 500 || config.FullChargeMilliamps > 5000)
	{
		LogError("FullChargeCurrentMilliamps is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.FullChargeMilliamps % 500 != 0)
	{
		LogError("FullChargeCurrentMilliamps should be in steps of 500");
		return false;
	}
	if (config.LowChargeAlarmPercent < 0 || config.LowChargeAlarmPercent > 100)
	{
		LogError("LowChargeAlarmPercent is not in the range that PBmsTools would send (or expect back)");
		return false;
	}

	const uint16_t payloadLen = 10;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedUShort(payload, payloadOffset, config.FullChargeMillivolts);
	WriteHexEncodedUShort(payload, payloadOffset, config.FullChargeMilliamps);
	WriteHexEncodedByte(payload, payloadOffset, config.LowChargeAlarmPercent);

	CreateRequest(busId, CID2_WriteFullChargeLowChargeConfiguration, payload, request);

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadChargeAndDischargeOverTemperatureConfigurationRequestV20[] = "~250046DD0000FD87\r";
const unsigned char PaceBmsProtocolV20::exampleReadChargeAndDischargeOverTemperatureConfigurationResponseV20[] = "~25004600501A010CA80CD00C9E0CDA0D020CD0F7BE\r";
const unsigned char PaceBmsProtocolV20::exampleWriteChargeAndDischargeOverTemperatureConfigurationRequestV20[] = "~250046DC501A010CA80CD00C9E0CDA0D020CD0F797\r";
const unsigned char PaceBmsProtocolV20::exampleWriteChargeAndDischargeOverTemperatureConfigurationResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, ChargeAndDischargeOverTemperatureConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	uint16_t unknown = ReadHexEncodedByte(response, byteOffset);
	if (unknown != 01)
	{
		LogWarning("Unknown payload byte does not match previously observed value");
		return false;
	}

	config.ChargeAlarm = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.ChargeProtection = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.ChargeProtectionRelease = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.DischargeAlarm = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.DischargeProtection = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.DischargeProtectionRelease = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;

	return true;
}
bool PaceBmsProtocolV20::CreateWriteConfigurationRequest(const uint8_t busId, const ChargeAndDischargeOverTemperatureConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.ChargeAlarm < 20 || config.ChargeAlarm > 100)
	{
		LogError("ChargeAlarm is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ChargeProtection < 20 || config.ChargeProtection > 100)
	{
		LogError("ChargeProtection is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ChargeProtectionRelease < 20 || config.ChargeProtectionRelease > 100)
	{
		LogError("ChargeProtectionRelease is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.DischargeAlarm < 20 || config.DischargeAlarm > 100)
	{
		LogError("DischargeAlarm is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.DischargeProtection < 20 || config.DischargeProtection > 100)
	{
		LogError("DischargeProtection is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.DischargeProtectionRelease < 20 || config.DischargeProtectionRelease > 100)
	{
		LogError("DischargeProtectionRelease is not in the range that PBmsTools would send (or expect back)");
		return false;
	}

	const uint16_t payloadLen = 26;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	// unknown value
	WriteHexEncodedByte(payload, payloadOffset, 0x01);
	WriteHexEncodedUShort(payload, payloadOffset, (config.ChargeAlarm * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.ChargeProtection * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.ChargeProtectionRelease * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.DischargeAlarm * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.DischargeProtection * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.DischargeProtectionRelease * 10) + 2730);

	CreateRequest(busId, CID2_WriteChargeAndDischargeOverTemperatureConfiguration, payload, request);

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadChargeAndDischargeUnderTemperatureConfigurationRequestV20[] = "~250046DF0000FD85\r";
const unsigned char PaceBmsProtocolV20::exampleReadChargeAndDischargeUnderTemperatureConfigurationResponseV20[] = "~25004600501A010AAA0A780AAA0A1409E20A14F7E5\r";
const unsigned char PaceBmsProtocolV20::exampleWriteChargeAndDischargeUnderTemperatureConfigurationRequestV20[] = "~250046DE501A010AAA0A780AAA0A1409E20A14F7BC\r";
const unsigned char PaceBmsProtocolV20::exampleWriteChargeAndDischargeUnderTemperatureConfigurationResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, ChargeAndDischargeUnderTemperatureConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	uint16_t unknown = ReadHexEncodedByte(response, byteOffset);
	if (unknown != 01)
	{
		LogWarning("Unknown payload byte does not match previously observed value");
		return false;
	}

	config.ChargeAlarm = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.ChargeProtection = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.ChargeProtectionRelease = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.DischargeAlarm = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.DischargeProtection = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.DischargeProtectionRelease = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;

	return true;
}
bool PaceBmsProtocolV20::CreateWriteConfigurationRequest(const uint8_t busId, const ChargeAndDischargeUnderTemperatureConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.ChargeAlarm < -35 || config.ChargeAlarm > 30)
	{
		LogError("ChargeAlarm is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ChargeProtection < -35 || config.ChargeProtection > 30)
	{
		LogError("ChargeProtection is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ChargeProtectionRelease < -35 || config.ChargeProtectionRelease > 30)
	{
		LogError("ChargeProtectionRelease is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.DischargeAlarm < -35 || config.DischargeAlarm > 30)
	{
		LogError("DischargeAlarm is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.DischargeProtection < -35 || config.DischargeProtection > 30)
	{
		LogError("DischargeProtection is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.DischargeProtectionRelease < -35 || config.DischargeProtectionRelease > 30)
	{
		LogError("DischargeProtectionRelease is not in the range that PBmsTools would send (or expect back)");
		return false;
	}

	const uint16_t payloadLen = 26;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	// unknown value
	WriteHexEncodedByte(payload, payloadOffset, 0x01);
	WriteHexEncodedUShort(payload, payloadOffset, (config.ChargeAlarm * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.ChargeProtection * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.ChargeProtectionRelease * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.DischargeAlarm * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.DischargeProtection * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.DischargeProtectionRelease * 10) + 2730);

	CreateRequest(busId, CID2_WriteChargeAndDischargeUnderTemperatureConfiguration, payload, request);

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadMosfetOverTemperatureConfigurationRequestV20[] = "~250046E10000FD99\r";
const unsigned char PaceBmsProtocolV20::exampleReadMosfetOverTemperatureConfigurationResponseV20[] = "~25004600200E010E2E0EF60DFCFA5D\r";
const unsigned char PaceBmsProtocolV20::exampleWriteMosfetOverTemperatureConfigurationRequestV20[] = "~250046E0200E010E2E0EF60DFCFA48\r";
const unsigned char PaceBmsProtocolV20::exampleWriteMosfetOverTemperatureConfigurationResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, MosfetOverTemperatureConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	uint16_t unknown = ReadHexEncodedByte(response, byteOffset);
	if (unknown != 01)
	{
		LogWarning("Unknown payload byte does not match previously observed value");
		return false;
	}

	config.Alarm = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.Protection = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.ProtectionRelease = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;

	return true;
}
bool PaceBmsProtocolV20::CreateWriteConfigurationRequest(const uint8_t busId, const MosfetOverTemperatureConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.Alarm < 30 || config.Alarm > 120)
	{
		LogError("Alarm is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.Protection < 30 || config.Protection > 120)
	{
		LogError("Protection is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.ProtectionRelease < 30 || config.ProtectionRelease > 120)
	{
		LogError("ProtectionRelease is not in the range that PBmsTools would send (or expect back)");
		return false;
	}

	const uint16_t payloadLen = 26;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	// unknown value
	WriteHexEncodedByte(payload, payloadOffset, 0x01);
	WriteHexEncodedUShort(payload, payloadOffset, (config.Alarm * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.Protection * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.ProtectionRelease * 10) + 2730);

	CreateRequest(busId, CID2_WriteMosfetOverTemperatureConfiguration, payload, request);

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadEnvironmentOverUnderTemperatureConfigurationRequestV20[] = "~250046E70000FD93\r";
const unsigned char PaceBmsProtocolV20::exampleReadEnvironmentOverUnderTemperatureConfigurationResponseV20[] = "~25004600501A0109E209B009E20D340D660D34F806\r";
const unsigned char PaceBmsProtocolV20::exampleWriteEnvironmentOverUnderTemperatureConfigurationRequestV20[] = "~250046E6501A0109E209B009E20D340D660D34F7EB\r";
const unsigned char PaceBmsProtocolV20::exampleWriteEnvironmentOverUnderTemperatureConfigurationResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, EnvironmentOverUnderTemperatureConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	uint16_t unknown = ReadHexEncodedByte(response, byteOffset);
	if (unknown != 01)
	{
		LogWarning("Unknown payload byte does not match previously observed value");
		return false;
	}

	config.UnderAlarm = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.UnderProtection = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.UnderProtectionRelease = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.OverAlarm = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.OverProtection = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.OverProtectionRelease = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;

	return true;
}
bool PaceBmsProtocolV20::CreateWriteConfigurationRequest(const uint8_t busId, const EnvironmentOverUnderTemperatureConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.UnderAlarm < -35 || config.UnderAlarm > 30)
	{
		LogError("UnderAlarm is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.UnderProtection < -35 || config.UnderProtection > 30)
	{
		LogError("UnderProtection is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.UnderProtectionRelease < -35 || config.UnderProtectionRelease > 30)
	{
		LogError("UnderProtectionRelease is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.OverAlarm < 20 || config.OverAlarm > 100)
	{
		LogError("OverAlarm is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.OverProtection < 20 || config.OverProtection > 100)
	{
		LogError("OverProtection is not in the range that PBmsTools would send (or expect back)");
		return false;
	}
	if (config.OverProtectionRelease < 20 || config.OverProtectionRelease > 100)
	{
		LogError("OverProtectionRelease is not in the range that PBmsTools would send (or expect back)");
		return false;
	}

	const uint16_t payloadLen = 26;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	// unknown value
	WriteHexEncodedByte(payload, payloadOffset, 0x01);
	WriteHexEncodedUShort(payload, payloadOffset, (config.UnderAlarm * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.UnderProtection * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.UnderProtectionRelease * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.OverAlarm * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.OverProtection * 10) + 2730);
	WriteHexEncodedUShort(payload, payloadOffset, (config.OverProtectionRelease * 10) + 2730);

	CreateRequest(busId, CID2_WriteEnvironmentOverUnderTemperatureConfiguration, payload, request);

	return true;
}

// ============================================================================
// 
// "System Configuration" tab of PBmsTools 2.4
// 
// ============================================================================

// note: "Charge Current Limiter Current Limit Gear Switch" is in this page in PBmsTools but I moved it to the SwitchCommand section above because it uses the same CID2 and fits in nicely with that code

const unsigned char PaceBmsProtocolV20::exampleReadChargeCurrentLimiterStartCurrentRequestV20[] = "~250046ED0000FD86\r";
const unsigned char PaceBmsProtocolV20::exampleReadChargeCurrentLimiterStartCurrentResponseV20[] = "~25004600C0040064FCCE\r";
const unsigned char PaceBmsProtocolV20::exampleWriteChargeCurrentLimiterStartCurrentRequestV20[] = "~250046EEC0040064FCA4\r";
const unsigned char PaceBmsProtocolV20::exampleWriteChargeCurrentLimiterStartCurrentResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::CreateReadChargeCurrentLimiterStartCurrentRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadChargeCurrentLimiterStartCurrent, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsProtocolV20::ProcessReadChargeCurrentLimiterStartCurrentResponse(const uint8_t busId, const std::vector<uint8_t>& response, uint8_t& current)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	uint16_t busIdResponding = ReadHexEncodedByte(response, byteOffset);
	if (busIdResponding != busId)
	{
		LogError("Response from wrong bus Id in payload");
		return false;
	}

	current = ReadHexEncodedByte(response, byteOffset);

	return true;
}
bool PaceBmsProtocolV20::CreateWriteChargeCurrentLimiterStartCurrentRequest(const uint8_t busId, const uint8_t current, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (current < 5 || current > 150)
	{
		LogError("current is not in the range that PBmsTools would send (or expect back)");
		return false;
	}

	const uint16_t payloadLen = 4;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, busId);
	WriteHexEncodedByte(payload, payloadOffset, current);

	CreateRequest(busId, CID2_WriteChargeCurrentLimiterStartCurrent, payload, request);

	return true;
}
bool PaceBmsProtocolV20::ProcessWriteChargeCurrentLimiterStartCurrentResponse(const uint8_t busId, const std::vector<uint8_t>& response)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	if (payloadLen != 0)
	{
		std::string message = std::string("Documentation indicates a write charge current limiter start current response should return no payload, but this response's payload length is ") + std::to_string(payloadLen);
		LogError(message);
		return false;
	}

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadRemainingCapacityRequestV20[] = "~250046A60000FD98\r";
const unsigned char PaceBmsProtocolV20::exampleReadRemainingCapacityResponseV20[] = "~25004600400C183C286A2710FB0E\r";

bool PaceBmsProtocolV20::CreateReadRemainingCapacityRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadRemainingCapacity, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsProtocolV20::ProcessReadRemainingCapacityResponse(const uint8_t busId, const std::vector<uint8_t>& response, uint32_t& remainingCapacityMilliampHours, uint32_t& actualCapacityMilliampHours, uint32_t& designCapacityMilliampHours)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	remainingCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset) * 10;
	actualCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset) * 10;
	designCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset) * 10;

	return true;
}

const unsigned char PaceBmsProtocolV20::exampleReadProtocolsRequestV20[] = "~250046EB0000FD88\r";
const unsigned char PaceBmsProtocolV20::exampleReadProtocolsResponseV20[] = "~25004600A006131400FC6F\r";
const unsigned char PaceBmsProtocolV20::exampleWriteProtocolsRequestV20[] = "~250046ECA006131400FC47\r";
const unsigned char PaceBmsProtocolV20::exampleWriteProtocolsResponseV20[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV20::CreateReadProtocolsRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadCommunicationsProtocols, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsProtocolV20::ProcessReadProtocolsResponse(const uint8_t busId, const std::vector<uint8_t>& response, Protocols& protocols)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	protocols.CAN = (ProtocolList_CAN)ReadHexEncodedByte(response, byteOffset);
	protocols.RS485 = (ProtocolList_RS485)ReadHexEncodedByte(response, byteOffset);
	protocols.Type = (ProtocolList_Type)ReadHexEncodedByte(response, byteOffset);

	return true;
}
bool PaceBmsProtocolV20::CreateWriteProtocolsRequest(const uint8_t busId, const Protocols& protocols, std::vector<uint8_t>& request)
{
	const uint16_t payloadLen = 12;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, protocols.CAN);
	WriteHexEncodedByte(payload, payloadOffset, protocols.RS485);
	WriteHexEncodedByte(payload, payloadOffset, protocols.Type);

	CreateRequest(busId, CID2_WriteCommunicationsProtocols, payload, request);

	return true;
}
bool PaceBmsProtocolV20::ProcessWriteProtocolsResponse(const uint8_t busId, const std::vector<uint8_t>& response)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	if (payloadLen != 0)
	{
		std::string message = std::string("Write protocols response should include no payload, but this response's payload length is ") + std::to_string(payloadLen);
		LogError(message);
		return false;
	}

	return true;
	
}
*/