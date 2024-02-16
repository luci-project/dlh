// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/stream/output.hpp>
#include <dlh/container/hash.hpp>
#include <dlh/container/tree.hpp>
#include <dlh/iterator.hpp>
#include <dlh/assert.hpp>
#include <dlh/string.hpp>


int main(int argc, const char *argv[]) {
	HashSet<int> s = { 888, 999 };
	s.insert(13);
	s.insert(3);
	s.insert(42);
	s.insert(23);
	s.insert(7);

	s.insert(13);
	s.insert(1549);
	s.emplace(666);
	s.insert(3085);
	s.insert(204);
	s.erase(666);

	s.erase(3085);
	s.insert(1572877);
	auto it1 = s.find(204);
	if (it1) {
		auto && tmp = s.extract(it1);
		tmp.value() = 205;
		s.insert(move(tmp));
	}
	s.insert(13);
	s.resize(128);
	s.insert(32);
	auto it2 = s.find(7);
	if (it2) {
		auto && tmp = s.extract(it2);
		tmp.value() = 13;
		s.insert(move(tmp));
	}
	s.insert(42);
	s.insert(52);

	#ifndef NDEBUG
	for (const auto & v : s)
		assert(s.contains(v));
	#endif

	HashSet<int> t(s);
	cout << "HashSet (using some integers): " << s << endl << endl;

	cout << "HashMap (using aruments)" << endl;
	HashMap<const char *, int> m;
	for (int i = 0; i < argc; i++) {
		auto x = m.insert(argv[i], i);
		cout << " - insert " << argv[i] << " = " << x.second << endl;
		if (x.first)
			cout << "   * " << x.first->key << ", " << (*x.first).value << endl;
	}
	m["bar"] = 23;
	m["baz"] = -42;
	const char * d = String::duplicate("bar");
	assert(d != nullptr);
	m[d] *= -1;
	Memory::free(d);

	auto i = m.find(StrPtr("foo"));
	if (i)
		cout << " - find: " << i->key << " = " << i->value << endl;
	auto j = m.erase(i);
	if (j)
		cout << " - erase: " << *j << endl;

	cout << "Full map: " << m << endl;
	return 0;
}
