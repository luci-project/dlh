#include <dlh/stream/output.hpp>
#include <dlh/container/tree.hpp>


int main(int argc, const char *argv[]) {
	cout << "TreeSet (using some integers):";
	TreeSet<int> s = { 888, 999 };
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
	s.check();
	#endif

	TreeSet<int> t(s);
	for (auto i : t)
		cout << ' ' << i;
	cout << endl;

	int n = 42;
	cout << " - Lowest: " << *s.lowest() << endl;
	cout << " - Lower than " << n << ": " << *s.lower(n) << endl;
	cout << " - Lower than or equal to " << n << ": " << *s.floor(n) << endl;
	cout << " - Higher than or equal to " << n << ": " << *s.ceil(n) << endl;
	cout << " - Higher than " << n << ": " << *s.higher(n) << endl;
	cout << " - Highest: " << *s.highest() << endl;
	cout << endl << endl;


	cout << "TreeMap (using aruments)" << endl;
	TreeMap<const char *, int> m;
	for (int i = 0; i < argc; i++) {
		auto x = m.insert(argv[i], i);
		cout << " - insert " << argv[i] << " = " << x.second << endl;
		if (x.first)
			cout << "   * " << x.first->key << ", " << (*x.first).value << endl;
	}
	m["bar"] = 23;
	m["baz"] = -42;
	m["bar"] *= -1;

	auto i = m.find(StrPtr("foo"));
	if (i)
		cout << " - find: " << i->key << " = " << i->value << endl;
	auto j = m.erase(i);
	if (j)
		cout << " - erase: " << *j << endl;

	#ifndef NDEBUG
	m.check();
	#endif

	cout << "Full map:" << endl;
	for (auto i : m)
		cout << "	m[" << i.key << "] = " << i.value << endl;

	return 0;
}
