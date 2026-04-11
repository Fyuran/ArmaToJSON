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
#include <stdexcept>

#define MAX_SIZE 20480

/// @brief limited buffer for RvExtension output, as of Arma 3 2.20
char buffer[MAX_SIZE];

using JSON = nlohmann::json;
using namespace ArmaToJSON;

/// @brief Wrapper for RvExtension calls
/// @param json The json to be passed as C-String to RVExtension.
JSON callRVExtensionFnc(const JSON &json) {
	RVExtension(buffer, MAX_SIZE, json.dump().c_str());
	JSON returnJSON(JSON::parse(buffer));
	// if receiving json from fnc has these two properties, it means the json is chunked and we
	// need to retrieve the missing pieces
	if (returnJSON.contains("ID") && returnJSON.contains("size")) {
		size_t chunksAmount{returnJSON.at("size").get<size_t>()};

		JSON chunkRequest{{"function", "getChunk"}, {"ID", returnJSON.at("ID").get<std::string>()}};
		std::string chunks{};
		for (size_t i = 0; i < chunksAmount; ++i) {
			chunkRequest["chunk"] = i;
			RVExtension(buffer, MAX_SIZE, chunkRequest.dump().c_str());
			chunks += buffer;
		}

		returnJSON = JSON::parse(chunks);
	}
	return returnJSON;
}

JSON getJSONFile(const std::filesystem::path &path) {
	std::ifstream ifs{path};
	if (!ifs)
		throw std::runtime_error(fmt::format("path: {} not found", path.string()));

	JSON j(JSON::object());
	ifs >> j;

	return j;
}

/// @brief Main test function.
/// @return Exit code.
int main() {
	try {
		JSON json_1(getJSONFile("./json_1.json"));
		json_1["data"] = getJSONFile(json_1["data"]);

		const JSON json_2(getJSONFile("./json_2.json"));
		const JSON json_3(getJSONFile("./json_3.json"));
		const JSON json_4(getJSONFile("./json_4.json"));
		const JSON json_5(getJSONFile("./json_5.json"));
		const JSON json_6(getJSONFile("./json_6.json"));

		LOG_WARN("Test has begun");

		LOG_DEBUG("json with createFile");
		JSON returnJSON(callRVExtensionFnc(json_1));
		LOG_DEBUG("assembled json is {}", returnJSON.dump());

		LOG_DEBUG("json with copyFile");
		returnJSON = callRVExtensionFnc(json_2);
		LOG_DEBUG("assembled json is {}", returnJSON.dump());

		LOG_DEBUG("json with renameFile");
		returnJSON = callRVExtensionFnc(json_3);
		LOG_DEBUG("assembled json is {}", returnJSON.dump());

		LOG_DEBUG("json with retrieveList");
		returnJSON = callRVExtensionFnc(json_5);
		LOG_DEBUG("assembled json is {}", returnJSON.dump());

		LOG_DEBUG("json with getFile");
		returnJSON = callRVExtensionFnc(json_6);
		LOG_DEBUG("assembled json is {}", returnJSON.dump());

		LOG_DEBUG("json with deleteFile");
		returnJSON = callRVExtensionFnc(json_4);
		LOG_DEBUG("assembled json is {}", returnJSON.dump());

		LOG_WARN("Test is done");
	} catch (const std::exception &e) {
		LOG_ERROR("Test ERROR: {}", e.what());
		std::cerr << e.what();
	}
	return 0;
}