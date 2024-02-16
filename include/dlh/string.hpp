// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

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
const char* find_or_end(const char *s, int c);

/*! \brief Find the first occurrence of a substring in a string
 * \param haystack string to search in
 * \param needle string to find
 * \return Pointer to first occurrence of the needle
 *         or to null byte at the end of the string if not found
 */
const char* find_or_end(const char *haystack, const char *needle);

/*! \brief Find the first occurrence of a character in a string
 * \param s string to search in
 * \param c character to find
 * \return Pointer to first occurrence of the character
 *         or to nullptr if not found
 */
const char* find(const char *s, int c);

/*! \brief Find the first occurrence of a substring in a string
 * \param haystack string to search in
 * \param needle string to find
 * \return Pointer to first occurrence of the needle
 *         or to nullptr if not found
 */
const char* find(const char *haystack, const char *needle);

/*! \brief Find the last occurrence of a character in a string
 * \param s string to search in
 * \param c character to find
 * \return Pointer to last occurrence of the character
 *         or to nullptr if not found
 */
const char* find_last(const char *s, int c);

/*! \brief Find the last occurrence of a substring in a string
 * \param haystack string to search in
 * \param needle string to find
 * \return Pointer to last occurrence of the needle
 *         or to nullptr if not found
 */
const char* find_last(const char *haystack, const char *needle);

/*! \brief Replace characters in a string
 * \param target pointer to string (target memory will be modified!)
 * \param from character to be replaced
 * \param to replacement character
 * \param max maximum number of replacements to be performed
 * \return Pointer to the target string
 */
char * replace_inplace(char *target, int from, int to, size_t max = SIZE_MAX);

/*! \brief Replace characters in a string
 * \param source pointer to string (string will be copied)
 * \param from character to be replaced
 * \param to replacement character
 * \param max maximum number of replacements to be performed
 * \return Pointer to the target string
 * \note Target pointer has to be freed after usage
 */
char * replace(const char *source, int from, int to, size_t max = SIZE_MAX);

/*! \brief Replace stubstrings in a string
 * \param target pointer to string  (target memory will be modified!)
 * \param from substring to be replaced
 * \param to replacement substring (must be equal or shorter than `from`)
 * \param max maximum number of replacements to be performed
 * \return Pointer to the target string
 */
char * replace_inplace(char *target, const char *from, const char * to, size_t max = SIZE_MAX);

/*! \brief Replace stubstrings in a string
 * \param target pointer to string  (target memory will be modified!)
 * \param target_len size of target buffer
 * \param from substring to be replaced
 * \param to replacement substring
 * \param max maximum number of replacements to be performed
 * \return Pointer to the target string
 */
char * replace_inplace(char *target, size_t target_len, const char *from, const char * to, size_t max = SIZE_MAX);

/*! \brief Replace stubstrings in a string
 * \param source pointer to string (string will be copied)
 * \param from substring to be replaced
 * \param to replacement substring
 * \param max maximum number of replacements to be performed
 * \return Pointer to the target string
 * \note Target pointer has to be freed after usage
 */
char * replace(const char *source, const char *from, const char * to, size_t max = SIZE_MAX);

/*! \brief Compare two strings
 * \param s1 first string
 * \param s2 second string
 * \return an integer less than, equal to, or greater than zero if first string is found, respectively,
 *         to be less than, to match, or be greater than second string
 */
constexpr int compare(const char *s1, const char *s2) {
	if (s1 == s2 || s1 == nullptr || s2 == nullptr)
		return 0;

	while(*s1 == *s2++)
		if (*s1++ == '\0')
			return 0;

	return static_cast<int>(*s1) - static_cast<int>(*(s2-1));
}

/*! \brief Compare two strings
 * \param s1 first string
 * \param s2 second string
 * \param n number of bytes to compare
 * \return an integer less than, equal to, or greater than zero if the given number of bytes of the first string are
 *          found, respectively, to be less than, to match, or be greater than second string
 */
constexpr int compare(const char *s1, const char *s2, size_t n) {
	if (s1 == s2)
		return 0;

	if (s1 != nullptr && s2 != nullptr) {
		for (size_t i = 0; i < n; i++) {
			if (s1[i] != s2[i])
				return static_cast<int>(s1[i]) - static_cast<int>(s2[i]);
			else if (s1[i] == '\0')
				break;
		}
	}

	return 0;
}

/*! \brief Compare two strings case insensetive
 * \param s1 first string
 * \param s2 second string
 * \return an integer less than, equal to, or greater than zero if first string is found, respectively,
 *         to be less than, to match, or be greater than second string
 */
int compare_case(const char *s1, const char *s2);

