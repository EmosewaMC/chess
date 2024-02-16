#ifndef __LOGGER__H__
#define __LOGGER__H__

#include <cstdint>
#include <format>
#include <string>
#include <source_location>

enum class LogLevel : uint32_t {
	CRITICAL = 0,
	Error, // Lowercase to avoid conflict with the macro ERROR
	WARNING,
	INFO
};

// Logs a message to the console and to a file with the given level, the message is not logged if the verbosity level is higher than the current logging level.
#define LOG(str, level, ...) do { GameGlobal::logger->Log(std::format(str, __VA_ARGS__), level, std::source_location::current()); } while (0)

class Logger {
public:

	/**
	 * @brief Construct a new Logger object and creates a file to log to.
	 * 
	 * @param name The file to log messages to.
	 * @param level The default verbosity of the logger.
	 */
	Logger(std::string_view name, LogLevel level = LogLevel::INFO);
	~Logger();

	// Old implementation.  Left here for reference.
	// template<typename... Args> // Variadic template / Parameter pack
	// void Log(const std::string& format, const LogLevel& level, const std::source_location location, Args&&... args) {
	// 	_Log(std::format(format, (args)...), level, location); // Fold expression
	// }

	// Logs a message to the log file.
	void Log(const std::string& message, const LogLevel& level, const std::source_location& location);

	// Flushes the log file.
	void Flush();

	// Sets the verbosity level of the logger.
	void SetLevel(const LogLevel& level) { m_Level = level; }
private:
	FILE* m_File = NULL;
	LogLevel m_Level;
	std::string m_Name;
};

#endif  //!__LOGGER__H__
