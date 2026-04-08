#pragma once
/// @brief Closes the library handle.
/// @param handle The handle to close.
#include <dlfcn.h>
#include <stdexcept>

void closeHandle(void *handle);

/// @brief Gets the library handle.
/// @return The library handle.
void *getHandle();

/// @brief Gets a function pointer from the library.
/// @tparam Fnc The function type.
/// @param handle The library handle.
/// @param fncName The function name.
/// @return The function pointer.
/// @throws std::runtime_error If the function is not found.
template <typename Fnc> Fnc getFunction(void *const handle, const std::string &fncName) {
	Fnc fnc{nullptr};
#ifdef _WIN32
	FARPROC fncAddress{GetProcAddress(reinterpret_cast<HMODULE>(handle), fncName.c_str())};
#elif __linux__
	void *fncAddress{dlsym(handle, fncName.c_str())};
#endif
	fnc = reinterpret_cast<Fnc>(fncAddress);
	if (!fnc) {
		closeHandle(handle);
		throw std::runtime_error("fnc is null");
	}
	return fnc;
}