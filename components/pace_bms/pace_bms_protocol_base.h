#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>
#include <optional>
#include <functional>
#include <memory>
#include <cstring>

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

const uint8_t FRAME_HEADER_SIZE = 13;
const uint8_t PAYLOAD_START_OFFSET = 13;
const uint8_t FRAME_SIZE_WITHOUT_PAYLOAD = 18; // SOI + VER + ADR + CID1 + CID2 + CKLEN + CHKSUM + EOI

// dependency injection
//typedef void (*LogFuncPtr)(std::string message);
typedef std::function<void(std::string)> LogFuncPtr;

// generic RAII-style scope guard which calls the provided functions on construction and destruction if active is true
class ScopeGuard 
{
private:
	std::function<void()> onScopeExit;
	bool active;

public:
	explicit ScopeGuard(std::function<void()> onScopeEntry, std::function<void()> onScopeExit, bool active = true) 
		: onScopeExit(onScopeExit), active(active) 
	{
		if (active)
		{
			onScopeEntry();
		}
	}

	~ScopeGuard() 
	{ 
		if (active)
		{
			onScopeExit();
		}
	}

	// cancel the scope exit action
	void dismiss() 
	{ 
		active = false; 
	}

	// Delete copy constructor and assignment operator to avoid double-exit
	ScopeGuard(const ScopeGuard&)             = delete;
    ScopeGuard(ScopeGuard&&)                  = delete;
    ScopeGuard& operator =(const ScopeGuard&) = delete;
    ScopeGuard& operator =(ScopeGuard&&)      = delete;

	// prevent heap allocation, this should be used only as a stack variable (or maybe a member variable?)
	static void* operator new     (size_t) = delete;
	static void* operator new[]   (size_t) = delete;
	static void  operator delete  (void*)  = delete;
	static void  operator delete[](void*)  = delete;
};

class PaceBmsProtocolBase
{
public:
	PaceBmsProtocolBase(uint8_t protocol_commandset, std::optional<std::string> protocol_variant, std::optional<uint8_t> protocol_version, std::optional<uint8_t> battery_chemistry,
		                LogFuncPtr logError, LogFuncPtr logWarning, LogFuncPtr logInfo, LogFuncPtr logDebug, LogFuncPtr logVerbose, LogFuncPtr logVeryVerbose)
	{
		this->protocol_commandset = protocol_commandset;
		this->protocol_variant = protocol_variant;
		this->protocol_version = protocol_version;
		if (battery_chemistry.has_value())
			this->cid1 = battery_chemistry.value();
		else
			this->cid1 = 0x46;

		this->LogErrorPtr = logError;
		this->LogWarningPtr = logWarning;
		this->LogInfoPtr = logInfo;
		this->LogDebugPtr = logDebug;
		this->LogVerbosePtr = logVerbose;
		this->LogVeryVerbosePtr = logVeryVerbose;
	}

	struct DateTime
	{
		uint16_t Year;
		uint8_t Month;
		uint8_t Day;
		uint8_t Hour;
		uint8_t Minute;
		uint8_t Second;
	};

	// if enabled, all dependency-injected logging functions will be redirected to LogVeryVerbose
	// used to "test" if a response is valid without spamming the logs
	// ScopeGuard is used to set / unset this flag for the duration of a call to a decode method
	void SetQuietMode(bool quietMode)
	{
		this->quietMode = quietMode;
	}

protected:
	bool quietMode = false;

	uint8_t protocol_commandset;
	std::optional<std::string> protocol_variant;
	std::optional<uint8_t> protocol_version;
	// battery chemistry
	uint8_t cid1;

	std::optional<std::string> detected_variant;

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
	uint16_t CreateChecksummedLength(const uint16_t cklen);

	// Checks if the checksum nibble in a "checksummed length" is valid
	static bool ValidateChecksummedLength(const uint16_t cklen);

	// Length is just the lower 12 bits of the checksummed length 
	static uint16_t LengthFromChecksummedLength(const uint16_t cklen);

	// Calculates the checksum for an entire request or response "packet" (this is not for the embedded length value)
	static uint16_t CalculateRequestOrResponseChecksum(const std::span<uint8_t>& data);

	// helper for WriteHexEncoded----
	// Works with ASCII encoding, not portable, but then that's what the protocol uses
	uint8_t NibbleToHex(const uint8_t nibbleByte);

	// helper for ReadHexEncoded----
	// Works with ASCII encoding, not portable, but then that's what the protocol uses
	uint8_t HexToNibble(const uint8_t hex);

	// decode a 'real' byte from the stream by reading two ASCII hex encoded bytes
	uint8_t ReadHexEncodedByte(const std::span<uint8_t>& data, uint16_t& dataOffset);

	// decode a 'real' uint16_t from the stream by reading four ASCII hex encoded bytes
	uint16_t ReadHexEncodedUShort(const std::span<uint8_t>& data, uint16_t& dataOffset);

	// decode a 'real' int16_t from the stream by reading four ASCII hex encoded bytes
	int16_t ReadHexEncodedSShort(const std::span<uint8_t>& data, uint16_t& dataOffset);

	// decode a 'real' uint32_t from the stream by reading eight ASCII hex encoded bytes
	uint32_t ReadHexEncodedULong(const std::span<uint8_t>& data, uint16_t& dataOffset);

	// encode a 'real' byte to the stream by writing two ASCII hex encoded bytes
	void WriteHexEncodedByte(std::vector<uint8_t>& data, uint16_t& dataOffset, uint8_t byte);

	// encode a 'real' uint16_t to the stream by writing four ASCII hex encoded bytes
	void WriteHexEncodedUShort(std::vector<uint8_t>& data, uint16_t& dataOffset, uint16_t ushort);

	// encode a 'real' int16_t to the stream by writing four ASCII hex encoded bytes
	void WriteHexEncodedSShort(std::vector<uint8_t>& data, uint16_t& dataOffset, int16_t sshort);

	std::string FormatReturnCode(const uint8_t returnCode);

	void CreateRequest(const uint8_t busId, const uint8_t cid2, const std::span<uint8_t> payload, std::vector<uint8_t>& request);

	int16_t ValidateResponseAndGetPayloadLength(const uint8_t busId, std::optional<uint8_t> respondingBusId, const std::span<uint8_t> response);
};

