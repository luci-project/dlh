#pragma once

#include <dlh/container/vector.hpp>
#include <dlh/types.hpp>

namespace Parser {

bool string(unsigned long long & target, const char * value);

bool string(long long & target, const char * value);

bool string(unsigned long & target, const char * value);

bool string(long & target, const char * value);

bool string(unsigned int & target, const char * value);

bool string(int & target, const char * value);

bool string(unsigned short & target, const char * value);

bool string(short & target, const char * value);

bool string(unsigned char & target, const char * value);

bool string(char & target, const char * value);

bool string(bool & target, const char * value);

bool string(const char * & target, const char * value);

template<typename T>
bool string(Vector<T> & target, const char * value) {
	T tmp;
	bool r = string(tmp, value);
	target.push_back(tmp);
	return r;
}

}  // namespace Parse
