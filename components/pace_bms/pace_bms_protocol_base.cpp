
#include "pace_bms_protocol_base.h"

void PaceBmsProtocolBase::LogError(std::string message)
{
	if (LogErrorPtr != 0)
	{
		LogErrorPtr(message);
	}
}
void PaceBmsProtocolBase::LogWarning(std::string message)
{
	if (LogWarningPtr != 0)
	{
		LogWarningPtr(message);
	}
}
void PaceBmsProtocolBase::LogInfo(std::string message)
{
	if (LogInfoPtr != 0)
	{
		LogInfoPtr(message);
	}
}
void PaceBmsProtocolBase::LogDebug(std::string message)
{
	if (LogDebugPtr != 0)
	{
		LogDebugPtr(message);
	}
}
void PaceBmsProtocolBase::LogVerbose(std::string message)
{
	if (LogVerbosePtr != 0)
	{
		LogVerbosePtr(message);
	}
}
void PaceBmsProtocolBase::LogVeryVerbose(std::string message)
{
	if (LogVeryVerbosePtr != 0)
	{
		LogVeryVerbosePtr(message);
	}
}

// Takes a length value and adds a checksum to the upper nibble, this is "CKLEN" used in command or response headers
uint16_t PaceBmsProtocolBase::CreateChecksummedLength(const uint16_t cklen)
{
	// max value of length is 12 bit, top 4 bits will be checksum of actual length value
	uint16_t len = (cklen & 0x0FFF);

	// bitwise NOT then add 1, mask off any carry (even though we'd shift it out anyway in the next step)
	uint16_t lcksum = (len & 0xF) + ((len >> 4) & 0xF) + ((len >> 8) & 0xF);
	lcksum = ~lcksum;
	lcksum++;
	lcksum = lcksum & 0x000F;

	// checksum goes in the top nibble, length in the bottom 3 nibbles
	return (lcksum << 12) | len;
}

// Checks if the checksum nibble in a "checksummed length" is valid
bool PaceBmsProtocolBase::ValidateChecksummedLength(const uint16_t cklen)
{
	// actual length from bottom 3 nibbles
	uint16_t len = (cklen & 0x0FFF);

	// bitwise NOT then add 1, mask off any carry (even though we'd shift it out anyway in the next step)
	uint16_t lcksum = (len & 0xF) + ((len >> 4) & 0xF) + ((len >> 8) & 0xF);
	lcksum = ~lcksum;
	lcksum++;
	lcksum = lcksum & 0x000F;

	// checksum under test from top nibble
	uint16_t lcksumtest = cklen & 0xF000;
	lcksumtest = lcksumtest >> 12;

	return lcksumtest == lcksum;
}

// Length is just the lower 12 bits of the checksummed length 
uint16_t PaceBmsProtocolBase::LengthFromChecksummedLength(const uint16_t cklen)
{
	return cklen & 0x0FFF;
}

// Calculates the checksum for an entire request or response "packet" (this is not for the embedded length value)
uint16_t PaceBmsProtocolBase::CalculateRequestOrResponseChecksum(const std::vector<uint8_t>& data)
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
uint8_t PaceBmsProtocolBase::NibbleToHex(const uint8_t nibbleByte)
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
uint8_t PaceBmsProtocolBase::HexToNibble(const uint8_t hex)
{
	if (hex < 65)
		return hex - '0';  // Return a value from 0 to 9
	else
		return hex + 10 - 'A';  // Return a value from 10 to 15
}

// decode a 'real' byte from the stream by reading two ASCII hex encoded bytes
uint8_t PaceBmsProtocolBase::ReadHexEncodedByte(const std::vector<uint8_t>& data, uint16_t& dataOffset)
{
	uint8_t byte = 0;
	byte |= (HexToNibble(data[dataOffset++]) << 4) & 0xF0;
	byte |= (HexToNibble(data[dataOffset++]) << 0) & 0x0F;
	return byte;
}

// decode a 'real' uint16_t from the stream by reading four ASCII hex encoded bytes
uint16_t PaceBmsProtocolBase::ReadHexEncodedUShort(const std::vector<uint8_t>& data, uint16_t& dataOffset)
{
	uint16_t ushort = 0;
	ushort |= ((HexToNibble(data[dataOffset++]) << 12) & 0xF000);
	ushort |= ((HexToNibble(data[dataOffset++]) << 8) & 0x0F00);
	ushort |= ((HexToNibble(data[dataOffset++]) << 4) & 0x00F0);
	ushort |= ((HexToNibble(data[dataOffset++]) << 0) & 0x000F);
	return ushort;
}

// decode a 'real' int16_t from the stream by reading four ASCII hex encoded bytes
int16_t PaceBmsProtocolBase::ReadHexEncodedSShort(const std::vector<uint8_t>& data, uint16_t& dataOffset)
{
	int16_t sshort = 0;
	sshort |= ((HexToNibble(data[dataOffset++]) << 12) & 0xF000);
	sshort |= ((HexToNibble(data[dataOffset++]) << 8) & 0x0F00);
	sshort |= ((HexToNibble(data[dataOffset++]) << 4) & 0x00F0);
	sshort |= ((HexToNibble(data[dataOffset++]) << 0) & 0x000F);
	return sshort;
}

// encode a 'real' byte to the stream by writing two ASCII hex encoded bytes
void PaceBmsProtocolBase::WriteHexEncodedByte(std::vector<uint8_t>& data, uint16_t& dataOffset, uint8_t byte)
{
	data[dataOffset++] = NibbleToHex((byte >> 4) & 0x0F);
	data[dataOffset++] = NibbleToHex((byte >> 0) & 0x0F);
}

