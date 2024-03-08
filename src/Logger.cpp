#include "Logger.h"

#include "GameGlobal.h"

Logger::Logger(std::string_view name, LogLevel level) : m_Name(name), m_Level(level) {
	m_File = fopen(name.data(), "w");
	if (!m_File) {
		throw std::runtime_error("Failed to open file " + std::string(name) + " for writing");
	}
}

Logger::~Logger() {
	LOG("Logger {0} destroyed", LogLevel::INFO, m_Name);
	if (m_File) {
		Flush();
		fclose(m_File);
		m_File = NULL;
	}
}

void Logger::Log(const std::string& message, const LogLevel& level, const std::source_location& location) {
	if (level > m_Level || !m_File) return;

	std::string_view levelString;
	switch (level) {
	case LogLevel::INFO:
		levelString = "INFO";
		break;
	case LogLevel::WARNING:
		levelString = "WARNING";
		break;
	case LogLevel::Error:
		levelString = "ERROR";
		break;
	case LogLevel::CRITICAL:
		levelString = "CRITICAL";
		break;
	default:
		break;
	}

	std::string_view file = location.file_name();

	// Only care about the file name and line number
	size_t pos = file.rfind('\\');
	if (pos != std::string::npos) {
		file.remove_prefix(pos + 1);
	}

	std::string_view function = location.function_name();
	uint32_t line = location.line();

	time_t t = std::time(NULL);
	struct tm* time = std::localtime(&t);
	char timeStr[70];
	strftime(timeStr, sizeof(timeStr), "%d-%m-%y %H:%M:%S", time);

	std::string formatted(std::format("{0}\t{1}\t\t{2}:{3}\t{4}\n", timeStr, levelString, file, line, message));
	fputs(formatted.c_str(), m_File);
	puts(formatted.c_str());
}

void Logger::Flush() {
	if (m_File) fflush(m_File);
}
