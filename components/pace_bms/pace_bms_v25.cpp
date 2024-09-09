
#include "pace_bms_v25.h"



// takes pointers to the "real" logging functions
PaceBmsV25::PaceBmsV25(PaceBmsV25::LogFuncPtr logError, PaceBmsV25::LogFuncPtr logWarning, PaceBmsV25::LogFuncPtr logInfo, PaceBmsV25::LogFuncPtr logDebug, PaceBmsV25::LogFuncPtr logVerbose, PaceBmsV25::LogFuncPtr logVeryVerbose)
{
	LogErrorPtr = logError;
	LogWarningPtr = logWarning;
	LogInfoPtr = logInfo;
	LogDebugPtr = logDebug;
	LogVerbosePtr = logVerbose;
	LogVeryVerbosePtr = logVeryVerbose;
}

// dependency injection
PaceBmsV25::LogFuncPtr LogErrorPtr;
PaceBmsV25::LogFuncPtr LogWarningPtr;
PaceBmsV25::LogFuncPtr LogInfoPtr;
PaceBmsV25::LogFuncPtr LogDebugPtr;
PaceBmsV25::LogFuncPtr LogVerbosePtr;
PaceBmsV25::LogFuncPtr LogVeryVerbosePtr;

void PaceBmsV25::LogError(std::string message)
{
	if (LogErrorPtr != 0)
	{
		LogErrorPtr(message);
	}
}
void PaceBmsV25::LogWarning(std::string message)
{
	if (LogWarningPtr != 0)
	{
		LogWarningPtr(message);
	}
}
void PaceBmsV25::LogInfo(std::string message)
{
	if (LogInfoPtr != 0)
	{
		LogInfoPtr(message);
	}
}
void PaceBmsV25::LogDebug(std::string message)
{
	if (LogDebugPtr != 0)
	{
		LogDebugPtr(message);
	}
}
void PaceBmsV25::LogVerbose(std::string message)
{
	if (LogVerbosePtr != 0)
	{
		LogVerbosePtr(message);
	}
}
void PaceBmsV25::LogVeryVerbose(std::string message)
{
	if (LogVeryVerbosePtr != 0)
	{
		LogVeryVerbosePtr(message);
	}
}

std::string PaceBmsV25::FormatReturnCode(const uint8_t returnCode)
{
	switch (returnCode)
	{
	case 0:
		return std::string("OK");
		break;
	case 1:
		return std::string("Documented as Undefined 1");
		break;
	case 2:
		return std::string("CKSUM Error (full request checksum)");
		break;
	case 3:
		return std::string("LCKSUM Error (checksum of embedded payload length value)");
		break;
	case 4:
		return std::string("CID2 Undefined (unknown command)");
		break;
	case 5:
		return std::string("Documented as Undefined 5");
		break;
	case 6:
		return std::string("Documented as Undefined 6");
		break;
	case 9:
		return std::string("Operation or Write Error");
		break;
	default:
		return std::string("Undocumented Response Error Code");
		break;
	}

	return std::string("impossible");
}

// Takes a length value and adds a checksum to the upper nibble, this is "CKLEN" used in command or response headers
uint16_t PaceBmsV25::CreateChecksummedLength(const uint16_t cklen)
{
	// max value of length is 12 bit, top 4 bits will be checksum of actual length value
	uint16_t len = (cklen & 0x0FFF);

	// bitwise NOT then add 1, mask off any carry (even though we'd shift it out anyway in the next step)
	uint16_t lcksum = ~len;
	lcksum++;
	if (lcksum == 0xFFF0)
		lcksum = 0xF;
	else if (lcksum == 0xFFE6)
		lcksum = 0x5;
	else
		lcksum = lcksum & 0x000F;

	// checksum goes in the top nibble, length in the bottom 3 nibbles
	return (lcksum << 12) | len;
}

// Checks if the checksum nibble in a "checksummed length" is valid
bool PaceBmsV25::ValidateChecksummedLength(const uint16_t cklen)
{
	// actual length from bottom 3 nibbles
	uint16_t len = (cklen & 0x0FFF);

	// bitwise NOT then add 1, mask off any carry (even though we'd shift it out anyway in the next step)
	uint16_t lcksum = ~len;
	lcksum++;
	lcksum = lcksum & 0x000F;

	// checksum under test from top nibble
	uint16_t lcksumtest = cklen & 0xF000;
	lcksumtest = lcksumtest >> 12;

	return lcksumtest == lcksum;
}

// Length is just the lower 12 bits of the checksummed length 
uint16_t PaceBmsV25::LengthFromChecksummedLength(const uint16_t cklen)
{
	return cklen & 0x0FFF;
}

// Calculates the checksum for an entire request or response "packet" (this is not for the embedded length value)
uint16_t PaceBmsV25::CalculateRequestOrResponseChecksum(const std::vector<uint8_t>& data)
{
	uint32_t cksum = 0;

	// ignore SOI (first byte) EOI (last byte) and the checksum itself (the 4 bytes before EOI)
	for (int i = 1; i < (int)data.size() - 5; i++)
	{
		cksum += data[i];
	}

	// bitwise NOT then add 1, mask off all but lowest short
	cksum = ~cksum;
	cksum++;
	cksum = cksum & 0xFFFF;

	return (uint16_t)cksum;
}

// helper for WriteHexEncoded----
// Works with ASCII encoding, not portable, but that's what the protocol uses
uint8_t PaceBmsV25::NibbleToHex(const uint8_t nibbleByte)
{
	uint8_t nibble = nibbleByte & 0x0F;

	if (nibble < 10)
		return nibble + '0';  // Return a character from '0' to '9'
	else
		return nibble + 'A' - 10;  // Return a character from 'A' to 'F'
}

// todo: error check input range and log error and/or handle upstream to abort decode
// helper for ReadHexEncoded----
// Works with ASCII encoding, not portable, but that's what the protocol uses
uint8_t PaceBmsV25::HexToNibble(const uint8_t hex)
{
	if (hex < 65)
		return hex - '0';  // Return a value from 0 to 9
	else
		return hex + 10 - 'A';  // Return a value from 10 to 15
}

// decode a 'real' byte from the stream by reading two ASCII hex encoded bytes
uint8_t PaceBmsV25::ReadHexEncodedByte(const std::vector<uint8_t>& data, uint16_t& dataOffset)
{
	uint8_t byte = 0;
	byte |= (HexToNibble(data[dataOffset++]) << 4) & 0xF0;
	byte |= (HexToNibble(data[dataOffset++]) << 0) & 0x0F;
	return byte;
}

// decode a 'real' uint16_t from the stream by reading four ASCII hex encoded bytes
uint16_t PaceBmsV25::ReadHexEncodedUShort(const std::vector<uint8_t>& data, uint16_t& dataOffset)
{
	uint16_t ushort = 0;
	ushort |= ((HexToNibble(data[dataOffset++]) << 12) & 0xF000);
	ushort |= ((HexToNibble(data[dataOffset++]) << 8)  & 0x0F00);
	ushort |= ((HexToNibble(data[dataOffset++]) << 4)  & 0x00F0);
	ushort |= ((HexToNibble(data[dataOffset++]) << 0)  & 0x000F);
	return ushort;
}

// decode a 'real' int16_t from the stream by reading four ASCII hex encoded bytes
int16_t PaceBmsV25::ReadHexEncodedSShort(const std::vector<uint8_t>& data, uint16_t& dataOffset)
{
	int16_t sshort = 0;
	sshort |= ((HexToNibble(data[dataOffset++]) << 12) & 0xF000);
	sshort |= ((HexToNibble(data[dataOffset++]) << 8)  & 0x0F00);
	sshort |= ((HexToNibble(data[dataOffset++]) << 4)  & 0x00F0);
	sshort |= ((HexToNibble(data[dataOffset++]) << 0)  & 0x000F);
	return sshort;
}

// encode a 'real' byte to the stream by writing two ASCII hex encoded bytes
void PaceBmsV25::WriteHexEncodedByte(std::vector<uint8_t>& data, uint16_t& dataOffset, uint8_t byte)
{
	data[dataOffset++] = NibbleToHex((byte >> 4) & 0x0F);
	data[dataOffset++] = NibbleToHex((byte >> 0) & 0x0F);
}

// encode a 'real' uint16_t to the stream by writing four ASCII hex encoded bytes
void PaceBmsV25::WriteHexEncodedUShort(std::vector<uint8_t>& data, uint16_t& dataOffset, uint16_t ushort)
{
	data[dataOffset++] = NibbleToHex((ushort >> 12) & 0x0F);
	data[dataOffset++] = NibbleToHex((ushort >> 8)  & 0x0F);
	data[dataOffset++] = NibbleToHex((ushort >> 4)  & 0x0F);
	data[dataOffset++] = NibbleToHex((ushort >> 0)  & 0x0F);
}

// encode a 'real' int16_t to the stream by writing four ASCII hex encoded bytes
void PaceBmsV25::WriteHexEncodedSShort(std::vector<uint8_t>& data, uint16_t& dataOffset, int16_t sshort)
{
	data[dataOffset++] = NibbleToHex((sshort >> 12) & 0x0F);
	data[dataOffset++] = NibbleToHex((sshort >> 8)  & 0x0F);
	data[dataOffset++] = NibbleToHex((sshort >> 4)  & 0x0F);
	data[dataOffset++] = NibbleToHex((sshort >> 0)  & 0x0F);
}

// create a standard request to the given busId for the given CID2, filling in the payload (if given)
void PaceBmsV25::CreateRequest(const uint8_t busId, const CID2 cid2, const std::vector<uint8_t> payload, std::vector<uint8_t>& request)
{
	uint16_t byteOffset = 0;

	request.resize(payload.size() + 18);

	// SOI marker
	request[byteOffset++] = '~';

	// version 25
	WriteHexEncodedByte(request, byteOffset, 0x25);

	// busId
	WriteHexEncodedByte(request, byteOffset, busId);

	// cid1
	WriteHexEncodedByte(request, byteOffset, CID1_LithiumIron);

	// cid2
	WriteHexEncodedByte(request, byteOffset, cid2);

	// checksummed payload length
	uint16_t ckLen = CreateChecksummedLength((uint16_t)payload.size());
	WriteHexEncodedUShort(request, byteOffset, ckLen);

	// copy payload
	std::copy(payload.data(), payload.data() + payload.size(), request.data() + byteOffset);
	byteOffset += (uint16_t)payload.size();

	// full request checksum
	uint16_t cksum = CalculateRequestOrResponseChecksum(request);
	WriteHexEncodedUShort(request, byteOffset, cksum);

	// EOI marker
	request[byteOffset++] = '\r';

	if (byteOffset != payload.size() + 18)
	{
		const char* message = "Length mismatch creating request, this is a code bug in PACE_BMS";
		LogError(message);
	}
}

