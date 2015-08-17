/* Description:	Use to parse the seed for torrent info: includes basical
 *		B-coding info parsing.
 * Author:	Tristan Xiao
 * Data:	28/7/2015
 */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "seed_parse.h"

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

/*
 * parse example: 5:abcde --> len = 5, ptr = "abcde"
 * return the next parse position.
 */
struct b_string *b_string_alloc(void)
{
	struct b_string *ptr = malloc(sizeof(struct b_string));

	if (ptr) {
		ptr->len = 0;
		ptr->string = NULL;
		ptr->prev = NULL;
		return ptr;
	}
	else
		return NULL;
}

void b_string_free(struct b_string *ptr)
{
	if (ptr->string)
		free(ptr->string);
	free(ptr);
}

void b_string_set(struct b_string *ptr, char *buf)
{
	ptr->string = buf;
}

char *b_string_get(struct b_string *ptr)
{
	return ptr->string;
}

void b_string_print(struct b_string *ptr)
{
	TRACE(INFO, "%s\n", ptr->string);
}

char *b_string_parse(char *buf, struct b_string *target)
{
	char *ptr = buf;
	char *s = NULL;
	char *endptr = NULL;
	char tmp[INTEGER_LEN + 1] = {};
	long val = 0;

	assert(buf != NULL);
	assert(target != NULL);
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
}

struct b_int *b_int_alloc(void)
{
	struct b_int *ptr = malloc(sizeof(struct b_int));

	if (ptr){
		ptr->val = 0;
		ptr->prev = NULL;
		return ptr;
	}
	else
		return NULL;
}

void b_int_free(struct b_int *ptr)
{
	free(ptr);
}

void b_int_set(struct b_int *ptr, unsigned int val)
{
	ptr->val = val;
}

unsigned int b_int_get(struct b_int *ptr)
{
	return ptr->val;
}

void b_int_print(struct b_int *ptr)
{
	TRACE(INFO, "%d\n", ptr->val);
}

