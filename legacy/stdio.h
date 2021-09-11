#pragma once

#include <dlh/types.hpp>

/*! \brief Wrapper for snprintf
 * \param str Target buffer to store the formated string
 * \param size Size of the target buffer
 * \param format format string
 * \return Bytes written
 */
extern "C" int snprintf(char *str, size_t size, const char *format, ...);

/*! \brief Wrapper for printf
 * \param str Target buffer to store the formated string
 * \param size Size of the target buffer
 * \param format format string
 * \return Bytes written (after flush)
 */
extern "C" int printf(const char *format, ...);

/*! \brief Wrapper for putchar
 * \param c character to print
 * \return printed Character
 */
extern "C" int putchar(int c);

/*! \brief Wrapper for puts
 * \param s String to print (plus trailing new line)
 * \return `0` on success, `-1` on error
 */
extern "C" int puts(const char *s);