// validate all fields in the response except the payload data: SOI marker, header values, checksum, EOI marker
// returns the detected payload length (payload always starts at offset 13), or -1 for error
int16_t PaceBmsV25::ValidateResponseAndGetPayloadLength(const uint8_t busId, const std::vector<uint8_t> response)
{
	uint16_t byteOffset = 0;

	// the number of bytes for a response with zero payload, we'll check again once we decode the checksummed length embedded in the response to make sure we don't run past the end of the buffer
	if (response.size() < 18)
	{
		const char* message = "Response is truncated, even a response without payload should be 18 bytes";
		LogError(message);
		return -1;
	}

	if (response[byteOffset++] != '~')
	{
		const char* message = "Response does not begin with SOI marker";
		LogError(message);
		return -1;
	}

	uint8_t ver = ReadHexEncodedByte(response, byteOffset);
	if (ver != 0x25)
	{
		const char* message = "Response has wrong version number, only version 2.5 is supported";
		LogError(message);
		return -1;
	}

	uint8_t addr = ReadHexEncodedByte(response, byteOffset);
	if (addr != busId)
	{
		const char* message = "Response from wrong bus Id";
		LogError(message);
		return -1;
	}

	uint8_t cid = ReadHexEncodedByte(response, byteOffset);
	if (cid != CID1_LithiumIron)
	{
		const char* message = "Response has wrong CID1, expect 0x46 = Lithium Iron";
		LogError(message);
		return -1;
	}

	uint8_t returnCode = ReadHexEncodedByte(response, byteOffset);
	if (returnCode != 0)
	{
		std::string message = std::string("Error code returned by device: ") + FormatReturnCode(returnCode);
		LogError(message);
		return -1;
	}

	// decode payload length + length-checksum
	uint16_t cklen = ReadHexEncodedUShort(response, byteOffset);
	if (!ValidateChecksummedLength(cklen))
	{
		// FIRMWARE BUG: I verified this "on the wire", my unit is not setting the length checksum (or setting it incorrectly) on some responses but not others
		// makes this check useless - guess we just log and move on
		const char* message = "Response contains an incorrect payload length checksum, ignoring since this is a known firmware bug";
		LogVerbose(message);
	}

	uint16_t payloadLen = LengthFromChecksummedLength(cklen);

	if (payloadLen + 18 < (uint16_t)response.size() && response[response.size() - 1] == '\r')
	{
		// FIRMWARE BUG: I verified this "on the wire", my unit is returning an inaccurate payload length on some responses but not others
		// here we can't ignore and have to actively fix the value for further processing, but at least we can calculate it against the EOI, and that is always present
		const char* message = "Response contains an incorrect payload length, fixing up the value by checking against EOI, this is a known firmware bug";
		LogVerbose(message);

		payloadLen = (uint16_t)response.size() - 18;
	}

	if ((uint16_t)response.size() < payloadLen + 18)
	{
		const char* message = "Response is truncated, should be 18 bytes + decoded payload length";
		LogError(message);
		return -1;
	}
	if ((uint16_t)response.size() > payloadLen + 18)
	{
		const char* message = "Response is oversize";
		LogError(message);
		return -1;
	}

	// now that we have payload length and have verified buffer size, jump past the payload and confirm the checksum of the entire packet
	byteOffset += payloadLen;
	uint16_t givenCksum = ReadHexEncodedUShort(response, byteOffset);
	uint16_t calcCksum = CalculateRequestOrResponseChecksum(response);
	if (givenCksum != calcCksum)
	{
		// FIRMWARE BUG: I verified this "on the wire", my unit is calculating incorrect checksums on some responses but not others
		// makes this check useless, guess we just log and move on
		const char* message = "Response contains an incorrect checksum, ignoring since this is a known firmware bug";
		LogVerbose(message);
	}

	if (response[byteOffset++] != '\r')
	{
		const char* message = "Response does not end with EOI marker";
		LogError(message);
		return -1;
	}

	if (byteOffset != payloadLen + 18)
	{
		const char* message = "Length mismatch validating response, this is a code bug in PACE_BMS";
		LogError(message);
		return -1;
	}

	return payloadLen;
}

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

const unsigned char PaceBmsV25::exampleReadAnalogInformationRequestV25[] = "~25014642E00201FD30\r";
const unsigned char PaceBmsV25::exampleReadAnalogInformationResponseV25[] = "~25014600F07A0001100CC70CC80CC70CC70CC70CC50CC60CC70CC70CC60CC70CC60CC60CC70CC60CC7060B9B0B990B990B990BB30BBCFF1FCCCD12D303286A008C2710E1E4\r";

bool PaceBmsV25::CreateReadAnalogInformationRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	// the payload is the requested busId (could be FF for "get all" when speaking to a set of daisy-chained units but this code doesn't support that)
	const uint16_t payloadLen = 2;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, busId);

	CreateRequest(busId, CID2_ReadAnalogInformation, payload, request);

	return true;
}
bool PaceBmsV25::ProcessReadAnalogInformationResponse(const uint8_t busId, const std::vector<uint8_t>& response, AnalogInformation& analogInformation)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	// SPEC BUG: doc says the response starts with the busId, but "on the wire" I see an extra byte value of 0x00 preceeding it
	uint8_t unknown = ReadHexEncodedByte(response, byteOffset);
	if (unknown != 0)
	{
		LogVerbose("Response contains a value other than zero before the BusId");
	}

	uint8_t busIdResponding = ReadHexEncodedByte(response, byteOffset);
	if (busIdResponding != busId)
	{
		const char* message = "Response from wrong bus Id in payload";
		LogError(message);
		return false;
	}

	analogInformation.cellCount = ReadHexEncodedByte(response, byteOffset);
	if (analogInformation.cellCount > MAX_CELL_COUNT)
	{
		const char* message = "Response contains more cell voltage readings than are supported, results will be truncated";
		LogWarning(message);
	}
	for (int i = 0; i < analogInformation.cellCount; i++)
	{
		uint16_t cellVoltage = ReadHexEncodedUShort(response, byteOffset);

		if (i > MAX_CELL_COUNT - 1)
			continue;

		analogInformation.cellVoltagesMillivolts[i] = cellVoltage;
	}

	analogInformation.temperatureCount = ReadHexEncodedByte(response, byteOffset);
	if (analogInformation.temperatureCount > MAX_TEMP_COUNT)
	{
		const char* message = "Response contains more temperature readings than are supported, results will be truncated";
		LogWarning(message);
	}
	for (int i = 0; i < analogInformation.temperatureCount; i++)
	{
		uint16_t temperature = ReadHexEncodedUShort(response, byteOffset);

		if (i > MAX_TEMP_COUNT - 1)
			continue;

		analogInformation.temperaturesTenthsCelcius[i] = (temperature - 2730);
	}

	analogInformation.currentMilliamps = ReadHexEncodedSShort(response, byteOffset) * 10;

	analogInformation.totalVoltageMillivolts = ReadHexEncodedUShort(response, byteOffset);

	analogInformation.remainingCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset) * 10;

	uint8_t P3 = ReadHexEncodedByte(response, byteOffset);
	if (P3 != 3)
	{
		const char* message = "Response contains a constant with an unexpected value";
		LogWarning(message);
		//return false;
	}

	analogInformation.fullCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset) * 10;

	analogInformation.cycleCount = ReadHexEncodedUShort(response, byteOffset);

	analogInformation.designCapacityMilliampHours = ReadHexEncodedUShort(response, byteOffset) * 10;

	if (byteOffset != payloadLen + 13)
	{
		const char* message = "Length mismatch reading analog information response, this is a code bug in PACE_BMS";
		LogError(message);
		return false;
	}

	// calculate some "extras"
	analogInformation.SoC = ((float)analogInformation.remainingCapacityMilliampHours / (float)analogInformation.fullCapacityMilliampHours) * 100.0f;
	analogInformation.SoH = (analogInformation.fullCapacityMilliampHours / analogInformation.designCapacityMilliampHours) * 100.0f;
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

	return true;
}

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

const unsigned char PaceBmsV25::exampleReadStatusInformationRequestV25[] = "~25014644E00201FD2E\r";
const unsigned char PaceBmsV25::exampleReadStatusInformationResponseV25[] = "~25014600004C000110000000000000000000000000000000000600000000000000000000000E000000000000EF3A\r";

bool PaceBmsV25::CreateReadStatusInformationRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	// the payload is the requested busId (could be FF for "get all" when speaking to a set of daisy-chained units but this code doesn't support that)
	const uint16_t payloadLen = 2;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, busId);

	CreateRequest(busId, CID2_ReadStatusInformation, payload, request);

	return true;
}

