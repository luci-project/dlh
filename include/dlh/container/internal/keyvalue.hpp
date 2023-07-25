// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/types.hpp>
#include <dlh/utility.hpp>
#include <dlh/type_traits.hpp>
#include <dlh/container/pair.hpp>

template<class K, class V>
struct KeyValue {
	K key;
	V value;

	KeyValue() : key(), value() { }

	explicit KeyValue(const K& key) : key(key), value() {}

	KeyValue(const K& key, const V& value) : key(key), value(value) {}

	explicit KeyValue(K&& key) : key(move(key)), value() {}

	KeyValue(K&& key, V&& value) : key(move(key)), value(move(value)) {}

	template<class OK, class OV>
	KeyValue(const KeyValue<OK, OV>& o) : key(o.key), value(o.value) {}

	template<class OF, class OS>
	explicit KeyValue(const Pair<OF, OS>& o) : key(o.first), value(o.second) {}

	template<class OK, class OV>
	KeyValue& operator=(const KeyValue<OK, OV>& o) {
		key = o.key;
		value = o.value;
		return *this;
	}

	template<class OF, class OS>
	KeyValue& operator=(const Pair<OF, OS>& o) {
		key = o.first;
		value = o.second;
		return *this;
	}

	template<class OK, class OV>
	constexpr bool operator==(const KeyValue<OK, OV>& other) const {
		return key == other.key && value == other.value;
	}

	template<class OF, class OS, typename enable_if<is_comparable<K, OF>::value && is_comparable<V, OS>::value, int>::type = 0>
	constexpr bool operator==(const Pair<OF, OS>& other) const {
		return key == other.first && value == other.second;
	}

	template<class OK, class OV>
	constexpr bool operator!=(const KeyValue<OK, OV>& other) const {
		return key != other.key || value != other.value;
	}

	template<class OF, class OS, typename enable_if<is_comparable<K, OF>::value && is_comparable<V, OS>::value, int>::type = 0>
	constexpr bool operator!=(const Pair<OF, OS>& other) const {
		return key != other.first || value != other.second;
	}
};


/*! \brief Print contents of a Key-Value-Element
 *
 *  \param s Target Stream
 *  \param element KeyValue to be printed
 *  \return Reference to BufferStream os; allows operator chaining.
 */
template<typename S, typename K, typename V>
static inline S & operator<<(S & s, const KeyValue<K, V> & element) {
	return s << element.key << ": " << element.value;
}
