#pragma once>

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

struct Logger {
	Logger() = default;
	~Logger() = default;

	static inline std::shared_ptr<spdlog::logger> logger = nullptr;

	static inline void initialize(const char* loggerName) {
		logger = spdlog::stdout_color_mt(loggerName);
		spdlog::set_pattern("%^[%Y-%m-%d %H:%M:%S] [%l] %v%$");
		spdlog::set_level(spdlog::level::trace);
	}

	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;
	Logger(Logger&&) = delete;
	Logger& operator=(Logger&&) = delete;
};

#define LOG_TRACE(...) Logger::logger->trace(__VA_ARGS__)
#define LOG_DEBUG(...) Logger::logger->debug(__VA_ARGS__)
#define LOG_INFO(...)  Logger::logger->info(__VA_ARGS__)
#define LOG_WARN(...)  Logger::logger->warn(__VA_ARGS__)
#define LOG_ERROR(...) Logger::logger->error(__VA_ARGS__)
#define LOG_FATAL(...) Logger::logger->critical(__VA_ARGS__)