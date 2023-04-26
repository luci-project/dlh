// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/container/optional.hpp>
#include <dlh/container/vector.hpp>
#include <dlh/types.hpp>

namespace Parser {

bool string(unsigned long long & target, const char * value);

bool string(long long & target, const char * value);

bool string(unsigned long & target, const char * value);

bool string(long & target, const char * value);

bool string(unsigned int & target, const char * value);

bool string(int & target, const char * value);

bool string(unsigned short & target, const char * value);

bool string(short & target, const char * value);

bool string(unsigned char & target, const char * value);

bool string(char & target, const char * value);

bool string(bool & target, const char * value);

bool string(const char * & target, const char * value);

template<typename T>
bool string(Vector<T> & target, const char * value) {
	T tmp;
	bool r = string(tmp, value);
	target.push_back(tmp);
	return r;
}

template<typename T>
Optional<T> string_as(const char * value) {
	T tmp;
	if (string(tmp, value))
		return Optional<T>{tmp};
	else
		return Optional<T>{};
}
}  // namespace Parser
