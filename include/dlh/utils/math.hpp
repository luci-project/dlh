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

	template <typename T, typename U>
	inline auto align(const T value, const U boundary) {
		return (value + boundary - 1) & (-boundary);
	}
}  // namespace Math
