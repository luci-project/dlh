#include <dlh/stream/output.hpp>
#include <dlh/syscall.hpp>

extern "C" [[noreturn]] void __cxa_pure_virtual() {
	cerr <<  "Pure virtual function was called -- this if obviously not valid!" << endl;
	Syscall::abort();
}
