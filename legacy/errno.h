#pragma once

#include <dlh/types.hpp>
#include <dlh/error.hpp>

#ifndef errno
#define errno           (*__errno_location())
#endif

extern "C" int *__errno_location() __attribute__((weak));

extern "C" char * strerror_r(int errnum, char *buf, size_t buflen) __attribute__((weak));
extern "C" char * strerror(int errnum) __attribute__((weak));

extern "C" void perror(const char * msg) __attribute__((weak));
