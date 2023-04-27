// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/stream/output.hpp>
#include <dlh/comparison.hpp>

static size_t uid_c = 0;
struct Foo {
	const size_t uid = 0;
	int id = 0;

	Foo * n = nullptr;
	Foo * p = nullptr;

	Foo(const Foo & o) : uid(uid_c++), id(o.id) {
		cerr << "Foo #" << uid << " (" << id << ") created (copy from Foo #" << o.uid << ")" << endl;
	}

	Foo(Foo && o) : uid(uid_c++), id(move(o.id)) {
		cerr << "Foo #" << uid << " (" << id << ") created (move from Foo #" << o.uid << ")" << endl;
	}

	Foo(int i) : uid(uid_c++), id(i) {  // NOLINT (explicitly avoided `explicit`)
		cerr << "Foo #" << uid << " (" << id << ") created" << endl;
	}

	~Foo()  {
		cerr << "Foo #" << uid << " (" << id << ") deleted" << endl;
	}
};

struct FooComp: public Comparison {
	using Comparison::compare;
	using Comparison::equal;
	using Comparison::hash;
	static inline int compare(const Foo & lhs, const Foo & rhs) { return Comparison::compare(lhs.id, rhs.id); }
	static inline int compare(int lhs, const Foo & rhs) { return Comparison::compare(lhs, rhs.id); }
	static inline int compare(const Foo & lhs, int rhs) { return Comparison::compare(lhs.id, rhs); }

	static inline uint32_t hash(const Foo& sym) { return Comparison::hash(sym.id); }

	template<typename T, typename U>
	static inline bool equal(const T& a, const U& b) { return compare(a, b) == 0; }
};


template<typename S>
static inline S & operator<<(S & s, const Foo & foo) {
	return s << "Foo #" << foo.uid << " = " << foo.id;
}
