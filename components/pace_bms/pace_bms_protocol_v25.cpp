
#include "pace_bms_protocol_v25.h"

// takes pointers to the "real" logging functions
PaceBmsProtocolV25::PaceBmsProtocolV25(
		std::optional<std::string> protocol_variant, std::optional<uint8_t> protocol_version_override, std::optional<uint8_t> batteryChemistry,
		LogFuncPtr logError, LogFuncPtr logWarning, LogFuncPtr logInfo, LogFuncPtr logDebug, LogFuncPtr logVerbose, LogFuncPtr logVeryVerbose) :
	PaceBmsProtocolBase(
		0x25, protocol_variant, protocol_version_override, batteryChemistry,
		logError, logWarning, logInfo, logDebug, logVerbose, logVeryVerbose)
{
}

// ============================================================================
// 
// Main "Realtime Monitoring" tab of PBmsTools 2.4
// These are the commands sent in a loop to fill out the display
// 
// ============================================================================

static const uint8_t exampleReadBmsCountRequestV25[] = " ~250146900000FDA5\r";
static const uint8_t exampleReadBmsCountResponseV25[] = "~25014600E00202FD35\r";

bool PaceBmsProtocolV25::CreateReadBmsCountRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadBmsCount, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsProtocolV25::ProcessReadBmsCountResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, uint8_t& bmsCount)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 2) {
		LogError("ProcessReadBmsCountResponse expected payload length of 2 but got " + std::to_string(payloadLen));
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	if (payloadLen != 2)
	{
		std::string message = std::string("Read BMS Count should return a 2 byte payload, but payload length is ") + std::to_string(payloadLen);
		LogError(message);
		return false;
	}

	bmsCount = ReadHexEncodedByte(response, byteOffset);

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadBmsCountResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}


const unsigned char PaceBmsProtocolV25::exampleReadAnalogInformationRequestV25[] = "~25014642E00201FD30\r";
const unsigned char PaceBmsProtocolV25::exampleReadAnalogInformationResponseV25[] = "~25014600F07A0001100CC70CC80CC70CC70CC70CC50CC60CC70CC70CC60CC70CC60CC60CC70CC60CC7060B9B0B990B990B990BB30BBCFF1FCCCD12D303286A008C2710E1E4\r";

bool PaceBmsProtocolV25::CreateReadAnalogInformationRequest(const uint8_t busId, const uint8_t targetId, std::vector<uint8_t>& request)
{
	// the payload is the requested busId (could be FF for "get all" when speaking to a set of daisy-chained units but this code doesn't support that)
	const uint16_t payloadLen = 2;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, targetId);

	CreateRequest(busId, CID2_ReadAnalogInformation, payload, request);

	return true;
}
bool PaceBmsProtocolV25::ProcessReadAnalogInformationResponse(const uint8_t busId, const uint8_t targetedBusId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, std::function<void(uint8_t payloadCount, uint8_t index, AnalogInformation& payload)> onPayload, bool quietMode)
{
	//std::memset(&analogInformation, 0, sizeof(AnalogInformation));

	LogFuncPtr logError = [this](std::string log) -> void { LogError(log); };
	LogFuncPtr logWarning = [this](std::string log) -> void { LogWarning(log); };
	LogFuncPtr logInfo = [this](std::string log) -> void { LogInfo(log); };
	LogFuncPtr logDebug = [this](std::string log) -> void { LogDebug(log); };
	LogFuncPtr logVerbose = [this](std::string log) -> void { LogVerbose(log); };
	LogFuncPtr logVeryVerbose = [this](std::string log) -> void { LogVeryVerbose(log); };

	if(quietMode == true) {
		logError = [this](std::string log) -> void { LogVeryVerbose("QuietMode: " + log); };
		logWarning = [this](std::string log) -> void { LogVeryVerbose("QuietMode: " + log); };
		logInfo = [this](std::string log) -> void { LogVeryVerbose("QuietMode: " + log); };
		logDebug = [this](std::string log) -> void { LogVeryVerbose("QuietMode: " + log); };
		logVerbose = [this](std::string log) -> void { LogVeryVerbose("QuietMode: " + log); };
		logVeryVerbose = [this](std::string log) -> void { LogVeryVerbose("QuietMode: " + log); };
	}

	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response, quietMode);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	// this check is explicitly done in several places as we go since different protocol variants behave differently here
	//if (payloadLen != 2) {
	//	LogError("ProcessReadAnalogInformationResponse expected payload length of 2 but got " + std::to_string(payloadLen));
	//	return false;
	//}

	if(payloadLen < 122)
	{
		logError("Sanity Check: AnalogInformation response payload length too short, must be at least 122 bytes but got " + std::to_string(payloadLen) + " bytes");
		return false;
	}

	// first thing we need to do is look ahead and check the AnalogInformation UserDefinedValue, which tells us how to interpret the rest of the payload
	uint16_t snoopOffset = 13 + 36;
	uint8_t lookAhead_TemperatureCount = ReadHexEncodedByte(response, snoopOffset, quietMode);
	uint8_t lookAhead_AnalogInformationUserDefinedValue = -1;
	if(lookAhead_TemperatureCount == 8)
	{
		// tsk tsk, Eenovance/Sunsynk have 8 temperature readings so the offset is advanced by 8 bytes, gotta love that vendor lock in!
		uint16_t snoopOffset = 13 + 116; // 129
		lookAhead_AnalogInformationUserDefinedValue = ReadHexEncodedByte(response, snoopOffset, quietMode);
	}
	else
	{
		// "normal" offset for the User Defined Value
		uint16_t snoopOffset = 13 + 108; // 121
		lookAhead_AnalogInformationUserDefinedValue = ReadHexEncodedByte(response, snoopOffset, quietMode);
	}
	currentProtocolVariant = GetProtocolVariantInfo(lookAhead_AnalogInformationUserDefinedValue);
	if(currentProtocolVariant == nullptr)
	{
		logWarning("Response contains a constant with an unexpected value '" + std::to_string(lookAhead_AnalogInformationUserDefinedValue) + "' this may be an incorrect protocol variant. This will be ignored, but please file an issue report with full logs at VERY_VERBOSE level.");
		// we can still try to parse the rest of the response, just assume the "standard" variant
		currentProtocolVariant = GetProtocolVariantInfo(3);
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	// SPEC BUG: doc says the response starts with the busId, but "on the wire" I see an extra byte value of 0x00 preceeding it
	uint8_t unknown = ReadHexEncodedByte(response, byteOffset, quietMode);
	if (unknown != 0)
	{
		logVerbose("Response contains a value other than zero before the BusId");
	}

	// by default we expect a single response, but if the request was sent to the broadcast address 0xFF, then 
	// instead of the next byte being the (payload) busId it is instead a count of how many responses are included
	// (I think, but need more examples to be sure this is the proper interpretation)
	uint8_t payloadCount = 1;
	if(targetedBusId != 0xFF) 
	{
		// note that this is the *payload* busId, not the header busId which was already validated
		uint8_t busIdResponding = ReadHexEncodedByte(response, byteOffset, quietMode);
		if (busIdResponding != targetedBusId)
		{
			logError("Response from wrong bus Id in payload, expected " + std::to_string(targetedBusId) + " but got " + std::to_string(busIdResponding));
			return false;
		}
	}
	else
	{
		bool error = false;

		payloadCount = ReadHexEncodedByte(response, byteOffset, quietMode);
		if (payloadCount < 1 || payloadCount > 16)
		{
			logError("Response to AnalogInformation broadcast request contains a payload count of " + std::to_string(payloadCount) + " which is outside the expected range of 1-16");
			error = true;
		}

		int remainder = (payloadLen - 4 /* initial zero byte plus payload len byte */) % ((118 /* standard analog info payload size */ + currentProtocolVariant->analogInformationExtraBytes));
		if(remainder != 0)
		{
			logError("Response to AnalogInformation broadcast request contains a total payload length that is not a multiple of the expected payload size, remainder " + std::to_string(remainder) + " bytes.");
			error = true;
		}

		int calculatedPayloadCount = (payloadLen - 4 /* initial zero byte plus payload len byte */) / ((118 /* standard analog info payload size */ + currentProtocolVariant->analogInformationExtraBytes));
		if(calculatedPayloadCount != payloadCount)
		{
			logWarning("Response to AnalogInformation broadcast request contains a payload count of " + std::to_string(payloadCount) + " but the total payload length indicates " + std::to_string(calculatedPayloadCount) + " payloads are present; using calculated value");
			payloadCount = calculatedPayloadCount;
		}

		if(error)
		{
			return false;
		}
	}
#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERY_VERBOSE
	logVeryVerbose(std::to_string(payloadCount) + " responses found in analog information payload");
#endif

	for(int index = 0; index < payloadCount; index++)
	{
		AnalogInformation analogInformation;
		//std::memset(&analogInformation, 0, sizeof(AnalogInformation));

		analogInformation.cellCount = ReadHexEncodedByte(response, byteOffset, quietMode);
		if (analogInformation.cellCount > MAX_CELL_COUNT)
		{
			logWarning("Response contains more cell voltage readings than are supported, results will be truncated");
		}
		int sanityCheck_totalVoltage = 0;
		for (int i = 0; i < analogInformation.cellCount; i++)
		{
			uint16_t cellVoltage = ReadHexEncodedUShort(response, byteOffset, quietMode);

			if (i > MAX_CELL_COUNT - 1)
				continue;

			analogInformation.cellVoltagesMillivolts[i] = cellVoltage;
			sanityCheck_totalVoltage += cellVoltage;
		}

		// sanity checks to reject nonsensical responses
		if(sanityCheck_totalVoltage == 0 || analogInformation.cellCount == 0)
		{
			logWarning("Sanity Check: Response contains zero cells, or all zero cell voltages, this looks like an invalid response.");
			return false;
		}

		analogInformation.temperatureCount = ReadHexEncodedByte(response, byteOffset, quietMode);
		if (analogInformation.temperatureCount > MAX_TEMP_COUNT)
		{
			// Eenovance/Sunsynk have 8 temperature readings, this is "expected" so we can log info instead of warning
			if(currentProtocolVariant->analogInformationUserDefinedValue == 4)
			{
				logInfo("Response contains more temperature readings than are supported, but that is expected for this protocol variant; the 7th and 8th readings will be ignored");
			}
			else
			{
				logWarning("Response contains more temperature readings than are supported, results will be truncated");
			}
		}
		for (int i = 0; i < analogInformation.temperatureCount; i++)
		{
			uint16_t temperature = ReadHexEncodedUShort(response, byteOffset, quietMode);

			if (i > MAX_TEMP_COUNT - 1)
				continue; // already logged above if count was too high

			analogInformation.temperaturesTenthsCelcius[i] = (temperature - 2730);
		}

		analogInformation.currentMilliamps = ReadHexEncodedSShort(response, byteOffset, quietMode) * 10;

		analogInformation.totalVoltageMillivolts = ReadHexEncodedUShort(response, byteOffset, quietMode);

		analogInformation.remainingCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset, quietMode) * 10;

		uint8_t again_AnalogInformationUserDefinedValue = ReadHexEncodedByte(response, byteOffset, quietMode);
		if (again_AnalogInformationUserDefinedValue != currentProtocolVariant->analogInformationUserDefinedValue)
		{
			logWarning("AnalogInformation UserDefinedValue lookahead mismatch, this is a bug in PACE_BMS. Please file an issue report with full logs at VERY_VERBOSE level.");
			return false;
		}

		analogInformation.fullCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset, quietMode) * 10;

		analogInformation.cycleCount = ReadHexEncodedUShort(response, byteOffset, quietMode);

		analogInformation.designCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset, quietMode) * 10;

		// calculate some "extras"
		analogInformation.SoC = ((float)analogInformation.remainingCapacityMilliampHours / (float)analogInformation.fullCapacityMilliampHours) * 100.0f;
		analogInformation.SoH = ((float)analogInformation.fullCapacityMilliampHours / (float)analogInformation.designCapacityMilliampHours) * 100.0f;
		if (analogInformation.SoH > 100)
		{
			// many packs have a little bit "extra" capacity to make sure they hit their nameplate value
			analogInformation.SoH = 100;
		}
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
	
		// skip any extra bytes that are part of this protocol variant
		byteOffset += currentProtocolVariant->analogInformationExtraBytes;

		if(onPayload != nullptr)
			onPayload(payloadCount, index, analogInformation);
	}

	// this check remains valid with broadcast responses due to the loop above
	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */)
	{
		logError("Length mismatch reading analog information response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. This will be ignored, but accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
		//return false;
	}

	return true;
}

