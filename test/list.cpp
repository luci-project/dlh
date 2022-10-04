#include <dlh/stream/output.hpp>
#include <dlh/container/list.hpp>

#include "foo.hpp"

int main(int argc, const char *argv[]) {
	(void) argc;
	(void) argv;

	const Foo five(5);

	List<Foo, Foo, &Foo::n, &Foo::p> s;
	auto p = s.push_front(13);
	auto q = s.insert(p, 666);
	s.push_back(Foo(3));
	s.push_back(five);
	s.emplace_front(42);
	s.emplace(q, Foo(23));
	s.insert(q, 7);
	s.erase(p);
	cout << "List: " << s << endl;
	return 0;
}
