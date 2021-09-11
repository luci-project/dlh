#include <dlh/auxiliary.hpp>

extern char **environ;

Auxiliary * Auxiliary::begin() {
	static int envc = -1;
	if (envc == -1)
		for (envc = 0; environ[envc] != nullptr; envc++) {}

	return reinterpret_cast<Auxiliary *>(environ + envc + 1);
}

Auxiliary * Auxiliary::data(Auxiliary::type type) {
	// Read current auxiliary vectors
	Auxiliary * auxv = begin();
	for (int auxc = 0 ; auxv[auxc].a_type != Auxiliary::AT_NULL; auxc++)
		if (auxv[auxc].a_type == type)
			return auxv + auxc;

	return nullptr;
}

Auxiliary Auxiliary::vector(Auxiliary::type type) {
	auto d = data(type);
	if (d == nullptr)
		return {};
	else
		return *d;
}
