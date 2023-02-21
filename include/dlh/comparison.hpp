#pragma once

#include <dlh/mem.hpp>
#include <dlh/string.hpp>
#include <dlh/strptr.hpp>
#include <dlh/utility.hpp>
#include <dlh/type_traits.hpp>
#include <dlh/container/internal/keyvalue.hpp>

struct Comparison {
	/*! \brief Three-Way-Compare (until the spaceship lands in C++20)
	 * \param a first value
	 * \param b second value
	 * \return a value less than, equal, or greater than zero if `a` is less than, equal or greater than `b`.
	 */
	template<typename T, typename U, typename enable_if<is_integral<T>::value && is_integral<U>::value, int>::type = 0>
	static constexpr inline int compare(const T& a, const U& b) {
		return (b < a) - (a < b);
	}

	template<typename T, typename U, typename enable_if<!is_integral<T>::value || !is_integral<U>::value, int>::type = 0>
	static constexpr inline int compare(const T& a, const U& b) {
		return Memory::compare(a, b);
	}

	template<typename K, typename V, typename O>
	static constexpr inline int compare(const KeyValue<K,V> & a, const O & b) {
		return compare(a.key, b);
	}

	template<typename O, typename K, typename V>
	static constexpr inline int compare(const O & a, const KeyValue<K,V> & b) {
		return compare(a, b.key);
	}

	template<typename K, typename V, typename U, typename W>
	static constexpr inline int compare(const KeyValue<K,V> & a, const KeyValue<U,W> & b) {
		return compare(a.key, b.key);
	}

	static constexpr inline int compare(const char * a, const char * b) {
		return String::compare(a, b);
	}

	static constexpr inline int compare(const char * a, const StrPtr & b) {
		return String::compare(a, b.str);
	}

	static constexpr inline int compare(const StrPtr & a, const char * b) {
		return String::compare(a.str, b);
	}

	static constexpr inline int compare(const StrPtr & a, const StrPtr & b) {
		return String::compare(a.str, b.str);
	}

	template<class OF, class OS>
	static constexpr inline int compare(const Pair<OF,OS>& a, const Pair<OF,OS>& b) {
		int f = compare(a.first, b.first);
		return f == 0 ? compare(a.second, b.second) : f;
	}

	/*! \brief Compare equality
	 * \param a first value
	 * \param b second value
	 * \return `true` if equal, `false` otherwise
	 */
	template<typename T, typename U>
	static constexpr inline bool equal(const T& a, const U& b) {
		return compare(a, b) == 0;
	}

	static constexpr inline bool equal(const StrPtr & a, const StrPtr & b) {
		return a == b;
	}

	/*! \brief Calculate 32bit hash value
	 * \param v value
	 * \param b second value
	 * \return hash value
	 */
	static constexpr inline uint32_t hash(uint64_t v) {
		union {
			struct {
				uint32_t a, b;
			} half;
			uint64_t full;
		};
		full = v;
		return half.a ^ half.b;
	}

	static constexpr inline uint32_t hash(int64_t v) {
		return hash(static_cast<uint64_t>(v));
	}

	static constexpr inline uint32_t hash(uint32_t v) {
		return v;
	}

	static constexpr inline uint32_t hash(int32_t v) {
		return static_cast<uint32_t>(v);
	}

	static constexpr inline uint32_t hash(uint16_t v) {
		return static_cast<uint32_t>(v);
	}

	static constexpr inline uint32_t hash(int16_t v) {
		return static_cast<uint32_t>(v);
	}

	static constexpr inline uint32_t hash(uint8_t v) {
		return static_cast<uint32_t>(v);
	}

	static constexpr inline uint32_t hash(int8_t v) {
		return static_cast<uint32_t>(v);
	}

	static constexpr inline uint32_t hash(void * v) {
		return hash(reinterpret_cast<uint64_t>(v));
	}

	static constexpr inline uint32_t hash(const char * v) {
		return String::hash(v);
	}

	static constexpr inline uint32_t hash(const StrPtr & v) {
		return v.hash;
	}

	template<typename T>
	static constexpr inline uint32_t hash(const T * v) {
		uint_fast32_t h = 0;
		const unsigned char *c = reinterpret_cast<const unsigned char *>(v);
		for (size_t i = 0; i < sizeof(T); i++)
			h = h * 31 + c[i];
		return h & 0xffffffff;
	}

	template<typename T>
	static constexpr inline uint32_t hash(const T& v) {
		return hash(&v);
	}

	template<class OK, class OV>
	static constexpr inline uint32_t hash(const KeyValue<OK,OV>& o) {
		return hash(o.key);
	}

	template<class OF, class OS>
	static constexpr inline uint32_t hash(const Pair<OF,OS>& o) {
		return hash(o.first) ^ hash(o.second);
	}
};
