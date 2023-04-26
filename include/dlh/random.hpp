// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include "types.hpp"

/*! Mersenne Twister (32 bit pseudorandom number generator)
 *
 * \see [Wikipedia](https://en.wikipedia.org/wiki/Mersenne_Twister)
 */
class Random {
 private:
	static const uint32_t N = 624;
	static const uint32_t M = 397;
	static const uint32_t R = 31;
	static const uint32_t A = 0x9908B0DF;

	// initialization multiplier
	static const uint32_t F = 1812433253;

	static const uint32_t U = 11;

	static const uint32_t S = 7;
	static const uint32_t B = 0x9D2C5680;

	static const uint32_t T = 15;
	static const uint32_t C = 0xEFC60000;

	static const uint32_t L = 18;

	static const uint32_t MASK_LOWER = (1UL << R) - 1;
	static const uint32_t MASK_UPPER = (1UL << R);

	uint32_t mt[N];
	uint16_t index;

 public:
	/*! \brief Constructor
	 *
	 *  \param seed initial value (seed) for the pseudorandom number generator.
	 */
	explicit Random(uint32_t seed = 0) {
		set(seed);
	}

	/*! \brief (Re)Seed
	 *
	 *  \param seed initial value (seed) for the pseudorandom number generator.
	 */
	void set(uint32_t seed) {
		mt[0] = seed;

		for (uint32_t i = 1; i < N; i++) {
			mt[i] = (F * (mt[i - 1] ^ (mt[i - 1] >> 30)) + i);
		}

		index = N;
	}

	/*! \brief Get the next (pseudo)random number.
	 *  \return some number
	 */
	uint32_t number() {
		uint16_t v = index;

		if (index >= N) {
			// Twist
			for (uint32_t i = 0; i < N; i++) {
				uint32_t x = (mt[i] & MASK_UPPER) + (mt[(i + 1) % N] & MASK_LOWER);

				uint32_t xA = x >> 1;

				if ((x & 0x1) != 0) {
					xA ^= A;
				}

				mt[i] = mt[(i + M) % N] ^ xA;
			}
			v = index = 0;
		}

		uint32_t y = mt[v];
		index = v + 1;

		y ^= (y >> U);
		y ^= (y << S) & B;
		y ^= (y << T) & C;
		y ^= (y >> L);

		return y;
	}
};
