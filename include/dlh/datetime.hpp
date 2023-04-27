// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/types.hpp>
#include <dlh/systypes.hpp>

/*! \brief Date and Time
 *
 * \note only Dates between 1 January 2000 and 31 December 2099 are supported
 */
struct DateTime {
	uint16_t year = 2000;  ///< Year (`0` (2000) - `99` (2099))
	uint8_t month = 1;     ///< Month (`1` - `12`)
	uint8_t day = 1;       ///< Day of Month (`1` - `31`)
	uint8_t weekday = 7;   ///< Weekday (`1` (Sun) - `7` (Sat))
	uint8_t hour = 0;      ///< Hour (`0` - `23`)
	uint8_t minute = 0;    ///< Minute (`0` - `59`)
	uint8_t second = 0;    ///< Second (`0` - `59`)
	long nanosecond = 0;   ///< nanosecond (`0` - `999999999`)


	/*! \brief Default constructor
	 */
	DateTime() {}

	DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour = 0, uint8_t minute = 0, uint8_t second = 0, long nanosecond = 0)
	 : year(year), month(month), day(day), weekday(0), hour(hour), minute(minute), second(second), nanosecond(nanosecond) {
		fix();
	}

	/*! \brief Construct from a timestamp (Unix epoch time)
	 */
	explicit DateTime(time_t epoch, long int nanosecond = 0) {
		from_timestamp(epoch, nanosecond);
	}

	/*! \brief Construct from a timespec struct
	 */
	explicit DateTime(const timespec & ts) {
		from_timespec(ts);
	}

	/* Copy constructor */
	constexpr DateTime(const DateTime & other)  = default;
	constexpr DateTime(DateTime && other) = default;

	/*! \brief Get name of weekday */
	const char * weekday_name(bool abbrev = false) const;

	/*! \brief Get name of month */
	const char * month_name(bool abbrev = false) const;

	/*! \brief Assign from Unix epoch time
	 */
	void from_timestamp(time_t epoch, long int nanosecond = 0);

	/*! \brief Assign from Unix timespec
	 */
	void from_timespec(const timespec & ts) {
		from_timestamp(ts.tv_sec, ts.tv_nsec);
	}

	/*! \brief Convert to Unix epoch time
	 * \return Seconds since 1 January 1970
	 */
	time_t to_timestamp() const;

	/*! \brief Convert to Unix timespec (including nanosecond)
	 */
	timespec to_timespec() const {
		timespec ts{ to_timestamp(), nanosecond };
		return ts;
	}

	/*! \brief Set to current time
	 */
	void to_current_time();

	/*! \brief Get current time
	 */
	static DateTime now();

	/*! \brief Check if date/time is valid
	 */
	bool valid() const;

	/*! \brief Fix date/time (in case of manual changes)
	 */
	void fix();


	DateTime& operator=(const time_t t) {
		from_timestamp(t);
		return *this;
	}

	DateTime& operator=(const timespec & ts) {
		from_timespec(ts);
		return *this;
	}

	constexpr DateTime& operator=(const DateTime & other) {
		this->nanosecond = other.nanosecond;
		this->second = other.second;
		this->minute = other.minute;
		this->hour = other.hour;
		this->weekday = other.weekday;
		this->day = other.day;
		this->month = other.month;
		this->year = other.year;
		return *this;
	}

	/* comparison */
	bool operator==(const time_t & other) const {
		return nanosecond == 0 && to_timestamp() == other;
	}

	bool operator==(const timespec & other) const {
		auto ts = to_timespec();
		return ts.tv_sec == other.tv_sec && ts.tv_nsec == other.tv_nsec;
	}

	bool operator==(const DateTime & other) const {
		return this->nanosecond == other.nanosecond
		    && this->second == other.second
		    && this->minute == other.minute
		    && this->hour == other.hour
		    && this->weekday == other.weekday
		    && this->day == other.day
		    && this->month == other.month
		    && this->year == other.year;
	}

	template<typename T>
	bool operator!=(const T & other) const {
		return !operator==(other);
	}


	bool operator<(const timespec & other) const {
		auto ts = to_timespec();
		return ts.tv_sec < other.tv_sec || (ts.tv_sec == other.tv_sec && ts.tv_nsec < other.tv_nsec);
	}

	bool operator<(const time_t & other) const {
		timespec other_ts = { other, 0 };
		return operator<(other_ts);
	}

	bool operator<(const DateTime & other) const {
		return operator<(other.to_timespec());
	}

	template<typename T>
	bool operator>=(const T & other) const {
		return !operator<(other);
	}

	bool operator<=(const timespec & other) const {
		auto ts = to_timespec();
		return ts.tv_sec < other.tv_sec || (ts.tv_sec == other.tv_sec && ts.tv_nsec <= other.tv_nsec);
	}

	bool operator<=(const time_t & other) const {
		timespec other_ts = { other, 0 };
		return operator<(other_ts);
	}

	bool operator<=(const DateTime & other) const {
		return operator<(other.to_timespec());
	}


	template<typename T>
	bool operator>(const T & other) const {
		return !operator<=(other);
	}
};

// Print date conform to RFC 3339 / ISO 8601
template<typename T>
T& operator<<(T &out, const DateTime &dt) {
	out.format("%04u-%02u-%02uT%02u:%02u:%02u.%06uZ", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second, dt.nanosecond / 1000);
	return out;
}