const unsigned char PaceBmsProtocolV25::exampleReadStatusInformationRequestV25[] = "~25014644E00201FD2E\r";
const unsigned char PaceBmsProtocolV25::exampleReadStatusInformationResponseV25[] = "~25014600004C000110000000000000000000000000000000000600000000000000000000000E000000000000EF3A\r";

bool PaceBmsProtocolV25::CreateReadStatusInformationRequest(const uint8_t busId, const uint8_t targetId, std::vector<uint8_t>& request)
{
	// the payload is the requested busId (could be FF for "get all" when speaking to a set of daisy-chained units but this code doesn't support that)
	const uint16_t payloadLen = 2;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, targetId);

	CreateRequest(busId, CID2_ReadStatusInformation, payload, request);

	return true;
}

// helper for: ProcessStatusInformationResponse
const std::string PaceBmsProtocolV25::DecodeWarningValue(const uint8_t val, std::string from)
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
	if (val >= WV_UserDefinedFaultRangeStartValue && val <= WV_UserDefinedFaultRangeEndValue)
	{
		return std::string("User Defined Fault");
	}
	if (val == WV_OtherFaultValue)
	{
		LogWarning(from + ": 'Other Fault'");
		return std::string("Other Fault");
	}

	LogWarning(from + ": Unknown Fault Value");
	return std::string("Unknown Fault Value");
}
// helper for: ProcessStatusInformationResponse
const std::string PaceBmsProtocolV25::DecodeProtectionStatus1Value(const uint8_t val)
{
	std::string str;

	if ((val & P1F_ChargerHighVoltageInProtect1Bit) != 0)
	{
		str.append("Charger High Voltage In Protect; ");
	}
	if ((val & P1F_ShortCircuitProtect1Bit) != 0)
	{
		str.append("Short Circuit Protect; ");
	}
	if ((val & P1F_DischargeCurrentProtect1Bit) != 0)
	{
		str.append("Discharge Current Protect; ");
	}
	if ((val & P1F_ChargeCurrentProtect1Bit) != 0)
	{
		str.append("Charge Current Protect; ");
	}
	if ((val & P1F_LowTotalVoltageProtect1Bit) != 0)
	{
		str.append("Low Total Voltage Protect; ");
	}
	if ((val & P1F_HighTotalVoltageProtect1Bit) != 0)
	{
		str.append("High Total Voltage Protect; ");
	}
	if ((val & P1F_LowCellVoltageProtect1Bit) != 0)
	{
		str.append("Low Cell Voltage Protect; ");
	}
	if ((val & P1F_HighCellVoltageProtect1Bit) != 0)
	{
		str.append("High Cell Voltage Protect; ");
	}

	return str;
}
// helper for: ProcessStatusInformationResponse
const std::string PaceBmsProtocolV25::DecodeProtectionStatus2ValueWithoutFullyChargedStatus(const uint8_t val)
{
	std::string str;

	if ((val & P2F_FullyProtect2Bit) != 0)
	{
		// special case, see DecodeProtectionStatus2ValueActuallyStatusFullyChargedOnly
	}
	if ((val & P2F_LowEnvironmentalTemperatureProtect2Bit) != 0)
	{
		str.append("Low Environmental Temperature Protect; ");
	}
	if ((val & P2F_HighEnvironmentalTemperatureProtect2Bit) != 0)
	{
		str.append("High Environmental Temperature Protect; ");
	}
	if ((val & P2F_HighMosfetTemperatureProtect2Bit) != 0)
	{
		str.append("High MOSFET Temperature Protect; ");
	}
	if ((val & P2F_LowDischargeTemperatureProtect2Bit) != 0)
	{
		str.append("Low Discharge Temperature Protect; ");
	}
	if ((val & P2F_LowChargeTemperatureProtect2Bit) != 0)
	{
		str.append("Low Charge Temperature Protect; ");
	}
	if ((val & P2F_HighDischargeTemperatureProtect2Bit) != 0)
	{
		str.append("High Discharge Temperature Protect; ");
	}
	if ((val & P2F_HighChargeTemperatureProtect2Bit) != 0)
	{
		str.append("High Charge Temperature Protect; ");
	}

	return str;
}
// helper for: ProcessStatusInformationResponse
const std::string PaceBmsProtocolV25::DecodeProtectionStatus2ValueActuallyStatusFullyChargedOnly(const uint8_t val)
{
	std::string str;

	if ((val & P2F_FullyProtect2Bit) != 0)
	{
		// based on (poor and incomplete) documentation and inference, this is not a protection flag, but means: the pack has been fully charged, the SoC and total capacity have been updated in the firmware
		str.append("Fully Charged; ");
	}

	return str;
}
// helper for: ProcessStatusInformationResponse
const std::string PaceBmsProtocolV25::DecodeStatusValue(const uint8_t val)
{
	std::string str;

	if ((val & SF_HeaterActiveBit) != 0)
	{
		str.append("Heater Active; "); 
	}
	if ((val & SF_AlternateCurrentInBit) != 0)
	{
		str.append("Alternate Current In; ");
	}
	if ((val & SF_ChargingBit) != 0)
	{
		str.append("Charging; ");
	}
	if ((val & SF_PositiveNegativeTerminalsReversedBit) != 0)
	{
		str.append("Positive/Negative Terminals Reversed; "); 
	}
	if ((val & SF_DischargingBit) != 0)
	{
		str.append("Discharging; ");
	}
	if ((val & SF_DischargeMosfetOnBit) != 0)
	{
		str.append("Discharge MOSFET On; ");
	}
	if ((val & SF_ChargeMosfetOnBit) != 0)
	{
		str.append("Charge MOSFET On; ");
	}
	if ((val & SF_ChargeCurrentLimiterTurnedOffBit) != 0)
	{
		str.append("Charge Current Limiter Disabled; ");
	}

	return str;
}
// helper for: ProcessStatusInformationResponse
const std::string PaceBmsProtocolV25::DecodeConfigurationStatusValue(const uint8_t val, std::string from)
{
	std::string str;

	if ((val & CF_UndefinedConfigurationStatusBit8) != 0)
	{
		str.append("Undefined ConfigurationStatus Bit8 Set; ");
		LogWarning(from + ": Undefined ConfigurationStatus Bit8 Set");
	}
	if ((val & CF_StaticBalanceBit) != 0)
	{
		str.append("Static Balance ('Enabled'?); "); // "Enabled" ??????????????
	}
	if ((val & CF_LedAlarmEnabledBit) != 0)
	{
		str.append("Warning LED Enabled; ");
	}
	if ((val & CF_ChargeCurrentLimiterEnabledBit) != 0)
	{
		str.append("Charge Current Limiter Enabled (" + std::string((val & CF_ChargeCurrentLimiterLowGearSetBit) != 0 ? "Low Gear" : "High Gear") + "); ");
	}
	//if ((val & CF_ChargeCurrentLimiterLowGearSetBit) != 0)
	//{
	//	oss.append("Current limit low-gear Set; ");
	//}
	if ((val & CF_DischargeMosfetTurnedOff) != 0)
	{
		str.append("Discharge MOSFET Turned Off; ");
	}
	if ((val & CF_ChargeMosfetTurnedOff) != 0)
	{
		str.append("Charge MOSFET Turned Off; ");
	}
	if ((val & CF_BuzzerAlarmEnabledBit) != 0)
	{
		str.append("Warning Buzzer Enabled; ");
	}

	return str;
}
// helper for: ProcessStatusInformationResponse
const std::string PaceBmsProtocolV25::DecodeFaultStatusValue(const uint8_t val)
{
	std::string str;

	if ((val & FF_HeaterBit) != 0)
	{
		str.append("Heater Fault; ");
	}
	if ((val & FF_CCBBit) != 0)
	{
		str.append("CCB Fault; ");
	}
	if ((val & FF_VCCSamplingBit) != 0)
	{
		str.append("VCC Sampling Fault; ");
	}
	if ((val & FF_CellBit) != 0)
	{
		str.append("Cell fault; ");
	}
	if ((val & FF_CommBit) != 0)
	{
		str.append("Comm Fault; "); //only on later PACE 
	}
	if ((val & FF_NTCBit) != 0)
	{
		str.append("NTC fault; ");
	}
	if ((val & FF_DischargeMosfetBit) != 0)
	{
		str.append("Discharge MOSFET fault; ");
	}
	if ((val & FF_ChargeMosfetBit) != 0)
	{
		str.append("Charge MOSFET fault; ");
	}

	return str;
}
// helper for: ProcessStatusInformationResponse
const std::string PaceBmsProtocolV25::DecodeWarningStatus1Value(const uint8_t val, std::string from)
{
	std::string str;

	if ((val & W1F_UndefinedWarning1Bit8) != 0)
	{
		str.append("Undefined WarnState1 Bit7 Warning; ");
		LogWarning(from + ": Undefined WarnState1 Bit7 Warning");
	}
	if ((val & W1F_UndefinedWarning1Bit7) != 0)
	{
		str.append("Undefined WarnState1 Bit6 Warning; ");
		LogWarning(from + ": Undefined WarnState1 Bit6 Warning");
	}
	if ((val & W1F_DischargeCurrentBit) != 0)
	{
		str.append("Discharge Current Warning; ");
	}
	if ((val & W1F_ChargeCurrentBit) != 0)
	{
		str.append("Charge Current Warning; ");
	}
	if ((val & W1F_LowTotalVoltageBit) != 0)
	{
		str.append("Low Total Voltage Warning; ");
	}
	if ((val & W1F_HighTotalVoltageBit) != 0)
	{
		str.append("High Total Voltage Warning; ");
	}
	if ((val & W1F_LowCellVoltageBit) != 0)
	{
		str.append("Low Cell Voltage Warning; ");
	}
	if ((val & W1F_HighCellVoltageBit) != 0)
	{
		str.append("High Cell Voltage Warning; ");
	}

	return str;
}
// helper for: ProcessStatusInformationResponse
const std::string PaceBmsProtocolV25::DecodeWarningStatus2Value(const uint8_t val)
{
	std::string str;

	if ((val & W2F_LowPower) != 0)
	{
		str.append("Low Power Warning; ");
	}
	if ((val & W2F_HighMosfetTemperature) != 0)
	{
		str.append("High MOSFET Temperature Warning; ");
	}
	if ((val & W2F_LowEnvironmentalTemperature) != 0)
	{
		str.append("Low Environmental Temperature Warning; ");
	}
	if ((val & W2F_HighEnvironmentalTemperature) != 0)
	{
		str.append("High Environmental Temperature Warning; ");
	}
	if ((val & W2F_LowDischargeTemperature) != 0)
	{
		str.append("Low Discharge Temperature Warning; ");
	}
	if ((val & W2F_LowChargeTemperature) != 0)
	{
		str.append("Low Charge Temperature Warning; ");
	}
	if ((val & W2F_HighDischargeTemperature) != 0)
	{
		str.append("High Discharge Temperature Warning; ");
	}
	if ((val & W2F_HighChargeTemperature) != 0)
	{
		str.append("High Charge Temperature Warning; ");
	}

	return str;
}

