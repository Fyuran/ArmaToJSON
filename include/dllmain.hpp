#pragma once
#include <cstddef>

/// @brief Callback function type for Arma extension communication.
/// @param name Make it unique name, for example the extension name, so that other modders can quickly filter out calls from own extensions.
/// @param function  Make it name of the function the extension sends the result to. (Note: The returned function is just a STRING! So compile is needed, before using call or spawn, to execute it)
/// @param data Make it the actual result. You can also format it as an array so it could be parsed by parseSimpleArray
/// @return This is the number of available slots in the input buffer left for this frame after your call and can range from 99 to -1.
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