// helper for: ProcessStatusInformationResponse
const std::string PaceBmsV25::DecodeWarningValue(const uint8_t val)
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
		return std::string("Other Fault");
	}

	return std::string("Unknown Fault Value");
}
// helper for: ProcessStatusInformationResponse
const std::string PaceBmsV25::DecodeProtectionStatus1Value(const uint8_t val)
{
	std::string oss;

	if ((val & P1F_UndefinedProtect1Bit) != 0)
	{
		oss.append("Undefined ProtectStatus1 Bit8; ");
	}
	if ((val & P1F_ShortCircuitProtect1Bit) != 0)
	{
		oss.append("Short Circuit Protect; ");
	}
	if ((val & P1F_DischargeCurrentProtect1Bit) != 0)
	{
		oss.append("Discharge Current Protect; ");
	}
	if ((val & P1F_ChargeCurrentProtect1Bit) != 0)
	{
		oss.append("Charge Current Protect; ");
	}
	if ((val & P1F_LowTotalVoltageProtect1Bit) != 0)
	{
		oss.append("Low Total Voltage Protect; ");
	}
	if ((val & P1F_HighTotalVoltageProtect1Bit) != 0)
	{
		oss.append("High Total Voltage Protect; ");
	}
	if ((val & P1F_LowCellVoltageProtect1Bit) != 0)
	{
		oss.append("Low Cell Voltage Protect; ");
	}
	if ((val & P1F_HighCellVoltageProtect1Bit) != 0)
	{
		oss.append("High Cell Voltage Protect; ");
	}

	return oss;
}
// helper for: ProcessStatusInformationResponse
const std::string PaceBmsV25::DecodeProtectionStatus2Value(const uint8_t val)
{
	std::string oss;

	if ((val & P2F_FullyProtect2Bit) != 0)
	{
		// ********************* based on (poor) documentation and inference, /possibly/ this is not a protection flag, but means: the pack has been fully charged, the SoC and total capacity have been updated in the firmware
		oss.append("'Fully' protect bit???; ");
	}
	if ((val & P2F_LowEnvironmentalTemperatureProtect2Bit) != 0)
	{
		oss.append("Low Environmental Temperature Protect; ");
	}
	if ((val & P2F_HighEnvironmentalTemperatureProtect2Bit) != 0)
	{
		oss.append("High Environmental Temperature Protect; ");
	}
	if ((val & P2F_HighMosfetTemperatureProtect2Bit) != 0)
	{
		oss.append("High MOSFET Temperature Protect; ");
	}
	if ((val & P2F_LowDischargeTemperatureProtect2Bit) != 0)
	{
		oss.append("Low Discharge Temperature Protect; ");
	}
	if ((val & P2F_LowChargeTemperatureProtect2Bit) != 0)
	{
		oss.append("Low Charge Temperature Protect; ");
	}
	if ((val & P2F_HighDischargeTemperatureProtect2Bit) != 0)
	{
		oss.append("High Discharge Temperature Protect; ");
	}
	if ((val & P2F_HighChargeTemperatureProtect2Bit) != 0)
	{
		oss.append("High Charge Temperature Protect; ");
	}

	return oss;
}
// helper for: ProcessStatusInformationResponse
const std::string PaceBmsV25::DecodeStatusValue(const uint8_t val)
{
	std::string oss;

	if ((val & SF_HeartIndicatorBit) != 0)
	{
		oss.append("('Heart' indicator?); "); //***
	}
	if ((val & SF_UndefinedStatusBit7) != 0)
	{
		oss.append("(Undefined Status Bit7 - Possibly this means 'Heater', or 'AC in'); ");
	}
	if ((val & SF_ChargingBit) != 0)
	{
		oss.append("Charging; ");
	}
	if ((val & SF_PositiveNegativeTerminalsReversedBit) != 0)
	{
		oss.append("Positive/Negative Terminals Reversed; "); //***
	}
	if ((val & SF_DischargingBit) != 0)
	{
		oss.append("Discharging; ");
	}
	if ((val & SF_DischargeMosfetOnBit) != 0)
	{
		oss.append("Discharge MOSFET On; ");
	}
	if ((val & SF_ChargeMosfetOnBit) != 0)
	{
		oss.append("Charge MOSFET On; ");
	}
	if ((val & SF_ChargeCurrentLimiterTurnedOffBit) != 0)
	{
		oss.append("Charge Current Limiter Disabled; ");
	}

	return oss;
}
// helper for: ProcessStatusInformationResponse
const std::string PaceBmsV25::DecodeConfigurationStatusValue(const uint8_t val)
{
	std::string oss;

	if ((val & CF_UndefinedConfigurationStatusBit8) != 0)
	{
		oss.append("Undefined ConfigurationStatus Bit8 Set; ");
	}
	if ((val & CF_UndefinedConfigurationStatusBit7) != 0)
	{
		oss.append("Undefined ConfigurationStatus Bit7 Set; ");
	}
	if ((val & CF_LedAlarmEnabledBit) != 0)
	{
		oss.append("Warning LED Enabled; ");
	}
	if ((val & CF_ChargeCurrentLimiterEnabledBit) != 0)
	{
		oss.append("Charge Current Limiter Enabled (" + std::string((val & CF_ChargeCurrentLimiterLowGearSetBit) != 0 ? "Low Gear" : "High Gear") + "); ");
	}
	//if ((val & CF_ChargeCurrentLimiterLowGearSetBit) != 0)
	//{
	//	oss.append("Current limit low-gear Set; ");
	//}
	if ((val & CF_DischargeMosfetTurnedOff) != 0)
	{
		oss.append("Discharge MOSFET Turned Off; ");
	}
	if ((val & CF_ChargeMosfetTurnedOff) != 0)
	{
		oss.append("Charge MOSFET Turned Off; ");
	}
	if ((val & CF_BuzzerAlarmEnabledBit) != 0)
	{
		oss.append("Warning Buzzer Enabled; ");
	}

	return oss;
}
// helper for: ProcessStatusInformationResponse
const std::string PaceBmsV25::DecodeFaultStatusValue(const uint8_t val)
{
	std::string oss;

	if ((val & FF_UndefinedFaultStatusBit8) != 0)
	{
		oss.append("Undefined FaultStatus Bit8 Fault; ");
	}
	if ((val & FF_UndefinedFaultStatusBit7) != 0)
	{
		oss.append("Undefined FaultStatus Bit7 Fault; ");
	}
	if ((val & FF_SampleBit) != 0)
	{
		oss.append("Sample (ADC?) fault; ");
	}
	if ((val & FF_CellBit) != 0)
	{
		oss.append("Cell fault; ");
	}
	if ((val & FF_UndefinedFaultStatusBit4) != 0)
	{
		oss.append("Undefined FaultStatus Bit4 Fault; ");
	}
	if ((val & FF_NTCBit) != 0)
	{
		oss.append("NTC fault; ");
	}
	if ((val & FF_DischargeMosfetBit) != 0)
	{
		oss.append("Discharge MOSFET fault; ");
	}
	if ((val & FF_ChargeMosfetBit) != 0)
	{
		oss.append("Charge MOSFET fault; ");
	}

	return oss;
}
// helper for: ProcessStatusInformationResponse
const std::string PaceBmsV25::DecodeWarningStatus1Value(const uint8_t val)
{
	std::string oss;

	if ((val & W1F_UndefinedWarning1Bit8) != 0)
	{
		oss.append("Undefined WarnState1 Bit7 Warning; ");
	}
	if ((val & W1F_UndefinedWarning1Bit7) != 0)
	{
		oss.append("Undefined WarnState1 Bit6 Warning; ");
	}
	if ((val & W1F_DischargeCurrentBit) != 0)
	{
		oss.append("Discharge Current Warning; ");
	}
	if ((val & W1F_ChargeCurrentBit) != 0)
	{
		oss.append("Charge Current Warning; ");
	}
	if ((val & W1F_LowTotalVoltageBit) != 0)
	{
		oss.append("Low Total Voltage Warning; ");
	}
	if ((val & W1F_HighTotalVoltageBit) != 0)
	{
		oss.append("High Total Voltage Warning; ");
	}
	if ((val & W1F_LowCellVoltageBit) != 0)
	{
		oss.append("Low Cell Voltage Warning; ");
	}
	if ((val & W1F_HighCellVoltageBit) != 0)
	{
		oss.append("High Cell Voltage Warning; ");
	}

	return oss;
}
// helper for: ProcessStatusInformationResponse
const std::string PaceBmsV25::DecodeWarningStatus2Value(const uint8_t val)
{
	std::string oss;

	if ((val & W2F_LowPower) != 0)
	{
		oss.append("Low Power Warning; ");
	}
	if ((val & W2F_HighMosfetTemperature) != 0)
	{
		oss.append("High MOSFET Temperature Warning; ");
	}
	if ((val & W2F_LowEnvironmentalTemperature) != 0)
	{
		oss.append("Low Environmental Temperature Warning; ");
	}
	if ((val & W2F_HighEnvironmentalTemperature) != 0)
	{
		oss.append("High Environmental Temperature Warning; ");
	}
	if ((val & W2F_LowDischargeTemperature) != 0)
	{
		oss.append("Low Discharge Temperature Warning; ");
	}
	if ((val & W2F_LowChargeTemperature) != 0)
	{
		oss.append("Low Charge Temperature Warning; ");
	}
	if ((val & W2F_HighDischargeTemperature) != 0)
	{
		oss.append("High Discharge Temperature Warning; ");
	}
	if ((val & W2F_HighChargeTemperature) != 0)
	{
		oss.append("High Charge Temperature Warning; ");
	}

	return oss;
}

