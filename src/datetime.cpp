#include <dlh/datetime.hpp>

#include <dlh/syscall.hpp>
#include <dlh/math.hpp>

static const long int ns_limit = 1000000000L;

// inspired by
// https://codereview.stackexchange.com/questions/38275/convert-between-date-time-and-time-stamp-without-using-standard-library-routines
static const unsigned days[4][12] = {
	{    0,   31,   59,   90,  120,  151,  181,  212,  243,  273,  304,  334},
	{  365,  396,  424,  455,  485,  516,  547,  577,  608,  638,  669,  699},
	{  730,  761,  790,  821,  851,  882,  912,  943,  974, 1004, 1035, 1065},  // Leap Year
	{ 1096, 1127, 1155, 1186, 1216, 1247, 1277, 1308, 1339, 1369, 1400, 1430},
};

static const unsigned days4y = 365 * 4 + 1;

static const char * const weekdays[12][2] =	{
	{ "Sun", "Sunday" },
	{ "Mon", "Monday" },
	{ "Tue", "Tuesday" },
	{ "Wed", "Wednesday" },
	{ "Thu", "Thursday" },
	{ "Fri", "Friday" },
	{ "Sat", "Saturday" }
};

static const char * const months[12][2] =	{
	{ "Jan", "January" },
	{ "Feb", "February" },
	{ "Mar", "March" },
	{ "Apr", "April" },
	{ "May", "May" },
	{ "Jun", "June" },
	{ "Jul", "July" },
	{ "Aug", "August" },
	{ "Sep", "September" },
	{ "Oct", "October" },
	{ "Nov", "November" },
	{ "Dec", "December" }
};

void DateTime::fromTimestamp(time_t epoch, long int nanosecond) {
	if (nanosecond != 0) {
		epoch += nanosecond / ns_limit;
		nanosecond %= ns_limit;
		if (nanosecond < 0) {
			epoch -= 1;
			nanosecond += ns_limit;
		}
	}
	this->nanosecond = nanosecond;

	second = epoch % 60;
	epoch /= 60;
	minute = epoch % 60;
	epoch /= 60;
	hour   = epoch % 24;
	epoch /= 24;

	weekday = ((epoch + 4) % 7) + 1;

	unsigned int y = 1970 + epoch / days4y * 4;
	epoch %= days4y;

	for (year = 3; year > 0 && epoch < days[year][0]; year--) {}
	for (month = 11; month > 0 && epoch < days[year][month]; month--) {}

	day = epoch - days[year][month] + 1;
	month++;
	year += y;
}

const char * DateTime::weekdayName(bool abbrev) const {
	return weekdays[Math::range(weekday, 1, 7) - 1][abbrev ? 0 : 1];
}

const char * DateTime::monthName(bool abbrev) const {
	return months[Math::range(month, 1, 12) - 1][abbrev ? 0 : 1];
}

time_t DateTime::toTimestamp() const {
	unsigned y = year - 1970;
	unsigned m = month - 1;
	unsigned d = (y / 4) * days4y + days[y % 4][m] + day - 1;
	return ((d * 24 + hour) * 60 + minute) * 60 + second;
}

bool DateTime::now() {
	timespec ts;
	if (auto gettime = Syscall::clock_gettime(CLOCK_REALTIME, &ts)) {
		fromTimespec(ts);
		return true;
	} else {
		return false;
	}
}

bool DateTime::valid()  const {
	DateTime other(toTimespec());
	return year >= 2000 && year <= 2099 && operator==(other);
}

void DateTime::fix() {
	auto ts = toTimespec();
	fromTimespec(ts);
}
