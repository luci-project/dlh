#include <dlh/stream/output.hpp>
#include <dlh/utils/auxiliary.hpp>
#include <dlh/unistd.hpp>
#include <dlh/errno.hpp>
#include <dlh/string.hpp>

extern char **environ;

int main(int argc, const char *argv[]) {
	cout << "Environment variables:" << endl;
	static int envc = -1;
	if (envc == -1)
		for (envc = 0; environ[envc] != nullptr; envc++) {
			cout << environ[envc] << endl;
		}

	cout << "Auciliary vectors:" << endl;
	Auxiliary * auxv = reinterpret_cast<Auxiliary *>(environ + envc + 1);
	for (int auxc = 0 ; auxv[auxc].a_type != Auxiliary::AT_NULL; auxc++)
		cout << "AUX[" << auxv[auxc].a_type << "] = "<< auxv[auxc].a_un.a_ptr << endl;

	cout << (char*)0x400000<<endl;
	return 23;
}