bool PaceBmsProtocolV25::ProcessReadStatusInformationResponse(const uint8_t busId, const uint8_t targetedBusId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, std::function<void(uint8_t payloadCount, uint8_t index, StatusInformation& payload)> onPayload, bool quietMode)
{
	//std::memset(&statusInformation, 0, sizeof(StatusInformation));

	LogFuncPtr logError = [this](std::string log) -> void { LogError(log); };
	LogFuncPtr logWarning = [this](std::string log) -> void { LogWarning(log); };
	LogFuncPtr logInfo = [this](std::string log) -> void { LogInfo(log); };
	LogFuncPtr logDebug = [this](std::string log) -> void { LogDebug(log); };
	LogFuncPtr logVerbose = [this](std::string log) -> void { LogVerbose(log); };
	LogFuncPtr logVeryVerbose = [this](std::string log) -> void { LogVeryVerbose(log); };

	if(quietMode == true) {
		logError = [this](std::string log) -> void { LogVeryVerbose("QuietMode: " + log); };
		logWarning = [this](std::string log) -> void { LogVeryVerbose("QuietMode: " + log); };
		logInfo = [this](std::string log) -> void { LogVeryVerbose("QuietMode: " + log); };
		logDebug = [this](std::string log) -> void { LogVeryVerbose("QuietMode: " + log); };
		logVerbose = [this](std::string log) -> void { LogVeryVerbose("QuietMode: " + log); };
		logVeryVerbose = [this](std::string log) -> void { LogVeryVerbose("QuietMode: " + log); };
	}

	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response, quietMode);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	// this check is explicitly done in several places as we go since different protocol variants behave differently here
	//if (payloadLen != 2) {
	//	LogError("ProcessReadStatusInformationResponse expected payload length of 2 but got " + std::to_string(payloadLen));
	//	return false;
	//}

	if(payloadLen < 76)
	{
		logError("Sanity Check: StatusInformation response payload length too short, must be at least 76 bytes but got " + std::to_string(payloadLen) + " bytes");
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	// SPEC BUG: doc says the response starts with the busId, but "on the wire" I see an extra byte value of 0x00 preceeding it
	uint8_t unknown = ReadHexEncodedByte(response, byteOffset, quietMode);
	if (unknown != 0)
	{
		logVerbose("Response contains a value other than zero before the BusId");
	}

	// by default we expect a single response, but if the request was sent to the broadcast address 0xFF, then 
	// instead of the next byte being the (payload) busId it is instead a count of how many responses are included
	// (I think, but need more examples to be sure this is the proper interpretation)
	uint8_t payloadCount = 1;
	if(targetedBusId != 0xFF) 
	{
		// note that this is the *payload* busId, not the header busId which was already validated
		uint8_t busIdResponding = ReadHexEncodedByte(response, byteOffset, quietMode);
		if (busIdResponding != targetedBusId)
		{
			logError("Response from wrong bus Id in payload, expected " + std::to_string(targetedBusId) + " but got " + std::to_string(busIdResponding));
			return false;
		}
	}
	else
	{
		bool error = false;

		payloadCount = ReadHexEncodedByte(response, byteOffset, quietMode);
		if (payloadCount < 1 || payloadCount > 16)
		{
			logError("Response to StatusInformation broadcast request contains a payload count of " + std::to_string(payloadCount) + " which is outside the expected range of 1-16");
			error = true;
		}

		int remainder = (payloadLen - 4 /* initial zero byte plus payload len byte */) % ((72 /* standard status info payload size */ + currentProtocolVariant->statusInformationExtraBytes));
		if(remainder != 0)
		{
			logError("Response to StatusInformation broadcast request contains a total payload length that is not a multiple of the expected payload size, remainder " + std::to_string(remainder) + " bytes.");
			error = true;
		}

		int calculatedPayloadCount = (payloadLen - 4 /* initial zero byte plus payload len byte */) / ((72 /* standard status info payload size */ + currentProtocolVariant->statusInformationExtraBytes));
		if(calculatedPayloadCount != payloadCount)
		{
			logWarning("Response to StatusInformation broadcast request contains a payload count of " + std::to_string(payloadCount) + " but the total payload length indicates " + std::to_string(calculatedPayloadCount) + " payloads are present; using calculated value");
			payloadCount = calculatedPayloadCount;
		}

		if(error)
		{
			return false;
		}
	}
#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERY_VERBOSE
	logVeryVerbose(std::to_string(payloadCount) + " responses found in status information payload");
#endif

	for(int index = 0; index < payloadCount; index++)
	{
		StatusInformation statusInformation;
		//std::memset(&statusInformation, 0, sizeof(StatusInformation));

		statusInformation.warningText.clear();
		statusInformation.balancingText.clear();
		statusInformation.systemText.clear();
		statusInformation.configurationText.clear();
		statusInformation.protectionText.clear();
		statusInformation.faultText.clear();

		// ========================== Warning / Alarm Status ==========================
		uint8_t cellCount = ReadHexEncodedByte(response, byteOffset, quietMode);
		if (cellCount > MAX_CELL_COUNT)
		{
			logWarning("Response contains more cell warnings than are supported, results will be truncated");
		}
		int sanityCheck_cellsWithoutVoltageWarnings = 0;
		for (int i = 0; i < cellCount; i++)
		{
			uint8_t cw = ReadHexEncodedByte(response, byteOffset, quietMode);
			statusInformation.warning_value_cell[i] = cw;

			sanityCheck_cellsWithoutVoltageWarnings += (cw == 0 ? 1 : 0);

			if (i > MAX_CELL_COUNT - 1)
				continue;

			if (cw == 0)
				continue;

			// below/above limit
			statusInformation.warningText.append(std::string("Cell ") + std::to_string(i + 1) + std::string(": ") + DecodeWarningValue(cw, "PaceBmsProtocolV25::ProcessReadStatusInformationResponse") + std::string("; "));
		}

		uint8_t tempCount = ReadHexEncodedByte(response, byteOffset, quietMode);
		if (tempCount > MAX_TEMP_COUNT)
		{
			logWarning("Response contains more temperature warnings than are supported, results will be truncated");
		}
		int sanityCheck_cellsWithoutTemperatureWarnings = 0;
		for (int i = 0; i < tempCount; i++)
		{
			uint8_t tw = ReadHexEncodedByte(response, byteOffset, quietMode);
			statusInformation.warning_value_temp[i] = tw;

			sanityCheck_cellsWithoutTemperatureWarnings += (tw == 0 ? 1 : 0);

			if (i > MAX_TEMP_COUNT - 1)
				continue;

			if (tw == 0)
				continue;

			// below/above limit
			statusInformation.warningText.append(std::string("Temperature ") + std::to_string(i + 1) + ": " + DecodeWarningValue(tw, "PaceBmsProtocolV25::ProcessReadStatusInformationResponse") + std::string("; "));
		}

		uint8_t chargeCurrentWarn = ReadHexEncodedByte(response, byteOffset, quietMode);
		statusInformation.warning_value_charge_current = chargeCurrentWarn;
		if (chargeCurrentWarn != 0)
		{
			// below/above limit
			statusInformation.warningText.append(std::string("Charge current: ") + DecodeWarningValue(chargeCurrentWarn, "PaceBmsProtocolV25::ProcessReadStatusInformationResponse") + std::string("; "));
		}

		uint8_t totalVoltageWarn = ReadHexEncodedByte(response, byteOffset, quietMode);
		statusInformation.warning_value_total_voltage = totalVoltageWarn;
		if (totalVoltageWarn != 0)
		{
			// below/above limit
			statusInformation.warningText.append(std::string("Total voltage: ") + DecodeWarningValue(totalVoltageWarn, "PaceBmsProtocolV25::ProcessReadStatusInformationResponse") + std::string("; "));
		}

		uint8_t dischargeCurrentWarn = ReadHexEncodedByte(response, byteOffset, quietMode);
		statusInformation.warning_value_discharge_current = dischargeCurrentWarn;
		if (dischargeCurrentWarn != 0)
		{
			// below/above limit
			statusInformation.warningText.append(std::string("Discharge current: ") + DecodeWarningValue(dischargeCurrentWarn, "PaceBmsProtocolV25::ProcessReadStatusInformationResponse") + std::string("; "));
		}

		// ========================== Protection Status ==========================
		uint8_t protectState1 = ReadHexEncodedByte(response, byteOffset, quietMode);
		statusInformation.protection_value1 = protectState1;
		if (protectState1 != 0)
		{
			statusInformation.protectionText.append(DecodeProtectionStatus1Value(protectState1));
		}

		uint8_t protectState2 = ReadHexEncodedByte(response, byteOffset, quietMode);
		statusInformation.protection_value2 = protectState2;
		if (protectState2 != 0)
		{
			// they put a status bit in here that is not really a protection bit
			statusInformation.protectionText.append(DecodeProtectionStatus2ValueWithoutFullyChargedStatus(protectState2));
			statusInformation.systemText.append(DecodeProtectionStatus2ValueActuallyStatusFullyChargedOnly(protectState2));
		}

		// ========================== System Status ==========================
		uint8_t systemState = ReadHexEncodedByte(response, byteOffset, quietMode);
		statusInformation.system_value = systemState;
		if (systemState != 0)
		{
			statusInformation.systemText.append(DecodeStatusValue(systemState));
		}

		// ========================== Configuration Status ==========================
		uint8_t controlState = ReadHexEncodedByte(response, byteOffset, quietMode);
		statusInformation.configuration_value = controlState;
		if (controlState != 0)
		{
			statusInformation.configurationText.append(DecodeConfigurationStatusValue(controlState, "PaceBmsProtocolV25::ProcessReadStatusInformationResponse"));
		}

		// ========================== Fault Status ==========================
		uint8_t faultState = ReadHexEncodedByte(response, byteOffset, quietMode);
		statusInformation.fault_value = faultState;
		if (faultState != 0)
		{
			statusInformation.faultText.append(DecodeFaultStatusValue(faultState));
		}

		// ========================== Balancing Status ==========================
		uint16_t balanceState = ReadHexEncodedUShort(response, byteOffset, quietMode);
		statusInformation.balancing_value = balanceState;
		for (int i = 0; i < 16; i++)
		{
			if ((balanceState & (1 << i)) != 0)
			{
				statusInformation.balancingText.append(std::string("Cell ") + std::to_string(i + 1) + " is balancing; ");
			}
		}

		// ========================== MORE Warning / Alarm Status ==========================
		// Note: It seems like these two may be a "summary" of the previous "Warning / Alarm" section as it duplicates some of the same warnings,
		//       but I'll leave it for completeness or in case the bit shows up in one place but not the other in practice.
		uint8_t warnState1 = ReadHexEncodedByte(response, byteOffset, quietMode);
		statusInformation.warning_value1 = warnState1;
		if (warnState1 != 0)
		{
			statusInformation.warningText.append(DecodeWarningStatus1Value(warnState1, "PaceBmsProtocolV25::ProcessReadStatusInformationResponse"));
		}

		uint8_t warnState2 = ReadHexEncodedByte(response, byteOffset, quietMode);
		statusInformation.warning_value2 = warnState1;
		if (warnState2 != 0)
		{
			statusInformation.warningText.append(DecodeWarningStatus2Value(warnState2));
		}

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

		// sanity checks to reject nonsensical responses
		if(cellCount == 0)
		{
			logError("Sanity Check: Payload contains zero cells, this looks like an invalid response.");
			return false;
		}
		if(sanityCheck_cellsWithoutVoltageWarnings == 0 && protectState1 == 0 && warnState1 == 0) // both state registers have multiple flags, some voltage related, some not, but on an invalid response everything is zeroed anyway (at least in the test data I've seen so far)
		{
			logError("Sanity Check: Payload indicates all cells have a voltage warning, yet there are no voltage warning (or protection) flags set, this looks like an invalid response.");
			return false;
		}
		if(sanityCheck_cellsWithoutTemperatureWarnings == 0 && protectState2 == 0 && warnState2 == 0) // both state registers have multiple flags, some temperature related, some not, but on an invalid response everything is zeroed anyway (at least in the test data I've seen so far)
		{
			logError("Sanity Check: Payload indicates all cells have a temperature warning, yet there are no temperature warning (or protection) flags set, this looks like an invalid response.");
			return false;
		}

		// skip any extra bytes that are part of this protocol variant
		byteOffset += currentProtocolVariant->statusInformationExtraBytes;

		if(onPayload != nullptr)
			onPayload(payloadCount, index, statusInformation);
	}

	// this check remains valid with broadcast responses due to the loop above
	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13)
	{
		logError("Length mismatch reading status information response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. This will be ignored, but accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
		//return false;
	}

	return true;
}

