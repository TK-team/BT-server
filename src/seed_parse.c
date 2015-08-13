/* Description:	Use to parse the seed for torrent info: includes basical
 *		B-coding info parsing.
 * Author:	Tristan Xiao
 * Data:	28/7/2015
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "bttrace.h"
#include "list.h"
#define INTEGER_LEN 20

#ifdef _UNIT_TEST
extern void mock_assert(const int result, const char* const expression, 
		                        const char * const file, const int line);
#undef assert
#define assert(expression) \
	mock_assert((int)(expression), #expression, __FILE__, __LINE__);

extern void* _test_malloc(const size_t size, const char* file, const int line);
extern void* _test_calloc(const size_t number_of_elements, const size_t size, 
		                          const char* file, const int line);
extern void _test_free(void* const ptr, const char* file, const int line);

#define malloc(size) _test_malloc(size, __FILE__, __LINE__)
#define calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__)
#define free(ptr) _test_free(ptr, __FILE__, __LINE__)
#endif /* _UNIT_TEST */

struct b_string {
	struct list_head head;
	unsigned int len;
	char *string;
};

struct b_int {
	struct list_head head;
	unsigned int val;
};

struct b_list {
	struct list_head head;
	struct list_head int_list;
	struct list_head str_list;
};

struct b_dict {
	struct list_head head;
	struct list_head int_list;
	struct list_head str_list;
	struct list_head list_list;
};

/*
 * parse example: 5:abcde --> len = 5, ptr = "abcde"
 * return the next parse position.
 */
char *parse_string(char *buf, struct b_string *target)
{
	char *ptr = buf;
	char *s = NULL;
	char *endptr = NULL;
	char tmp[INTEGER_LEN + 1] = {};
	long val = 0;

	assert(buf);
	assert(target);
	s = strchr(ptr, ':');
	
	if (s) {
		memcpy(tmp, buf, (INTEGER_LEN > (s - buf) ? (s - buf) : INTEGER_LEN));
		val = strtol(tmp, &endptr, 10);
		
		if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
                   || (errno != 0 && val == 0)) {
			TRACE(ERROR, "strtol error\n"); 
			return NULL;
           	}

		if (endptr == ptr) {
			TRACE(ERROR, "No digits were found\n");
			return NULL;
		}
		target->len = val;
		target->string = malloc(sizeof(char) * (val + 1));
		memcpy(target->string, s + 1, val);
		*(target->string + val) = '\0';
		return (s + val + 1);
	}
	else {
		TRACE(ERROR, "B-coding string format error: %s\n", buf);
		return NULL;
	}
	return 0;
}

char *parse_int(char *buf, struct b_int *ret)
{
	char *ptr = buf;
	char *end = NULL;
	char tmp[INTEGER_LEN] = {};
	assert(buf);
	assert(ret);

	unsigned int len = 0;
	end = strchr(ptr, 'e');

	if (end) {
		char *endptr = NULL;
		len = end - (++ptr) + 1;
		memcpy(tmp, ptr, (INTEGER_LEN > len ? len : INTEGER_LEN));
		ret->val = strtol(tmp, &endptr, 10);

		if ((errno == ERANGE && (ret->val == LONG_MAX || ret->val == LONG_MIN))
		   || (errno != 0 && ret->val == 0)) {
			TRACE(ERROR, "strtol error\n"); 
			return NULL;
		}

		if (endptr == ptr) {
			TRACE(ERROR, "No digits were found\n");
			return NULL;
		}
		return (end + 1);
	}
	else {
		TRACE(ERROR, "B-coding string format error: %s\n", buf);
		return NULL;
	}
}

int init_b_list(struct b_list *list)
{
	INIT_LIST_HEAD(&list->int_list);
	INIT_LIST_HEAD(&list->str_list);
	return 0;
}

int free_b_list(struct b_list *list)
{
	struct b_int *tmp_int;
	struct b_int *vec_int;
	struct b_string *tmp_str;
	struct b_string *vec_str;

	list_for_each_entry_safe(vec_int, tmp_int, &list->int_list, head) {
		free(vec_int);
	}
	list_for_each_entry_safe(vec_str, tmp_str, &list->str_list, head) {
		if (vec_str->string)
			free(vec_str->string);
		free(vec_str);
	}
	return 0;
}

void print_list(struct b_list *ret)
{
	struct b_string *str_v;
	struct b_int *int_v;

	list_for_each_entry(str_v, &(ret->str_list), head) {
		TRACE(ERROR, "String: %s\n", str_v->string);
	}

	list_for_each_entry(int_v, &(ret->int_list), head) {
		TRACE(ERROR, "Integer: %d\n", int_v->val);
	}
}

