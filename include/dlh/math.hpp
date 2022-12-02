#pragma once

/*! \brief Basic math helper functions
 */
namespace Math {
	template <typename T>
	inline auto abs(const T a) {
		return (a >= 0 ? a : -a);
	}

	template <typename T, typename U>
	inline auto min(const T a, const U b) {
		return a > b ? b : a;
	}

	template <typename T, typename U>
	inline auto max(const T a, const U b) {
		return a > b ? a : b;
	}

	template <typename T, typename U, typename V>
	inline auto range(const T value, const U from, const V to) {
		return Math::min(Math::max(value, from), to);
	}

	template <typename T, typename U>
	inline auto align_up(const T value, const U boundary) {
		return boundary > 1 ? (value + boundary - 1) & (-boundary) : value;
	}

	template <typename T, typename U>
	inline auto align_down(const T value, const U boundary) {
		return boundary > 1 ? value & (-boundary) : value;
	}

	template <typename T, typename U>
	inline auto align(const T value, const U boundary) {
		return align_up(value, boundary);
	}

}  // namespace Math
