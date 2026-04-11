#include "dllmain.hpp"
#include "logger.hpp"
#include "manager.hpp"

#if defined(_MSC_VER) && !defined(__GNUC__)
// Pure MSVC
#pragma section(".CRT$XCU", read)
#pragma section(".CRT$XPU", read)

#define CONSTRUCTOR(f)                                                                             \
	static void f();                                                                               \
	__declspec(allocate(".CRT$XCU")) void (*f##_ptr)() = f;                                        \
	static void f()

#define DESTRUCTOR(f)                                                                              \
	static void f();                                                                               \
	__declspec(allocate(".CRT$XPU")) void (*f##_ptr)() = f;                                        \
	static void f()

#elif defined(__GNUC__)
// GCC or MinGW
#define CONSTRUCTOR(f) static void __attribute__((constructor)) f()

#define DESTRUCTOR(f) static void __attribute__((destructor)) f()

#else
#error "Unsupported compiler"
#endif

/* CONSTRUCTOR(init) {
	LOG_INFO("Initializing ArmaToJSON_x64");
}
DESTRUCTOR(end) {
	LOG_INFO("Deconstructing ArmaToJSON_x64");
} */

void RVExtension(char *const output, const size_t outputMaxSize, const char *const data) {
	LOG_DEBUG("RVExtension called with {} out of {} bytes of data", std::string_view{data}.size(), outputMaxSize);
	ArmaToJSON::manager::manageIO(output, outputMaxSize - 1, std::string_view{data});
}
