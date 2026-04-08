#pragma once

#include <spdlog/logger.h>
// remove windows shit ass macros
#undef DEBUG
#undef INFO
#undef WARN
#undef ERROR

/// @brief Warning log macro.
/// @param ... Variable arguments for logging.
#define LOG_WARN(...) ArmaToJSON::logger::getLogger()->warn(__VA_ARGS__)

/// @brief Error log macro.
/// @param ... Variable arguments for logging.
#define LOG_ERROR(...) ArmaToJSON::logger::getLogger()->error(__VA_ARGS__)

/// @brief Info log macro.
/// @param ... Variable arguments for logging.
#define LOG_INFO(...) ArmaToJSON::logger::getLogger()->info(__VA_ARGS__)

#ifndef NDEBUG
/// @brief Debug log macro (enabled in debug builds).
/// @param ... Variable arguments for logging.
#define LOG_DEBUG(...) ArmaToJSON::logger::getLogger()->debug(__VA_ARGS__)
#else
/// @brief Debug log macro (disabled in release builds).
#define LOG_DEBUG(...) (void)0
#endif

/// @brief Conditional log macro based on flags.
/// @param flag The log flags to check.
/// @param ... Variable arguments for logging.
#define LOG(flag, ...)                                                                             \
	if ((flag & ArmaToJSON::logger::LogFlags::DEBUG) != ArmaToJSON::logger::LogFlags::NONE)                        \
		LOG_DEBUG(__VA_ARGS__);                                                                    \
	if ((flag & ArmaToJSON::logger::LogFlags::INFO) != ArmaToJSON::logger::LogFlags::NONE)                         \
		LOG_INFO(__VA_ARGS__);                                                                     \
	if ((flag & ArmaToJSON::logger::LogFlags::WARN) != ArmaToJSON::logger::LogFlags::NONE)                         \
		LOG_WARN(__VA_ARGS__);                                                                     \
	if ((flag & ArmaToJSON::logger::LogFlags::ERROR) != ArmaToJSON::logger::LogFlags::NONE)                        \
		LOG_ERROR(__VA_ARGS__);

/// @namespace ArmaToJSON
/// @brief ArmaToJSON library namespace.
namespace ArmaToJSON {
namespace logger {
/// @brief Enumeration of log flags.
enum class LogFlags : uint8_t {
	NONE = 0,		///< No logging.
	DEBUG = 1 << 0, ///< Debug level logging.
	INFO = 1 << 1,	///< Info level logging.
	WARN = 1 << 2,	///< Warning level logging.
	ERROR = 1 << 3	///< Error level logging.
};

/// @brief Bitwise OR operator for LogFlags.
/// @param a First flag.
/// @param b Second flag.
/// @return Combined flags.
LogFlags operator|(LogFlags a, LogFlags b);

/// @brief Bitwise AND operator for LogFlags.
/// @param a First flag.
/// @param b Second flag.
/// @return Intersection of flags.
LogFlags operator&(LogFlags a, LogFlags b);

/// @brief Bitwise NOT operator for LogFlags.
/// @param a Flag to invert.
/// @return Inverted flags.
LogFlags operator~(LogFlags a);

/// @brief Gets the shared logger instance.
/// @return Shared pointer to the spdlog logger.
std::shared_ptr<spdlog::logger> getLogger();
} // namespace logger
} // namespace ArmaToJSON