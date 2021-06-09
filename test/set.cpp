#include <dlh/stream/output.hpp>
#include <dlh/container/hash.hpp>
#include <dlh/container/tree.hpp>


int main(int argc, const char *argv[]) {
	HashMap<const char *, int> c;
	for (int i = 0; i < argc; i++) {
		auto x = c.insert(argv[i], i);
		cout << "- emplace " << argv[i] << " = " << x.second << endl;
		if (x.first)
			cout << "   * " << x.first->key << ", " << (*x.first).value << endl;
	}

	for (auto i : c)
		cout << "c[" << i.key << "] = " << i.value << endl;

	auto i = c.find(StrPtr("asd"));
	if (i)
		cout << "Find: " << i->key << " = " << i->value << endl;
	auto j = c.erase(i);
	if (j)
		cout << "Deleted: " << *j << endl;

	for (auto i : c)
		cout << "c[" << i.key << "] = " << i.value << endl;

	cout << endl;

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

	cout << endl;
/*	cout << *s.lowest() << endl;
	cout << *s.lower(43) << endl;
	cout << *s.floor(43) << endl;
	cout << *s.ceil(43) << endl;
	cout << *s.higher(43) << endl;
	cout << *s.highest() << endl;
	cout << endl;
*/
	for (auto i : s)
		cout << i << endl;

	return 0;
}
