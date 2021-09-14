#pragma once

// Stringify macro
#define STRINTERN(var) #var
#define STR(var) STRINTERN(var)

// Export with C linkage
#define EXPORT extern "C" __attribute__ ((visibility("default")))
