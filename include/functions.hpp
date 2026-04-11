#pragma once
#include <nlohmann/json_fwd.hpp>
#include <string>

/// @namespace ArmaToJSON
/// @brief ArmaToJSON library namespace.
namespace ArmaToJSON {
namespace storage {
extern size_t ID;
const std::string createNewID();
nlohmann::json storeData(const nlohmann::json &json, const size_t outputMaxSize);

/// @brief Retrieves packet chunk from packetStorage.
/// @param inPacket The packet containing the chunk index.
/// @param outPacket The output packet to store the chunk.
void getChunk(char *const output, const size_t outputMaxSize, const nlohmann::json &json);

} // namespace storage

namespace file {
/// @brief Copies a file from one path to another.
/// @param inPacket The packet containing source and destination paths.
/// @param outPacket Optional output packet.
nlohmann::json copyFile(const nlohmann::json &json);

/// @brief Creates a new file with the specified content.
/// @param inPacket The packet containing file path and data.
/// @param outPacket Optional output packet.
nlohmann::json createFile(const nlohmann::json &json);

/// @brief Retrieves a list of files or directories at the given path.
/// @param inPacket The packet containing the path.
/// @param outPacket The output packet to store the list.
nlohmann::json retrieveList(const nlohmann::json &json);

/// @brief Retrieves a file at the given path.
/// @param inPacket The packet containing the path.
/// @param outPacket The output packet to store the file.
nlohmann::json getFile(const nlohmann::json &json);

/// @brief Deletes a file at the specified path.
/// @param inPacket The packet containing the file path.
/// @param outPacket Optional output packet.
nlohmann::json deleteFile(const nlohmann::json &json);

/// @brief Renames a file from one path to another.
/// @param inPacket The packet containing old and new paths.
/// @param outPacket Optional output packet.
nlohmann::json renameFile(const nlohmann::json &json);

} // namespace file
} // namespace ArmaToJSON
