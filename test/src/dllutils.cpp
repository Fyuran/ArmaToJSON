#include "dllutils.hpp"
#include "logger.hpp"

/// @brief Closes the library handle.
/// @param handle The handle to close.
void closeHandle(void *handle) {
	if (!handle) {
		LOG_ERROR("No handle found");
	}
#ifdef _WIN32
	CloseHandle(handle);
#elif __linux__
	dlclose(handle);
#endif

	LOG_DEBUG("Closing handle");
}

/// @brief Gets the library handle.
/// @return The library handle.
/// @throws std::runtime_error If the handle is null.
void *getHandle() {
	LOG_DEBUG("Opening handle");
	static void *handle = nullptr;
#ifdef _WIN32
	handle = LoadLibraryA(".\\btc_ArmaToJSON_x64");
#elif __linux__
	handle = dlopen("./btc_ArmaToJSON_x64.so", RTLD_LAZY);
#endif
	if (!handle) {
		throw std::runtime_error("handle is null");
	}
	return handle;
}