const unsigned char PaceBmsProtocolV25::exampleReadHardwareVersionRequestV25[]  = "~250146C10000FD9A\r";
const unsigned char PaceBmsProtocolV25::exampleReadHardwareVersionResponseV25[] = "~25014600602850313653313030412D313831322D312E30302000F58E\r";

bool PaceBmsProtocolV25::CreateReadHardwareVersionRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadHardwareVersion, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsProtocolV25::ProcessReadHardwareVersionResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, std::string& hardwareVersion)
{
	hardwareVersion.clear();

	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 40) {
		LogError("ProcessReadHardwareVersionResponse expected payload length of 40 but got " + std::to_string(payloadLen));
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	hardwareVersion.resize(20);
	for (int i = 0; i < 20; i++)
	{
		hardwareVersion[i] = ReadHexEncodedByte(response, byteOffset);
	}

	// remove trailing spaces
	while (hardwareVersion.length() > 0 && (hardwareVersion[hardwareVersion.length() - 1] == ' ' || hardwareVersion[hardwareVersion.length() - 1] == 0)){
		hardwareVersion.pop_back();
	}

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadHardwareVersionResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}

const unsigned char PaceBmsProtocolV25::exampleReadSerialNumberRequestV25[] = "~250146C20000FD99\r";
const unsigned char PaceBmsProtocolV25::exampleReadSerialNumberResponseV25[] = "~25014600B05031383132313031333830333039442020202020202020202020202020202020202020202020202020EE0F\r";

