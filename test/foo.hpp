#pragma once

#include <dlh/stream/output.hpp>
#include <dlh/comparison.hpp>

static size_t uid_c = 0;
struct Foo {
	const size_t uid;
	int id;

	Foo * n;
	Foo * p;

	Foo(const Foo & o) : uid(uid_c++), id(o.id) {
		cerr << "Foo #" << uid << " (" << id << ") created (copy from Foo #" << o.uid << ")" << endl;
	}

	Foo(Foo && o) : uid(uid_c++), id(move(o.id)) {
		cerr << "Foo #" << uid << " (" << id << ") created (move from Foo #" << o.uid << ")" << endl;
	}

	Foo(int i) : uid(uid_c++), id(i) {
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
