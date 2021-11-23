#include <dlh/stream/output.hpp>
#include <dlh/container/vector.hpp>

#include "foo.hpp"

int main(int argc, const char *argv[]) {
	(void) argc;
	(void) argv;

	const Foo five(5);

	// Test 1
	{
		Vector<Foo> s;
		auto p = s.push_front(13);
		auto q = s.insert(p, 666);
		s.push_back(Foo(3));
		s.push_back(five);
		s.emplace_front(42);
		s.emplace(q, Foo(23));
		auto x = s.insert(q, 7);
		s.erase(x);

		for (const auto & i : s)
			cout << " - Foo #" << i.uid << " = " << i.id << endl;
		cout << endl;
	}

	// Test 2
	{
		Vector<Foo> bla = { 1001, 1002 };
		bla.push_back(1003);
		bla.push_front(1000);
		bla.emplace_back(1004);
		{
			const Foo thousandandfive(five.id + 1000);
			bla += thousandandfive;
		}
		for (const auto & i : bla)
			cout << " - Foo #" << i.uid << " = " << i.id << endl;
		cout << endl;
	}

	return 0;
}
