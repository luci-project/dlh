#include <dlh/stream/output.hpp>
#include <dlh/syscall.hpp>

int main(int argc, const char ** argv) {
	if (argc == 1) {
		const char * args[4] = { argv[0], "Hello", "world!", NULL };
		auto exec = Syscall::execv(argv[0], args);
		cout << "Exec failed: " << exec.error_message() << endl;
	} else {
		cout << (argc - 1) << " arguments:";
		for (int i = 1; i < argc; i++)
			cout << ' ' << argv[i];
		cout << endl;
	}
	return 0;
}