bool PaceBmsV25::ProcessReadStatusInformationResponse(const uint8_t busId, const std::vector<uint8_t>& response, StatusInformation& statusInformation)
{
	// todo: consider using string
	statusInformation.warningText.clear();
	statusInformation.balancingText.clear();
	statusInformation.systemText.clear();
	statusInformation.configurationText.clear();
	statusInformation.protectionText.clear();
	statusInformation.faultText.clear();

	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	// SPEC BUG: doc says the response starts with the busId, but "on the wire" I see an extra byte value of 0x00 preceeding it
	uint8_t unknown = ReadHexEncodedByte(response, byteOffset);
	if (unknown != 0)
	{
		LogVerbose("Response contains a value other than zero before the BusId");
	}

	uint8_t busIdResponding = ReadHexEncodedByte(response, byteOffset);
	if (busIdResponding != busId)
	{
		// todo: replace *ALL* of these with a direct log call
		const char* message = "Response from wrong bus Id in payload";
		LogError(message);
		return false;
	}

	// ========================== Warning / Alarm Status ==========================
	uint8_t cellCount = ReadHexEncodedByte(response, byteOffset);
	if (cellCount > MAX_CELL_COUNT)
	{
		const char* message = "Response contains more cell warnings than are supported, results will be truncated";
		LogWarning(message);
	}
	for (int i = 0; i < cellCount; i++)
	{
		uint8_t cw = ReadHexEncodedByte(response, byteOffset);
		statusInformation.warning_value_cell[i] = cw;

		if (i > MAX_CELL_COUNT - 1)
			continue;

		if (cw == 0)
			continue;

		// below/above limit
		statusInformation.warningText.append(std::string("Cell ") + std::to_string(i+1) + std::string(": ") + DecodeWarningValue(cw) + std::string("; "));
	}

	uint8_t tempCount = ReadHexEncodedByte(response, byteOffset);
	if (tempCount > MAX_TEMP_COUNT)
	{
		const char* message = "Response contains more temperature warnings than are supported, results will be truncated";
		LogWarning(message);
	}
	for (int i = 0; i < tempCount; i++)
	{
		uint8_t tw = ReadHexEncodedByte(response, byteOffset);
		statusInformation.warning_value_temp[i] = tw;

		if (i > MAX_TEMP_COUNT - 1)
			continue;

		if (tw == 0)
			continue;

		// below/above limit
		statusInformation.warningText.append(std::string("Temperature ") + std::to_string(i+1) + " " + DecodeWarningValue(tw) + std::string("; "));
	}

	uint8_t chargeCurrentWarn = ReadHexEncodedByte(response, byteOffset);
	statusInformation.warning_value_charge_current = chargeCurrentWarn;
	if (chargeCurrentWarn != 0)
	{
		// below/above limit
		statusInformation.warningText.append(std::string("Charge current ") + DecodeWarningValue(chargeCurrentWarn) + std::string("; "));
	}

	uint8_t totalVoltageWarn = ReadHexEncodedByte(response, byteOffset);
	statusInformation.warning_value_total_voltage = totalVoltageWarn;
	if (totalVoltageWarn != 0)
	{
		// below/above limit
		statusInformation.warningText.append(std::string("Total voltage ") + DecodeWarningValue(totalVoltageWarn) + std::string("; "));
	}

	uint8_t dischargeCurrentWarn = ReadHexEncodedByte(response, byteOffset);
	statusInformation.warning_value_discharge_current = dischargeCurrentWarn;
	if (dischargeCurrentWarn != 0)
	{
		// below/above limit
		statusInformation.warningText.append(std::string("Discharge current ") + DecodeWarningValue(dischargeCurrentWarn) + std::string("; "));
	}

	// ========================== Protection Status ==========================
	uint8_t protectState1 = ReadHexEncodedByte(response, byteOffset);
	statusInformation.protection_value1 = protectState1;
	if (protectState1 != 0)
	{
		statusInformation.protectionText.append(DecodeProtectionStatus1Value(protectState1));
	}

	uint8_t protectState2 = ReadHexEncodedByte(response, byteOffset);
	statusInformation.protection_value2 = protectState2;
	if (protectState2 != 0)
	{
		statusInformation.protectionText.append(DecodeProtectionStatus2Value(protectState2));
	}

	// ========================== System Status ==========================
	uint8_t systemState = ReadHexEncodedByte(response, byteOffset);
	statusInformation.system_value = systemState;
	if (systemState != 0)
	{
		statusInformation.systemText.append(DecodeStatusValue(systemState));
	}

	// ========================== Configuration Status ==========================
	uint8_t controlState = ReadHexEncodedByte(response, byteOffset);
	statusInformation.configuration_value = controlState;
	if (controlState != 0)
	{
		statusInformation.configurationText.append(DecodeConfigurationStatusValue(controlState));
	}

	// ========================== Fault Status ==========================
	uint8_t faultState = ReadHexEncodedByte(response, byteOffset);
	statusInformation.fault_value = faultState;
	if (faultState != 0)
	{
		statusInformation.faultText.append(DecodeFaultStatusValue(faultState));
	}

	// ========================== Balancing Status ==========================
	uint16_t balanceState = ReadHexEncodedUShort(response, byteOffset);
	statusInformation.balancing_value = balanceState;
	for (int i = 0; i < 16; i++)
	{
		if ((balanceState & (1 << i)) != 0)
		{
			statusInformation.balancingText.append(std::string("Cell ") + std::to_string(i+1) + " is balancing; ");
		}
	}

	// ========================== MORE Warning / Alarm Status ==========================
	// Note: It seems like these two may be a "summary" of the previous "Warning / Alarm" section as it duplicates some of the same warnings,
	//       but I'll leave it for completeness or in case the bit shows up in one place but not the other in practice.
	uint8_t warnState1 = ReadHexEncodedByte(response, byteOffset);
	statusInformation.warning_value1 = warnState1;
	if (warnState1 != 0)
	{
		statusInformation.warningText.append(DecodeWarningStatus1Value(warnState1));
	}

	uint8_t warnState2 = ReadHexEncodedByte(response, byteOffset);
	statusInformation.warning_value2 = warnState1;
	if (warnState2 != 0)
	{
		statusInformation.warningText.append(DecodeWarningStatus2Value(warnState2));
	}

	if (byteOffset != payloadLen + 13)
	{
		const char* message = "Length mismatch reading warning information response, this is a code bug in PACE_BMS";
		LogError(message);
		return false;
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

	return true;
}

// ==== Read Hardware Version
// 1 Hardware Version string (may be ' ' padded at the end), the length header value will tell you how long it is, should be 20 'actual character' bytes (40 ASCII hex chars)
// req:   ~250146C10000FD9A.
// resp:  ~25014600602850313653313030412D313831322D312E30302000F58E.
//                     1111111111111111111111111111111111111111

const unsigned char PaceBmsV25::exampleReadHardwareVersionRequestV25[] = "~250146C10000FD9A\r";
const unsigned char PaceBmsV25::exampleReadHardwareVersionResponseV25[] = "~25014600602850313653313030412D313831322D312E30302000F58E\r";

bool PaceBmsV25::CreateReadHardwareVersionRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadHardwareVersion, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsV25::ProcessReadHardwareVersionResponse(const uint8_t busId, const std::vector<uint8_t>& response, std::string& hardwareVersion)
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

	if (payloadLen != 40)
	{
		std::string message = std::string("Documentation indicates a hardware version request should return a 40 byte payload in the response, but this response's payload length is ") + std::to_string(payloadLen);
		LogError(message);
		return false;
	}

	hardwareVersion.resize(20);
	for (int i = 0; i < 20; i++)
	{
		hardwareVersion[i] = ReadHexEncodedByte(response, byteOffset);
	}

	// remove trailing spaces
	while (hardwareVersion.length() > 0 && (hardwareVersion[hardwareVersion.length() - 1] == ' ' || hardwareVersion[hardwareVersion.length() - 1] == 0))
	{
		hardwareVersion.pop_back();
	}

	return true;
}

// ==== Read Serial Number
// 1 Serial Number string (may be ' ' padded at the end), the length header value will tell you how long it is, should be 20 or 40 'actual character' bytes (40 or 80 ASCII hex chars)
// req:   ~250146C20000FD99.
// resp:  ~25014600B05031383132313031333830333039442020202020202020202020202020202020202020202020202020EE0F.
//                     11111111111111111111111111111111111111111111111111111111111111111111111111111111

const unsigned char PaceBmsV25::exampleReadSerialNumberRequestV25[] = "~250146C20000FD99\r";
const unsigned char PaceBmsV25::exampleReadSerialNumberResponseV25[] = "~25014600B05031383132313031333830333039442020202020202020202020202020202020202020202020202020EE0F\r";

bool PaceBmsV25::CreateReadSerialNumberRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadSerialNumber, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsV25::ProcessReadSerialNumberResponse(const uint8_t busId, const std::vector<uint8_t>& response, std::string& serialNumber)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	if (payloadLen != 80 && payloadLen != 40)
	{
		std::string message = std::string("Documentation indicates a serial number information request should return either a 40 or 80 byte payload in the response, but this response's payload length is ") + std::to_string(payloadLen);
		LogError(message);
		return false;
	}

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

	return true;
}

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

const unsigned char PaceBmsV25::exampleWriteDisableBuzzerSwitchCommandRequestV25[] = "~25004699E0020DFD12\r";
const unsigned char PaceBmsV25::exampleWriteDisableBuzzerSwitchCommandResponseV25[] = "~25004600C0040D01FCC3\r";
const unsigned char PaceBmsV25::exampleWriteEnableBuzzerSwitchCommandRequestV25[] = "~25004699E0020CFD13\r";
const unsigned char PaceBmsV25::exampleWriteEnableBuzzerSwitchCommandResponseV25[] = "~25004600C0040C00FCC5\r";

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

const unsigned char PaceBmsV25::exampleWriteDisableLedWarningSwitchCommandRequestV25[] = "~25004699E00206FD20\r";
const unsigned char PaceBmsV25::exampleWriteDisableLedWarningSwitchCommandResponseV25[] = "~25004600C0040602FCD0\r";
const unsigned char PaceBmsV25::exampleWriteEnableLedWarningSwitchCommandRequestV25[] = "~25004699E00207FD1F\r";
const unsigned char PaceBmsV25::exampleWriteEnableLedWarningSwitchCommandResponseV25[] = "~25004600C0040722FCCD\r";

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

const unsigned char PaceBmsV25::exampleWriteDisableChargeCurrentLimiterSwitchCommandRequestV25[] = "~25004699E0020AFD15\r";
const unsigned char PaceBmsV25::exampleWriteDisableChargeCurrentLimiterSwitchCommandResponseV25[] = "~25004600C0040A22FCC3\r";
const unsigned char PaceBmsV25::exampleWriteEnableChargeCurrentLimiterSwitchCommandRequestV25[] = "~25004699E0020BFD14\r";
const unsigned char PaceBmsV25::exampleWriteEnableChargeCurrentLimiterSwitchCommandResponseV25[] = "~25004600C0040B32FCC1\r";

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

const unsigned char PaceBmsV25::exampleWriteSetChargeCurrentLimiterCurrentLimitLowGearSwitchCommandRequestV25[] = "~25004699E00209FD1D\r";
const unsigned char PaceBmsV25::exampleWriteSetChargeCurrentLimiterCurrentLimitLowGearSwitchCommandResponseV25[] = "~25004600C0040938FCC4\r";
const unsigned char PaceBmsV25::exampleWriteSetChargeCurrentLimiterCurrentLimitHighGearSwitchCommandRequestV25[] = "~25004699E00208FD1E\r";
const unsigned char PaceBmsV25::exampleWriteSetChargeCurrentLimiterCurrentLimitHighGearSwitchCommandResponseV25[] = "~25004600C0040830FCCD\r";

bool PaceBmsV25::CreateWriteSwitchCommandRequest(const uint8_t busId, const SwitchCommand command, std::vector<uint8_t>& request)
{
	// the payload is the control command code
	const uint16_t payloadLen = 2;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, command);

	CreateRequest(busId, CID2_WriteSwitchCommand, payload, request);

	return true;
}
bool PaceBmsV25::ProcessWriteSwitchCommandResponse(const uint8_t busId, const SwitchCommand command, const std::vector<uint8_t>& response)
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

const unsigned char PaceBmsV25::exampleWriteMosfetChargeOpenSwitchCommandRequestV25[] = "~2500469AE00200FD1E\r";
const unsigned char PaceBmsV25::exampleWriteMosfetChargeOpenSwitchCommandResponseV25[] = "~25004600E00226FD30\r";
const unsigned char PaceBmsV25::exampleWriteMosfetChargeCloseSwitchCommandRequestV25[] = "~2500469AE00201FD1D\r";
const unsigned char PaceBmsV25::exampleWriteMosfetChargeCloseSwitchCommandResponseV25[] = "~25004600E00224FD32\r";

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

