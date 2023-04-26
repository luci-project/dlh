// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

template<typename T>
class Reverse {
	T& c;

 public:
	explicit Reverse(T& c) : c(c) {}

	auto begin() {
		return c.rbegin();
	}

	auto begin() const {
		return c.rbegin();
	}

	auto end() {
		return c.rend();
	}

	auto end() const {
		return c.rend();
	}
};

template <typename T>
Reverse<T> reverse(T&& c) {
	return Reverse<T>{c};
}
