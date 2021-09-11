#include <dlh/string.hpp>
#include <dlh/mem.hpp>

namespace String {

char * find_or_end(const char *s, int c) {
	if (s != nullptr)
		while(*s != '\0') {
			if (*s == c)
				break;
			s++;
		}

	return const_cast<char *>(s);
}

char * find(const char *s, int c) {
	if (s != nullptr)
		while(*s != '\0') {
			if (*s == c)
				return const_cast<char *>(s);
			s++;
		}

	return nullptr;
}

char * find_last(const char *s, int c) {
	char * last = nullptr;
	if (s != nullptr)
		while(*s != '\0') {
			if (*s == c)
				last = const_cast<char *>(s);
			s++;
		}

	return last;
}

int compare(const char *s1, const char *s2) {
	if (s1 == s2 || s1 == nullptr || s2 == nullptr)
		return 0;

	while(*s1 == *s2++)
		if (*s1++ == '\0')
			return 0;

	return static_cast<int>(*s1) - static_cast<int>(*(s2-1));
}

int compare(const char *s1, const char *s2, size_t n) {
	if (s1 == s2)
		return 0;

	if (s1 != nullptr && s2 != nullptr)
		for (size_t i = 0; i < n; i++) {
			if (s1[i] != s2[i])
				return static_cast<int>(s1[i]) - static_cast<int>(s2[i]);
			else if (s1[i] == '\0')
				break;
		}

	return 0;
}

size_t len(const char *s) {
	size_t len = 0;
	if (s != nullptr)
		while (*s++ != '\0')
			len++;

	return len;
}

char * copy(char *dest, const char *src) { //NOLINT
	char *r = dest;
	if (dest != nullptr && src != nullptr)
		while ((*dest++ = *src++) != '\0') {}

	return r;
}

char * copy(char *dest, const char *src, size_t n) {
	char *r = dest;
	if (dest != nullptr && src != nullptr)
		while ((n--) != 0 && (*dest++ = *src++) != '\0') {}

	return r;
}

char * duplicate(const char *s) {
	return s == nullptr ? nullptr : duplicate(s, len(s));
}

char * duplicate(const char *s, size_t n) {
	if (s == nullptr)
		return nullptr;

	char * d = reinterpret_cast<char*>(Memory::alloc(n));

	return d == nullptr ? nullptr : copy(d, s, n);
}

Vector<const char *> split(char * source, const char delimiter) {
	Vector<const char *> r;
	if (source != nullptr) {
		bool push_next = true;
		for (; *source != '\0'; ++source)
			if (*source == delimiter) {
				*source = '\0';
				push_next = true;
			} else if (push_next) {
				r.push_back(source);
				push_next = false;
			}
	}
	return r;
}

}  // namespace String