// encode a 'real' uint16_t to the stream by writing four ASCII hex encoded bytes
void PaceBmsProtocolBase::WriteHexEncodedUShort(std::vector<uint8_t>& data, uint16_t& dataOffset, uint16_t ushort)
{
	data[dataOffset++] = NibbleToHex((ushort >> 12) & 0x0F);
	data[dataOffset++] = NibbleToHex((ushort >> 8) & 0x0F);
	data[dataOffset++] = NibbleToHex((ushort >> 4) & 0x0F);
	data[dataOffset++] = NibbleToHex((ushort >> 0) & 0x0F);
}

// encode a 'real' int16_t to the stream by writing four ASCII hex encoded bytes
void PaceBmsProtocolBase::WriteHexEncodedSShort(std::vector<uint8_t>& data, uint16_t& dataOffset, int16_t sshort)
{
	data[dataOffset++] = NibbleToHex((sshort >> 12) & 0x0F);
	data[dataOffset++] = NibbleToHex((sshort >> 8) & 0x0F);
	data[dataOffset++] = NibbleToHex((sshort >> 4) & 0x0F);
	data[dataOffset++] = NibbleToHex((sshort >> 0) & 0x0F);
}

std::string PaceBmsProtocolBase::FormatReturnCode(const uint8_t returnCode)
{
	switch (returnCode)
	{
	case 0:
		return std::string("OK");
		break;
	case 1:
		return std::string("Version Error");
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
		return std::string("Command Format Error");
		break;
	case 6:
		return std::string("Invalid Data");
		break;
	case 9:
		return std::string("Operation or Write Error");
		break;
	case 90:
		return std::string("ADR Error");
		break;
	case 91:
		return std::string("Communication Error");
		break;
	default:
		return std::string("Undocumented Response Error Code");
		break;
	}

	return std::string("impossible");
}

// create a standard request to the given busId for the given CID2, filling in the payload (if given)
void PaceBmsProtocolBase::CreateRequest(const uint8_t busId, const uint8_t cid2, const std::vector<uint8_t> payload, std::vector<uint8_t>& request)
{
	uint16_t byteOffset = 0;

	request.resize(payload.size() + 18);

	// SOI marker
	request[byteOffset++] = '~';

	uint8_t target_ver = this->protocol_version;
	if (this->protocol_version_override.has_value())
		target_ver = this->protocol_version_override.value();
	WriteHexEncodedByte(request, byteOffset, target_ver);

	// busId
	WriteHexEncodedByte(request, byteOffset, busId);

	// cid1
	WriteHexEncodedByte(request, byteOffset, cid1);

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
		LogError("Length mismatch creating request, this is a code bug in PACE_BMS");
	}
}

// validate all fields in the response except the payload data: SOI marker, header values, checksum, EOI marker
// returns the detected payload length (payload always starts at offset 13), or -1 for error
int16_t PaceBmsProtocolBase::ValidateResponseAndGetPayloadLength(const uint8_t busId, const std::vector<uint8_t> response)
{
	uint16_t byteOffset = 0;

	// the number of bytes for a response with zero payload, we'll check again once we decode the checksummed length embedded in the response to make sure we don't run past the end of the buffer
	if (response.size() < 18)
	{
		LogError("Response is truncated, even a response without payload should be 18 bytes");
		return -1;
	}

	if (response[byteOffset++] != '~')
	{
		LogError("Response does not begin with SOI marker");
		return -1;
	}

	uint8_t ver = ReadHexEncodedByte(response, byteOffset);
	uint8_t target_ver = this->protocol_version;
	if (this->protocol_version_override.has_value())
		target_ver = this->protocol_version_override.value();
	if (ver != target_ver)
	{
		LogError("Response has wrong version number");
		return -1;
	}

	uint8_t addr = ReadHexEncodedByte(response, byteOffset);
	if (addr != busId)
	{
		LogError("Response from wrong bus Id");
		return -1;
	}

	uint8_t cid = ReadHexEncodedByte(response, byteOffset);
	if (cid != cid1)
	{
		LogError("Response has wrong CID1");
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
		LogError("Response contains an incorrect payload length checksum, ignoring since this is a known firmware bug");
		return -1;
	}

	uint16_t payloadLen = LengthFromChecksummedLength(cklen);

	if (payloadLen + 18 < (uint16_t)response.size() && response[response.size() - 1] == '\r')
	{
		LogError("Response contains an incorrect payload length, fixing up the value by checking against EOI, ignoring since this is a known firmware bug");
		return -1;
		//payloadLen = (uint16_t)response.size() - 18;
	}

	if ((uint16_t)response.size() < payloadLen + 18)
	{
		LogError("Response is truncated, should be 18 bytes + decoded payload length");
		return -1;
	}
	if ((uint16_t)response.size() > payloadLen + 18)
	{
		LogError("Response is oversize");
		return -1;
	}

	// now that we have payload length and have verified buffer size, jump past the payload and confirm the checksum of the entire packet
	byteOffset += payloadLen;
	uint16_t givenCksum = ReadHexEncodedUShort(response, byteOffset);
	uint16_t calcCksum = CalculateRequestOrResponseChecksum(response);
	if (givenCksum != calcCksum)
	{
		LogError("Response contains an incorrect frame checksum");
		return -1;
	}

	if (response[byteOffset++] != '\r')
	{
		LogError("Response does not end with EOI marker");
		return -1;
	}

	if (byteOffset != payloadLen + 18)
	{
		LogError("Length mismatch validating response, this is a code bug in PACE_BMS");
		return -1;
	}

	return payloadLen;
}
