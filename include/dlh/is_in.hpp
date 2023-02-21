#pragma once

#include <dlh/comparison.hpp>

template<typename T, typename C = Comparison>
class is {
	T value;

 public:
	constexpr is(T value) : value(value) {}

	constexpr bool in() const {
		return false;
	}

	template<typename... Targs>
	constexpr bool in(T arg, Targs... args) const {
		return C::equal(value, arg) ? true : in(args...);
	}

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
