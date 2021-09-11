#pragma once

#include <dlh/types.hpp>
#include <dlh/container/vector.hpp>

namespace String {

/*! \brief Find the first occurrence of a character in a string
* \param s string to search in
* \param c character to find
* \return Pointer to first occurrence of the character
*         or to null byte at the end of the string if not found
*/
char* find_or_end(const char *s, int c);

/*! \brief Find the first occurrence of a character in a string
* \param s string to search in
* \param c character to find
* \return Pointer to first occurrence of the character
*         or to nullptr if not found
*/
char* find(const char *s, int c);

/*! \brief Find the last occurrence of a character in a string
* \param s string to search in
* \param c character to find
* \return Pointer to last occurrence of the character
*         or to nullptr if not found
*/
char* find_last(const char *s, int c);

/*! \brief Compare two strings
* \param s1 first string
* \param s2 second string
* \return an integer less than, equal to, or greater than zero if first string is found, respectively,
*         to be less than, to match, or be greater than second string
*/
int compare(const char *s1, const char *s2);

/*! \brief Compare two strings
* \param s1 first string
* \param s2 second string
* \param n number of bytes to compare
* \return an integer less than, equal to, or greater than zero if the given number of bytes of the first string are
*          found, respectively, to be less than, to match, or be greater than second string
*/
int compare(const char *s1, const char *s2, size_t n);

/*! \brief Calculate the length of a string
* \param s pointer to a string
* \return number of bytes in the string
*/
size_t len(const char *s);

/*! \brief Copy the contents of a string
* including the terminating null byte (`\0`)
* \param dest destination string buffer
* \param src source string buffer
* \return a pointer to the destination string buffer
* \note Beware of buffer overruns!
*/
char* copy(char * dest, const char * src);  //NOLINT

/*! \brief Copy the contents of a string up to a maximum length
* or the terminating null byte (`\0`), whatever comes first.
* \param dest destination string buffer
* \param src source string buffer
* \param n maximum number of bytes to copy
* \return a pointer to the destination string buffer
* \note If there is no null byte (`\0`) among the first `n` bytes, the destination will not be null-terminated!
*/
char* copy(char * dest, const char * src, size_t n);

/*! \brief Duplicate a string
* \param s pointer to a string
* \return pointer to a duplicated string allocated with malloc or `nullptr` if allocation failed.
*/
char* duplicate(const char *s);

/*! \brief Duplicate a string
* \param s pointer to a string
* \param n maximum length
* \return pointer to a duplicated string (up to maximum length, always null-terminated),
*         allocated with malloc or `nullptr` if allocation failed.
*/
char* duplicate(const char *s, size_t n);

/*! \brief Split a string by an delimiter
* \param source pointer to a string (target memory will be modified!)
* \param delimiter character to split string (will be overwritten with NULL character)
* \return Vector with pointers to the start of each substring
*/
Vector<const char *> split(char * source, const char delimiter);
}  // namespace String
