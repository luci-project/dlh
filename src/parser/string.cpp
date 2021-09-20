#include <dlh/parser/string.hpp>
#include <dlh/types.hpp>

namespace Parser {

bool string(unsigned long long & target, const char * value) {
	if (value == nullptr)
		return false;

	for (target = 0; value != nullptr && *value != '\0'; value++)
		if (*value >= '0' && *value <= '9') {
			unsigned long long o = target;
			target = target * 10 + *value - '0';
			if (target < o)
				return false;
		} else if (*value != ' ' && *value != ',' && *value != '\'') {
			return false;
		}

	return true;
}

bool string(long long & target, const char * value) {
	if (value == nullptr)
		return false;

	for (target = 1; *value != '\0'; value++)
		if (*value == '-')  {
			target = -1;
		} else if (*value >= '0' && *value <= '9') {
			break;
		} else if (*value != ' ') {
			return false;
		}

	unsigned long long v = 0;
	bool r = string(v, value);
	target *= v;
	return r;
}

bool string(unsigned long & target, const char * value) {
	unsigned long long v = 0;
	bool r = string(v, value);
	target = static_cast<unsigned long>(v);
	return v > static_cast<unsigned long>(-1) ? false : r ;
}

bool string(long & target, const char * value) {
	long long v = 0;
	bool r = string(v, value);
	target = static_cast<long>(v);
	return v > __LONG_MAX__ || v < (-__LONG_MAX__ - 1) ? false : r ;
}

bool string(unsigned int & target, const char * value) {
	unsigned long long v = 0;
	bool r = string(v, value);
	target = static_cast<unsigned int>(v);
	return v > static_cast<unsigned int>(-1) ? false : r ;
}

bool string(int & target, const char * value) {
	long long v = 0;
	bool r = string(v, value);
	target = static_cast<int>(v);
	return v > __INT_MAX__ || v < (-__INT_MAX__ - 1) ? false : r ;
}

bool string(unsigned short & target, const char * value) {
	unsigned long long v = 0;
	bool r = string(v, value);
	target = static_cast<unsigned short>(v);
	return v > static_cast<unsigned short>(-1) ? false : r ;
}

bool string(short & target, const char * value) {
	long long v = 0;
	bool r = string(v, value);
	target = static_cast<short>(v);
	return v > __SHRT_MAX__ || v < (-__SHRT_MAX__ - 1) ? false : r ;
}

bool string(unsigned char & target, const char * value) {
	unsigned long long v = 0;
	bool r = string(v, value);
	target = static_cast<unsigned char>(v);
	return v > static_cast<unsigned char>(-1) ? false : r ;
}

bool string(char & target, const char * value) {
	long long v = 0;
	bool r = string(v, value);
	target = static_cast<char>(v);
	return v > __SCHAR_MAX__ || v < (-__SCHAR_MAX__ - 1) ? false : r ;
}

bool string(bool & target, const char * value) {
	if (value == nullptr)
		return false;
	target = (*value != '\0' && *value != '0' && *value != 'n' && *value != 'N' && *value != 'f' && *value != 'F');
	return true;
}

bool string(const char * & target, const char * value) {
	if (value == nullptr)
		return false;
	target = value;
	return true;
}

}  // namespace Parse
