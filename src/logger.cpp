#include <logger.hpp>
#include <iostream>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

namespace ArmaToJSON {
namespace logger {
std::shared_ptr<spdlog::logger> getLogger() {
	static auto logger = spdlog::get("armatojson_logger");
	if (!logger) {
		auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
			"./btc_ArmaToJSON.log", 1048576 * 100, 0); // size is in binary MB
		try {
			logger = std::make_shared<spdlog::logger>("armatojson_logger", sink);
			logger->set_level(spdlog::level::debug);
			logger->flush_on(spdlog::level::debug);
			spdlog::register_logger(logger);
		} catch (const spdlog::spdlog_ex &ex) {
			std::cerr << "Log init failed: " << ex.what() << std::endl;
		}
	}

	return logger;
}

LogFlags operator|(LogFlags a, LogFlags b) {
	return static_cast<LogFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

LogFlags operator&(LogFlags a, LogFlags b) {
	return static_cast<LogFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

LogFlags operator~(LogFlags a) { return static_cast<LogFlags>(~static_cast<uint8_t>(a)); }
} // namespace logger
} // namespace ArmaToJSON