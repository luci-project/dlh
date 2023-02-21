#pragma once

#include <dlh/comparison.hpp>

template<typename T, typename C = Comparison>
class is {
	T value;

 public:
	is(T value) : value(value) {}

	bool in() {
		return false;
	}

	template<typename... Targs>
	bool in(T arg, Targs... args) {
		return C::equal(value, arg) ? true : in(args...);
	}

	bool in(std::initializer_list<T> list) {
		for (const auto & arg : list)
			if (C::equal(value, arg))
				return true;
		return false;
	}

	template<size_t S>
	bool in(T (&a)[S]) {
		for (size_t s = 0; s < S; s++)
			if (C::equal(value, a[s]))
				return true;
		return false;
	}
};
