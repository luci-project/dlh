#pragma once

#include <dlh/stream/output.hpp>

class Log : public OutputStream<2048> {
 public:
	enum Level {
		NONE,
		FATAL,
		ERROR,
		WARNING,
		INFO,
		VERBOSE,
		DEBUG,
		TRACE
	};

	/*! \brief Default constructor  */
	Log() : OutputStream(2), severity(Level::VERBOSE), limit(Level::WARNING) {}

	/*! \brief change output */
	bool output(int fd);
	bool output(const char * file, bool truncate = true);

	/*! \brief Set severity limit (for displaying log messages) */
	void set(Level limit) { this->limit = limit > Level::TRACE ? Level::TRACE : limit; }

	/*! \brief check if message would be visible */
	bool visible(Level level) { return level <= limit; }

	/*! \brief Flush the buffer.
	 */
	void flush() override;

	/*! \brief New log entry */
	Log& entry(Level level, const char * file = nullptr, unsigned line = 0, const char * module = nullptr);

 private:
	/*! \brief Level of current message */
	Level severity;

	/*! \brief Current display levels */
	Level limit;
};

extern Log logger;

#define LOG logger

#ifndef __MODULE__
#define __MODULE_NAME__ nullptr
#else
#include <dlh/utils/macro.hpp>
#define __MODULE_NAME__ STR(__MODULE__)
#endif
#define LOG_LEVEL(LEVEL) if (LOG.visible(LEVEL)) LOG.entry(LEVEL, __BASE_FILE__, __LINE__, __MODULE_NAME__)

#define LOG_FATAL   LOG_LEVEL(Log::FATAL)
#define LOG_ERROR   LOG_LEVEL(Log::ERROR)
#define LOG_WARNING LOG_LEVEL(Log::WARNING)
#define LOG_INFO    LOG_LEVEL(Log::INFO)
#define LOG_VERBOSE LOG_LEVEL(Log::VERBOSE)
#define LOG_DEBUG   LOG_LEVEL(Log::DEBUG)
#define LOG_TRACE   LOG_LEVEL(Log::TRACE)
