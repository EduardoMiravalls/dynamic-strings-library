/*
 * File:    string_test.c
 * Author:  Eduardo Miravalls Sierra          <edu.miravalls@hotmail.com>
 *
 * Date:    2014-08-30 16:45
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "String_t.h"

void test_new(void)
{
	String s;
	char *src = "Hello World!\n";

	printf("%s: ", __func__);

	s = String_new(src, strlen(src));
	assert(0 == strcmp(String_raw(s), src));
	assert(String_length(s) == (strlen(src) + 1));

	String_free(&s);
	printf("passed!\n");
}

void test_ncpy(void)
{
	String s1;
	String s2;
	unsigned n = 6;
	char *str = "Hello World!\n";

	printf("%s: ", __func__);

	s1 = String_new(NULL, strlen(str));
	s2 = String_new_steal(str, 0);

	String_ncpy(s1, String_raw(s2), n);
	assert(String_nequals(s1, s2, n));

	String_free(&s2);
	String_free(&s1);
	printf("passed!\n");
}

void test_new_steal(void)
{
	String s1, s2, s3;
	char buff[] = "Hello World!\n";
	printf("%s: ", __func__);

	s1 = String_new_steal(buff, 2);
	s2 = String_new_steal(buff, strlen(buff));
	s3 = String_new_steal("abcdefghijklmnopqrstuvwxyz", 8);

	assert(0 == String_ncpy(s1, String_raw(s3), 8));
	assert(0 != String_ncpy(s2, String_raw(s3), 5));

	assert(0 == String_ncmp(s1, s3, 8));

	String_free(&s2);
	String_free(&s1);
	printf("passed!\n");
}

void test_ncpy_overlapping(void)
{
	String s;
	char *str = "Hello World!\n";

	printf("%s: ", __func__);

	s = String_new(str, strlen(str));

	String_ncpy_at(s, 6, String_raw(s), String_length(s) - 1);
	assert(0 == strcmp(String_raw(s), "Hello Hello World!\n"));

	String_free(&s);
	printf("passed!\n");
}

void test_ncpy_gap(void)
{
	String s;
	char *greeting = "Hello World!\n";
	char result[] = {
		'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\n', 0,
		0, 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\n', 0
	};

	printf("%s: ", __func__);

	s = String_new(greeting, strlen(greeting));

	String_ncpy_at(s, 16, String_raw(s), String_length(s) - 1);
	assert(0 == strcmp(String_raw(s), greeting));

	assert(String_length(s) == (16 + strlen(greeting) + 1));
	assert(0 == memcmp(String_raw(s), result, strlen(result) + 1));

	String_free(&s);
	printf("passed!\n");
}

void test_ncat_with_initial_string_empty(void)
{
	String s;
	char *greeting = "Hello World!\n";
	unsigned greeting_len = strlen(greeting);
	unsigned i;
	unsigned n = 5;

	printf("%s: ", __func__);

	s = String_new(NULL, strlen(greeting));

	for (i = 0; i < n; i++) {
		String_ncat(s, greeting, greeting_len);
	}

	assert(greeting_len * n + 1 == String_length(s));

	for (i = 0; i < n; i++) {
		assert(0 == memcmp(String_raw(s) + greeting_len * i,
		                   greeting,
		                   greeting_len));
	}

	String_free(&s);
	printf("passed!\n");
}

void test_ncat_with_initial_string(void)
{
	String s;
	char *greeting = "Hello World!\n";
	unsigned greeting_len = strlen(greeting);
	unsigned i;
	unsigned n = 5;

	printf("%s: ", __func__);

	s = String_new(greeting, strlen(greeting));

	for (i = 0; i < (n - 1); i++) {
		String_ncat(s, greeting, greeting_len);
	}

	assert(greeting_len * n + 1 == String_length(s));

	for (i = 0; i < n; i++) {
		assert(0 == memcmp(String_raw(s) + greeting_len * i,
		                   greeting,
		                   greeting_len));
	}

	String_free(&s);
	printf("passed!\n");
}

#if 0
void test_(void)
{
	String s;

	printf("%s: ", __func__);

	s = String_new(, );

	String_free(&s);
	printf("passed!\n");
}
#endif

int main()
{
	test_new();
	test_ncpy();
	test_ncpy_overlapping();
	test_ncpy_gap();
	test_ncat_with_initial_string_empty();
	test_ncat_with_initial_string();

	printf("All tests passed!\n");
	return 0;
}