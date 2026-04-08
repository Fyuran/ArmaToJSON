#pragma once
#include <cstddef>

/// @brief Callback function type for Arma extension communication.
/// @param name The name parameter.
/// @param function The function name.
/// @param data The data string.
/// @return Integer result.
using armaCallback = int (*)(char const *name, char const *function, char const *data);

extern "C" {
/// @brief Main entry point for Arma extension calls.
/// @param output Buffer to store the output.
/// @param outputMaxSize Size of the output buffer.
/// @param data Input data from Arma.
void RVExtension(char *const output, const size_t outputMaxSize, const char *const data);

// void RVExtensionRegisterCallback(armaCallback);
// int RVExtensionArgs(char *output, std::size_t outputSize, const char *function, const char
// **argv, const std::size_t argc);
}
