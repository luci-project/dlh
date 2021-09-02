#include <dlh/stream/output.hpp>
#include <dlh/container/hash.hpp>
#include <dlh/container/tree.hpp>


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

struct FooComp: public Comparison {
	using Comparison::compare;
	using Comparison::equal;
	using Comparison::hash;
	static inline int compare(const Foo & lhs, const Foo & rhs) { return Comparison::compare(lhs.id, rhs.id); }
	static inline int compare(int lhs, const Foo & rhs) { return Comparison::compare(lhs, rhs.id); }
	static inline int compare(const Foo & lhs, int rhs) { return Comparison::compare(lhs.id, rhs); }

	static inline uint32_t hash(const Foo& sym) { cout << "HASH " << sym.id << endl; return Comparison::hash(sym.id); }

	template<typename T, typename U>
	static inline bool equal(const T& a, const U& b) { return compare(a, b) == 0; }
};

int main(int argc, const char *argv[]) {
	TreeSet<Foo, FooComp> s;
	s.insert(13);
	s.insert(666);
	s.insert(Foo(3));
	s.emplace(42);
	s.emplace(Foo(23));
	s.insert(7);
	s.erase(666);

	cout << "find " <<endl;
	auto i = s.find(13);
	cout << "done " <<endl;
	if (i) {
		auto && j = s.extract(i);
		cout << "extracted " << j.value().uid << " at " << &j <<endl;
		j.value().id = 32;
		s.insert(move(j));
		cout << "inserted "  <<endl;
	}

cout << endl << endl;
	for (const auto & i : s)
		cout << " - Foo #" << i.uid << " = " << i.id << endl;
	cout << endl << endl;

	return 0;
}
