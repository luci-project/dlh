#pragma once

#include <dlh/container/vector.hpp>

namespace File {

bool exists(const char * path);

bool readable(const char * path);

bool writeable(const char * path);

bool executable(const char * path);

void * contents(const char * path, size_t & size);

Vector<const char *> lines(const char * path);

}  // Namespace File
