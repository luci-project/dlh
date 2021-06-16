#pragma once

#include <dlh/types.hpp>

typedef __builtin_va_list va_list;

#define va_start(v,l) __builtin_va_start(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_copy(d,s)  __builtin_va_copy(d,s)

/*! \brief Wrapper for vsnprintf
 * \param str Target buffer to store the formated string
 * \param size Size of the target buffer
 * \param format format string
 * \param ap value identifying a variable arguments list
 * \return Bytes written
 */
extern "C" int vsnprintf(char *str, size_t size, const char *format, va_list ap);


/*! \brief Wrapper for vprintf
 * \param format format string
 * \param ap value identifying a variable arguments list
 * \return Bytes written (after flush)
 */
extern "C" int vprintf(const char *format, va_list ap);