/*! \brief Compare two strings case insensetive
 * \param s1 first string
 * \param s2 second string
 * \param n number of bytes to compare
 * \return an integer less than, equal to, or greater than zero if the given number of bytes of the first string are
 *          found, respectively, to be less than, to match, or be greater than second string
 */
int compare_case(const char *s1, const char *s2, size_t n);


/*! \brief Check if string starts with a certain substring
 * \param str string to search in
 * \param start required starting substring
 * \return `true` if string starts with the required substring
 */
bool starts_with(const char *str, const char * start);

/*! \brief Check if string ends with a certain substring
 * \param str string to search in
 * \param start required ending substring
 * \return `true` if string ends with the required substring
 */
bool ends_with(const char *str, const char * end);

/*! \brief String hash (djb2)
 * \see http://www.cse.yorku.ca/~oz/hash.html
 * \param s String to hash
 * \return unsigned 32bit integer with string hash
 */
constexpr uint32_t hash(const char * s) {
	if (s == nullptr)
		return 0;
	uint_fast32_t h = 5381;
	for (unsigned char c = *s; c != '\0'; c = *++s)
		h = h * 33 + c;
	return h & 0xffffffff;
}


/*! \brief Calculate the length of a string
 * \param s pointer to a string
 * \param n max length to check
 * \return number of bytes in the string
 */
constexpr size_t len(const char *s, size_t n = SIZE_MAX) {
	size_t len = 0;
	if (s != nullptr)
		while (*s++ != '\0' && n-- > 0)
			len++;

	return len;
}

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
 * \param zero_remainder if length of `src` is less then `n`, fill remainder with zeros
 * \return a pointer to the destination string buffer
 * \note If there is no null byte (`\0`) among the first `n` bytes, the destination will not be null-terminated!
 */
char* copy(char * dest, const char * src, size_t n, bool zero_remainder = false);

/*! \brief Duplicate a string
 * \param s pointer to a string
 * \return pointer to a duplicated string allocated with malloc or `nullptr` if allocation failed.
 */
char* duplicate(const char *s);

/*! \brief Duplicate a string
 * \param s pointer to a string
 * \param n available bytes to allocate
 * \return pointer to a duplicated string (with `n+1` bytes, always null-terminated),
 *         allocated with malloc or `nullptr` if allocation failed.
 */
char* duplicate(const char *s, size_t n);

/*! \brief Split a string by a delimiter
 * \param source pointer to a string (target memory will be modified!)
 * \param delimiter character to split string
 * \param max maximum number of splits (hence the vector contains not more than max + 1 elements)
 * \return Vector with pointers to the start of each substring (empty substrings are omitted)
 */
Vector<const char *> split_inplace(char * source, int delimiter, size_t max = SIZE_MAX);

/*! \brief Split a string by a delimiter
 * \param source pointer to a string
 * \param delimiter character to split string
 * \param max maximum number of splits (hence the vector contains not more than max + 1 elements)
 * \return Vector with pointers to the start of each substring (empty substrings are omitted)
 * \note You have to free each element of the result vector!
 */
Vector<const char *> split(const char * source, int delimiter, size_t max = SIZE_MAX);

/*! \brief Split a string by any of the given delimiters
 * \param source pointer to a string (target memory will be modified!)
 * \param delimiter null-terminated list of characters to split the string
 * \param max maximum number of splits (hence the vector contains not more than max + 1 elements)
 * \return Vector with pointers to the start of each substring (empty substrings are omitted)
 */
Vector<const char *> split_any_inplace(char * source, const char * delimiter, size_t max = SIZE_MAX);

/*! \brief Split a string by any of the given delimiters
 * \param source pointer to a string
 * \param delimiter null-terminated list of characters to split the string
 * \param max maximum number of splits (hence the vector contains not more than max + 1 elements)
 * \return Vector with pointers to the start of each substring (empty substrings are omitted)
 * \note You have to free each element of the result vector!
 */
Vector<const char *> split_any(const char * source, const char * delimiter, size_t max = SIZE_MAX);

/*! \brief Split a string by a delimiter
 * \param source pointer to a string (target memory will be modified!)
 * \param delimiter substring to split string
 * \param max maximum number of splits (hence the vector contains not more than max + 1 elements)
 * \return Vector with pointers to the start of each substring (empty substrings are omitted)
 */
Vector<const char *> split_inplace(char * source, const char * delimiter, size_t max = SIZE_MAX);

/*! \brief Split a string by a delimiter
 * \param source pointer to a string
 * \param delimiter substring to split string
 * \param max maximum number of splits (hence the vector contains not more than max + 1 elements)
 * \return Vector with pointers to the start of each substring (empty substrings are omitted)
 * \note You have to free each element of the result vector!
 */
Vector<const char *> split(const char * source, const char * delimiter, size_t max = SIZE_MAX);
}  // namespace String
