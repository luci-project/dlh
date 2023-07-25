// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/types.hpp>
#include <dlh/type_traits.hpp>

template<typename F, typename S>
struct Pair {
	F first;
	S second;

	Pair() : first(), second() { }

	Pair(const F& first, const S& second) : first(first), second(second) { }

	template<typename OF, typename OS>
	Pair& operator=(const Pair<OF, OS>& o) {
		first = o.first;
		second = o.second;
		return *this;
	}

	template<typename OF, typename OS>
	void assign(OF & f, OS & s) {
		f = first;
		s = second;
	}

	constexpr bool operator==(const Pair<F, S>& other) const {
		return first == other.first && second == other.second;
	}

	template<typename OF, typename OS, typename enable_if<is_comparable<F, OF>::value && is_comparable<S, OS>::value, int>::type = 0>
	constexpr bool operator==(const Pair<OF, OS>& other) const {
		return first == other.first && second == other.second;
	}

	constexpr bool operator!=(const Pair<F, S>& other) const {
		return first != other.first || second != other.second;
	}

	template<typename OF, typename OS, typename enable_if<is_comparable<F, OF>::value && is_comparable<S, OS>::value, int>::type = 0>
	constexpr bool operator!=(const Pair<OF, OS>& other) const {
		return first != other.first || second != other.second;
	}

	template<typename OF, typename OS>
	constexpr bool operator<=(const Pair<OF, OS>& other) const {
		return first < other.first || (first == other.first && second <= other.second);
	}

	template<typename OF, typename OS>
	constexpr bool operator<(const Pair<OF, OS>& other) const {
		return first < other.first || (first == other.first && second < other.second);
	}

	template<typename OF, typename OS>
	constexpr bool operator>=(const Pair<OF, OS>& other) const {
		return first > other.first || (first == other.first && second >= other.second);
	}

	template<typename OF, typename OS>
	constexpr bool operator>(const Pair<OF, OS>& other) const {
		return first > other.first || (first == other.first && second > other.second);
	}
};

/*! \brief Print contents of a Pair
 *
 *  \param s Target Stream
 *  \param pair Pair to be printed
 *  \return Reference to Stream; allows operator chaining.
 */
template<typename S, typename OF, typename OS>
static inline S & operator<<(S & s, const Pair<OF, OS> & pair) {
	return s << '(' << ' ' << pair.first << ',' << ' ' << pair.second << ' ' << ')';
}
