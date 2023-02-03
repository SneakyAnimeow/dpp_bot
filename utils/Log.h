#ifndef LOG_H
#define LOG_H


#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>

#define LOG_INFO(...) spdlog::info(__VA_ARGS__); spdlog::get("file_logger")->info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__); spdlog::get("file_logger")->warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__); spdlog::get("file_logger")->error(__VA_ARGS__)
#define LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__); spdlog::get("file_logger")->critical(__VA_ARGS__)
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__); spdlog::get("file_logger")->debug(__VA_ARGS__)

/**
 * \brief Initializes the logger.
 * It is recommended to call this function before any other logging function.
 */
inline void InitLogger() {
    spdlog::daily_logger_mt("file_logger", "logs/log.txt", 2, 30);

    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_every(std::chrono::seconds(2));
}


#endif // LOG_H
