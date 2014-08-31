/*
 * File:    DStrings.h
 * Author:  Eduardo Miravalls Sierra          <edu.miravalls@hotmail.com>
 *
 * Date:    2014-08-30 09:47
 *
 * Library to handle dynamic strings.
 */

/*
 * Dynamic C Strings library.
 * Copyright (C) 2014 Eduardo Miravalls Sierra
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _STRING_H_
#define _STRING_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include <string.h> /* strlen */

/**
 * @brief opaque data type
 */
typedef struct string *String;

/**
 * @brief copies n bytes to dest from src.
 * @details if dest_offset > String_length(dest), dest's gap will be padded
 * with 0's.
 *
 * @param dest String.
 * @param dest_offset first char to be overwritten.
 * @param src buffer.
 * @param n number of chars to copy.
 *
 * @return 0 on success, -1 if a reallocation error occurred.
 */
int String_ncpy_at(String dest, unsigned dest_offset,
                   const void *src, unsigned n);

/**
 * @brief convenience macro.
 *
 * @param dest String.
 * @param src buffer.
 * @param n chars to copy.
 */
#define String_ncpy(dest, src, n) String_ncpy_at(dest, 0, src, n)

/**
 * @brief convenience macro.
 *
 * @param dest String.
 * @param src String.
 */
#define String_cpy(dest, src) String_ncpy_at(dest, 0, src, String_length(src))

/**
 * @brief convenience macro.
 *
 * @param dest String.
 * @param src raw string.
 */
#define String_cpy_str(dest, src) String_ncpy_at(dest, 0, src, strlen(src))

/**
 * @brief convenience macro.
 *
 * @param dest String.
 * @param dest_offset first char to be overwritten.
 * @param src raw string.
 */
#define String_cpy_str_at(dest, dest_offset, src) \
	String_ncpy_at(dest, dest_offset, src, strlen(src))

/**
 * @brief appends n chars to dest from src.
 *
 * @param dest String.
 * @param src source.
 * @param n number of chars to be copied.
 *
 * @return 0 on success.
 * @return -1 if dest has to be reallocated in order to succeed but failed.
 */
int String_ncat(String dest,
                const void *src, unsigned n);

/**
 * @brief convenience macro.
 *
 * @param dest String.
 * @param src String.
 */
#define String_cat(dest, src) String_ncat(dest, src, String_length(src))

/**
 * @brief convenience macro.
 *
 * @param dest String.
 * @param src raw string.
 */
#define String_cat_str(dest, src) String_ncat(dest, src, strlen(src))

/**
 * @brief returns the number of chars that s has, including a the
 * terminating '\0' char.
 *
 * @param s String.
 *
 * @return the number of chars s has.
 */
unsigned String_length(String s);

/**
 * @brief returns s's allocated space, which should be always greater or
 * equal to its length.
 *
 * @param s String.
 *
 * @return String's size.
 */
unsigned String_size(String s);

/**
 * @brief reallocates s so it can hold at least size chars.
 * @detail if newsize < 2, newsize will be treated as 1 so it can hold
 * the empty string "".
 *
 * @param s
 * @param newsize new size.
 *
 * @return -1 if reallocation failed or the string is non resizable.
 * @return 0 otherwise.
 */
int String_set_size(String s, unsigned newsize);

/**
 * @brief convenience macro. Shrinks s to a size that can still hold its
 * raw string.
 *
 * @param s String
 */
#define String_shrink(s) String_set_size(s, String_length(s))

/**
 * @brief returns the underlaying raw string.
 *
 * @param s String.
 *
 * @return the raw string.
 */
#define String_raw(s) (*(char **)s)

/**
 * @brief creates a new String from a copy of a slice of s.
 *
 * @param s src
 * @param from first char to copy.
 * @param to last char to copy.
 *
 * @return a new String.
 */
String String_dup_slice(String s, unsigned from, unsigned to);

/**
 * @brief convenience macro.
 *
 * @param s String.
 */
#define String_dup(s) String_dup_slice(s, 0, String_length(s))

/**
 * @brief convenience macro.
 *
 * @param s String.
 * @param to last char to copy.
 */
#define String_dup_to(s, to) String_dup_slice(s, 0, to)

/**
 * @brief convenience macro.
 *
 * @param s String.
 * @param from first char to copy.
 */
#define String_dup_from(s, from) String_dup_slice(s, from, String_length(s))

/**
 * @brief compares s1 and s2.
 * @details upto n bytes of s1 and s2 are compared.
 *
 * @param s1 String.
 * @param s2 String.
 * @param n maximum number of bytes to be compared.
 *
 * @return -1 if s1 < s2
 * @return  0 if s1 == s2
 * @return  1 if s1 > s2
 */
int String_ncmp(String s1, String s2, unsigned n);

/**
 * @brief convenience macro.
 *
 * @param s1 string.
 * @param s2 string.
 */
#define String_cmp(s1, s2) String_ncmp(s1, s2, (unsigned)-1)

/**
 * @brief convenience macro that tests if upto n chars, of s1 are equal
 * to s2's chars.
 *
 * @param s1 String.
 * @param s2 String.
 * @param n maximum number of chars to be compared.
 *
 * @return 0 if they're not equal.
 * @return != 0 if they are equal.
 */
#define String_nequals(s1, s2, n) (String_ncmp(s1, s2, n) == 0)

/**
 * @brief convenience macro that tests if s1 is equal to s2.
 *
 * @param s1 String.
 * @param s2 String.
 *
 * @return 0 if they're not equal.
 * @return != 0 if they are equal.
 */
#define String_equals(s1, s2) String_nequals(s1, s2, (unsigned)-1)


#if _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L

/**
 * @brief write the conversion output of fmt in s.
 * @details if offset > s' length, the gap will be filled with zeros.
 *
 * @param s String
 * @param offset first byte of s to be overwritten.
 * @param fmt printf-like format string.
 * @param ... additional arguments
 *
 * @return -1 if an error occurred. The content of s' raw string will be
 * undefined.
 * @return 0, everything went ok.
 */
int String_format_at(String s, unsigned offset, const char *fmt, ...);

/**
 * @brief convenience macro.
 *
 * @param s String
 * @param fmt printf-like format string.
 * @param ... additional arguments
 */
#define String_format(s, fmt, ...) String_format_at(s, 0, fmt, __VA_ARGS__)

#endif

/**
 * @brief allocates a new String and copies upto n chars of src.
 *
 * @param src source. Can be NULL.
 * @param n maximum number of chars to be copied.
 *
 * @return a new String.
 * @return NULL if allocation failed.
 */
String String_new(const char *src, unsigned n);

/**
 * @brief convenience macro.
 *
 * @param src raw string.
 */
#define String_new_str(src) String_new(src, strlen(src))

/**
 * @brief convenience macro.
 */
#define String_new_empty() String_new((void *)0x0, 0)

/**
 * @brief allocates a new String using src as its raw string.
 * @details if size < strlen(src) then src will be treated as a non resizable
 * raw string. Operations that need resizing it to succeed will fail.
 *
 * @param src raw string. Can be NULL. Has to be NUL terminated.
 * @param size src's allocatd memory.
 *
 * @return a new String
 * @return NULL if allocation failed.
 */
String String_new_steal(char *src, unsigned size);

/**
 * @brief frees a String.
 *
 * @param s String to be freed.
 */
void String_free(String *s);

#ifdef  __cplusplus
}
#endif

#endif /* _STRING_H_ */

