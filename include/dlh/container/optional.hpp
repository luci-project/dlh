#pragma once

#include <dlh/assert.hpp>
#include <dlh/utility.hpp>
#include <dlh/stream/output.hpp>

template<class T>
class Optional {
	bool _assigned;
	union {
		bool _used;
		T _value;
	};

 public:
	Optional() : _assigned(false), _used(false) {}

	Optional(const Optional& other) : _assigned(other._assigned) {
		if (_assigned)
			new (&_value) T(other._value);
	}

	Optional(Optional&& other) = default;

	Optional(const T& value) : _assigned(true), _value(value) {}

	Optional(T&& value) : _assigned(true), _value(value) {
		value.~T();
	}

	template <class... ARGS>
	explicit Optional(const ARGS&... args) : _assigned(true), _value(args...) {}

	template <class... ARGS>
	explicit Optional(ARGS&&... args) : _assigned(true), _value(forward<ARGS>(args)...) {}

	~Optional() {
		if (_assigned)
			_value.~T();
	}

	T* operator->() {
		assert(_assigned);
		return &_value;
	}

	const T* operator->() const {
		assert(_assigned);
		return const_cast<const T*>(&_value);
	}

	T& operator*() & {
		assert(_assigned);
		return _value;
	}

	const T& operator*() const & {
		assert(_assigned);
		return _value;
	}

	T&& operator*() && {
		assert(_assigned);
		return move(_value);
	}

	T& value() & {
		assert(_assigned);
		return _value;
	}

	const T& value() const & {
		assert(_assigned);
		return _value;
	}

	T&& value() && {
		assert(_assigned);
		return move(_value);
	}

	T& value_or(T& alt) & {
		return _assigned ? _value : alt;
	}

	T&& value_or(T&& alt) && {
		return _assigned ? move(_value) : move(alt);
	}

	operator bool() const {
		return _assigned;
	}

	template <class... ARGS>
	void emplace(ARGS&&... args) {
		if (_assigned)
			_value.~T();
		else
			_assigned = true;
		new (&_value) T(forward<ARGS>(args)...);
	}

	bool has_value() const {
		return _assigned;
	}

	void clear() {
		if (_assigned) {
			_value.~T();
			_assigned = false;
		}
	}

	Optional & operator=(const Optional<T>& other) {
		if ((_assigned = other._assigned))
			new (&_value) T(other._value);
		return *this;
	}

	template<typename O>
	Optional & operator=(const Optional<O>& other) {
		if ((_assigned = other._assigned))
			new (&_value) T(other._value);
		return *this;
	}

	Optional & operator=(const T& other) {
		new (&_value) T(other);
		_assigned = true;
		return *this;
	}

	template<typename O>
	bool operator==(const Optional<O>& other) const {
		return _assigned == other._assigned && (!_assigned || _value == other._value);
	}

	template<typename O>
	bool operator==(const O& other) const {
		return _assigned ? _value == other : false;
	}

	template<typename O>
	bool operator!=(const Optional<O>& other) const {
		return _assigned != other._assigned || (_assigned && _value != other._value);
	}

	template<typename O>
	bool operator!=(const O& other) const {
		return _assigned ? _value != other : true;
	}
};
