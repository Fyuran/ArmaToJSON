#pragma once

/// @namespace ArmaToJSON
/// @brief ArmaToJSON library namespace.
#include <nlohmann/json_fwd.hpp>
#include <string_view>
#include <map>
#include <unordered_map>

namespace ArmaToJSON {
/// @namespace packet
/// @brief Packet handling utilities for the ArmaToJSON library.
namespace manager {

/// @brief Storage for packets string chunks keyed by identifier.
extern std::unordered_map<std::string, std::map<size_t, std::string>> chunkStorage;
/// @brief Function pointer type for packet operations.
using Function = nlohmann::json (*)(const nlohmann::json &json);
/// @brief Map of function names to their implementations.
extern const std::unordered_map<std::string, Function> packetFunctions;

std::string_view strip_quotes(const std::string_view s);

/// @brief Manages a packet and writes to output.
/// @param output The buffer to write to.
/// @param outputMaxSize The maximum size for the output packet.
/// @param data The input data to manage.
void manageIO(char *const output, const size_t outputMaxSize, const std::string_view data);

/// @brief Writes from src to dest, maximum outputSize chars.
/// @param output destination.
/// @param outputMaxSize Amount of chars output can hold.
/// @param json The incoming json.
void writeToOutput(char *const output, const size_t outputMaxSize, const std::string& input);
} // namespace manager
} // namespace ArmaToJSON
