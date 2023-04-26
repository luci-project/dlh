// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/assert.hpp>
#include <dlh/types.hpp>
#include <dlh/type_traits.hpp>

extern "C++" {

// Taken from http://bajamircea.github.io/coding/cpp/2016/04/07/move-forward.html
template<typename T>
constexpr typename remove_reference<T>::type && move(T && arg) noexcept {
	return static_cast<typename remove_reference<T>::type &&>(arg);
}

template<typename T>
constexpr T&& forward(typename remove_reference<T>::type & arg) noexcept {
	return static_cast<T&&>(arg);
}

template<typename T>
constexpr T&& forward(typename remove_reference<T>::type && arg) noexcept {
	static_assert(!is_lvalue_reference<T>::value, "invalid rvalue to lvalue conversion");
	return static_cast<T&&>(arg);
}

template<typename T, size_t S>
constexpr size_t count(const T(&)[S]) {
	return S;
}

inline void *operator new(size_t n, void * ptr) {
	(void)n;
	assert(ptr != nullptr);
	return ptr;
}

}
