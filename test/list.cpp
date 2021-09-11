#include <dlh/stream/output.hpp>
#include <dlh/container/list.hpp>

size_t uid_c = 0;
struct Foo {
	const size_t uid;
	int id;

	Foo * n;
	Foo * p;

	Foo(const Foo & o) : uid(uid_c++), id(o.id) {
		cerr << "Foo #" << uid << " ("<< id << ") created (copy from Foo #" << o.uid << ")" << endl;
	}

	Foo(Foo && o) : uid(uid_c++), id(move(o.id)) {
		cerr << "Foo #" << uid << " ("<< id << ") created (move from Foo #" << o.uid << ")" << endl;
	}

	Foo(int i) : uid(uid_c++), id(i) {
		cerr << "Foo #" << uid << " ("<< id << ") created" << endl;
	}

	~Foo()  {
		cerr << "Foo #" << uid << " ("<< id << ") deleted" << endl;
	}
};


int main(int argc, const char *argv[]) {
	(void) argc;
	(void) argv;

	List<Foo, Foo, &Foo::n, &Foo::p> s;
	auto p = s.push_front(13);
	auto q = s.insert(p, 666);
	s.push_back(Foo(3));
	s.emplace_front(42);
	s.emplace(q, Foo(23));
	s.insert(q, 7);
	s.erase(p);

	for (const auto & i : s)
		cout << " - Foo #" << i.uid << " = " << i.id << endl;
	cout << endl << endl;

	return 0;
}
