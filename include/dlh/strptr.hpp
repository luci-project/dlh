#pragma once

#include <dlh/types.hpp>
#include <dlh/string.hpp>
#include <dlh/stream/buffer.hpp>

struct StrPtr {
	const char * str;
	uint32_t hash;
	size_t len;

	constexpr StrPtr(const char * s = nullptr) : str(s), hash(String::hash(s)), len(String::len(s)) {}

	constexpr StrPtr(const StrPtr& s) : str(s.str), hash(s.hash), len(s.len) {}

	/*! \brief Substring starting at first occurence of given character
	 * \param c character to search for
	 * \return string starting at first occurence of character or full string if not found
	 */
	constexpr StrPtr find(char c) const {
		if (str != nullptr)
			for (const char * i = str; *i != '\0'; ++i)
				if (*i == c)
					return StrPtr(i);
		return *this;
	}

	/*! \brief Substring starting after last occurence of given character
	 * \param c character to search for
	 * \return string starting after last occurence of character or full string if not found
	 */
	constexpr StrPtr find_last(char c) const {
		if (str != nullptr)
			for (size_t l = len; l > 0; --l)
				if (str[l] == c)
					return StrPtr(str + l + 1);
		return *this;
	}

	constexpr inline bool empty() const {
		return str == nullptr || len == 0;
	}

	constexpr inline const char * c_str() {
		return str;
	}

	constexpr bool operator==(const StrPtr & other) const {
		if (str == other.str)
			return true;
		else if (hash != other.hash || len != other.len || str == nullptr || other.str == nullptr)
			return false;
		for (size_t i = 0; i < len; ++i)
			if (str[i] != other.str[i])
				return false;
		return true;
	}

	constexpr bool operator!=(const StrPtr & other) const {
		return !operator==(other);
	}

	constexpr StrPtr& operator=(const StrPtr& other) {
		str = other.str;
		hash = other.hash;
		len = other.len;
		return *this;
	}

	constexpr StrPtr& operator=(const char * other) {
		str = other;
		hash = String::hash(other);
		len = String::len(other);
		return *this;
	}
};

static constexpr inline bool operator==(const StrPtr &a, const char * b) {
	return a.str == b || String::compare(a.str, b) == 0;
}

static constexpr inline bool operator==(const char * a, const StrPtr & b) {
	return a == b.str || String::compare(a, b.str) == 0;
}

static inline BufferStream& operator<<(BufferStream& bs, const StrPtr & s) {
	if (s.str == nullptr)
		bs << "(nullptr)";
	else
		bs << s.str;
	return bs;
}
