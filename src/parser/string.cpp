// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/parser/string.hpp>
#include <dlh/assert.hpp>
#include <dlh/types.hpp>

namespace Parser {

bool string(unsigned long long & target, const char * value) {
	if (value == nullptr)
		return false;

	for (; *value == ' '; value++) {}
	if (*value == '+') {
		value++;
		for (; *value == ' '; value++) {}
	}

	unsigned long long base = 10;
	if (value[0] == '0' && value[1] != '\0') {
		if (value[1] == 'x' || value[1] == 'X') {
			base = 16;
			value += 2;
		} else if (value[1] == 'b' || value[1] == 'B') {
			base = 2;
			value += 2;
		} else if (value[1] >= '1' && value[1] <= '7') {
			base = 8;
			value++;
		} else {
			// Invalid format
			return false;
		}
	}

	unsigned long long n = 0;
	char spacer = '\0';
	for (; *value != '\0'; value++) {
		if (*value == ' ' || *value == ',' || *value == '\'') {
			// Invalid space char
			if (spacer != '\0' && spacer != *value)
				return false;
			spacer = *value;
			continue;
		}
		unsigned long long v = 0;
		if (*value >= '0' && *value <= '9') {
			v = *value - '0';
		} else if (*value >= 'a' && *value <= 'f') {
			v = *value - 'a' + 10;
		} else if (*value >= 'A' && *value <= 'F') {
			v = *value - 'A' + 10;
		} else {
			// Invalid character
			return false;
		}

		if (v > base || n * base + v < n) {
			return false;
		}

		n = n * base + v;
	}

	target = n;

	return true;
}

bool string(long long & target, const char * value) {
	if (value == nullptr)
		return false;

	long long sign = 1;
	for (; *value != '\0'; value++)
		if (*value == '-')  {
			sign = -1;
			value++;
			break;
		} else if (*value == '+') {
			value++;
			break;
		} else if (*value >= '0' && *value <= '9') {
			break;
		} else if (*value != ' ') {
			return false;
		}

	unsigned long long v = 0;
	if (string(v, value)) {
		target = static_cast<long long>(sign * v);
		return (sign < 0 && v <= __LONG_LONG_MAX__ + 1ULL) || (sign >= 0 && v <= __LONG_LONG_MAX__);
	}
	return false;
}

bool string(unsigned long & target, const char * value) {
	unsigned long long v = 0;
	bool r = string(v, value);
	target = static_cast<unsigned long>(v);
	return v > static_cast<unsigned long>(-1) ? false : r;
}

bool string(long & target, const char * value) {
	long long v = 0;
	bool r = string(v, value);
	target = static_cast<long>(v);
	return v > __LONG_MAX__ || v < (-__LONG_MAX__ - 1) ? false : r;
}

bool string(unsigned int & target, const char * value) {
	unsigned long long v = 0;
	bool r = string(v, value);
	target = static_cast<unsigned int>(v);
	return v > static_cast<unsigned int>(-1) ? false : r;
}

bool string(int & target, const char * value) {
	long long v = 0;
	bool r = string(v, value);
	target = static_cast<int>(v);
	return v > __INT_MAX__ || v < (-__INT_MAX__ - 1) ? false : r;
}

bool string(unsigned short & target, const char * value) {
	unsigned long long v = 0;
	bool r = string(v, value);
	target = static_cast<unsigned short>(v);
	return v > static_cast<unsigned short>(-1) ? false : r;
}

bool string(short & target, const char * value) {
	long long v = 0;
	bool r = string(v, value);
	target = static_cast<short>(v);
	return v > __SHRT_MAX__ || v < (-__SHRT_MAX__ - 1) ? false : r;
}

bool string(unsigned char & target, const char * value) {
	unsigned long long v = 0;
	bool r = string(v, value);
	target = static_cast<unsigned char>(v);
	return v > static_cast<unsigned char>(-1) ? false : r;
}

bool string(char & target, const char * value) {
	long long v = 0;
	bool r = string(v, value);
	target = static_cast<char>(v);
	return v > __SCHAR_MAX__ || v < (-__SCHAR_MAX__ - 1) ? false : r;
}

bool string(bool & target, const char * value) {
	if (value == nullptr)
		return false;
	target = (*value != '\0' && *value != '0' && *value != 'n' && *value != 'N' && *value != 'f' && *value != 'F');
	return true;
}

bool string(const char * & target, const char * value) {
	if (value == nullptr)
		return false;
	target = value;
	return true;
}

}  // namespace Parser
