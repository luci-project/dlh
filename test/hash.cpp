#include <dlh/stream/output.hpp>
#include <dlh/container/hash.hpp>
#include <dlh/container/tree.hpp>


int main(int argc, const char *argv[]) {
	cout << "HashSet (using some integers):";
	HashSet<int> s;
	s.insert(13);
	s.insert(42);
	s.insert(23);
	s.insert(7);

	s.insert(13);
	s.insert(1549);
	s.emplace(666);
	s.insert(3085);
	s.insert(205);
	s.erase(666);

	s.erase(3085);
	s.insert(1572877);
	s.insert(13);
	s.resize(128);
	s.insert(32);
	s.insert(42);
	s.insert(52);

	for (auto i : s)
		cout << ' ' << i;
	cout << endl << endl;

	cout << "HashMap (using aruments)" << endl;
	HashMap<const char *, int> m;
	for (int i = 0; i < argc; i++) {
		auto x = m.insert(argv[i], i);
		cout << " - insert " << argv[i] << " = " << x.second << endl;
		if (x.first)
			cout << "   * " << x.first->key << ", " << (*x.first).value << endl;
	}

	auto i = m.find(StrPtr("foo"));
	if (i)
		cout << " - find: " << i->key << " = " << i->value << endl;
	auto j = m.erase(i);
	if (j)
		cout << " - erase: " << *j << endl;

	cout << "Full map:" << endl;
	for (auto i : m)
		cout << "	m[" << i.key << "] = " << i.value << endl;

	return 0;
}