bool PaceBmsProtocolV25::CreateReadSerialNumberRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadSerialNumber, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsProtocolV25::ProcessReadSerialNumberResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, std::string& serialNumber)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 40 && payloadLen != 80) {
		LogError("ProcessReadSerialNumberResponse expected payload length is 40 or 80 but got " + std::to_string(payloadLen));
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	serialNumber.resize(payloadLen / 2);
	for (int i = 0; i < payloadLen / 2; i++)
	{
		serialNumber[i] = ReadHexEncodedByte(response, byteOffset);
	}

	// remove trailing spaces
	while (serialNumber.length() > 0 && (serialNumber[serialNumber.length() - 1] == ' ' || serialNumber[serialNumber.length() - 1] == 0))
	{
		serialNumber.pop_back();
	}

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadSerialNumberResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}

// ============================================================================
// 
// Main "Realtime Monitoring" tab of PBmsTools 2.4
// These are in the "Switch Control" section
// 
// ============================================================================

const unsigned char PaceBmsProtocolV25::exampleWriteDisableBuzzerSwitchCommandRequestV25[]  = "~25004699E0020DFD12\r";
const unsigned char PaceBmsProtocolV25::exampleWriteDisableBuzzerSwitchCommandResponseV25[] = "~25004600C0040D01FCC3\r";
const unsigned char PaceBmsProtocolV25::exampleWriteEnableBuzzerSwitchCommandRequestV25[]   = "~25004699E0020CFD13\r";
const unsigned char PaceBmsProtocolV25::exampleWriteEnableBuzzerSwitchCommandResponseV25[]  = "~25004600C0040C00FCC5\r";

const unsigned char PaceBmsProtocolV25::exampleWriteDisableLedWarningSwitchCommandRequestV25[]  = "~25004699E00206FD20\r";
const unsigned char PaceBmsProtocolV25::exampleWriteDisableLedWarningSwitchCommandResponseV25[] = "~25004600C0040602FCD0\r";
const unsigned char PaceBmsProtocolV25::exampleWriteEnableLedWarningSwitchCommandRequestV25[]   = "~25004699E00207FD1F\r";
const unsigned char PaceBmsProtocolV25::exampleWriteEnableLedWarningSwitchCommandResponseV25[]  = "~25004600C0040722FCCD\r";

const unsigned char PaceBmsProtocolV25::exampleWriteDisableChargeCurrentLimiterSwitchCommandRequestV25[]  = "~25004699E0020AFD15\r";
const unsigned char PaceBmsProtocolV25::exampleWriteDisableChargeCurrentLimiterSwitchCommandResponseV25[] = "~25004600C0040A22FCC3\r";
const unsigned char PaceBmsProtocolV25::exampleWriteEnableChargeCurrentLimiterSwitchCommandRequestV25[]   = "~25004699E0020BFD14\r";
const unsigned char PaceBmsProtocolV25::exampleWriteEnableChargeCurrentLimiterSwitchCommandResponseV25[]  = "~25004600C0040B32FCC1\r";

const unsigned char PaceBmsProtocolV25::exampleWriteSetChargeCurrentLimiterCurrentLimitLowGearSwitchCommandRequestV25[]   = "~25004699E00209FD1D\r";
const unsigned char PaceBmsProtocolV25::exampleWriteSetChargeCurrentLimiterCurrentLimitLowGearSwitchCommandResponseV25[]  = "~25004600C0040938FCC4\r";
const unsigned char PaceBmsProtocolV25::exampleWriteSetChargeCurrentLimiterCurrentLimitHighGearSwitchCommandRequestV25[]  = "~25004699E00208FD1E\r";
const unsigned char PaceBmsProtocolV25::exampleWriteSetChargeCurrentLimiterCurrentLimitHighGearSwitchCommandResponseV25[] = "~25004600C0040830FCCD\r";

bool PaceBmsProtocolV25::CreateWriteSwitchCommandRequest(const uint8_t busId, const SwitchCommand command, std::vector<uint8_t>& request)
{
	// the payload is the control command code
	const uint16_t payloadLen = 2;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, command);

	CreateRequest(busId, CID2_WriteSwitchCommand, payload, request);

	return true;
}
bool PaceBmsProtocolV25::ProcessWriteSwitchCommandResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const SwitchCommand command, const std::span<uint8_t>& response)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	// documented as 2 (decoded) bytes but the meaning of them is undocumented
	// in any case this is the only thing I can be certain enough about to elevate to error status and return failure
	if (payloadLen != 4) {
		LogError("ProcessWriteSwitchCommandResponse expected payload length of 4 but got " + std::to_string(payloadLen));
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

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

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessWriteSwitchCommandResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}

const unsigned char PaceBmsProtocolV25::exampleWriteMosfetChargeOpenSwitchCommandRequestV25[] = "~2500469AE00200FD1E\r";
const unsigned char PaceBmsProtocolV25::exampleWriteMosfetChargeOpenSwitchCommandResponseV25[] = "~25004600E00226FD30\r";
const unsigned char PaceBmsProtocolV25::exampleWriteMosfetChargeCloseSwitchCommandRequestV25[] = "~2500469AE00201FD1D\r";
const unsigned char PaceBmsProtocolV25::exampleWriteMosfetChargeCloseSwitchCommandResponseV25[] = "~25004600E00224FD32\r";

