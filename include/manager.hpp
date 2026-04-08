#pragma once

#include "packet.hpp"

/// @namespace ArmaToJSON
/// @brief ArmaToJSON library namespace.
namespace ArmaToJSON {
/// @namespace packet
/// @brief Packet handling utilities for the ArmaToJSON library.
namespace packet_manager {

/// @brief Manages a packet and writes to output.
/// @param output The buffer to write to.
/// @param outputSize The maximum size for the output packet.
/// @param inPacket The input packet to manage.
void managePacketIO(char *const output, const size_t outputSize, const Packet &inPacket);

/// @brief Manages function calls on a packet.
/// @param inPacket The packet containing function name and parameters.
/// @param outPacket The output packet for results.
void managePacketFunction(const Packet &inPacket, Packet &outPacket, const size_t outputMaxSize);

/// @brief Writes to dest from src outputSize chars.
/// @param output Char* destination.
/// @param outPacket Src source.
/// @param outputMaxSize Amount of chars dest can hold.
void writeToOutput(char *const output, Packet& outPacket, const size_t outputMaxSize);

/// @brief Writes to dest from src outputSize chars.
/// @param output Char* destination.
/// @param input Src source.
/// @param outputMaxSize Amount of chars dest can hold.
void writeToOutput(char *const output, const std::string& input, const size_t outputMaxSize);
} // namespace packet_manager
} // namespace ArmaToJSON
