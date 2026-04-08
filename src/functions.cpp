#include "logger.hpp"
#include "manager.hpp"
#include <boost/date_time.hpp>
#include <functions.hpp>

using JSON = nlohmann::json;
namespace fs = std::filesystem;
namespace ArmaToJSON {
std::unordered_map<std::string, std::string> packetDataStorage{};
const std::unordered_map<std::string, PacketFunction> packetFunctions = {
	{"copyFile", file::copyFile},		  {"createFile", file::createFile},
	{"retrieveList", file::retrieveList}, {"getFile", file::getFile},
	{"deleteFile", file::deleteFile},	  {"renameFile", file::renameFile}};

std::string getCurrentDateTime() {

	std::ostringstream ss;
	boost::posix_time::ptime time{boost::posix_time::second_clock::local_time()};

	auto time_facet = boost::posix_time::time_facet{"%d-%m-%Y %H-%M"};
	ss.imbue(std::locale(ss.getloc(), &time_facet));
	ss << time;

	LOG_DEBUG("retrieving time {}", ss.str());

	return ss.str();
}

namespace storage {
void storePacketData(Packet &outPacket, const size_t outputMaxSize) {
	const std::string dump{outPacket.dump()};
	auto storageID{Packet::getNewID()};
	packetDataStorage.emplace(storageID, dump);
	outPacket.clear(); // data has been stored, we don't need anything else fancy
	outPacket["storageID"] = storageID;

	size_t totalChunks{(dump.size() + outputMaxSize - 1) / outputMaxSize};
	outPacket["chunks"] = totalChunks;
}

void getChunk(char *const output, const Packet &inPacket, const size_t outputMaxSize) {
	if (packetDataStorage.empty())
		throw std::runtime_error{"storage is empty"};

	auto id{inPacket.value<std::string>("storageID", "")};
	auto requestChunk{inPacket.value<size_t>("requestChunk", 0)};
	auto chunks{inPacket.value<size_t>("chunks", 0)};

	if (!packetDataStorage.contains(id))
		throw std::runtime_error(
			fmt::format("ID: {} has not been found", id.empty() ? "EMPTY" : id));

	if (chunks == 0)
		throw std::runtime_error("chunks property is 0");

	if (requestChunk >= chunks)
		throw std::runtime_error(fmt::format("requestChunks: {} is above the maximum limit of {}",
											 requestChunk, chunks));

	std::string chunk{packetDataStorage.at(id).substr(requestChunk * outputMaxSize, outputMaxSize)};
	if (requestChunk == (chunks - 1)) //if last chunk, remove element to avoid memory leak
		packetDataStorage.erase(id);

	packet_manager::writeToOutput(output, chunk, outputMaxSize);
}

/* void chunkPacket(Packet &outPacket, const size_t outputMaxSize) {
	if (outPacket["function"] == "getChunk") {
		throw std::runtime_error(fmt::format("function of packet to be chunked cannot be "
											 "getChunk, size of chunk {}, outputMaxSize {}",
											 outPacket.size(), outputMaxSize));
	}
	std::map<size_t, std::string> chunks{};
	const std::string outPacketDump{outPacket.dump()};
	size_t totalChunks{(outPacketDump.size() + outputMaxSize - 1) / outputMaxSize};

	for (size_t i = 0; i < totalChunks; i++) {
		std::string chunk{outPacketDump.substr(i * outputMaxSize, outputMaxSize)};
		LOG_DEBUG("i * outputMaxSize: {} size {}, outputMaxSize: {}", i * outputMaxSize,
				  chunk.size(), outputMaxSize);
		assert(chunk.size() <= outputMaxSize);
		chunks.emplace(i, std::move(chunk));
	}

	auto storageID{Packet::getNewID()};
	outPacket.clear(); // whole json has been chunked, we don't need anything fancy
	outPacket["returnCode"] = 206;
	outPacket["storageID"] = storageID;
	outPacket["chunks"] = chunks.size();

	chunksStorage.emplace(storageID, std::move(chunks));
} */

} // namespace storage

namespace file {
void copyFile(const Packet &inPacket, Packet *const outPacket) {
	if (!outPacket)
		throw std::runtime_error{"outPacket is null"};

	const fs::path path{inPacket.value<fs::path>("path", "")};
	const fs::path newPath{inPacket.value<fs::path>("newPath", "")};

	if (path.empty())
		throw std::runtime_error{"path is empty"};
	if (newPath.empty())
		return;
	if (auto filename{path.filename()}; filename.empty() || filename.extension() != ".json")
		throw std::runtime_error{"path is not a valid file"};

	bool hasCopied = fs::copy_file(path, newPath, fs::copy_options::overwrite_existing);
	LOG_DEBUG("copying file {}", path.string());
}

void createFile(const Packet &inPacket, Packet *const outPacket) {
	if (!outPacket)
		throw std::runtime_error{"outPacket is null"};

	const fs::path path{inPacket.value<fs::path>("path", "")};
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

	file << inPacket;

	LOG_DEBUG("creating file {}", path.string());
}

void retrieveList(const Packet &inPacket, Packet *const outPacket) {
	if (!outPacket)
		throw std::runtime_error{"outPacket is null"};

	const fs::path path{inPacket.value<fs::path>("path", "")};
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
#endif
	outPacket->setData(paths);
}

void getFile(const Packet &inPacket, Packet *const outPacket) {
	if (!outPacket)
		throw std::runtime_error{"outPacket is null"};

	const fs::path path{inPacket.value<fs::path>("path", "")};

	if (path.empty())
		throw std::runtime_error{"path is empty"};
	if (auto filename{path.filename()}; filename.empty() || filename.extension() != ".json")
		throw std::runtime_error{"path is not a valid file"};

	std::ifstream file(path, std::ios::binary);
	if (!file)
		throw std::runtime_error("failed to open file: " + path.string());

	JSON j;
	file >> j;
	outPacket->setData(j);
}

void deleteFile(const Packet &inPacket, Packet *const outPacket) {
	if (!outPacket)
		throw std::runtime_error{"outPacket is null"};

	const fs::path path{inPacket.value<fs::path>("path", "")};

	if (path.empty())
		throw std::runtime_error{"path is empty"};
	if (auto filename{path.filename()}; filename.empty() || filename.extension() != ".json")
		throw std::runtime_error{"path is not a valid file"};

	fs::remove(path);
	LOG_DEBUG("deleting {}", path.string());
}

void renameFile(const Packet &inPacket, Packet *const outPacket) {
	if (!outPacket)
		throw std::runtime_error{"outPacket is null"};

	const fs::path path{inPacket.value<fs::path>("path", "")};
	const fs::path newPath{inPacket.value<fs::path>("newPath", "")};

	if (path.empty())
		throw std::runtime_error{"path is empty"};
	if (newPath.empty())
		return;
	if (auto filename{path.filename()}; filename.empty() || filename.extension() != ".json")
		throw std::runtime_error{"path is not a valid file"};

	fs::rename(path, newPath);
	LOG_DEBUG("renaming {} to {}", path.string(), newPath.string());
}
} // namespace file
} // namespace ArmaToJSON
