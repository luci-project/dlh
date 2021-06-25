#include <dlh/utils/string.hpp>

namespace String {

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
