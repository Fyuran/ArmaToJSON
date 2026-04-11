#include "manager.hpp"
#include "functions.hpp"
#include "logger.hpp"
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ranges>

using JSON = nlohmann::json;

namespace ArmaToJSON {
namespace manager {
std::unordered_map<std::string, std::map<size_t, std::string>> chunkStorage{};

const std::unordered_map<std::string, Function> packetFunctions = {
	{"copyFile", file::copyFile},		  {"createFile", file::createFile},
	{"retrieveList", file::retrieveList}, {"getFile", file::getFile},
	{"deleteFile", file::deleteFile},	  {"renameFile", file::renameFile}};

void manageIO(char *const output, const size_t outputMaxSize, const std::string_view data) {
	JSON json(JSON::parse(data));
	assert(json.is_object());
	try {
		if (!json.contains("function"))
			throw std::runtime_error{"\"function\" key not found"};

		const std::string fncName{json.at("function")};
		if (packetFunctions.contains(fncName)) {
			json = packetFunctions.at(fncName)(json);
		} else if(fncName == "getChunk"){
			storage::getChunk(output, outputMaxSize, json);
			return;
		}
		else {
			throw std::runtime_error{
				fmt::format("fnc: {} not implemented, available functions are : {}", fncName.empty() ? "EMPTY" : fncName, packetFunctions | std::views::keys)};
		}

		if(json.dump().size() > outputMaxSize) {
			json = storage::storeData(json, outputMaxSize - 2);
		}
		writeToOutput(output, outputMaxSize, json.dump()); // reserve 1 for null terminator
	} catch (const std::exception &e) {
		json.clear();
		json["error"] = e.what();
		LOG_ERROR("manageIO ERROR: {}", e.what());
		std::cerr << e.what();
		writeToOutput(output, outputMaxSize, json.dump());
	}
}

void writeToOutput(char *const output, const size_t outputMaxSize, const std::string &input) {
	if (!output) {
		throw std::runtime_error{"output is null"};
	}
	LOG_DEBUG("outputMaxSize is {}, input.size() is {}", outputMaxSize, input.size());

	if (input.empty())
		return;

	if (input.size() > outputMaxSize)
		throw std::runtime_error{
			fmt::format("input is too big, input.size() is {}, outputMaxSize is {}", input.size(),
						outputMaxSize)};

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
	LOG_DEBUG("RVExtension writing to output {} out of {} bytes of data", input.size(),
			 outputMaxSize);
}
} // namespace manager
} // namespace ArmaToJSON
