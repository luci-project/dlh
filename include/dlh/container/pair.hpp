#pragma once

template<typename F, typename S>
struct Pair {
	F first;
	S second;

	Pair() : first(), second() { }

	Pair(const F& first, const S& second) : first(first), second(second) { }

	template<typename OF, typename OS>
	Pair(const Pair<OF, OS>& o) : first(o.first), second(o.second) {}

	template<typename OF, typename OS>
	Pair& operator=(const Pair<OF,OS>& o) {
		first = o.first;
		second = o.second;
		return *this;
	}

	template<typename OF, typename OS>
	void assign(OF & f, OS & s) {
		f = first;
		s = second;
	}

	template<typename OF, typename OS>
	constexpr bool operator==(const Pair<OF,OS>& other) const {
		return first == other.first && second == other.second;
	}

	template<typename OF, typename OS>
	constexpr bool operator!=(const Pair<OF,OS>& other) const {
		return first != other.first || second != other.second;
	}

	template<typename OF, typename OS>
	constexpr bool operator<=(const Pair<OF,OS>& other) const {
		return first < other.first || (first == other.first && second <= other.second);
	}

	template<typename OF, typename OS>
	constexpr bool operator<(const Pair<OF,OS>& other) const {
		return first < other.first || (first == other.first && second < other.second);
	}

	template<typename OF, typename OS>
	constexpr bool operator>=(const Pair<OF,OS>& other) const {
		return first > other.first || (first == other.first && second >= other.second);
	}

	template<typename OF, typename OS>
	constexpr bool operator>(const Pair<OF,OS>& other) const {
		return first > other.first || (first == other.first && second > other.second);
	}
};

/*! \brief Print contents of a Pair
 *
 *  \param s Target Stream
 *  \param pair Pair to be printed
 *  \return Reference to Stream; allows operator chaining.
 */
template<typename S, typename OF, typename OS>
static inline S & operator<<(S & s, const Pair<OF, OS> & pair) {
	return s << '(' << ' ' << pair.first << ',' << ' ' << pair.second << ' ' << ')';
}
