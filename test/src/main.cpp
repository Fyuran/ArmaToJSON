/** @file main.cpp
 * @brief Test program for the ArmaToJSON library.
 */

#include "main.hpp"
#include "dllmain.hpp"
#include "logger.hpp"
#include <cassert>
#include <exception>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <packet.hpp>
#include <stdexcept>
#include <string_view>
#include <vector>

#define MAX_SIZE 20480

/// @brief limited buffer for RvExtension output, as of Arma 3 2.20
char buffer[MAX_SIZE];

using JSON = nlohmann::json;
using namespace ArmaToJSON;

/// @brief Wrapper for RvExtension calls
/// @param json The json to be passed as C-String to RVExtension.
Packet callRVExtensionFnc(const Packet &outPacket) {
	RVExtension(buffer, MAX_SIZE, outPacket.dump().c_str());
	Packet inPacket{std::string_view{buffer}};
	// if receiving packet from fnc has these two properties, it means the packet is chunked and we
	// need to retrieve the missing pieces
	if (inPacket.hasKey("chunks") && inPacket.hasKey("storageID")) {
		auto chunksAmount{inPacket.value<size_t>("chunks", 0)};
		if(chunksAmount <= 0)
			throw std::runtime_error("chunks cannot be 0 or negative");

		Packet chunkRequestPacket{};
		chunkRequestPacket["function"] = "getChunk";
		chunkRequestPacket["storageID"] = inPacket.value<std::string>("storageID", "");
		chunkRequestPacket["chunks"] = chunksAmount;

		std::string chunks{};
		for (size_t requestChunk = 0; requestChunk < chunksAmount; ++requestChunk) {
			chunkRequestPacket["requestChunk"] = requestChunk;
			callRVExtensionFnc(chunkRequestPacket);
			chunks += buffer;
		}

		inPacket.clear();
		inPacket.setData(JSON::parse(chunks));
	}
	return inPacket;
}

JSON getJSONFile(const std::filesystem::path &path) {
	std::ifstream ifs{path};
	if (!ifs)
		throw std::runtime_error(fmt::format("path: {} not found", path.string()));

	JSON j(JSON::value_t::object);
	ifs >> j;

	return j;
}

/// @brief Main test function.
/// @return Exit code.
int main() {
	try {
		Packet packet_1{getJSONFile("./json_1.json")};
		packet_1["data"] = getJSONFile(packet_1["data"]);

		const Packet packet_2{getJSONFile("./json_2.json")};
		const Packet packet_3{getJSONFile("./json_3.json")};
		const Packet packet_4{getJSONFile("./json_4.json")};
		const Packet packet_5{getJSONFile("./json_5.json")};
		const Packet packet_6{getJSONFile("./json_6.json")};

		LOG_WARN("Test has begun");

		LOG_DEBUG("packet with createFile");
		auto inPacket{callRVExtensionFnc(packet_1)};
		LOG_DEBUG("assembled packet is {}", inPacket);

		LOG_DEBUG("packet with copyFile");
		inPacket = callRVExtensionFnc(packet_2);
		LOG_DEBUG("assembled packet is {}", inPacket);

		LOG_DEBUG("packet with renameFile");
		inPacket = callRVExtensionFnc(packet_3);
		LOG_DEBUG("assembled packet is {}", inPacket);
	
		LOG_DEBUG("packet with retrieveList");
		inPacket = callRVExtensionFnc(packet_5);
		LOG_DEBUG("assembled packet is {}", inPacket);

		LOG_DEBUG("packet with getFile");
		inPacket = callRVExtensionFnc(packet_6);
		LOG_DEBUG("assembled packet is {}", inPacket);

		LOG_DEBUG("packet with deleteFile");
		inPacket = callRVExtensionFnc(packet_4);
		LOG_DEBUG("assembled packet is {}", inPacket);

		LOG_WARN("Test is done");
	} catch (const std::exception &e) {
		LOG_ERROR("Test ERROR: {}", e.what());
		std::cerr << e.what();
	}
	return 0;
}