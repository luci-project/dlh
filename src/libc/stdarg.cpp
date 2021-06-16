#include <dlh/stdarg.hpp>
#include <dlh/stream/buffer.hpp>
#include <dlh/stream/output.hpp>

extern "C" int vsnprintf(char *str, size_t size, const char *format, va_list ap) {
	return BufferStream(str, size).format(format, ap);
}

extern "C" int vprintf(const char *format, va_list ap) {
	return cout.format(format, ap);
}
