// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/stream/output.hpp>
#include <dlh/syscall.hpp>

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

	enum Time {
		DISABLED,
		ABSOLUTE,
		DELTA
	};

	/*! \brief Default constructor  */
	Log() : OutputStream(2), severity(Level::VERBOSE), limit(Level::WARNING), time_mode(DISABLED), time_start(0) {}

	/*! \brief change output */
	bool output(int fd);
	bool output(const char * file, bool truncate = true);

	/*! \brief Set severity limit (for displaying log messages) and start time for delta */
	void set(Level limit, Time mode = DISABLED);

	/*! \brief Get severity limit (for displaying log messages) */
	Level get() { return this->limit; }

	/*! \brief check if message would be visible */
	bool visible(Level level) { return level <= limit; }

	/*! \brief Flush the buffer.
	 */
	void flush() override;

	/*! \brief New log entry */
	Log& entry(Level level, const char * file = nullptr, unsigned line = 0, const char * module = nullptr);

	/*! \brief Append to log entry */
	Log& append() { return *this; }

 private:
	/*! \brief Level of current message */
	Level severity;

	/*! \brief Current display levels */
	Level limit;

	/*! \brief Time for delta */
	enum Time time_mode;
	time_t time_start;
};

extern Log logger;

#define LOG logger

#ifndef __MODULE__
#define __MODULE_NAME__ nullptr
#else
#include <dlh/macro.hpp>
#define __MODULE_NAME__ STR(__MODULE__)
#endif
#define LOG_LEVEL(LEVEL) if (LOG.visible(LEVEL)) LOG.entry(LEVEL, __BASE_FILE__, __LINE__, __MODULE_NAME__)
#define LOG_LEVEL_APPEND(LEVEL) if (LOG.visible(LEVEL)) LOG.append()

#define LOG_FATAL   LOG_LEVEL(Log::FATAL)
#define LOG_ERROR   LOG_LEVEL(Log::ERROR)
#define LOG_WARNING LOG_LEVEL(Log::WARNING)
#define LOG_INFO    LOG_LEVEL(Log::INFO)
#define LOG_VERBOSE LOG_LEVEL(Log::VERBOSE)
#define LOG_DEBUG   LOG_LEVEL(Log::DEBUG)
#define LOG_TRACE   LOG_LEVEL(Log::TRACE)

#define LOG_FATAL_APPEND   LOG_LEVEL_APPEND(Log::FATAL)
#define LOG_ERROR_APPEND   LOG_LEVEL_APPEND(Log::ERROR)
#define LOG_WARNING_APPEND LOG_LEVEL_APPEND(Log::WARNING)
#define LOG_INFO_APPEND    LOG_LEVEL_APPEND(Log::INFO)
#define LOG_VERBOSE_APPEND LOG_LEVEL_APPEND(Log::VERBOSE)
#define LOG_DEBUG_APPEND   LOG_LEVEL_APPEND(Log::DEBUG)
#define LOG_TRACE_APPEND   LOG_LEVEL_APPEND(Log::TRACE)
