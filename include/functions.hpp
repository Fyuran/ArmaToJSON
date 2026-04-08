#pragma once
#include "packet.hpp"
#include <nlohmann/json.hpp>
#include <string>

/// @namespace ArmaToJSON
/// @brief ArmaToJSON library namespace.
namespace ArmaToJSON {
/// @brief Storage for packets string chunks keyed by identifier.
extern std::unordered_map<std::string, std::string> packetDataStorage;
/// @brief Function pointer type for packet operations.
using PacketFunction = void (*)(const Packet &inPacket, Packet *const outPacket);
/// @brief Map of function names to their implementations.
extern const std::unordered_map<std::string, PacketFunction> packetFunctions;

/// @brief Gets the current date and time as a string.
/// @return A string representing the current date and time.
std::string getCurrentDateTime();

namespace storage {
/// @brief Retrieves packet chunk from packetStorage.
/// @param inPacket The packet containing the chunk index.
/// @param outPacket The output packet to store the chunk.
void getChunk(char *const output, const Packet &inPacket, const size_t outputMaxSize);

/// @brief Chunks a packet into smaller strings that will be stored.
/// @param outPacket The input packet to manage.
/// @param outputSize The maximum size for the output packet.
// void chunkPacket(Packet& outPacket, const size_t outputMaxSize);

/// @brief Stores the packet in a global namespace container
/// @param outPacket The packet to store.
void storePacketData(Packet &outPacket, const size_t outputMaxSize);
} // namespace storage

namespace file {
/// @brief Copies a file from one path to another.
/// @param inPacket The packet containing source and destination paths.
/// @param outPacket Optional output packet.
void copyFile(const Packet &inPacket, Packet *const outPacket);

/// @brief Creates a new file with the specified content.
/// @param inPacket The packet containing file path and data.
/// @param outPacket Optional output packet.
void createFile(const Packet &inPacket, Packet *const outPacket);

/// @brief Retrieves a list of files or directories at the given path.
/// @param inPacket The packet containing the path.
/// @param outPacket The output packet to store the list.
void retrieveList(const Packet &inPacket, Packet *const outPacket);

/// @brief Retrieves a file at the given path.
/// @param inPacket The packet containing the path.
/// @param outPacket The output packet to store the file.
void getFile(const Packet &inPacket, Packet *const outPacket);

/// @brief Deletes a file at the specified path.
/// @param inPacket The packet containing the file path.
/// @param outPacket Optional output packet.
void deleteFile(const Packet &inPacket, Packet *const outPacket);

/// @brief Renames a file from one path to another.
/// @param inPacket The packet containing old and new paths.
/// @param outPacket Optional output packet.
void renameFile(const Packet &inPacket, Packet *const outPacket);
} // namespace file
} // namespace ArmaToJSON
