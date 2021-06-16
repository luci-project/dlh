#pragma once

template<typename T>
class Reverse {
	T& c;

 public:
	Reverse(T& c) : c(c) {}

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
	return { c };
}
