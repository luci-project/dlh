#include <dlh/stream/output.hpp>
#include <dlh/xxhash.hpp>
#include <dlh/random.hpp>
#include <dlh/mem.hpp>

int main(int argc, const char *argv[]) {
	(void) argc;
	(void) argv;

	size_t n = 200;
	char * foo = Memory::alloc<char>(n);
	{
		Random random(23);
		for (size_t i = 0; i < n ; i++)
			foo[i] = static_cast<char>(random.number());
	}

	XXHash64 a(0);
	const char f[1337] = { 0 };
	a.add("foobar", 6);
	a.add(f, 33);
	a.add("ar rcs libdlh.a .build/utils/environ.o .build/utils/string.o .build/utils/mutex.o .build/utils/parser/string.o .build/utils/file.o .build/utils/auxiliary.o .build/utils/log.o .build/utils/stream/output.o .build/utils/stream/buffer.o .build/gnu/stack_chk.o .build/libc/stdarg.o .build/libc/stdlib.o .build/libc/string.o .build/libc/init.o .build/libc/errno.o .build/libc/assert.o .build/libc/unistd.o .build/libc/alloc.o .build/libc/stdio.o .build/libstdcxx/alloc.o .build/libstdcxx/atexit.o .build/libstdcxx/pure_virtual.o Makefile", 531);
	a.add(f, 1337);
	a.add("baz", 3);
	a.add(f, 1336);
	a.add("baz", 3);
	a.add(f, 1335);
	a.add("baz", 3);
	a.add(f, 1334);
	a.add("baz", 3);
	a.add(foo, 123);
	cout << "a: " << a.hash() << endl;

	XXHash64 b(0);
	b.add("foobar", 6);
	b.add(nullptr, 33);
	b.add("ar rcs libdlh.a .build/utils/environ.o .build/utils/string.o .build/utils/mutex.o .build/utils/parser/string.o .build/utils/file.o .build/utils/auxiliary.o .build/utils/log.o .build/utils/stream/output.o .build/utils/stream/buffer.o .build/gnu/stack_chk.o .build/libc/stdarg.o .build/libc/stdlib.o .build/libc/string.o .build/libc/init.o .build/libc/errno.o .build/libc/assert.o .build/libc/unistd.o .build/libc/alloc.o .build/libc/stdio.o .build/libstdcxx/alloc.o .build/libstdcxx/atexit.o .build/libstdcxx/pure_virtual.o Makefile", 531);
	b.addZeros(1337);
	b.add("baz", 3);
	b.addZeros(1336);
	b.add("baz", 3);
	b.addZeros(1335);
	b.add("baz", 3);
	b.addZeros(1334);
	b.add("baz", 3);
	b.add(foo, 123);
	cout << "b: " << b.hash() << endl;
	return 0;
}
