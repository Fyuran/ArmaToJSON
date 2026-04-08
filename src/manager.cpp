#include "manager.hpp"
#include "functions.hpp"
#include "logger.hpp"

using JSON = nlohmann::json;

namespace ArmaToJSON {
namespace packet_manager {

void managePacketIO(char *const output, const size_t outputMaxSize, const Packet &inPacket) {
	Packet outPacket{};

	try {
		if (inPacket.hasKey("function")) {
			const std::string fncName{inPacket.value<std::string>("function", "")};
			if (fncName == "getChunk") {
				storage::getChunk(output, inPacket, outputMaxSize);
				return; // quit with getChunk, it will handle output, json shenanigans when
						// assigning a string
			} else
				managePacketFunction(inPacket, outPacket, outputMaxSize);
		}
		writeToOutput(output, outPacket, outputMaxSize - 1); // reserve 1 for null terminator
	} catch (const std::exception &e) {
		outPacket.clear();
		outPacket["error"] = e.what();
		LOG_ERROR("manageIO ERROR: {}", e.what());
		writeToOutput(output, outPacket, outputMaxSize - 1);
	}
}

void managePacketFunction(const Packet &inPacket, Packet &outPacket, const size_t outputMaxSize) {
	const std::string fnc{inPacket.value<std::string>("function", "")};

	LOG_DEBUG("calling packet function: {}", fnc);

	if (packetFunctions.contains(fnc)) {
		packetFunctions.at(fnc)(inPacket, &outPacket);
	} else {
		throw std::runtime_error{
			fmt::format("fnc: {} not implemented", fnc.empty() ? "EMPTY" : fnc)};
	}
}

void writeToOutput(char *const output, Packet &outPacket, const size_t outputMaxSize) {
	if (outPacket.size() > outputMaxSize)
		storage::storePacketData(
			outPacket, outputMaxSize); // will modify outPacket to hold metadata for chunk retrieval

	writeToOutput(output, outPacket.dump(), outputMaxSize);
}

void writeToOutput(char *const output, const std::string& input, const size_t outputMaxSize) {
	if (!output) {
		throw std::runtime_error{"output is null"};
	}
	LOG_DEBUG("outputMaxSize is {}, input.size() is {}", outputMaxSize, input.size());

	if (input.empty())
		return;

	if (input.size() > outputMaxSize)
		throw std::runtime_error{"input is too big"};

#ifdef _WIN32
	// if count is _TRUNCATE, then as much of packet as will it into output is copied
	// while still leaving room for the terminating null, which is always
	// appended.
	strncpy_s(output, outputMaxSize, input.c_str(), _TRUNCATE);
#elif __linux__
	// this function will take the full size of the buffer(not just the length)
	// and guarantee to NUL-terminate the result (as long as size is larger than
	// 0)
	strlcpy(output, input.c_str(), outputMaxSize);
#endif
	LOG_INFO("RVExtension returning char* with {} out of {} bytes of data", input.size(),
			 outputMaxSize);
}
} // namespace packet_manager
} // namespace ArmaToJSON