char *parse_list(char *buf, struct b_list *ret)
{
	char *ptr = buf;
	assert(buf);
	assert(ret);

	ptr ++;
	while (*ptr != 'e') {
		if (*ptr == 'i') {
			struct b_int *vec = malloc(sizeof(struct b_int));

			list_add(&vec->head, &ret->int_list);
			ptr = parse_int(ptr, vec);
		} 
		else if (isdigit(*ptr))
		{
			struct b_string *vec = malloc(sizeof(struct b_string));

			list_add(&vec->head, &ret->str_list);
			ptr = parse_string(ptr, vec);
		}
	}
	return (ptr + 1);
}

#ifdef _UNIT_TEST
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include "unittest/cmockery.h"
/* case 1: feature test */
void string_feature_test(void **state) {
	struct b_string a;
	struct b_int ret = {};
	char s[] = "5:abcde";

	parse_string(s, &a);
	parse_int("i1234e", &ret);
	TRACE(ERROR, "string length %d, context %s\ninteger %d\n", a.len, a.string, ret.val);
	free(a.string);
}

/* case 2: parameters test */
void string_null_parameters_test(void **state) {
	struct b_string a;
	expect_assert_failure(parse_string("5:abcde", NULL));
	expect_assert_failure(parse_string(NULL, &a));
	expect_assert_failure(parse_int("5:abcde", NULL));
	expect_assert_failure(parse_int(NULL, NULL));
}

/* case 3: bad parameters test */
void string_bad_parameters_test_1(void **state) {
	struct b_string a = {};
	char s[] = "x5:abcde";

	parse_string(s, &a);
	TRACE(ERROR, "string length %d, context %s\n", a.len, a.string);
	if (a.string)
		free(a.string);
}

/* case 4: bad parameters test */
void string_bad_parameters_test_2(void **state) {
	struct b_string a = {};
	char s[] = "5abcde";

	parse_string(s, &a);
	TRACE(ERROR, "string length %d, context %s\n", a.len, a.string);
	if (a.string)
		free(a.string);
}

/* case 5: bad parameters test */
void string_bad_parameters_test_3(void **state) {
	struct b_string a = {};

	parse_string(":12345", &a);
	TRACE(ERROR, "string length %d, context %s\n", a.len, a.string);
	if (a.string)
		free(a.string);
}

/* case 6: bad parameters test */
void string_bad_parameters_test_4(void **state) {
	struct b_string a = {};
	char s[] = "20:1231231asdfasdfl;asdf\nasdf\n%%12345";

	parse_string(s, &a);
	TRACE(ERROR, "string length %d, context %s\n", a.len, a.string);
	if (a.string)
		free(a.string);
}

/* case 7: bad parameters test */
void int_bad_parameters_test_1(void **state) {
	struct b_int ret = {};
	char input[] = "1231241";

	parse_int(input, &ret);
	TRACE(ERROR, "Input %s\t val %d\n", input, ret.val);
}

/* case 8: bad parameters test */
void int_bad_parameters_test_2(void **state) {
	struct b_int ret = {};
	char input[] = "i1231241";

	parse_int(input, &ret);
	TRACE(ERROR, "Input %s\t val %d\n", input, ret.val);
}

/* case 9: bad parameters test */
void int_bad_parameters_test_3(void **state) {
	struct b_int ret = {};
	char input[] = "1231241e";

	parse_int(input, &ret);
	TRACE(ERROR, "Input %s\t val %d\n", input, ret.val);
}

/* case 10: bad parameters test */
void int_bad_parameters_test_4(void **state) {
	struct b_int ret = {};
	char input[] = "i1231231231231231231231241e";

	parse_int(input, &ret);
	TRACE(ERROR, "Input %s\t val %d\n", input, ret.val);
}

/* case 11: bad parameters test */
void int_bad_parameters_test_5(void **state) {
	struct b_int ret = {};
	char input[] = "i1231i23123e1231231231231241e";

	parse_int(input, &ret);
	TRACE(ERROR, "Input %s\t val %d\n", input, ret.val);
}

/* case 11: feature test */
void list_feature_test(void **state) {
	struct b_list ret;
	char input[] = "li100e3:123e";

	init_b_list(&ret);
	parse_list(input, &ret);
	print_list(&ret);
	free_b_list(&ret);
}

int main(int argc, char **argv)
{
	const UnitTest tests[] = {
	#if 0
	    unit_test(string_feature_test),
	    unit_test(string_null_parameters_test),
	    unit_test(string_bad_parameters_test_1),
	    unit_test(string_bad_parameters_test_2),
	    unit_test(string_bad_parameters_test_3),
	    unit_test(string_bad_parameters_test_4),
	    unit_test(int_bad_parameters_test_1),
	    unit_test(int_bad_parameters_test_2),
	    unit_test(int_bad_parameters_test_3),
	    unit_test(int_bad_parameters_test_4),
	    unit_test(int_bad_parameters_test_5),
	#endif
	    unit_test(list_feature_test),
	};

	run_tests(tests);
	exit(EXIT_SUCCESS);
}

#endif
