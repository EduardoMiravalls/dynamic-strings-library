/*
 * File:    DStrings.c
 * Author:  Eduardo Miravalls Sierra          <edu.miravalls@hotmail.com>
 *
 * Date:    2014-08-30 09:47
 */

#include <stdlib.h>
#include <assert.h>

#include "DStrings.h"

enum {NO = 0, YES};

struct string {
	char *raw;     /**< raw string */
	unsigned len;  /**< includes trailing \0 */
	unsigned size; /**< allocated space */
	char resizable;
};

#define resizable(s) ((s)->resizable)

/**
 * @brief allocates an empty string.
 *
 * @return a String.
 */
static String String_alloc(void);

/**
 * @brief resizes s so it can hold at least n chars.
 * @detail swaps s' old raw string for the new one, and updates its size
 * variable.
 *
 * @param s String.
 * @param size new minimum capacity.
 *
 * @return NULL on error or if s is not resizable.
 * @return if s' old raw string was NULL, the newly allocated buffer.
 * @return the old raw string otherwise.
 */
static char *resize(String s, unsigned n);

/**
 * @brief resizes s so it can hold at least n chars and copies upto offset
 * bytes from old to new, depending on s->len.
 *
 * @param s String.
 * @param size new minimum capacity.
 * @param offset of the first byte that should be left uninitialized.
 *
 * @return NULL on error or if s is not resizable.
 * @return if s' old raw string was NULL, the newly allocated buffer.
 * @return the old raw string otherwise.
 */
static char *resize_and_cpy(String s, unsigned n, unsigned offset);

/**
 * @brief the name says exactly what it does.
 *
 * @param num
 *
 * @return num's next power of 2.
 * @return if num is a power of 2, it returns it.
 */
static unsigned round_up_to_the_next_power_of_2(unsigned num);

#if !(_XOPEN_SOURCE >= 700 || _POSIX_C_SOURCE >= 200809L)
/**
 * @brief strlen(s) that returns if it reaches n.
 *
 * @param s raw string.
 * @param n maximum number of bytes to test.
 *
 * @return min(n, strlen(s))
 */
static unsigned strnlen(const char *s, unsigned n);
#endif

int String_ncpy_at(String dest, unsigned dest_offset,
                   const void *src, unsigned n)
{
	void *old_raw = NULL;

	assert(dest != NULL);
	assert(src != NULL || n == 0);

	if (dest->size < (dest_offset + n + 1)) {
		old_raw = resize_and_cpy(dest, dest_offset + n + 1, dest_offset);

		if (NULL == old_raw) {
			return -1;
		}

		if (old_raw == dest->raw) {
			old_raw = NULL;
		}
	}

	/*
	 * if len < offset:
	 *     means len bytes will be kept untouched and (offset - len) bytes
	 *     have to be zeroed.
	 */
	if (dest->len < dest_offset) {
		memset(dest->raw + dest->len, 0, dest_offset - dest->len);
	}

	memmove(dest->raw + dest_offset, src, n);
	dest->len = n + dest_offset + 1;
	dest->raw[n + dest_offset] = '\0';
	free(old_raw);
	return 0;
}

int String_ncat(String dest,
                const void *src, unsigned n)
{
	assert(dest != NULL);
	assert(src != NULL);

	return String_ncpy_at(dest, dest->len - 1, src, n);
}

unsigned String_length(String s)
{
	assert(s != NULL);

	if (!resizable(s)) {
		return s->len - 1;
	}

	return s->len;
}

unsigned String_size(String s)
{
	assert(s != NULL);
	return s->size;
}

int String_set_size(String s, unsigned size)
{
	void *temp;
	assert(s != NULL);

	if (!resizable(s)) {
		return -1;
	}

	if (size == s->size) { /* no changes */
		return 0;
	}

	if (size < 2) {
		size = 1;
	}

	if ((temp = realloc(s->raw, size)) == NULL) {
		return -1;
	}

	s->size = size;
	s->raw = temp;

	if (s->len > size) {
		s->len = size;
		s->raw[size - 1] = '\0';
	}

	return 0;
}

String String_dup_slice(String s, unsigned from, unsigned to)
{
	String cpy;

	assert(s != NULL);
	assert(from <= to);

	if ((cpy = String_alloc()) == NULL) {
		return NULL;
	}

	if (String_ncpy_at(cpy, 0, s->raw + from, to - from + 1)) {
		String_free(&cpy);
	}

	return cpy;
}