const unsigned char PaceBmsProtocolV25::exampleWriteMosfetDischargeOpenSwitchCommandRequestV25[] = "~2500469BE00200FD1D\r";
const unsigned char PaceBmsProtocolV25::exampleWriteMosfetDischargeOpenSwitchCommandResponseV25[] = "~25004600E00204FD34\r";
const unsigned char PaceBmsProtocolV25::exampleWriteMosfetDischargeCloseSwitchCommandRequestV25[] = "~2500469BE00201FD1C\r";
const unsigned char PaceBmsProtocolV25::exampleWriteMosfetDischargeCloseSwitchCommandResponseV25[] = "~25004609E00204FD2B\r";

bool PaceBmsProtocolV25::CreateWriteMosfetSwitchCommandRequest(const uint8_t busId, const MosfetType type, const MosfetState command, std::vector<uint8_t>& request)
{
	// the payload is the mosfet state to set
	const uint16_t payloadLen = 2;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, command);

	CreateRequest(busId, (CID2)type, payload, request);

	return true;
}
bool PaceBmsProtocolV25::ProcessWriteMosfetSwitchCommandResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const MosfetType type, const MosfetState command, const std::span<uint8_t>& response)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	// documented as 1 (decoded) byte but the meaning of it is undocumented
	// in any case this is the only thing I can be certain enough about to elevate to error status and return failure
	if (payloadLen != 2) {
		LogError("ProcessWriteMosfetSwitchCommandResponse expected payload length of 2 but got " + std::to_string(payloadLen));
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

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

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessWriteMosfetSwitchCommandResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}

const unsigned char PaceBmsProtocolV25::exampleWriteRebootCommandRequestV25[]  = "~2500469CE00201FD1B\r";
const unsigned char PaceBmsProtocolV25::exampleWriteRebootCommandResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::CreateWriteShutdownCommandRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	// the payload is the mosfet state to set
	const uint16_t payloadLen = 2;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, 0x01);

	CreateRequest(busId, CID2_WriteShutdownCommand, payload, request);

	return true;
}
bool PaceBmsProtocolV25::ProcessWriteShutdownCommandResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 0) {
		LogError("ProcessWriteShutdownCommandResponse expected payload length of 0 but got " + std::to_string(payloadLen));
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	// according to documentation, if the RTN code is 0 (this is checked by ValidateResponseAndGetPayloadLength) then it worked, no need to check response payload

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessWriteShutdownCommandResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}

// ============================================================================
// 
// "Memory Information" tab of PBmsTools 2.4 
// 
// ============================================================================

const unsigned char PaceBmsProtocolV25::exampleReadSystemTimeRequestV25[]   = "~250046B10000FD9C\r";
const unsigned char PaceBmsProtocolV25::exampleReadSystemTimeResponseV25[]  = "~25004600400C180815051D1FFB10\r";
const unsigned char PaceBmsProtocolV25::exampleWriteSystemTimeRequestV25[]  = "~250046B2400C1808140E0F25FAFC\r";
const unsigned char PaceBmsProtocolV25::exampleWriteSystemTimeResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::CreateReadSystemDateTimeRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadDateTime, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsProtocolV25::ProcessReadSystemDateTimeResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, DateTime& dateTime)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 12) {
		LogError("ProcessReadSystemDateTimeResponse expected payload length of 12 but got " + std::to_string(payloadLen));
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	dateTime.Year = ReadHexEncodedByte(response, byteOffset) + 2000;
	dateTime.Month = ReadHexEncodedByte(response, byteOffset);
	dateTime.Day = ReadHexEncodedByte(response, byteOffset);
	dateTime.Hour = ReadHexEncodedByte(response, byteOffset);
	dateTime.Minute = ReadHexEncodedByte(response, byteOffset);
	dateTime.Second = ReadHexEncodedByte(response, byteOffset);

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadSystemDateTimeResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteSystemDateTimeRequest(const uint8_t busId, const DateTime dateTime, std::vector<uint8_t>& request)
{
	const uint16_t payloadLen = 12;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, dateTime.Year - 2000);
	WriteHexEncodedByte(payload, payloadOffset, dateTime.Month);
	WriteHexEncodedByte(payload, payloadOffset, dateTime.Day);
	WriteHexEncodedByte(payload, payloadOffset, dateTime.Hour);
	WriteHexEncodedByte(payload, payloadOffset, dateTime.Minute);
	WriteHexEncodedByte(payload, payloadOffset, dateTime.Second);

	CreateRequest(busId, CID2_WriteDateTime, payload, request);

	return true;
}
bool PaceBmsProtocolV25::ProcessWriteSystemDateTimeResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 0) {
		LogError("ProcessWriteSystemDateTimeResponse expected payload length of 0 but got " + std::to_string(payloadLen));
		return false;
	}

	return true;
}

// ============================================================================
// 
// "Parameter Setting" tab of PBmsTools 2.4 with DIP set to address 00 
// PBmsTools 2.4 is broken and can't address packs other than 00 for configuration
// 
// ============================================================================

bool PaceBmsProtocolV25::CreateReadConfigurationRequest(const uint8_t busId, const ReadConfigurationType configType, std::vector<uint8_t>& request)
{
	CreateRequest(busId, (CID2)configType, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsProtocolV25::ProcessWriteConfigurationResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 0) {
		LogError("ProcessWriteConfigurationResponse (empty) expected payload length of 0 but got " + std::to_string(payloadLen));
		return false;
	}

	return true;
}

const unsigned char PaceBmsProtocolV25::exampleReadCellOverVoltageConfigurationRequestV25[]   = "~250046D10000FD9A\r";
const unsigned char PaceBmsProtocolV25::exampleReadCellOverVoltageConfigurationResponseV25[]  = "~25004600F010010E100E740D340AFA35\r";
const unsigned char PaceBmsProtocolV25::exampleWriteCellOverVoltageConfigurationRequestV25[]  = "~250046D0F010010E100E740D340AFA21\r";
const unsigned char PaceBmsProtocolV25::exampleWriteCellOverVoltageConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::ProcessReadConfigurationResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, CellOverVoltageConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 16) {
		LogError("ProcessReadConfigurationResponse (CellOverVoltageConfiguration) expected payload length of 16 but got " + std::to_string(payloadLen));
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

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadConfigurationResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteConfigurationRequest(const uint8_t busId, const CellOverVoltageConfiguration& config, std::vector<uint8_t>& request)
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

const unsigned char PaceBmsProtocolV25::exampleReadPackOverVoltageConfigurationRequestV25[]   = "~250046D50000FD96\r";
const unsigned char PaceBmsProtocolV25::exampleReadPackOverVoltageConfigurationResponseV25[]  = "~25004600F01001E100E740D2F00AFA24\r";
const unsigned char PaceBmsProtocolV25::exampleWritePackOverVoltageConfigurationRequestV25[]  = "~250046D4F01001E10AE740D2F00AF9FB\r";
const unsigned char PaceBmsProtocolV25::exampleWritePackOverVoltageConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::ProcessReadConfigurationResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, PackOverVoltageConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 16) {
		LogError("ProcessReadConfigurationResponse (PackOverVoltageConfiguration) expected payload length of 16 but got " + std::to_string(payloadLen));
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

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadConfigurationResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteConfigurationRequest(const uint8_t busId, const PackOverVoltageConfiguration& config, std::vector<uint8_t>& request)
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

const unsigned char PaceBmsProtocolV25::exampleReadCellUnderVoltageConfigurationRequestV25[]   = "~250046D30000FD98\r";
const unsigned char PaceBmsProtocolV25::exampleReadCellUnderVoltageConfigurationResponseV25[]  = "~25004600F010010AF009C40B540AFA24\r";
const unsigned char PaceBmsProtocolV25::exampleWriteCellUnderVoltageConfigurationRequestV25[]  = "~250046D2F010010AF009C40B540AFA0E\r";
const unsigned char PaceBmsProtocolV25::exampleWriteCellUnderVoltageConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::ProcessReadConfigurationResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, CellUnderVoltageConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 16) {
		LogError("ProcessReadConfigurationResponse (CellUnderVoltageConfiguration) expected payload length of 16 but got " + std::to_string(payloadLen));
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

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadConfigurationResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteConfigurationRequest(const uint8_t busId, const CellUnderVoltageConfiguration& config, std::vector<uint8_t>& request)
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

const unsigned char PaceBmsProtocolV25::exampleReadPackUnderVoltageConfigurationRequestV25[]   = "~250046D70000FD94\r";
const unsigned char PaceBmsProtocolV25::exampleReadPackUnderVoltageConfigurationResponseV25[]  = "~25004600F01001AF009C40B5400AFA24\r";
const unsigned char PaceBmsProtocolV25::exampleWritePackUnderVoltageConfigurationRequestV25[]  = "~250046D6F01001AF009C40B5400AFA0A\r";
const unsigned char PaceBmsProtocolV25::exampleWritePackUnderVoltageConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::ProcessReadConfigurationResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, PackUnderVoltageConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 16) {
		LogError("ProcessReadConfigurationResponse (PackUnderVoltageConfiguration) expected payload length of 16 but got " + std::to_string(payloadLen));
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

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadConfigurationResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteConfigurationRequest(const uint8_t busId, const PackUnderVoltageConfiguration& config, std::vector<uint8_t>& request)
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

const unsigned char PaceBmsProtocolV25::exampleReadChargeOverCurrentConfigurationRequestV25[]   = "~250046D90000FD92\r";
const unsigned char PaceBmsProtocolV25::exampleReadChargeOverCurrentConfigurationResponseV25[]  = "~25004600400C010068006E0AFB1D\r";
const unsigned char PaceBmsProtocolV25::exampleWriteChargeOverCurrentConfigurationRequestV25[]  = "~250046D8400C010068006E0AFB01\r";
const unsigned char PaceBmsProtocolV25::exampleWriteChargeOverCurrentConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::ProcessReadConfigurationResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, ChargeOverCurrentConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 12) {
		LogError("ProcessReadConfigurationResponse (ChargeOverCurrentConfiguration) expected payload length of 12 but got " + std::to_string(payloadLen));
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

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadConfigurationResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteConfigurationRequest(const uint8_t busId, const ChargeOverCurrentConfiguration& config, std::vector<uint8_t>& request)
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

	return true;
}

