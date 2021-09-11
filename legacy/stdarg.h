#pragma once

#include <dlh/stdarg.hpp>

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
