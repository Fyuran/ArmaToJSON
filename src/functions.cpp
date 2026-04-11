#include "logger.hpp"
#include "manager.hpp"
#include <boost/date_time.hpp>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <functions.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>

#define MAX_SIZE 20480

using JSON = nlohmann::json;
namespace fs = std::filesystem;
namespace ArmaToJSON {

namespace storage {
size_t ID = 0;
const std::string createNewID() {
	return fmt::format("{}-{}", ID++, std::chrono::system_clock::now());
}

JSON storeData(const JSON &json, const size_t outputMaxSize) {
	const std::string dump{json.dump()};
	const std::string ID{createNewID()};
	const size_t size{(dump.size() + outputMaxSize - 1) / outputMaxSize};
	std::map<size_t, std::string> chunks;

	for (size_t i = 0; i < size; ++i) {
		chunks.emplace(i, dump.substr(i * outputMaxSize, outputMaxSize));
	}
	manager::chunkStorage.emplace(std::make_pair(ID, chunks));

	LOG_WARN("json is too big, storing json at {}", ID);
	return {{"ID", ID}, {"size", size}};
}

void getChunk(char *const output, const size_t outputMaxSize, const JSON &json) {
	if (manager::chunkStorage.empty())
		throw std::runtime_error{"storage is empty"};

	std::string id{json.at("ID").get<std::string>()};
	size_t requestChunk{json.at("chunk").get<size_t>()};

	if (!manager::chunkStorage.contains(id))
		throw std::runtime_error(
			fmt::format("ID: {} has not been found", id.empty() ? "EMPTY" : id));

	std::string chunk{manager::chunkStorage.at(id).at(requestChunk)};
	manager::chunkStorage.at(id).erase(requestChunk);

	LOG_INFO("getting chunk {} at {}", id, requestChunk);
	manager::writeToOutput(output, outputMaxSize, chunk);
}

} // namespace storage

namespace file {
JSON copyFile(const JSON &json) {
	const fs::path path{json.at("path").get<fs::path>()};
	const fs::path newPath{json.at("newPath").get<fs::path>()};

	if (path.empty())
		throw std::runtime_error{"path is empty"};
	if (newPath.empty())
		throw std::runtime_error{"newPath is empty"};

	if (auto filename{path.filename()}; filename.empty() || filename.extension() != ".json")
		throw std::runtime_error{"path is not a valid file"};

	bool hasCopied = fs::copy_file(path, newPath, fs::copy_options::overwrite_existing);

	assert(hasCopied);
	LOG_INFO("copying file {}", path.string());
	return {{"hasCopied", hasCopied}};
}

JSON createFile(const JSON &json) {
	const fs::path path{json.at("path").get<fs::path>()};
	if (path.empty())
		throw std::runtime_error{"path is empty"};

	if (path.has_parent_path()) {
		const auto &parent = path.parent_path();
		// "." doesn't need creating, anything else does
		if (parent != "." && !std::filesystem::exists(parent)) {
			std::filesystem::create_directories(parent);
		}
	}

	std::ofstream file(path, std::ios::binary);
	if (!file)
		throw std::runtime_error("failed to open file: " + path.string());

	file << json.value("data", JSON::object());

	LOG_INFO("creating file {}", path.string());
	return JSON::object();
}

JSON retrieveList(const JSON &json) {
	const fs::path path{json.at("path").get<fs::path>()};

	if (path.empty())
		throw std::runtime_error{"path is empty"};

	std::vector<std::string> paths;
	bool isCurrentPath = path == "." || path.empty();
	fs::path searchPath = isCurrentPath ? fs::current_path() : fs::path(path);

	if (!fs::exists(searchPath) || !fs::is_directory(searchPath)) {
		throw std::runtime_error{"path is wrong or missing"};
	}

	// dir iterators are completely unrelated so i have to split the code in two
	if (isCurrentPath) {
		for (const auto &entry : fs::recursive_directory_iterator(searchPath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".json")
				paths.push_back(entry.path().string());
		}
	} else {
		for (const auto &entry : fs::directory_iterator(searchPath)) {
			if (entry.is_regular_file() && entry.path().extension() == ".json")
				paths.push_back(entry.path().string());
		}
	}
#ifndef NDEBUG
	for (const auto &path : paths) {
		LOG_DEBUG("retrieveList: path {}", path);
	}
#else
	LOG_INFO("retrieving list of files at {}", path.string());
#endif
	return {{"paths", paths}};
}

JSON getFile(const JSON &json) {
	const fs::path path{json.at("path").get<fs::path>()};

	if (path.empty())
		throw std::runtime_error{"path is empty"};
	if (auto filename{path.filename()}; filename.empty() || filename.extension() != ".json")
		throw std::runtime_error{"path is not a valid file"};

	std::ifstream file(path, std::ios::binary);
	if (!file)
		throw std::runtime_error("failed to open file: " + path.string());

	JSON j{};
	file >> j;

	LOG_INFO("retrieving file {}", path.string());
	return j;
}

JSON deleteFile(const JSON &json) {
	const fs::path path{json.at("path").get<fs::path>()};

	if (path.empty())
		throw std::runtime_error{"path is empty"};
	if (auto filename{path.filename()}; filename.empty() || filename.extension() != ".json")
		throw std::runtime_error{"path is not a valid file"};

	bool hasRemoved = fs::remove(path);

	LOG_INFO("deleting {}", path.string());
	return {{"hasRemoved", hasRemoved}};
}

JSON renameFile(const JSON &json) {
	const fs::path path{json.at("path").get<fs::path>()};
	const fs::path newPath{json.at("newPath").get<fs::path>()};

	if (path.empty())
		throw std::runtime_error{"path is empty"};
	if (newPath.empty())
		throw std::runtime_error{"newPath is empty"};
	if (auto filename{path.filename()}; filename.empty() || filename.extension() != ".json")
		throw std::runtime_error{"path is not a valid file"};

	fs::rename(path, newPath);
	LOG_INFO("renaming {} to {}", path.string(), newPath.string());
	return JSON::object();
}
} // namespace file
} // namespace ArmaToJSON
