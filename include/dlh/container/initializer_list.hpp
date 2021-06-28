#pragma once

#include <dlh/types.hpp>

namespace std {
template<class T>
class initializer_list {
public:
	 typedef T        value_type;
	 typedef const T& reference;
	 typedef const T& const_reference;
	 typedef size_t   size_type;
	 typedef const T* iterator;
	 typedef const T* const_iterator;

 private:
	const T* _array;
	size_t _len;

 public:
	constexpr initializer_list(const T* array, size_t len) : _array(array), _len(len) { }

	constexpr initializer_list() : _array(nullptr), _len(0) { }

	constexpr size_t size() const {
		return _len;
	}

	constexpr const T* begin() const {
		return _array;
	}

	constexpr const T* end() const {
		return _array + _len;
	}
};
}  // namespace std