const unsigned char PaceBmsProtocolV25::exampleReadDishargeOverCurrent1ConfigurationRequestV25[]   = "~250046DB0000FD89\r";
const unsigned char PaceBmsProtocolV25::exampleReadDishargeOverCurrent1ConfigurationResponseV25[]  = "~25004600400C01FF97FF920AFAD3\r";
const unsigned char PaceBmsProtocolV25::exampleWriteDishargeOverCurrent1ConfigurationRequestV25[]  = "~250046DA400C010069006E0AFAF7\r";
const unsigned char PaceBmsProtocolV25::exampleWriteDishargeOverCurrent1ConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::ProcessReadConfigurationResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, DischargeOverCurrent1Configuration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 12) {
		LogError("ProcessReadConfigurationResponse (DischargeOverCurrent1Configuration) expected payload length of 12 but got " + std::to_string(payloadLen));
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

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadConfigurationResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteConfigurationRequest(const uint8_t busId, const DischargeOverCurrent1Configuration& config, std::vector<uint8_t>& request)
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

const unsigned char PaceBmsProtocolV25::exampleReadDishargeOverCurrent2ConfigurationRequestV25[]   = "~250046E30000FD97\r";
const unsigned char PaceBmsProtocolV25::exampleReadDishargeOverCurrent2ConfigurationResponseV25[]  = "~25004600400C009604009604FB32\r";
const unsigned char PaceBmsProtocolV25::exampleWriteDishargeOverCurrent2ConfigurationRequestV25[]  = "~250046E2A006009604FC4E\r";
const unsigned char PaceBmsProtocolV25::exampleWriteDishargeOverCurrent2ConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::ProcessReadConfigurationResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, DischargeOverCurrent2Configuration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	// accept the "correct" and the "seen" value in case they fix their firmware at some point
	if (payloadLen != 6 && payloadLen != 12) {
		LogError("ProcessReadConfigurationResponse (DischargeOverCurrent2Configuration) expected payload length of 6 or 12 but got " + std::to_string(payloadLen));
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
	byteOffset += 6;

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadConfigurationResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteConfigurationRequest(const uint8_t busId, const DischargeOverCurrent2Configuration& config, std::vector<uint8_t>& request)
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

const unsigned char PaceBmsProtocolV25::exampleReadShortCircuitProtectionConfigurationRequestV25[]   = "~250046E50000FD95\r";
const unsigned char PaceBmsProtocolV25::exampleReadShortCircuitProtectionConfigurationResponseV25[]  = "~25004600E0020CFD25\r";
const unsigned char PaceBmsProtocolV25::exampleWriteShortCircuitProtectionConfigurationRequestV25[]  = "~250046E4E0020CFD0C\r";
const unsigned char PaceBmsProtocolV25::exampleWriteShortCircuitProtectionConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::ProcessReadConfigurationResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, ShortCircuitProtectionConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 2) {
		LogError("ProcessReadConfigurationResponse (ShortCircuitProtectionConfiguration) expected payload length of 2 but got " + std::to_string(payloadLen));
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	config.ProtectionDelayMicroseconds = ReadHexEncodedByte(response, byteOffset) * 25;

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadConfigurationResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteConfigurationRequest(const uint8_t busId, const ShortCircuitProtectionConfiguration& config, std::vector<uint8_t>& request)
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

const unsigned char PaceBmsProtocolV25::exampleReadCellBalancingConfigurationRequestV25[]   = "~250046B60000FD97\r";
const unsigned char PaceBmsProtocolV25::exampleReadCellBalancingConfigurationResponseV25[]  = "~2500460080080D48001EFBE9\r";
const unsigned char PaceBmsProtocolV25::exampleWriteCellBalancingConfigurationRequestV25[]  = "~250046B580080D48001EFBD2\r";
const unsigned char PaceBmsProtocolV25::exampleWriteCellBalancingConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::ProcessReadConfigurationResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, CellBalancingConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 8) {
		LogError("ProcessReadConfigurationResponse (CellBalancingConfiguration) expected payload length of 8 but got " + std::to_string(payloadLen));
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	config.ThresholdMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.DeltaCellMillivolts = ReadHexEncodedUShort(response, byteOffset);

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadConfigurationResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteConfigurationRequest(const uint8_t busId, const CellBalancingConfiguration& config, std::vector<uint8_t>& request)
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

const unsigned char PaceBmsProtocolV25::exampleReadSleepConfigurationRequestV25[]   = "~250046A00000FD9E\r";
const unsigned char PaceBmsProtocolV25::exampleReadSleepConfigurationResponseV25[]  = "~2500460080080C1C0005FBF3\r";
const unsigned char PaceBmsProtocolV25::exampleWriteSleepConfigurationRequestV25[]  = "~250046A880080C1C0005FBDA\r";
const unsigned char PaceBmsProtocolV25::exampleWriteSleepConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::ProcessReadConfigurationResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, SleepConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 8) {
		LogError("ProcessReadConfigurationResponse (SleepConfiguration) expected payload length of 8 but got " + std::to_string(payloadLen));
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

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadConfigurationResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteConfigurationRequest(const uint8_t busId, const SleepConfiguration& config, std::vector<uint8_t>& request)
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

const unsigned char PaceBmsProtocolV25::exampleReadFullChargeLowChargeConfigurationRequestV25[]   = "~250046AF0000FD88\r";
const unsigned char PaceBmsProtocolV25::exampleReadFullChargeLowChargeConfigurationResponseV25[]  = "~25004600600ADAC007D005FB60\r";
const unsigned char PaceBmsProtocolV25::exampleWriteFullChargeLowChargeConfigurationRequestV25[]  = "~250046AE600ADAC007D005FB3A\r";
const unsigned char PaceBmsProtocolV25::exampleWriteFullChargeLowChargeConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::ProcessReadConfigurationResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, FullChargeLowChargeConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 10) {
		LogError("ProcessReadConfigurationResponse (FullChargeLowChargeConfiguration) expected payload length of 10 but got " + std::to_string(payloadLen));
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	config.FullChargeMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.FullChargeMilliamps = ReadHexEncodedUShort(response, byteOffset);
	config.LowChargeAlarmPercent = ReadHexEncodedByte(response, byteOffset);

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadConfigurationResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteConfigurationRequest(const uint8_t busId, const FullChargeLowChargeConfiguration& config, std::vector<uint8_t>& request)
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

const unsigned char PaceBmsProtocolV25::exampleReadChargeAndDischargeOverTemperatureConfigurationRequestV25[]   = "~250046DD0000FD87\r";
const unsigned char PaceBmsProtocolV25::exampleReadChargeAndDischargeOverTemperatureConfigurationResponseV25[]  = "~25004600501A010CA80CD00C9E0CDA0D020CD0F7BE\r";
const unsigned char PaceBmsProtocolV25::exampleWriteChargeAndDischargeOverTemperatureConfigurationRequestV25[]  = "~250046DC501A010CA80CD00C9E0CDA0D020CD0F797\r";
const unsigned char PaceBmsProtocolV25::exampleWriteChargeAndDischargeOverTemperatureConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::ProcessReadConfigurationResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, ChargeAndDischargeOverTemperatureConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 26) {
		LogError("ProcessReadConfigurationResponse (ChargeAndDischargeOverTemperatureConfiguration) expected payload length of 26 but got " + std::to_string(payloadLen));
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

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadConfigurationResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteConfigurationRequest(const uint8_t busId, const ChargeAndDischargeOverTemperatureConfiguration& config, std::vector<uint8_t>& request)
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

const unsigned char PaceBmsProtocolV25::exampleReadChargeAndDischargeUnderTemperatureConfigurationRequestV25[]   = "~250046DF0000FD85\r";
const unsigned char PaceBmsProtocolV25::exampleReadChargeAndDischargeUnderTemperatureConfigurationResponseV25[]  = "~25004600501A010AAA0A780AAA0A1409E20A14F7E5\r";
const unsigned char PaceBmsProtocolV25::exampleWriteChargeAndDischargeUnderTemperatureConfigurationRequestV25[]  = "~250046DE501A010AAA0A780AAA0A1409E20A14F7BC\r";
const unsigned char PaceBmsProtocolV25::exampleWriteChargeAndDischargeUnderTemperatureConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::ProcessReadConfigurationResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, ChargeAndDischargeUnderTemperatureConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 26) {
		LogError("ProcessReadConfigurationResponse (ChargeAndDischargeUnderTemperatureConfiguration) expected payload length of 26 but got " + std::to_string(payloadLen));
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

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadConfigurationResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteConfigurationRequest(const uint8_t busId, const ChargeAndDischargeUnderTemperatureConfiguration& config, std::vector<uint8_t>& request)
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

const unsigned char PaceBmsProtocolV25::exampleReadMosfetOverTemperatureConfigurationRequestV25[]   = "~250046E10000FD99\r";
const unsigned char PaceBmsProtocolV25::exampleReadMosfetOverTemperatureConfigurationResponseV25[]  = "~25004600200E010E2E0EF60DFCFA5D\r";
const unsigned char PaceBmsProtocolV25::exampleWriteMosfetOverTemperatureConfigurationRequestV25[]  = "~250046E0200E010E2E0EF60DFCFA48\r";
const unsigned char PaceBmsProtocolV25::exampleWriteMosfetOverTemperatureConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::ProcessReadConfigurationResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, MosfetOverTemperatureConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 14) {
		LogError("ProcessReadConfigurationResponse (MosfetOverTemperatureConfiguration) expected payload length of 14 but got " + std::to_string(payloadLen));
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

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadConfigurationResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteConfigurationRequest(const uint8_t busId, const MosfetOverTemperatureConfiguration& config, std::vector<uint8_t>& request)
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

const unsigned char PaceBmsProtocolV25::exampleReadEnvironmentOverUnderTemperatureConfigurationRequestV25[]   = "~250046E70000FD93\r";
const unsigned char PaceBmsProtocolV25::exampleReadEnvironmentOverUnderTemperatureConfigurationResponseV25[]  = "~25004600501A0109E209B009E20D340D660D34F806\r";
const unsigned char PaceBmsProtocolV25::exampleWriteEnvironmentOverUnderTemperatureConfigurationRequestV25[]  = "~250046E6501A0109E209B009E20D340D660D34F7EB\r";
const unsigned char PaceBmsProtocolV25::exampleWriteEnvironmentOverUnderTemperatureConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::ProcessReadConfigurationResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, EnvironmentOverUnderTemperatureConfiguration& config)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 26) {
		LogError("ProcessReadConfigurationResponse (EnvironmentOverUnderTemperatureConfiguration) expected payload length of 26 but got " + std::to_string(payloadLen));
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

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadConfigurationResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteConfigurationRequest(const uint8_t busId, const EnvironmentOverUnderTemperatureConfiguration& config, std::vector<uint8_t>& request)
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

const unsigned char PaceBmsProtocolV25::exampleReadChargeCurrentLimiterStartCurrentRequestV25[]   = "~250046ED0000FD86\r";
const unsigned char PaceBmsProtocolV25::exampleReadChargeCurrentLimiterStartCurrentResponseV25[]  = "~25004600C0040064FCCE\r";
const unsigned char PaceBmsProtocolV25::exampleWriteChargeCurrentLimiterStartCurrentRequestV25[]  = "~250046EEC0040064FCA4\r";
const unsigned char PaceBmsProtocolV25::exampleWriteChargeCurrentLimiterStartCurrentResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::CreateReadChargeCurrentLimiterStartCurrentRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadChargeCurrentLimiterStartCurrent, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsProtocolV25::ProcessReadChargeCurrentLimiterStartCurrentResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, uint8_t& current)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 4) {
		LogError("ProcessReadChargeCurrentLimiterStartCurrentResponse expected payload length of 4 but got " + std::to_string(payloadLen));
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	// note that this is the *payload* busId, not the header busId which was already validated
	uint8_t busIdResponding = ReadHexEncodedByte(response, byteOffset);
	if (busIdResponding != busId)
	{
		LogError("Response from wrong bus Id in payload, expected " + std::to_string(busId) + " but got " + std::to_string(busIdResponding));
		return false;
	}

	current = ReadHexEncodedByte(response, byteOffset);

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadChargeCurrentLimiterStartCurrentResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteChargeCurrentLimiterStartCurrentRequest(const uint8_t busId, const uint8_t current, std::vector<uint8_t>& request)
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
bool PaceBmsProtocolV25::ProcessWriteChargeCurrentLimiterStartCurrentResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 0) {
		LogError("ProcessWriteChargeCurrentLimiterStartCurrentResponse expected payload length of 0 but got " + std::to_string(payloadLen));
		return false;
	}

	return true;
}

