// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

// Stringify macro
#define STRINTERN(var) #var
#define STR(var) STRINTERN(var)

// Export with C linkage
#define EXPORT extern "C" __attribute__((visibility("default")))
#define EXPORT_WEAK extern "C" __attribute__((weak, visibility("default")))
