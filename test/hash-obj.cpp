// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/stream/output.hpp>
#include <dlh/container/hash.hpp>
#include <dlh/assert.hpp>

#include "foo.hpp"

int main(int argc, const char *argv[]) {
	(void) argc;
	(void) argv;

	HashSet<Foo, FooComp> s = { 888, 999 };
	s.insert(13);
	s.insert(Foo(3));
	Foo fourtytwo(42);
	s.insert(fourtytwo);
	s.insert(23);
	s.emplace(7);

	s.emplace(Foo(13));
	s.insert(1549);
	const Foo numberofthebeast(666);
	s.emplace(numberofthebeast);
	s.insert(3085);
	s.insert(204);
	s.erase(numberofthebeast);

	s.erase(3085);
	s.insert(1572877);
	auto it1 = s.find(204);
	if (it1) {
		auto && tmp = s.extract(it1);
		tmp.value().id = 205;
		s.insert(move(tmp));
	}
	s.insert(13);
	s.resize(128);
	s.insert(32);
	auto it2 = s.find(7);
	if (it2) {
		auto && tmp = s.extract(it2);
		tmp.value().id = 13;
		s.insert(move(tmp));
	}
	s.insert(42);
	s.insert(52);

	#ifndef NDEBUG
	for (const auto & v : s)
		assert(s.contains(v));
	#endif

	HashSet<Foo, FooComp> t(s);
	for (auto i : t)
		cout << " " << i.id;
	cout << endl;

	return 0;
}
