#pragma once

/*! \brief Basic math helper functions
 */
namespace Math {
	template <typename T>
	auto abs(const T a) {
		return (a >= 0 ? a : -a);
	}

	template <typename T, typename U>
	auto min(const T a, const U b) {
		return a > b ? b : a;
	}

	template <typename T, typename U>
	auto max(const T a, const U b) {
		return a > b ? a : b;
	}
}  // namespace Math
