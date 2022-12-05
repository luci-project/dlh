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

template<bool B, class T = void>
struct enable_if {};

template<class T>
struct enable_if<true, T> {
	typedef T type;
};

template <bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;

inline void *operator new(size_t n, void * ptr) {
	(void)n;
	assert(ptr != nullptr);
	return ptr;
}

}
