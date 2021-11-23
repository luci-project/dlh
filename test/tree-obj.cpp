#include <dlh/stream/output.hpp>
#include <dlh/container/tree.hpp>

#include "foo.hpp"

int main(int argc, const char *argv[]) {
	(void) argc;
	(void) argv;

	cout << "TreeSet:";
	TreeSet<Foo, FooComp> s = { 888, 999 };
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
	s.check();
	#endif

	TreeSet<Foo, FooComp> t(s);
	for (auto i : t)
		cout << ' ' << i.id;
	cout << endl;

	int n = 42;
	cout << " - Lowest: " << s.lowest()->id << endl;
	cout << " - Lower than " << n << ": " << s.lower(n)->id << endl;
	cout << " - Lower than or equal to " << n << ": " << s.floor(n)->id << endl;
	cout << " - Higher than or equal to " << n << ": " << s.ceil(n)->id << endl;
	cout << " - Higher than " << n << ": " << s.higher(n)->id << endl;
	cout << " - Highest: " << s.highest()->id << endl;
	cout << endl;

	return 0;
}