const unsigned char PaceBmsV25::exampleWriteMosfetDischargeOpenSwitchCommandRequestV25[] = "~2500469BE00200FD1D\r";
const unsigned char PaceBmsV25::exampleWriteMosfetDischargeOpenSwitchCommandResponseV25[] = "~25004600E00204FD34\r";
const unsigned char PaceBmsV25::exampleWriteMosfetDischargeCloseSwitchCommandRequestV25[] = "~2500469BE00201FD1C\r";
const unsigned char PaceBmsV25::exampleWriteMosfetDischargeCloseSwitchCommandResponseV25[] = "~25004609E00204FD2B\r";

bool PaceBmsV25::CreateWriteMosfetSwitchCommandRequest(const uint8_t busId, const MosfetType type, const MosfetState command, std::vector<uint8_t>& request)
{
	// the payload is the mosfet state to set
	const uint16_t payloadLen = 2;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, command);

	CreateRequest(busId, (CID2)type, payload, request);

	return true;
}
bool PaceBmsV25::ProcessWriteMosfetSwitchCommandResponse(const uint8_t busId, const MosfetType type, const MosfetState command, const std::vector<uint8_t>& response)
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

// ==== Reboot (labeled as "Shutdown" in PBmsTools, but it actually causes a reboot in my experience)
// x: unknown payload, this may be a command code and there may be more but I'm not going to test that due to potentially unknown consequences
// write: ~2500469CE00201FD1B.
//                     xx
// resp:  ~250046000000FDAF.

const unsigned char PaceBmsV25::exampleWriteRebootCommandRequestV25[] = "~2500469CE00201FD1B\r";
const unsigned char PaceBmsV25::exampleWriteRebootCommandResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::CreateWriteShutdownCommandRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	// the payload is the mosfet state to set
	const uint16_t payloadLen = 2;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, 0x01);

	CreateRequest(busId, CID2_WriteRebootCommand, payload, request);

	return true;
}
bool PaceBmsV25::ProcessWriteShutdownCommandResponse(const uint8_t busId, const std::vector<uint8_t>& response)
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

const unsigned char PaceBmsV25::exampleReadSystemTimeRequestV25[] = "~250046B10000FD9C\r";
const unsigned char PaceBmsV25::exampleReadSystemTimeResponseV25[] = "~25004600400C180815051D1FFB10\r";
const unsigned char PaceBmsV25::exampleWriteSystemTimeRequestV25[] = "~250046B2400C1808140E0F25FAFC\r";
const unsigned char PaceBmsV25::exampleWriteSystemTimeResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::CreateReadSystemDateTimeRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadDateTime, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsV25::ProcessReadSystemDateTimeResponse(const uint8_t busId, const std::vector<uint8_t>& response, DateTime& dateTime)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
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

	return true;
}
bool PaceBmsV25::CreateWriteSystemDateTimeRequest(const uint8_t busId, const DateTime dateTime, std::vector<uint8_t>& request)
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
bool PaceBmsV25::ProcessWriteSystemDateTimeResponse(const uint8_t busId, const std::vector<uint8_t>& response)
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



// ============================================================================
// 
// "Parameter Setting" tab of PBmsTools 2.4 with DIP set to address 00 
// PBmsTools 2.4 is broken and can't address packs other than 00 for configuration
// 
// ============================================================================