char *b_int_parse(char *buf, struct b_int *ret)
{
	char *ptr = buf;
	char *end = NULL;
	char tmp[INTEGER_LEN] = {};

	assert(ret != NULL);
	assert(buf != NULL);

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

struct l_entry *l_entry_alloc(char type)
{
	struct l_entry *ptr = malloc(sizeof(struct l_entry));

	if (ptr) {
		ptr->type = type;
		ptr->entry = NULL;
		INIT_LIST_HEAD(&ptr->head);
		return ptr;
	}
	else
		return NULL;
}

void l_entry_free(struct l_entry *ptr)
{
	assert(ptr != NULL);

	switch (ptr->type) {
	case 'l':
		b_list_free((struct b_list *)(ptr->entry));
		break;
	case 'i':
		b_int_free((struct b_int *)(ptr->entry));
		break;
	case 's':
		b_string_free((struct b_string *)(ptr->entry));
		break;
	case 'd':
		b_dict_free((struct b_dict *)(ptr->entry));
		break;
	default:
		TRACE(ERROR, "Error list entry type\n");
	}
	free(ptr);
}

void l_entry_print(struct l_entry *ptr)
{
	assert(ptr != NULL);
	switch (ptr->type) {
	case 'l':
		b_list_print((struct b_list *)(ptr->entry));
		break;
	case 'i':
		b_int_print((struct b_int *)(ptr->entry));
		break;
	case 's':
		b_string_print((struct b_string *)(ptr->entry));
		break;
	case 'd':
		b_dict_print((struct b_dict *)(ptr->entry));
		break;
	default:
		TRACE(ERROR, "Error list entry type\n");
	}
}

struct b_list *b_list_alloc(void)
{
	struct b_list *ptr = malloc(sizeof(struct b_list));
	if (ptr) {
		INIT_LIST_HEAD(&ptr->l_list);
		ptr->prev = NULL;
		return ptr;
	}
	else
		return NULL;
}

void b_list_add(void *entry, struct b_list *list_ptr, char type)
{
	struct l_entry *ptr = l_entry_alloc(type);

	if (ptr) {
		ptr->entry = entry;
		switch (type) {
		case 'l':
		{
			struct b_list *tmp_list = (struct b_list *)entry;
			tmp_list->prev = (void *)ptr;
			break;
		}
		case 'i':
		{
			struct b_int *tmp_int = (struct b_int *)entry;
			tmp_int->prev = (void *)ptr;
			break;
		}
		case 's':
		{
			struct b_string *tmp_str = (struct b_string *)entry;
			tmp_str->prev = (void *)ptr;
			break;
		}
		case 'd':
		{
			struct b_dict *tmp_dict = (struct b_dict *)entry;
			tmp_dict->prev = (void *)ptr;
			break;
		}
		default:
			TRACE(ERROR, "Error list entry type\n");
			goto DEAL_ERR;
		}
		list_add_tail(&ptr->head, &list_ptr->l_list);
	}
	else
		TRACE(ERROR, "No memory to alloc l_entry\n");
DEAL_ERR:
	return;
}

void b_list_del(void *entry)
{
	struct l_entry *ptr = (struct l_entry *)entry;
	l_entry_free(ptr);
}

void b_list_free(struct b_list *ptr)
{
	struct l_entry *tmp;
	struct l_entry *vec;

	list_for_each_entry_safe(vec, tmp, &ptr->l_list, head)
		l_entry_free(vec);
	free(ptr);
}

char *b_list_parse(char *buf, struct b_list *ret)
{
	char *ptr = buf;
	assert(buf != NULL);
	assert(ret != NULL);

	if (*ptr != 'l')
		goto DEAL_ERR;
	ptr ++;
	while (*ptr != 'e') {
		switch (*ptr) {
		case 'l':
		{
			struct b_list *new_list = b_list_alloc();
			if (new_list) {
				ptr = b_list_parse(ptr, new_list);
				if (ptr)
					b_list_add((void *)new_list, ret, 'l');
				else {
					b_list_free(new_list);
					goto DEAL_ERR;
				}
			}
			break;
		}	
		case 'i':
		{
			struct b_int *new_int = b_int_alloc();
			if (new_int) {
				ptr = b_int_parse(ptr, new_int);
				if (ptr)
					b_list_add((void *)new_int, ret, 'i');
				else {
					b_int_free(new_int);
					goto DEAL_ERR;
				}
			}
			break;
		}
		case 'd':
		{
			struct b_dict *new_dict = b_dict_alloc();
			if (new_dict) {
				ptr = b_dict_parse(ptr, new_dict);
				if (ptr)
					b_list_add((void *)new_dict, ret, 'd');
				else {
					b_dict_free(new_dict);
					goto DEAL_ERR;
				}
			}
			break;
		}
		default:
		{
			struct b_string *new_str = b_string_alloc();
			if (new_str) {
				ptr = b_string_parse(ptr, new_str);
				if (ptr)
					b_list_add((void *)new_str, ret, 's');
				else {
					b_string_free(new_str);
					goto DEAL_ERR;
				}
			}
			break;
		}
		}
	}
	return (ptr + 1);
DEAL_ERR:
	return NULL;
}

void b_list_print(struct b_list *ret)
{
	struct l_entry *ptr;

	TRACE(DUMP, "[ List Print begin ]\n");
	list_for_each_entry(ptr, &(ret->l_list), head) {
		l_entry_print(ptr);
	}
	TRACE(DUMP, "[ List Print end ]\n");
}

struct d_entry *d_entry_alloc(char *key, char type)
{
	struct d_entry *ptr = malloc(sizeof(struct d_entry));
	if (ptr) {
		INIT_LIST_HEAD(&ptr->head);
		ptr->type = type;
		memset(ptr->key, 0, KEY_LEN);
		strncpy(ptr->key, key, KEY_LEN);
		return ptr;
	}
	else
		return NULL;
}

void d_entry_free(struct d_entry *ptr)
{
	assert(ptr != NULL);
	switch (ptr->type) {
	case 'l':
		b_list_free((struct b_list *)(ptr->val));
		break;
	case 'i':
		b_int_free((struct b_int *)(ptr->val));
		break;
	case 's':
		b_string_free((struct b_string *)(ptr->val));
		break;
	case 'd':
		b_dict_free((struct b_dict *)(ptr->val));
		break;
	default:
		TRACE(ERROR, "Error dictionary entry type\n");
	}
	free(ptr);
}

void d_entry_print(struct d_entry *ptr)
{
	assert(ptr != NULL);
	switch (ptr->type) {
	case 'l':
		b_list_print((struct b_list *)(ptr->val));
		break;
	case 'i':
		b_int_print((struct b_int *)(ptr->val));
		break;
	case 's':
		b_string_print((struct b_string *)(ptr->val));
		break;
	case 'd':
		b_dict_print((struct b_dict *)(ptr->val));
		break;
	default:
		TRACE(ERROR, "Error list entry type\n");
	}
}

struct b_dict *b_dict_alloc(void)
{
	struct b_dict *ptr = malloc(sizeof(struct b_dict));
	if (ptr) {
		INIT_LIST_HEAD(&ptr->d_list);
		ptr->prev = NULL;
		return ptr;
	}
	else
		return NULL;
}

void b_dict_add(void *entry, char *key, struct b_dict *d_ptr, char type)
{
	struct d_entry *ptr = d_entry_alloc(key, type);

	if (ptr) {
		ptr->val = entry;
		switch (type) {
		case 'l':
		{
			struct b_list *tmp_list = (struct b_list *)entry;
			tmp_list->prev = (void *)ptr;
			break;
		}
		case 'i':
		{
			struct b_int *tmp_int = (struct b_int *)entry;
			tmp_int->prev = (void *)ptr;
			break;
		}
		case 's':
		{
			struct b_string *tmp_str = (struct b_string *)entry;
			tmp_str->prev = (void *)ptr;
			break;
		}
		case 'd':
		{
			struct b_dict *tmp_dict = (struct b_dict *)entry;
			tmp_dict->prev = (void *)ptr;
			break;
		}
		default:
			TRACE(ERROR, "Error list entry type\n");
			goto DEAL_ERR;
		}
		list_add_tail(&ptr->head, &d_ptr->d_list);
	}
	else
		TRACE(ERROR, "No memory to alloc l_entry\n");
DEAL_ERR:
	return;
}

void b_dict_del(void *entry)
{
	struct d_entry *ptr = (struct d_entry *)entry;
	d_entry_free(ptr);
}

char *b_dict_parse(char *buf, struct b_dict *ret)
{
	char *ptr = buf;
	assert(buf != NULL);
	assert(ret != NULL);

	if (*ptr != 'd')
		goto DEAL_ERR;
	ptr ++;
	while (*ptr != 'e') {
		/* get the key name from the buf first */
		struct b_string *new_key = b_string_alloc();
		if (new_key) {
			ptr = b_string_parse(ptr, new_key);
			if (!ptr) {
				b_string_free(new_key);
				goto DEAL_ERR;
			}
		}
		else
			goto DEAL_ERR;

		switch (*ptr) {
		case 'l':
		{
			struct b_list *new_list = b_list_alloc();
			if (new_list) {
				ptr = b_list_parse(ptr, new_list);
				if (ptr)
					b_dict_add((void *)new_list, b_string_get(new_key), ret, 'l');
				else {
					b_string_free(new_key);
					b_list_free(new_list);
					goto DEAL_ERR;
				}
			}
			break;
		}
		case 'i':
		{
			struct b_int *new_int = b_int_alloc();
			if (new_int) {
				ptr = b_int_parse(ptr, new_int);
				if (ptr)
					b_dict_add((void *)new_int, b_string_get(new_key), ret, 'i');
				else {
					b_string_free(new_key);
					b_int_free(new_int);
					goto DEAL_ERR;
				}
			}
			break;
		}
		case 'd':
		{
			struct b_dict *new_dict = b_dict_alloc();
			if (new_dict) {
				ptr = b_dict_parse(ptr, new_dict);
				if (ptr)
					b_dict_add((void *)new_dict, b_string_get(new_key), ret, 'd');
				else {
					b_string_free(new_key);
					b_dict_free(new_dict);
					goto DEAL_ERR;
				}
			}
			break;
		}
		default:
		{
			struct b_string *new_str = b_string_alloc();

			if (new_str) {
				ptr = b_string_parse(ptr, new_str);
				if (ptr)
					b_dict_add((void *)new_str, b_string_get(new_key), ret, 's');
				else {
					b_string_free(new_key);
					b_string_free(new_str);
					TRACE(ERROR, "Error list entry type\n");
					goto DEAL_ERR;
				}
			}
			break;
		}
		}
		b_string_free(new_key);
	}
	return (ptr + 1);
DEAL_ERR:
	return NULL;
}

void b_dict_free(struct b_dict *ptr)
{
	struct d_entry *tmp;
	struct d_entry *vec;

	list_for_each_entry_safe(vec, tmp, &ptr->d_list, head)
		d_entry_free(vec);
	free(ptr);
}

void b_dict_print(struct b_dict *ptr)
{
	struct d_entry *vec;
	int i = 0;

	TRACE(DUMP, "[ Dictionary Print begin ]\n");
	list_for_each_entry(vec, &ptr->d_list, head) {
		TRACE(DUMP, "[ (%d) Key(%c) ] %s\n", i++, vec->type, vec->key);
		d_entry_print(vec);
	}
	TRACE(DUMP, "[ Dictionary Print end ]\n");
}

#ifdef _UNIT_TEST
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include "unittest/cmockery.h"

/* case 1: feature test */
void feature_test(void **state) {
	struct b_int *a = b_int_alloc();
	struct b_string *b = b_string_alloc();
	struct b_list *c = b_list_alloc();
	struct b_dict *d = b_dict_alloc();
	char buf[] = "i12345e4:eeeel1:ai1234e1:bi4321eed7tristanli1234e4:eeeeee";
	char *ptr = buf;

	ptr = b_int_parse(ptr, a);
	b_int_print(a);
	ptr = b_string_parse(ptr, b);
	b_string_print(b);
	ptr = b_list_parse(ptr, c);
	b_list_print(c);
	ptr = b_dict_parse(ptr, d);
	b_dict_print(d);

	b_int_free(a);
	b_string_free(b);
	b_list_free(c);
	b_dict_free(d);
}
#if 0
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
#endif

int main(int argc, char **argv)
{
	const UnitTest tests[] = {
	    unit_test(feature_test),
	#if 0
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
	    //unit_test(list_feature_test),
	};

	run_tests(tests);
	exit(EXIT_SUCCESS);
}

#endif