const unsigned char PaceBmsProtocolV25::exampleReadRemainingCapacityRequestV25[]  = "~250046A60000FD98\r";
const unsigned char PaceBmsProtocolV25::exampleReadRemainingCapacityResponseV25[] = "~25004600400C183C286A2710FB0E\r";

bool PaceBmsProtocolV25::CreateReadRemainingCapacityRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadRemainingCapacity, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsProtocolV25::ProcessReadRemainingCapacityResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, uint32_t& remainingCapacityMilliampHours, uint32_t& actualCapacityMilliampHours, uint32_t& designCapacityMilliampHours)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 12) {
		LogError("ProcessReadRemainingCapacityResponse expected payload length of 12 but got " + std::to_string(payloadLen));
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	remainingCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset) * 10;
	actualCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset) * 10;
	designCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset) * 10;

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadRemainingCapacityResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}

const unsigned char PaceBmsProtocolV25::exampleReadProtocolsRequestV25[]   = "~250046EB0000FD88\r";
const unsigned char PaceBmsProtocolV25::exampleReadProtocolsResponseV25[]  = "~25004600A006131400FC6F\r";
const unsigned char PaceBmsProtocolV25::exampleWriteProtocolsRequestV25[]  = "~250046ECA006131400FC47\r";
const unsigned char PaceBmsProtocolV25::exampleWriteProtocolsResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsProtocolV25::CreateReadProtocolsRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadCommunicationsProtocols, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsProtocolV25::ProcessReadProtocolsResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response, Protocols& protocols)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 6) {
		LogError("ProcessReadProtocolsResponse expected payload length of 6 but got " + std::to_string(payloadLen));
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	protocols.CAN = (ProtocolList_CAN)ReadHexEncodedByte(response, byteOffset);
	protocols.RS485 = (ProtocolList_RS485)ReadHexEncodedByte(response, byteOffset);
	protocols.Type = (ProtocolList_Type)ReadHexEncodedByte(response, byteOffset);

	// we expect to be exactly at the end of the payload now
	if (byteOffset != payloadLen + 13 /* frame header length */) {
		LogError("Length mismatch reading ProcessReadProtocolsResponse response: " + std::to_string(payloadLen + 13 - byteOffset) + " bytes off. Accuracy of readouts may be compromised. Please file an issue report with full logs at VERY_VERBOSE level.");
	}

	return true;
}
bool PaceBmsProtocolV25::CreateWriteProtocolsRequest(const uint8_t busId, const Protocols& protocols, std::vector<uint8_t>& request)
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
bool PaceBmsProtocolV25::ProcessWriteProtocolsResponse(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t>& response)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, respondingBusId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}
	if (payloadLen != 0) {
		LogError("ProcessWriteProtocolsResponse expected payload length of 0 but got " + std::to_string(payloadLen));
		return false;
	}

	return true;
}
