#pragma once

#include <string>
//#include <cstring>
#include <vector>

// uncomment the std version if using a C++17 compiler, otherwise esphome provides an equivalent implementation
//#include <optional>
//using namespace std;
#include "esphome/core/optional.h"
//using namespace esphome;

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

class PaceBmsProtocolBase
{
public:
	// dependency injection
	typedef void (*LogFuncPtr)(std::string message);

protected:
	uint8_t protocol_version;
	optional<uint8_t> protocol_version_override;
	// battery chemistry
	uint8_t cid1;

	optional<uint8_t> analog_cell_count_override;
	optional<uint8_t> analog_temperature_count_override;
	optional<uint8_t> status_cell_count_override;
	optional<uint8_t> status_temperature_count_override;

	uint32_t design_capacity_mah_override;

	// dependency injection
	LogFuncPtr LogErrorPtr;
	LogFuncPtr LogWarningPtr;
	LogFuncPtr LogInfoPtr;
	LogFuncPtr LogDebugPtr;
	LogFuncPtr LogVerbosePtr;
	LogFuncPtr LogVeryVerbosePtr;

	void LogError(std::string message);
	void LogWarning(std::string message);
	void LogInfo(std::string message);
	void LogDebug(std::string message);
	void LogVerbose(std::string message);
	void LogVeryVerbose(std::string message);

	// Takes a length value and adds a checksum to the upper nibble, this is "CKLEN" used in command or response headers
	static uint16_t CreateChecksummedLength(const uint16_t cklen);

	// Checks if the checksum nibble in a "checksummed length" is valid
	static bool ValidateChecksummedLength(const uint16_t cklen);

	// Length is just the lower 12 bits of the checksummed length 
	static uint16_t LengthFromChecksummedLength(const uint16_t cklen);

	// Calculates the checksum for an entire request or response "packet" (this is not for the embedded length value)
	static uint16_t CalculateRequestOrResponseChecksum(const std::vector<uint8_t>& data);

	// helper for WriteHexEncoded----
	// Works with ASCII encoding, not portable, but then that's what the protocol uses
	static uint8_t NibbleToHex(const uint8_t nibbleByte);

	// helper for ReadHexEncoded----
	// Works with ASCII encoding, not portable, but then that's what the protocol uses
	static uint8_t HexToNibble(const uint8_t hex);

	// decode a 'real' byte from the stream by reading two ASCII hex encoded bytes
	static uint8_t ReadHexEncodedByte(const std::vector<uint8_t>& data, uint16_t& dataOffset);

	// decode a 'real' uint16_t from the stream by reading four ASCII hex encoded bytes
	static uint16_t ReadHexEncodedUShort(const std::vector<uint8_t>& data, uint16_t& dataOffset);

	// decode a 'real' int16_t from the stream by reading four ASCII hex encoded bytes
	static int16_t ReadHexEncodedSShort(const std::vector<uint8_t>& data, uint16_t& dataOffset);

	// encode a 'real' byte to the stream by writing two ASCII hex encoded bytes
	static void WriteHexEncodedByte(std::vector<uint8_t>& data, uint16_t& dataOffset, uint8_t byte);

	// encode a 'real' uint16_t to the stream by writing four ASCII hex encoded bytes
	static void WriteHexEncodedUShort(std::vector<uint8_t>& data, uint16_t& dataOffset, uint16_t ushort);

	// encode a 'real' int16_t to the stream by writing four ASCII hex encoded bytes
	static void WriteHexEncodedSShort(std::vector<uint8_t>& data, uint16_t& dataOffset, int16_t sshort);

	std::string FormatReturnCode(const uint8_t returnCode);

	void CreateRequest(const uint8_t busId, const uint8_t cid2, const std::vector<uint8_t> payload, std::vector<uint8_t>& request);

	int16_t ValidateResponseAndGetPayloadLength(const uint8_t busId, const std::vector<uint8_t> response);

};

