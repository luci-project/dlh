// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/comparison.hpp>

template<typename T, typename C = Comparison>
class is {
	T value;

 public:
	explicit constexpr is(T value) : value(value) {}

#if __cplusplus >= 201703L
	template<typename... Targs>
	constexpr bool in(Targs... args) const {
		return (C::equal(value, args) || ... || false);
	}
#else
	constexpr bool in() const {
		return false;
	}

	template<typename... Targs>
	constexpr bool in(T arg, Targs... args) const {
		return C::equal(value, arg) ? true : in(args...);
	}
#endif

	constexpr bool in(std::initializer_list<T> list) const {
		for (const auto & arg : list)
			if (C::equal(value, arg))
				return true;
		return false;
	}

	template<size_t S>
	constexpr bool in(T (&a)[S]) const {
		for (size_t s = 0; s < S; s++)
			if (C::equal(value, a[s]))
				return true;
		return false;
	}
};
