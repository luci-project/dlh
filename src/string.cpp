#include <dlh/string.hpp>
#include <dlh/mem.hpp>

namespace String {

static inline char * concat(char *dest, const char *src, size_t n = SIZE_MAX) {
	if (dest != nullptr && src != nullptr)
		while ((n--) != 0 && (*dest++ = *src++) != '\0') {}

	return dest;
}

static inline void prefix(const char* needle, size_t needle_len, size_t * n) {
	assert(needle_len > 0 && n != nullptr);

	for (size_t i = 0; i <= needle_len; i++)
		n[i] = 0;

	for (size_t j, i = 1; i < needle_len; i++)  {
		for (j = n[i + 1]; j > 0 && needle[j] != needle[i]; j = n[j]);
		if (j > 0 || needle[j] == needle[i])
			n[i + 1] = j + 1;
	}
}

static inline const char * find(const char *haystack, const char* needle, size_t haystack_len, size_t needle_len, size_t * n, size_t & pos) {
	assert(haystack_len > 0 && needle_len > 0 && n != nullptr);

	if (haystack_len - pos >= needle_len)
		for (size_t i = pos, j = 0; i < haystack_len; i++) {
			if (haystack[i] == needle[j]) {
				if (++j == needle_len) {
					pos = i - j + 1;
					return haystack + pos;
				}
			} else if (j > 0) {
				j = n[j];
				i--;
			}
		}

	return nullptr;
}

const char * find_or_end(const char *s, int c) {
	if (s != nullptr)
		while(*s != '\0') {
			if (*s == c)
				break;
			s++;
		}

	return s;
}

const char* find_or_end(const char *haystack, const char* needle) {
	size_t needle_len = len(needle);
	if (needle_len == 0)
		return haystack;

	size_t haystack_len = len(haystack);
	if (haystack_len == needle_len && compare(haystack, needle) == 0) {
		return haystack;
	} else if (haystack_len > needle_len) {
		size_t n[needle_len + 1];
		prefix(needle, needle_len, n);
		size_t pos = 0;
		const char * t = find(haystack, needle, haystack_len, needle_len, n, pos);
		if (t != nullptr)
			return t;
	}

	return haystack + haystack_len;
}

const char * find(const char *s, int c) {
	if (s != nullptr)
		while(*s != '\0') {
			if (*s == c)
				return s;
			s++;
		}

	return nullptr;
}

const char* find(const char *haystack, const char* needle) {
	if (haystack == nullptr || needle == nullptr)
		return haystack;

	size_t needle_len = len(needle);
	if (needle_len == 0)
		return haystack;

	size_t haystack_len = len(haystack);
	if (haystack_len == needle_len && compare(haystack, needle) == 0) {
		return haystack;
	} else if (haystack_len > needle_len) {
		size_t n[needle_len + 1];
		prefix(needle, needle_len, n);
		size_t pos = 0;
		return find(haystack, needle, haystack_len, needle_len, n, pos);
	} else {
		return nullptr;
	}
}

const char * find_last(const char *s, int c) {
	const char * last = nullptr;
	if (s != nullptr)
		while(*s != '\0') {
			if (*s == c)
				last = s;
			s++;
		}

	return last;
}

const char* find_last(const char *haystack, const char* needle) {
	if (haystack == nullptr || needle == nullptr)
		return haystack;

	size_t needle_len = len(needle);
	if (needle_len == 0)
		return const_cast<char*>(haystack);

	size_t haystack_len = len(haystack);
	if (haystack_len == needle_len && compare(haystack, needle) == 0) {
		return const_cast<char*>(haystack);
	} else if (haystack_len > needle_len) {
		size_t n[needle_len + 1];
		prefix(needle, needle_len, n);
		size_t pos = 0;
		const char *tmp, *last = nullptr;
		while ((tmp = find(haystack, needle, haystack_len, needle_len, n, pos)) != nullptr) {
			last = tmp;
			pos++;
		}
		return last;
	} else {
		return nullptr;
	}
}

char * replace_inplace(char *target, int from, int to, size_t max) {
	for (size_t i = 0; target != nullptr && target[i] != '\0'; i++)
		if (max > 0 && target[i] == from) {
			target[i] = to;
			max--;
		}
	return target;
}

char * replace(const char *target, int from, int to, size_t max) {
	return target == nullptr ? nullptr : replace_inplace(duplicate(target), from, to, max);
}

static inline size_t split(const char *source, const char * delimiter, size_t source_len, size_t delimiter_len, size_t *part, size_t max) {
	assert(source_len > 0 && delimiter_len > 0);
	if (delimiter_len > source_len)
		return 0;

	size_t n[delimiter_len + 1];
	prefix(delimiter, delimiter_len, n);

	size_t found = 0, pos = 0;
	const char *tmp;
	while (max > 0 && (tmp = find(source, delimiter, source_len, delimiter_len, n, pos)) != nullptr) {
		part[found++] = pos;
		pos += delimiter_len;
		max--;
	}

	return found;
}

static inline void combine( char *target, const char * source, const char * to, const size_t *part, size_t source_len, size_t from_len, size_t to_len, size_t part_len) {
	if (target != nullptr) {
		size_t pos = 0;
		for (size_t p = 0; p < part_len; p++) {
			target = concat(target, source + pos, part[p] - pos);
			pos = part[p] + from_len;
			target = concat(target, to, to_len);
		}
		if (pos < source_len)
			concat(target, source + pos);
	}
}

char * replace_inplace(char *target, const char * from, const char * to, size_t max) {
	if (target != nullptr) {
		assert(len(to) <= len(from));
		replace_inplace(target, len(target), from, to,  max);
	}
	return target;
}

char * replace_inplace(char *target, size_t target_len, const char * from, const char * to, size_t max) {
	if (target != nullptr) {
		size_t from_len = len(from);
		assert(from_len > 0);
		size_t to_len = len(to);

		size_t part[target_len / from_len];
		size_t parts = split(target, from, target_len, from_len, part, max);

		char tmp[target_len];
		combine(target, copy(tmp, target, target_len), to, part, target_len, from_len, to_len, parts);
	}
	return target;
}

char * replace(const char *source, const char * from, const char * to, size_t max) {
	if (source == nullptr)
		return nullptr;

	size_t from_len = len(from);
	assert(from_len > 0);
	size_t to_len = len(to);
	size_t source_len = len(source);
	size_t part[source_len / from_len];
	size_t parts = split(source, from, source_len, from_len, part, max);

	char * target = reinterpret_cast<char*>(Memory::alloc(1 + source_len + parts * (0 + to_len - from_len)));
	combine(target, source, to, part, source_len, from_len, to_len, parts);
	return target;
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

static char to_lower(char c) {
	return c >= 'A' && c <= 'Z' ? c + 32 : c;
}

int compare_case(const char *s1, const char *s2) {
	if (s1 == s2 || s1 == nullptr || s2 == nullptr)
		return 0;

	while(to_lower(*s1) == to_lower(*s2++))
		if (*s1++ == '\0')
			return 0;

	return static_cast<int>(to_lower(*s1)) - static_cast<int>(to_lower(*(s2-1)));
}

int compare_case(const char *s1, const char *s2, size_t n) {
	if (s1 == s2)
		return 0;

	if (s1 != nullptr && s2 != nullptr)
		for (size_t i = 0; i < n; i++) {
			const char s1l = to_lower(s1[i]);
			const char s2l = to_lower(s2[i]);
			if (s1l != s2l)
				return static_cast<int>(s1l) - static_cast<int>(s2l);
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
	concat(dest, src);
	return dest;
}

char * copy(char *dest, const char *src, size_t n) {
	concat(dest, src, n);
	return dest;
}

char * duplicate(const char *s) {
	return s == nullptr ? nullptr : duplicate(s, len(s) + 1);
}

char * duplicate(const char *s, size_t n) {
	if (s == nullptr)
		return nullptr;

	char * d = reinterpret_cast<char*>(Memory::alloc(n));

	return d == nullptr ? nullptr : copy(d, s, n);
}

Vector<const char *> split_inplace(char * source, int delimiter, size_t max) {
	Vector<const char *> r;
	if (source != nullptr) {
		bool push_next = true;
		for (; *source != '\0'; ++source)
			if (*source == delimiter && max > 0) {
				*source = '\0';
				if (!push_next)
					max--;
				push_next = true;
			} else if (push_next) {
				r.push_back(source);
				push_next = false;
			}
	}
	return r;
}

Vector<const char *> split(const char * source, int delimiter, size_t max) {
	Vector<const char *> r;
	if (source != nullptr) {
		size_t s = 0, i;
		for (i = 0; source[i] != '\0'; ++i)
			if (source[i] == delimiter && max > 0) {
				if (s < i) {
					char * t = duplicate(source + s, i - s + 1);
					if (t != nullptr) {
						t[i - s] = '\0';
						r.push_back(t);
						max--;
					}
				}
				s = i + 1;
			}
		if (s < i) {
			char * t = duplicate(source + s, i - s + 1);
			if (t != nullptr)
				r.push_back(t);
		}
	}

	return r;
}

Vector<const char *> split_inplace(char * source, const char * delimiter, size_t max) {
	Vector<const char *> r;

	if (source != nullptr) {
		size_t delimiter_len = len(delimiter);
		assert(delimiter_len > 0);
		size_t source_len = len(source);
		size_t part[source_len / delimiter_len];
		size_t found = split(source, delimiter, source_len, delimiter_len, part, max);

		size_t pos = 0;
		for (size_t f = 0; f < found; f++) {
			size_t len = part[f] - pos;
			if (len > 0 && max > 0) {
				r.push_back(source + pos);
				source[pos + len] = '\0';
				max--;
			}
			pos = part[f] + delimiter_len;
			if (max == 0)
				break;
		}
		if (pos < source_len)
			r.push_back(source + pos);
	}

	return r;
}


Vector<const char *> split(const char * source, const char * delimiter, size_t max) {
	Vector<const char *> r;

	if (source != nullptr) {
		size_t delimiter_len = len(delimiter);
		assert(delimiter_len > 0);
		size_t source_len = len(source);
		size_t part[source_len / delimiter_len];
		size_t found = split(source, delimiter, source_len, delimiter_len, part, max);

		size_t pos = 0;
		for (size_t f = 0; f < found; f++) {
			size_t len = part[f] - pos;
			if (len > 0 && max > 0) {
				char * t = duplicate(source + pos, len + 1);
				if (t != nullptr) {
					t[len] = '\0';
					r.push_back(t);
					max--;
				}
			}
			pos = part[f] + delimiter_len;
			if (max == 0)
				break;
		}
		if (pos < source_len) {
			char * t = duplicate(source + pos, source_len - pos + 1);
			if (t != nullptr)
				r.push_back(t);
		}
	}

	return r;
}

}  // namespace String