bool PaceBmsV25::CreateReadConfigurationRequest(const uint8_t busId, const ReadConfigurationType configType, std::vector<uint8_t>& request)
{
	CreateRequest(busId, (CID2)configType, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsV25::ProcessWriteConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response)
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

const unsigned char PaceBmsV25::exampleReadCellOverVoltageConfigurationRequestV25[] = "~250046D10000FD9A\r";
const unsigned char PaceBmsV25::exampleReadCellOverVoltageConfigurationResponseV25[] = "~25004600F010010E100E740D340AFA35\r";
const unsigned char PaceBmsV25::exampleWriteCellOverVoltageConfigurationRequestV25[] = "~250046D0F010010E100E740D340AFA21\r";
const unsigned char PaceBmsV25::exampleWriteCellOverVoltageConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, CellOverVoltageConfiguration& config)
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
		const char* message = "Unknown payload byte does not match previously observed value";
		LogWarning(message);
		return false;
	}

	config.AlarmMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionReleaseMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionDelayMilliseconds = ReadHexEncodedByte(response, byteOffset) * 100;

	return true;
}
bool PaceBmsV25::CreateWriteConfigurationRequest(const uint8_t busId, const CellOverVoltageConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.AlarmMillivolts < 2500 || config.AlarmMillivolts > 4500)
	{
		const char* message = "AlarmVoltage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.AlarmMillivolts % 10 != 0)
	{
		const char* message = "AlarmVoltage should be in steps of 0.01 volts";
		LogError(message);
		return false;
	}
	if (config.ProtectionMillivolts < 2500 || config.ProtectionMillivolts > 4500)
	{
		const char* message = "ProtectionVoltage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionMillivolts % 10 != 0)
	{
		const char* message = "ProtectionVoltage should be in steps of 0.01 volts";
		LogError(message);
		return false;
	}
	if (config.ProtectionReleaseMillivolts < 2500 || config.ProtectionReleaseMillivolts > 4500)
	{
		const char* message = "ProtectionReleaseVoltage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionReleaseMillivolts % 10 != 0)
	{
		const char* message = "ProtectionReleaseVoltage should be in steps of 0.01 volts";
		LogError(message);
		return false;
	}
	if (config.ProtectionDelayMilliseconds < 1000 || config.ProtectionDelayMilliseconds > 20000)
	{
		const char* message = "ProtectionDelaySeconds is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionDelayMilliseconds % 500 != 0)
	{
		const char* message = "ProtectionDelaySeconds should be in steps of 0.5 seconds";
		LogError(message);
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

// ==== Pack Over Voltage Configuration
// 1 Pack OV Alarm (V): 57.6 - stored as v * 100, so 57.6 is 57600 - valid range reported by PBmsTools as 20-65 in steps of 0.01
// 2 Pack OV Protect (V): 59.2 - stored as v * 100, so 59.2 is 59200 - valid range reported by PBmsTools as 20-65 in steps of 0.01
// 3 Pack OVP Release (V): 54.0 - stored as v * 100, so 54.0 is 54000 - valid range reported by PBmsTools as 20-65 in steps of 0.01
// 4 Pack OVP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 1000 to 5000 in steps of 500
// read:  ~250046D50000FD96.
// resp:  ~25004600F01001E100E740D2F00AFA24.
//                     ??11112222333344
// write: ~250046D4F01001E10AE740D2F00AF9FB.
// resp:  ~250046000000FDAF.

const unsigned char PaceBmsV25::exampleReadPackOverVoltageConfigurationRequestV25[] = "~250046D50000FD96\r";
const unsigned char PaceBmsV25::exampleReadPackOverVoltageConfigurationResponseV25[] = "~25004600F01001E100E740D2F00AFA24\r";
const unsigned char PaceBmsV25::exampleWritePackOverVoltageConfigurationRequestV25[] = "~250046D4F01001E10AE740D2F00AF9FB\r";
const unsigned char PaceBmsV25::exampleWritePackOverVoltageConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t> response, PackOverVoltageConfiguration& config)
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
		const char* message = "Unknown payload byte does not match previously observed value";
		LogWarning(message);
		return false;
	}

	config.AlarmMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionReleaseMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionDelayMilliseconds = ReadHexEncodedByte(response, byteOffset) * 100;

	return true;
}
bool PaceBmsV25::CreateWriteConfigurationRequest(const uint8_t busId, const PackOverVoltageConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.AlarmMillivolts < 20000 || config.AlarmMillivolts > 65000)
	{
		const char* message = "AlarmVoltage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.AlarmMillivolts % 10)
	{
		const char* message = "AlarmVoltage should be in steps of 0.01";
		LogError(message);
		return false;
	}
	if (config.ProtectionMillivolts < 20000 || config.ProtectionMillivolts > 65000)
	{
		const char* message = "ProtectionVoltage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionMillivolts % 10 != 0)
	{
		const char* message = "ProtectionVoltage should be in steps of 0.01";
		LogError(message);
		return false;
	}
	if (config.ProtectionReleaseMillivolts < 20000 || config.ProtectionReleaseMillivolts > 65000)
	{
		const char* message = "ProtectionReleaseVoltage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionReleaseMillivolts % 10 != 0)
	{
		const char* message = "ProtectionReleaseVoltage should be in steps of 0.01";
		LogError(message);
		return false;
	}
	if (config.ProtectionDelayMilliseconds < 1000 || config.ProtectionDelayMilliseconds > 20000)
	{
		const char* message = "ProtectionDelayMilliseconds is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionDelayMilliseconds % 500 != 0)
	{
		const char* message = "ProtectionDelayMilliseconds should be in steps of 500";
		LogError(message);
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

// ==== Cell Under Voltage Configuration
// 1 Cell UV Alarm (V): 2.8 - stored as v * 100, so 2.8 is 2800 - valid range reported by PBmsTools as 2-3.5 in steps of 0.01
// 2 Cell UV Protect (V): 2.5 - stored as v * 100, so 2.5 is 2500 - valid range reported by PBmsTools as 2-3.5 in steps of 0.01
// 3 Cell UVP Release (V): 2.9 - stored as v * 100, so 2.9 is 2900 - valid range reported by PBmsTools as 2-3.5 in steps of 0.01
// 4 Cell UVP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 1000 to 5000 in steps of 500
// read:  ~250046D30000FD98.
// resp:  ~25004600F010010AF009C40B540AFA24.
//                     ??11112222333344
// write: ~250046D2F010010AF009C40B540AFA0E.
// resp:  ~250046000000FDAF.

const unsigned char PaceBmsV25::exampleReadCellUnderVoltageConfigurationRequestV25[] = "~250046D30000FD98\r";
const unsigned char PaceBmsV25::exampleReadCellUnderVoltageConfigurationResponseV25[] = "~25004600F010010AF009C40B540AFA24\r";
const unsigned char PaceBmsV25::exampleWriteCellUnderVoltageConfigurationRequestV25[] = "~250046D2F010010AF009C40B540AFA0E\r";
const unsigned char PaceBmsV25::exampleWriteCellUnderVoltageConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, CellUnderVoltageConfiguration& config)
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
		const char* message = "Unknown payload byte does not match previously observed value";
		LogWarning(message);
		return false;
	}

	config.AlarmMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionReleaseMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionDelayMilliseconds = ReadHexEncodedByte(response, byteOffset) * 100;

	return true;
}
bool PaceBmsV25::CreateWriteConfigurationRequest(const uint8_t busId, const CellUnderVoltageConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.AlarmMillivolts < 2000 || config.AlarmMillivolts > 3500)
	{
		const char* message = "AlarmVoltage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.AlarmMillivolts % 10 != 0)
	{
		const char* message = "AlarmVoltage should be in steps of 0.01";
		LogError(message);
		return false;
	}
	if (config.ProtectionMillivolts < 2000 || config.ProtectionMillivolts > 3500)
	{
		const char* message = "ProtectionVoltage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionMillivolts % 10 != 0)
	{
		const char* message = "ProtectionVoltage should be in steps of 0.01";
		LogError(message);
		return false;
	}
	if (config.ProtectionReleaseMillivolts < 2000 || config.ProtectionReleaseMillivolts > 3500)
	{
		const char* message = "ProtectionReleaseVoltage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionReleaseMillivolts % 10 != 0)
	{
		const char* message = "ProtectionReleaseVoltage should be in steps of 0.01";
		LogError(message);
		return false;
	}
	if (config.ProtectionDelayMilliseconds < 1000 || config.ProtectionDelayMilliseconds > 20000)
	{
		const char* message = "ProtectionDelayMilliseconds is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionDelayMilliseconds % 500 != 0)
	{
		const char* message = "ProtectionDelayMilliseconds should be in steps of 500";
		LogError(message);
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

// ==== Pack Under Voltage Configuration
// 1 Pack UV Alarm (V): 44.8 - stored as v * 100, so 44.8 is 44800 - valid range reported by PBmsTools as 15-50 in steps of 0.01
// 2 Pack UV Protect (V): 40.0 - stored as v * 100, so 40.0 is 40000 - valid range reported by PBmsTools as 15-50 in steps of 0.01
// 3 Pack UVP Release (V): 46.4 - stored as v * 100, so 46.4 is 46400 - valid range reported by PBmsTools as 15-50 in steps of 0.01
// 4 Pack UVP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 1000 to 5000 in steps of 500
// read:  ~250046D70000FD94.
// resp:  ~25004600F01001AF009C40B5400AFA24.
//                     ??11112222333344
// write: ~250046D6F01001AF009C40B5400AFA0A.
// resp:  ~250046000000FDAF.

const unsigned char PaceBmsV25::exampleReadPackUnderVoltageConfigurationRequestV25[] = "~250046D70000FD94\r";
const unsigned char PaceBmsV25::exampleReadPackUnderVoltageConfigurationResponseV25[] = "~25004600F01001AF009C40B5400AFA24\r";
const unsigned char PaceBmsV25::exampleWritePackUnderVoltageConfigurationRequestV25[] = "~250046D6F01001AF009C40B5400AFA0A\r";
const unsigned char PaceBmsV25::exampleWritePackUnderVoltageConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, PackUnderVoltageConfiguration& config)
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
		const char* message = "Unknown payload byte does not match previously observed value";
		LogWarning(message);
		return false;
	}

	config.AlarmMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionReleaseMillivolts = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionDelayMilliseconds = ReadHexEncodedByte(response, byteOffset) * 100;

	return true;
}
bool PaceBmsV25::CreateWriteConfigurationRequest(const uint8_t busId, const PackUnderVoltageConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.AlarmMillivolts < 15000 || config.AlarmMillivolts > 50000)
	{
		const char* message = "AlarmVoltage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.AlarmMillivolts % 10 != 0)
	{
		const char* message = "AlarmVoltage should be in steps of 0.01";
		LogError(message);
		return false;
	}
	if (config.ProtectionMillivolts < 15000 || config.ProtectionMillivolts > 50000)
	{
		const char* message = "ProtectionVoltage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionMillivolts % 10 != 0)
	{
		const char* message = "ProtectionVoltage should be in steps of 0.01";
		LogError(message);
		return false;
	}
	if (config.ProtectionReleaseMillivolts < 15000 || config.ProtectionReleaseMillivolts > 50000)
	{
		const char* message = "ProtectionReleaseVoltage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionReleaseMillivolts % 10 != 0)
	{
		const char* message = "ProtectionReleaseVoltage should be in steps of 0.01";
		LogError(message);
		return false;
	}
	if (config.ProtectionDelayMilliseconds < 1000 || config.ProtectionDelayMilliseconds > 20000)
	{
		const char* message = "ProtectionDelayMilliseconds is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionDelayMilliseconds % 500 != 0)
	{
		const char* message = "ProtectionDelayMilliseconds should be in steps of 500";
		LogError(message);
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

// ==== Charge Over Current Configuration
// 1 Charge OC Alarm (A): 104 - stored directly in amps - valid range reported by PBmsTools as 1-150
// 2 Charge OC Protect (A): 110 - stored directly in amps - valid range reported by PBmsTools as 1-150
// 3 Charge OCP Delay Time (ms): 1000 - stored in 100ms steps, so 1000ms is 10 - valid range reported by PBmsTools as 500 to 10000 in steps of 500
// read:  ~250046D90000FD92.
// resp:  ~25004600400C010068006E0AFB1D.
//                     ??1111222233
// write: ~250046D8400C010068006E0AFB01.
// resp:  ~250046000000FDAF.

const unsigned char PaceBmsV25::exampleReadChargeOverCurrentConfigurationRequestV25[] = "~250046D90000FD92\r";
const unsigned char PaceBmsV25::exampleReadChargeOverCurrentConfigurationResponseV25[] = "~25004600400C010068006E0AFB1D\r";
const unsigned char PaceBmsV25::exampleWriteChargeOverCurrentConfigurationRequestV25[] = "~250046D8400C010068006E0AFB01\r";
const unsigned char PaceBmsV25::exampleWriteChargeOverCurrentConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, ChargeOverCurrentConfiguration& config)
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
		const char* message = "Unknown payload byte does not match previously observed value";
		LogWarning(message);
		return false;
	}

	config.AlarmAmperage = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionAmperage = ReadHexEncodedUShort(response, byteOffset);
	config.ProtectionDelayMilliseconds = ReadHexEncodedByte(response, byteOffset) * 100;

	return true;
}
bool PaceBmsV25::CreateWriteConfigurationRequest(const uint8_t busId, const ChargeOverCurrentConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.AlarmAmperage < 1 || config.AlarmAmperage > 220)
	{
		const char* message = "AlarmAmperage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionAmperage < 1 || config.ProtectionAmperage > 220)
	{
		const char* message = "ProtectionAmperage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionDelayMilliseconds < 500 || config.ProtectionDelayMilliseconds > 25000)
	{
		const char* message = "ProtectionDelayMilliseconds is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionDelayMilliseconds % 500 != 0)
	{
		const char* message = "ProtectionDelayMilliseconds should be in steps of 500";
		LogError(message);
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

const unsigned char PaceBmsV25::exampleReadDishargeOverCurrent1ConfigurationRequestV25[] = "~250046DB0000FD89\r";
const unsigned char PaceBmsV25::exampleReadDishargeOverCurrent1ConfigurationResponseV25[] = "~25004600400C01FF97FF920AFAD3\r";
const unsigned char PaceBmsV25::exampleWriteDishargeOverCurrent1ConfigurationRequestV25[] = "~250046DA400C010069006E0AFAF7\r";
const unsigned char PaceBmsV25::exampleWriteDishargeOverCurrent1ConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, DischargeOverCurrent1Configuration& config)
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
		const char* message = "Unknown payload byte does not match previously observed value";
		LogWarning(message);
		return false;
	}

	config.AlarmAmperage = ReadHexEncodedSShort(response, byteOffset) * -1;
	config.ProtectionAmperage = ReadHexEncodedSShort(response, byteOffset) * -1;
	config.ProtectionDelayMilliseconds = ReadHexEncodedByte(response, byteOffset) * 100;

	return true;
}
bool PaceBmsV25::CreateWriteConfigurationRequest(const uint8_t busId, const DischargeOverCurrent1Configuration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.AlarmAmperage < 1 || config.AlarmAmperage > 220)
	{
		const char* message = "AlarmAmperage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionAmperage < 1 || config.ProtectionAmperage > 220)
	{
		const char* message = "ProtectionAmperage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionDelayMilliseconds < 500 || config.ProtectionDelayMilliseconds > 25000)
	{
		const char* message = "ProtectionDelayMilliseconds is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionDelayMilliseconds % 500 != 0)
	{
		const char* message = "ProtectionDelayMilliseconds should be in steps of 500";
		LogError(message);
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

// ==== Dicharge FAST Over Current Configuration
// 1 Discharge OC 2 Protect: 150 - stored directly in amps - valid range reported by PBmsTools as 5-300 in steps of 5, but since this is an 8 bit store location, the actual max is 255????????
// 2 Discharge OC 2 Delay Time (ms): 100 - stored in 25ms steps, so 100 is 4 (4x25=100), 400 is 16 (16x25=400) - valid range reported by PBmsTools as 100-2000 in steps of 100
// x = apparently, garbage written by the firmware - it's not included in the PBmsTools write
// read:  ~250046E30000FD97.
// resp:  ~25004600400C009604009604FB32.
//                     ??1122xxxxxx
// write: ~250046E2A006009604FC4E.
// resp:  ~250046000000FDAF.

const unsigned char PaceBmsV25::exampleReadDishargeOverCurrent2ConfigurationRequestV25[] = "~250046E30000FD97\r";
const unsigned char PaceBmsV25::exampleReadDishargeOverCurrent2ConfigurationResponseV25[] = "~25004600400C009604009604FB32\r";
const unsigned char PaceBmsV25::exampleWriteDishargeOverCurrent2ConfigurationRequestV25[] = "~250046E2A006009604FC4E\r";
const unsigned char PaceBmsV25::exampleWriteDishargeOverCurrent2ConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, DischargeOverCurrent2Configuration& config)
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
		const char* message = "Unknown payload byte does not match previously observed value";
		LogWarning(message);
		return false;
	}

	config.ProtectionAmperage = ReadHexEncodedByte(response, byteOffset);
	config.ProtectionDelayMilliseconds = ReadHexEncodedByte(response, byteOffset) * 25;

	// ignore the garbage tail, likely firmware bug since it's not sent on the write

	return true;
}
bool PaceBmsV25::CreateWriteConfigurationRequest(const uint8_t busId, const DischargeOverCurrent2Configuration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.ProtectionAmperage < 5 || config.ProtectionAmperage > 255)
	{
		const char* message = "ProtectionAmperage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionAmperage % 5 != 0)
	{
		const char* message = "ProtectionAmperage should be in steps of 5";
		LogError(message);
		return false;
	}
	if (config.ProtectionDelayMilliseconds < 100 || config.ProtectionDelayMilliseconds > 2000)
	{
		const char* message = "ProtectionDelayMilliseconds is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionDelayMilliseconds % 100 != 0)
	{
		const char* message = "ProtectionDelayMilliseconds should be in steps of 100";
		LogError(message);
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

// ==== Short Circuit Protection Configuration
// 1 Delay Time (us): 300 - stored in 25 microsecond steps, 300 is 12 - valid range reported by PBmsTools as as 100-500 in steps of 50
// read:  ~250046E50000FD95.
// resp:  ~25004600E0020CFD25.
//                     11
// write: ~250046E4E0020CFD0C.
// resp:  ~250046000000FDAF.

const unsigned char PaceBmsV25::exampleReadShortCircuitProtectionConfigurationRequestV25[] = "~250046E50000FD95\r";
const unsigned char PaceBmsV25::exampleReadShortCircuitProtectionConfigurationResponseV25[] = "~25004600E0020CFD25\r";
const unsigned char PaceBmsV25::exampleWriteShortCircuitProtectionConfigurationRequestV25[] = "~250046E4E0020CFD0C\r";
const unsigned char PaceBmsV25::exampleWriteShortCircuitProtectionConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, ShortCircuitProtectionConfiguration& config)
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
bool PaceBmsV25::CreateWriteConfigurationRequest(const uint8_t busId, const ShortCircuitProtectionConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.ProtectionDelayMicroseconds < 100 || config.ProtectionDelayMicroseconds > 500)
	{
		const char* message = "ProtectionDelayMicroseconds is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionDelayMicroseconds % 50 != 0)
	{
		const char* message = "ProtectionDelayMicroseconds should be in steps of 50";
		LogError(message);
		return false;
	}

	const uint16_t payloadLen = 2;
	std::vector<uint8_t> payload(payloadLen);
	uint16_t payloadOffset = 0;
	WriteHexEncodedByte(payload, payloadOffset, config.ProtectionDelayMicroseconds / 25);

	CreateRequest(busId, CID2_WriteShortCircuitProtectionConfiguration, payload, request);

	return true;
}

// ==== Cell Balancing Configuration
// 1 Balance Threshold (V): 3.4 - stored as v * 100, so 3.4 is 3400 - valid range reported by PBmsTools as 3.3-4.5 in steps of 0.01
// 2 Balance Delta Cell (mv): 30 - stored directly, so 30 is 30 - valid range reported by PBmsTools as 20-500 in steps of 5
// read:  ~250046B60000FD97.
// resp:  ~2500460080080D48001EFBE9.
//                     11112222
// write: ~250046B580080D48001EFBD2.
// resp:  ~250046000000FDAF.

const unsigned char PaceBmsV25::exampleReadCellBalancingConfigurationRequestV25[] = "~250046B60000FD97\r";
const unsigned char PaceBmsV25::exampleReadCellBalancingConfigurationResponseV25[] = "~2500460080080D48001EFBE9\r";
const unsigned char PaceBmsV25::exampleWriteCellBalancingConfigurationRequestV25[] = "~250046B580080D48001EFBD2\r";
const unsigned char PaceBmsV25::exampleWriteCellBalancingConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, CellBalancingConfiguration& config)
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
bool PaceBmsV25::CreateWriteConfigurationRequest(const uint8_t busId, const CellBalancingConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.ThresholdMillivolts < 3300 || config.ThresholdMillivolts > 4500)
	{
		const char* message = "ThresholdVolts is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ThresholdMillivolts % 10 != 0)
	{
		const char* message = "ThresholdVolts should be in steps of 0.01";
		LogError(message);
		return false;
	}
	if (config.DeltaCellMillivolts < 20 || config.DeltaCellMillivolts > 500)
	{
		const char* message = "DeltaCellMillivolts is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
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

// ==== Sleep Configuration
// 1 Sleep v-cell: 3.1 - stored as v * 100, so 3.1 is 3100 - valid range reported by PBmsTools as 2-4 in steps of 0.01
// 2 Delay Time (minute): 5 - stored directly - valid range reported by PBmsTools as 1-120
// read:  ~250046A00000FD9E.
// resp:  ~2500460080080C1C0005FBF3.
//                     1111??22
// write: ~250046A880080C1C0005FBDA.
// resp:  ~250046000000FDAF.

const unsigned char PaceBmsV25::exampleReadSleepConfigurationRequestV25[] = "~250046A00000FD9E\r";
const unsigned char PaceBmsV25::exampleReadSleepConfigurationResponseV25[] = "~2500460080080C1C0005FBF3\r";
const unsigned char PaceBmsV25::exampleWriteSleepConfigurationRequestV25[] = "~250046A880080C1C0005FBDA\r";
const unsigned char PaceBmsV25::exampleWriteSleepConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, SleepConfiguration& config)
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
		const char* message = "Unknown2 value in payload is not zero";
		LogError(message);
		return false;
	}

	return true;
}
bool PaceBmsV25::CreateWriteConfigurationRequest(const uint8_t busId, const SleepConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.CellMillivolts < 2000 || config.CellMillivolts > 4000)
	{
		const char* message = "CellVoltage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.CellMillivolts % 10 != 0)
	{
		const char* message = "CellVoltage should be in steps of 0.01";
		LogError(message);
		return false;
	}
	if (config.DelayMinutes < 1.0f || config.DelayMinutes > 120.0f)
	{
		const char* message = "DelayMinutes is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
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

// ==== Full Charge and Low Charge
// 1 Pack Full Charge Voltage: 56.0 - stored as v * 1000, so 56 is 56000 - valid range reported by PBmsTools as 20-65 in steps of 0.01
// 2 Pack Full Charge Current (ma): 2000 - stored directly in ma - valid range reported by PBmsTools as 500-5000 in steps of 500
// 3 State of Charge Low Alarm (%): 5 - stored directly - valid range reported by PBmsTools as 1-100
// read:  ~250046AF0000FD88.
// resp:  ~25004600600ADAC007D005FB60.
//                     1111222233
// write: ~250046AE600ADAC007D005FB3A.
// resp:  ~250046000000FDAF.

const unsigned char PaceBmsV25::exampleReadFullChargeLowChargeConfigurationRequestV25[] = "~250046AF0000FD88\r";
const unsigned char PaceBmsV25::exampleReadFullChargeLowChargeConfigurationResponseV25[] = "~25004600600ADAC007D005FB60\r";
const unsigned char PaceBmsV25::exampleWriteFullChargeLowChargeConfigurationRequestV25[] = "~250046AE600ADAC007D005FB3A\r";
const unsigned char PaceBmsV25::exampleWriteFullChargeLowChargeConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, FullChargeLowChargeConfiguration& config)
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
bool PaceBmsV25::CreateWriteConfigurationRequest(const uint8_t busId, const FullChargeLowChargeConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.FullChargeMillivolts < 20000 || config.FullChargeMillivolts > 65000)
	{
		const char* message = "FullChargeVoltage is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.FullChargeMillivolts % 10 != 0)
	{
		const char* message = "FullChargeVoltage should be in steps of 0.001";
		LogError(message);
		return false;
	}
	if (config.FullChargeMilliamps < 500 || config.FullChargeMilliamps > 5000)
	{
		const char* message = "FullChargeCurrentMilliamps is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.FullChargeMilliamps % 500 != 0)
	{
		const char* message = "FullChargeCurrentMilliamps should be in steps of 500";
		LogError(message);
		return false;
	}
	if (config.LowChargeAlarmPercent < 0 || config.LowChargeAlarmPercent > 100)
	{
		const char* message = "LowChargeAlarmPercent is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
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

const unsigned char PaceBmsV25::exampleReadChargeAndDischargeOverTemperatureConfigurationRequestV25[] = "~250046DD0000FD87\r";
const unsigned char PaceBmsV25::exampleReadChargeAndDischargeOverTemperatureConfigurationResponseV25[] = "~25004600501A010CA80CD00C9E0CDA0D020CD0F7BE\r";
const unsigned char PaceBmsV25::exampleWriteChargeAndDischargeOverTemperatureConfigurationRequestV25[] = "~250046DC501A010CA80CD00C9E0CDA0D020CD0F797\r";
const unsigned char PaceBmsV25::exampleWriteChargeAndDischargeOverTemperatureConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, ChargeAndDischargeOverTemperatureConfiguration& config)
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
		const char* message = "Unknown payload byte does not match previously observed value";
		LogWarning(message);
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
bool PaceBmsV25::CreateWriteConfigurationRequest(const uint8_t busId, const ChargeAndDischargeOverTemperatureConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.ChargeAlarm < 20 || config.ChargeAlarm > 100)
	{
		const char* message = "ChargeAlarm is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ChargeProtection < 20 || config.ChargeProtection > 100)
	{
		const char* message = "ChargeProtection is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ChargeProtectionRelease < 20 || config.ChargeProtectionRelease > 100)
	{
		const char* message = "ChargeProtectionRelease is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.DischargeAlarm < 20 || config.DischargeAlarm > 100)
	{
		const char* message = "DischargeAlarm is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.DischargeProtection < 20 || config.DischargeProtection > 100)
	{
		const char* message = "DischargeProtection is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.DischargeProtectionRelease < 20 || config.DischargeProtectionRelease > 100)
	{
		const char* message = "DischargeProtectionRelease is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
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

const unsigned char PaceBmsV25::exampleReadChargeAndDischargeUnderTemperatureConfigurationRequestV25[] = "~250046DF0000FD85\r";
const unsigned char PaceBmsV25::exampleReadChargeAndDischargeUnderTemperatureConfigurationResponseV25[] = "~25004600501A010AAA0A780AAA0A1409E20A14F7E5\r";
const unsigned char PaceBmsV25::exampleWriteChargeAndDischargeUnderTemperatureConfigurationRequestV25[] = "~250046DE501A010AAA0A780AAA0A1409E20A14F7BC\r";
const unsigned char PaceBmsV25::exampleWriteChargeAndDischargeUnderTemperatureConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, ChargeAndDischargeUnderTemperatureConfiguration& config)
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
		const char* message = "Unknown payload byte does not match previously observed value";
		LogWarning(message);
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
bool PaceBmsV25::CreateWriteConfigurationRequest(const uint8_t busId, const ChargeAndDischargeUnderTemperatureConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.ChargeAlarm < -35 || config.ChargeAlarm > 30)
	{
		const char* message = "ChargeAlarm is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ChargeProtection < -35 || config.ChargeProtection > 30)
	{
		const char* message = "ChargeProtection is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ChargeProtectionRelease < -35 || config.ChargeProtectionRelease > 30)
	{
		const char* message = "ChargeProtectionRelease is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.DischargeAlarm < -35 || config.DischargeAlarm > 30)
	{
		const char* message = "DischargeAlarm is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.DischargeProtection < -35 || config.DischargeProtection > 30)
	{
		const char* message = "DischargeProtection is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.DischargeProtectionRelease < -35 || config.DischargeProtectionRelease > 30)
	{
		const char* message = "DischargeProtectionRelease is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
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

// ==== Mosfet Over Temperature Protection Configuration
// 1 Mosfet Over Temperature Alarm: 90 - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as 30-120
// 2 Mosfet Over Temperature Protection: 110 - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as 30-120
// 3 Mosfet Over Temperature Release: 85 - stored as (value * 10) + 2730 = , to decode (value - 2730) / 10.0 =  - valid range reported by PBmsTools as 30-120
// read:  ~250046E10000FD99.
// resp:  ~25004600200E010E2E0EF60DFCFA5D.
//                     ??111122223333
// write: ~250046E0200E010E2E0EF60DFCFA48.
// resp:  ~250046000000FDAF.

const unsigned char PaceBmsV25::exampleReadMosfetOverTemperatureConfigurationRequestV25[] = "~250046E10000FD99\r";
const unsigned char PaceBmsV25::exampleReadMosfetOverTemperatureConfigurationResponseV25[] = "~25004600200E010E2E0EF60DFCFA5D\r";
const unsigned char PaceBmsV25::exampleWriteMosfetOverTemperatureConfigurationRequestV25[] = "~250046E0200E010E2E0EF60DFCFA48\r";
const unsigned char PaceBmsV25::exampleWriteMosfetOverTemperatureConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, MosfetOverTemperatureConfiguration& config)
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
		const char* message = "Unknown payload byte does not match previously observed value";
		LogWarning(message);
		return false;
	}

	config.Alarm = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.Protection = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;
	config.ProtectionRelease = (ReadHexEncodedUShort(response, byteOffset) - 2730) / 10;

	return true;
}
bool PaceBmsV25::CreateWriteConfigurationRequest(const uint8_t busId, const MosfetOverTemperatureConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.Alarm < 30 || config.Alarm > 120)
	{
		const char* message = "Alarm is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.Protection < 30 || config.Protection > 120)
	{
		const char* message = "Protection is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.ProtectionRelease < 30 || config.ProtectionRelease > 120)
	{
		const char* message = "ProtectionRelease is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
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

const unsigned char PaceBmsV25::exampleReadEnvironmentOverUnderTemperatureConfigurationRequestV25[] = "~250046E70000FD93\r";
const unsigned char PaceBmsV25::exampleReadEnvironmentOverUnderTemperatureConfigurationResponseV25[] = "~25004600501A0109E209B009E20D340D660D34F806\r";
const unsigned char PaceBmsV25::exampleWriteEnvironmentOverUnderTemperatureConfigurationRequestV25[] = "~250046E6501A0109E209B009E20D340D660D34F7EB\r";
const unsigned char PaceBmsV25::exampleWriteEnvironmentOverUnderTemperatureConfigurationResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::ProcessReadConfigurationResponse(const uint8_t busId, const std::vector<uint8_t>& response, EnvironmentOverUnderTemperatureConfiguration& config)
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
		const char* message = "Unknown payload byte does not match previously observed value";
		LogWarning(message);
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
bool PaceBmsV25::CreateWriteConfigurationRequest(const uint8_t busId, const EnvironmentOverUnderTemperatureConfiguration& config, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (config.UnderAlarm < -35 || config.UnderAlarm > 30)
	{
		const char* message = "UnderAlarm is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.UnderProtection < -35 || config.UnderProtection > 30)
	{
		const char* message = "UnderProtection is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.UnderProtectionRelease < -35 || config.UnderProtectionRelease > 30)
	{
		const char* message = "UnderProtectionRelease is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.OverAlarm < 20 || config.OverAlarm > 100)
	{
		const char* message = "OverAlarm is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.OverProtection < 20 || config.OverProtection > 100)
	{
		const char* message = "OverProtection is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
		return false;
	}
	if (config.OverProtectionRelease < 20 || config.OverProtectionRelease > 100)
	{
		const char* message = "OverProtectionRelease is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
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

// "The charge limiter limits the charge current if it goes > 100A to 10A, this is useful for a multi-pack setup where the packs are not balanced"

// ==== Charge Current Limiter Start Current 
// 1 Charge Current Limiter Start Current: 100 - stored directly - valid range reported by PBmsTools as 5-150 
// read:  ~250046ED0000FD86.
// resp:  ~25004600C0040064FCCE.
//                     ??11
// write: ~250046EEC0040064FCA4.
// resp:  ~250046000000FDAF.

const unsigned char PaceBmsV25::exampleReadChargeCurrentLimiterStartCurrentRequestV25[] = "~250046ED0000FD86\r";
const unsigned char PaceBmsV25::exampleReadChargeCurrentLimiterStartCurrentResponseV25[] = "~25004600C0040064FCCE\r";
const unsigned char PaceBmsV25::exampleWriteChargeCurrentLimiterStartCurrentRequestV25[] = "~250046EEC0040064FCA4\r";
const unsigned char PaceBmsV25::exampleWriteChargeCurrentLimiterStartCurrentResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::CreateReadChargeCurrentLimiterStartCurrentRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadChargeCurrentLimiterStartCurrent, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsV25::ProcessReadChargeCurrentLimiterStartCurrentResponse(const uint8_t busId, const std::vector<uint8_t>& response, uint8_t& current)
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
		const char* message = "Response from wrong bus Id in payload";
		LogError(message);
		return false;
	}

	current = ReadHexEncodedByte(response, byteOffset);

	return true;
}
bool PaceBmsV25::CreateWriteChargeCurrentLimiterStartCurrentRequest(const uint8_t busId, const uint8_t current, std::vector<uint8_t>& request)
{
	// validate values conform to what PBmsTools would send
	if (current < 5 || current > 150)
	{
		const char* message = "current is not in the range that PBmsTools would send (or expect back)";
		LogError(message);
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
bool PaceBmsV25::ProcessWriteChargeCurrentLimiterStartCurrentResponse(const uint8_t busId, const std::vector<uint8_t>& response)
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

// ==== Read Remaining Capacity
// 1 Remaining Capacity (mAh): 62040 - stored in 10mAh hours, so 62040 is 6204
// 2 Actual Capacity (mAh): 103460 - stored in 10mAh hours, so 103460 is 10346
// 3 Design Capacity (mAh): 100000 - stored in 10mAh hours, so 100000 is 10000
// read:  ~250046A60000FD98.
// resp:  ~25004600400C183C286A2710FB0E.
//                     111122223333

const unsigned char PaceBmsV25::exampleReadRemainingCapacityRequestV25[] = "~250046A60000FD98\r";
const unsigned char PaceBmsV25::exampleReadRemainingCapacityResponseV25[] = "~25004600400C183C286A2710FB0E\r";

bool PaceBmsV25::CreateReadRemainingCapacityRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadRemainingCapacity, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsV25::ProcessReadRemainingCapacityResponse(const uint8_t busId, const std::vector<uint8_t>& response, uint32_t& remainingCapacityMilliampHours, uint32_t& actualCapacityMilliampHours, uint32_t& designCapacityMilliampHours)
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

// ==== Protocols
// 1 - CAN protocol, see enum, this example is "AFORE"
// 2 - RS485 protocol, see enum, this example is "RONGKE"
// 3 - "Type", see enum, not sure what this means exactly, I'd go with "Auto" which is in this example
// read:  ~250046EB0000FD88.
// resp:  ~25004600A006131400FC6F.
//                     112233
// write: ~250046ECA006131400FC47.
// resp:  ~250046000000FDAF.

const unsigned char PaceBmsV25::exampleReadProtocolsRequestV25[] = "~250046EB0000FD88\r";
const unsigned char PaceBmsV25::exampleReadProtocolsResponseV25[] = "~25004600A006131400FC6F\r";
const unsigned char PaceBmsV25::exampleWriteProtocolsRequestV25[] = "~250046ECA006131400FC47\r";
const unsigned char PaceBmsV25::exampleWriteProtocolsResponseV25[] = "~250046000000FDAF\r";

bool PaceBmsV25::CreateReadProtocolsRequest(const uint8_t busId, std::vector<uint8_t>& request)
{
	CreateRequest(busId, CID2_ReadCommunicationsProtocols, std::vector<uint8_t>(), request);
	return true;
}
bool PaceBmsV25::ProcessReadProtocolsResponse(const uint8_t busId, const std::vector<uint8_t>& response, Protocols& protocols)
{
	int16_t payloadLen = ValidateResponseAndGetPayloadLength(busId, response);
	if (payloadLen == -1)
	{
		// failed to validate, the call would have done it's own logging
		return false;
	}

	// payload starts here, everything else was validated by the initial call to ValidateResponseAndGetPayloadLength
	uint16_t byteOffset = 13;

	protocols.CAN = (NewStyleProtocolList_CAN)ReadHexEncodedByte(response, byteOffset);
	protocols.RS485 = (NewStyleProtocolList_RS485)ReadHexEncodedByte(response, byteOffset);
	protocols.Type = (NewStyleProtocolList_Type)ReadHexEncodedByte(response, byteOffset);

	return true;
}
bool PaceBmsV25::CreateWriteProtocolsRequest(const uint8_t busId, const Protocols& protocols, std::vector<uint8_t>& request)
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
bool PaceBmsV25::ProcessWriteProtocolsResponse(const uint8_t busId, const std::vector<uint8_t>& response)
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

// There are many other settings in "System Configuration" that can be written and/or calibrated here, 
// none of which I am exposing because it would be a Very Bad Idea to mess with them

