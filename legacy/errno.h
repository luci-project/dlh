// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

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
