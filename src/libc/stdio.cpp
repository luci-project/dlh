#include <dlh/stdarg.hpp>
#include <dlh/stream/buffer.hpp>
#include <dlh/stream/output.hpp>

#ifdef DLH_LEGACY
extern "C" int vsnprintf(char *str, size_t size, const char *format, va_list ap) {
	return BufferStream(str, size).format(format, ap);
}

extern "C" int vprintf(const char *format, va_list ap) {
	return cout.format(format, ap);
}

extern "C" int snprintf(char *str, size_t size, const char *format, ...) {
	va_list args;
	va_start(args, format);
	size_t ret = vsnprintf(str, size, format, args);
	va_end(args);
	return ret;
}

extern "C" int printf(const char *format, ...) {
	va_list args;
	va_start(args, format);
	size_t ret = vprintf(format, args);
	va_end(args);
	return ret;
}

extern "C" int putchar(int c) {
	cout << static_cast<char>(c);
	return c;
}

extern "C" int puts(const char *s) {
	cout << s << endl;
	return 0;
}
#endif