int String_ncmp(String s1, String s2, unsigned n)
{
	assert(s1 != NULL);
	assert(s2 != NULL);

	return strncmp(String_raw(s1), String_raw(s2), n);
}

String String_new(const char *src, unsigned n)
{
	String s;

	if ((s = String_alloc()) == NULL) {
		return NULL;
	}

	if (src != NULL) {
		n = strnlen(src, n);

	} else {
		n = 0;
	}

	if (String_ncpy_at(s, 0, src, n)) {
		String_free(&s);
	}

	return s;
}

String String_new_steal(char *src, unsigned size)
{
	String s;

	if ((s = String_alloc()) == NULL) {
		return NULL;
	}

	if (src != NULL) {
		s->len = strlen(src) + 1;

	} else {
		s->len = 0;
	}

	if (size < s->len) {
		size = s->len;
		s->resizable = NO;
	}

	s->size = size;
	s->raw = src;
	return s;
}

void String_free(String *s)
{
	assert(s != NULL);

	if (*s != NULL) {
		if (resizable(*s)) {
			free((*s)->raw);
		}

		free(*s);
		*s = NULL; /* avoid YADF */
	}
}

#if _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L

#include <stdio.h>
#include <stdarg.h>

int String_format_at(String s, unsigned offset, const char *fmt, ...)
{
	void *old_raw;
	va_list vargs;
	unsigned bytes_written;

	assert(s != NULL);
	assert(fmt != NULL);

	/* let's see if we're lucky */
	va_start(vargs, fmt);
	bytes_written = vsnprintf(s->raw + offset, s->size - offset, fmt, vargs) + 1;
	va_end(vargs);

	if (bytes_written > s->size) {
		/* ops */
		old_raw = resize_and_cpy(s, offset + bytes_written + 1, offset);

		if (NULL == old_raw) {
			return -1;
		}

		free(old_raw);

		/*
		 * if len < offset:
		 *     means len bytes will be kept untouched and (offset - len) bytes
		 *     have to be zeroed.
		 */
		if (s->len < offset) {
			memset(s->raw + s->len, 0, offset - s->len);
		}

		/* ok, second try... */
		va_start(vargs, fmt);
		bytes_written = vsnprintf(s->raw + offset, s->size - offset, fmt, vargs) + 1;
		va_end(vargs);

		if (bytes_written > s->size) {
			/* uh!? */
			return -1;
		}
	}

	s->len = offset + bytes_written;
	return 0;
}

#endif

static String String_alloc(void)
{
	String s;

	if ((s = malloc(sizeof(*s))) == NULL) {
		return NULL;
	}

	s->raw = NULL;
	s->size = 0;
	s->len = 0;
	s->resizable = YES;
	return s;
}

/* http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2 */
static unsigned round_up_to_the_next_power_of_2(unsigned num)
{
	num--;
	num |= num >> 1;
	num |= num >> 2;
	num |= num >> 4;
	num |= num >> 8;
	num |= num >> 16;
	num++;
	return  num += (num == 0);
}

static char *resize(String s, unsigned size)
{
	char *old_raw;

	if (!resizable(s)) {
		return NULL;
	}

	size = round_up_to_the_next_power_of_2(size);
	old_raw = s->raw;

	if ((s->raw = malloc(size)) == NULL) {
		s->raw = old_raw;
		return NULL;
	}

	s->size = size;

	if (old_raw == NULL) {
		return s->raw;
	}

	return old_raw;
}

static char *resize_and_cpy(String s, unsigned size, unsigned offset)
{
	char *old_raw;

	if ((old_raw = resize(s, size)) == NULL) {
		return NULL;
	}

	/*
	 * raw[len] is uninitialized because len == (strlen(raw) + 1), so
	 *
	 * if len == offset:
	 *     means len bytes will be kept untouched.
	 *
	 * if len < offset:
	 *     means len bytes will be kept untouched and (offset - len) bytes
	 *     have to be zeroed.
	 *
	 * if len > offset:
	 *     means that offset bytes will be kept untouched.
	 */
	if (s->len <= offset) {
		memmove(s->raw, old_raw, s->len);

	} else {
		memmove(s->raw, old_raw, offset);
	}

	return old_raw;
}

#if !(_XOPEN_SOURCE >= 700 || _POSIX_C_SOURCE >= 200809L)
static unsigned strnlen(const char *s, unsigned n)
{
	unsigned i = 0;

	while (s[i] != '\0' && i < n) {
		i++;
	}

	return i;
}
#endif
