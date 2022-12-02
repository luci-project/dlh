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
	 : year(year), month(month), day(day), weekday(0), hour(hour), minute(minute), second(second), nanosecond(nanosecond) { fix(); }

	/*! \brief Construct from a timestamp (Unix epoch time)
	 */
	DateTime(time_t epoch, long int nanosecond = 0) { fromTimestamp(epoch, nanosecond); }

	/*! \brief Construct from a timespec struct
	 */
	DateTime(const timespec & ts) { fromTimespec(ts); }

	/*! \brief Get name of weekday */
	const char * weekdayName(bool abbrev = false) const;

	/*! \brief Get name of month */
	const char * monthName(bool abbrev = false) const;

	/*! \brief Assign from Unix epoch time
	 */
	void fromTimestamp(time_t epoch, long int nanosecond = 0);

	/*! \brief Assign from Unix timespec
	 */
	void fromTimespec(const timespec & ts) { fromTimestamp(ts.tv_sec, ts.tv_nsec); };

	/*! \brief Convert to Unix epoch time
	 * \return Seconds since 1 January 1970
	 */
	time_t toTimestamp() const;

	/*! \brief Convert to Unix timespec (including nanosecond)
	 */
	timespec toTimespec() const {
		timespec ts{ toTimestamp(), nanosecond };
		return ts;
	}

	/*! \brief Get current time
	 */
	bool now();

	/*! \brief Check if date/time is valid
	 */
	bool valid() const;

	/*! \brief Fix date/time (in case of manual changes)
	 */
	void fix();


	DateTime& operator=(const time_t t) {
		fromTimestamp(t);
		return *this;
	}

	DateTime& operator=(const timespec & ts) {
		fromTimespec(ts);
		return *this;
	}

	DateTime& operator=(const DateTime & other) {
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
		return nanosecond == 0 && toTimestamp() == other;
	}

	bool operator==(const timespec & other) const {
		auto ts = toTimespec();
		return ts.tv_sec == other.tv_sec && ts.tv_nsec == other.tv_nsec;
	}

	inline bool operator==(const DateTime & other) const {
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
		auto ts = toTimespec();
		return ts.tv_sec < other.tv_sec || (ts.tv_sec == other.tv_sec && ts.tv_nsec < other.tv_nsec);
	}

	bool operator<(const time_t & other) const {
		timespec other_ts = { other, 0 };
		return operator<(other_ts);
	}

	bool operator<(const DateTime & other) const {
		return operator<(other.toTimespec());
	}

	template<typename T>
	bool operator>=(const T & other) const {
		return !operator<(other);
	}

	bool operator<=(const timespec & other) const {
		auto ts = toTimespec();
		return ts.tv_sec < other.tv_sec || (ts.tv_sec == other.tv_sec && ts.tv_nsec <= other.tv_nsec);
	}

	bool operator<=(const time_t & other) const {
		timespec other_ts = { other, 0 };
		return operator<(other_ts);
	}

	bool operator<=(const DateTime & other) const {
		return operator<(other.toTimespec());
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
