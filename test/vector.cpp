#include <dlh/stream/output.hpp>
#include <dlh/container/vector.hpp>

size_t uid_c = 0;
struct Foo {
	const size_t uid;
	int id;

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

	Vector<Foo> s;
	auto p = s.push_front(13);
	auto q = s.insert(p, 666);
	s.push_back(Foo(3));
	s.emplace_front(42);
	s.emplace(q, Foo(23));
	auto x = s.insert(q, 7);
	s.erase(x);

	for (const auto & i : s)
		cout << " - Foo #" << i.uid << " = " << i.id << endl;
	cout << endl << endl;

	Vector<Foo> bla = { 1001, 1002 };
	bla.push_back(1003);
	bla.push_front(1000);
	//bla += s;
	bla.emplace_back(1004);
	for (const auto & i : bla)
		cout << " - Foo #" << i.uid << " = " << i.id << endl;
	cout << endl << endl;
	return 0;
}
