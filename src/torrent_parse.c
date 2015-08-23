#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "torrent.h"

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


struct multi_files *multi_files_alloc(struct b_dict *dict)
{
	struct multi_files *ptr = malloc(sizeof(struct multi_files));

	if (ptr && dict) {
		struct d_entry *vec = NULL;
		struct d_entry *tmp = NULL;

		memset(ptr, 0, sizeof(struct multi_files));
		list_for_each_entry_safe(vec, tmp, &dict->d_list, head) {
			if (CMP_EQUAL == strncmp(vec->key, "path", KEY_LEN) && vec->type == 's') {
				ptr->path = (struct b_string *)(vec->val);
				vec->val = NULL;
				continue;
			}
			if (CMP_EQUAL == strncmp(vec->key, "path.utf-8", KEY_LEN) && vec->type == 's') {
				ptr->path_utf8 = (struct b_string *)(vec->val);
				vec->val = NULL;
				continue;
			}
			if (CMP_EQUAL == strncmp(vec->key, "length", KEY_LEN) && vec->type == 'i') {
				ptr->length = b_int_get((struct b_int *)(vec->val));
				continue;
			}
		}
		return ptr;
	}
	else {
		TRACE(ERROR, "No memory to alloc multi_files\n");
		return NULL;
	}
}

void multi_files_free(struct multi_files *ptr)
{
	if (ptr) {
		b_string_free(ptr->path);
		b_string_free(ptr->path_utf8);
		free(ptr);
	}
}

void multi_files_add(struct multi_files *ptr, struct info_multi_file *head)
{
	list_add_tail(&ptr->head, &head->files_list);
}

void info_multi_files_free(struct info_multi_file *ptr)
{
	if (ptr) {
		struct multi_files *tmp;
		struct multi_files *vec;

		list_for_each_entry_safe(vec, tmp, &ptr->files_list, head)
			multi_files_free(vec);

		b_string_free(ptr->dir_name);
	}
}

struct torrent *torrent_alloc(void)
{
	struct torrent *ptr = malloc(sizeof(struct torrent));

	if (ptr) {
		memset(ptr, 0, sizeof(struct torrent));
		INIT_LIST_HEAD(&ptr->info.multi.files_list);
		return ptr;
	}
	return NULL;
}

int torrent_free(struct torrent *ptr)
{
	if (ptr) {
		b_string_free(ptr->announce);
		b_string_free(ptr->comment);
		b_string_free(ptr->comment_utf8);
		b_list_free(ptr->announce_list);
		b_string_free(ptr->info.piece_hash);
		b_string_free(ptr->info.name);
		b_string_free(ptr->info.name_utf8);
		b_string_free(ptr->info.single_md5);
		b_string_free(ptr->info.publisher);
		b_string_free(ptr->info.publisher_utf8);
		b_string_free(ptr->info.publisher_url);
		b_string_free(ptr->info.publisher_url_utf8);
		info_multi_files_free(&ptr->info.multi);
		free(ptr);
	}
	else
		TRACE(ERROR, "Duplicated free torrent %p\n", ptr);
	return 0;
}

int torrent_info_parse(struct b_dict *info_dict, struct torrent_info *info)
{
	struct d_entry *vec;
	struct d_entry *tmp;

	list_for_each_entry_safe(vec, tmp, &info_dict->d_list, head) {
		if (CMP_EQUAL == strncmp(vec->key, "piece length", KEY_LEN) && vec->type == 'i') {
			info->piece_len = b_int_get((struct b_int *)(vec->val));
			continue;
		}
		if (CMP_EQUAL == strncmp(vec->key, "pieces", KEY_LEN) && vec->type == 's') {
			info->piece_hash = (struct b_string *)(vec->val);
			vec->val = NULL;
			continue;
		}
		if (CMP_EQUAL == strncmp(vec->key, "name", KEY_LEN) && vec->type == 's') {
			info->name = (struct b_string *)(vec->val);
			vec->val = NULL;
			continue;
		}
		if (CMP_EQUAL == strncmp(vec->key, "name.utf-8", KEY_LEN) && vec->type == 's') {
			info->name = (struct b_string *)(vec->val);
			vec->val = NULL;
			continue;
		}
		if (CMP_EQUAL == strncmp(vec->key, "length", KEY_LEN) && vec->type == 'i') {
			info->length = b_int_get((struct b_int *)(vec->val));
			continue;
		}
		if (CMP_EQUAL == strncmp(vec->key, "publisher", KEY_LEN) && vec->type == 's') {
			info->publisher = (struct b_string *)(vec->val);
			vec->val = NULL;
			continue;
		}
		if (CMP_EQUAL == strncmp(vec->key, "publisher utf-8", KEY_LEN) && vec->type == 's') {
			info->publisher_utf8 = (struct b_string *)(vec->val);
			vec->val = NULL;
			continue;
		}
		if (CMP_EQUAL == strncmp(vec->key, "publisher url", KEY_LEN) && vec->type == 's') {
			info->publisher_url = (struct b_string *)(vec->val);
			vec->val = NULL;
			continue;
		}
		if (CMP_EQUAL == strncmp(vec->key, "publisher url utf-8", KEY_LEN) && vec->type == 's') {
			info->publisher_url_utf8 = (struct b_string *)(vec->val);
			vec->val = NULL;
			continue;
		}
		if (CMP_EQUAL == strncmp(vec->key, "files", KEY_LEN) && vec->type == 'l') {
			struct l_entry *vec_l = NULL;
			struct l_entry *tmp_l = NULL;
			list_for_each_entry_safe(vec_l, tmp_l, &(((struct b_list *)(vec->val))->l_list), head) {
				if (vec_l->type == 'd') {
					struct multi_files *files_ptr = multi_files_alloc(
							(struct b_dict *)(vec_l->entry));
					if (files_ptr) {
						multi_files_add(files_ptr, &info->multi);
					}
					else {
						return -1;
					}
				}
			}
			vec->val = NULL;
			continue;
		}
	}
	return 0;
}

int torrent_parse(char *buf, struct torrent *ptr)
{
	struct b_dict *dict_ptr = b_dict_alloc();

	assert(buf != NULL);
	if (dict_ptr) {
		struct d_entry *vec;
		struct d_entry *tmp;
		char *t = b_dict_parse(buf, dict_ptr);
		if (!t) {
			b_dict_free(dict_ptr);
			return -1;
		}

		list_for_each_entry_safe(vec, tmp, &dict_ptr->d_list, head) {
			if (CMP_EQUAL == strncmp(vec->key, "announce", KEY_LEN) && vec->type == 's') {
				ptr->announce = (struct b_string *)vec->val;
				vec->val = NULL;
				continue;
			}
			if (CMP_EQUAL == strncmp(vec->key, "announce-list", KEY_LEN) && vec->type == 'l') {
				ptr->announce_list = (struct b_list *)vec->val;
				vec->val = NULL;
				continue;
			}
			if (CMP_EQUAL == strncmp(vec->key, "comment", KEY_LEN) && vec->type == 's') {
				ptr->comment = (struct b_string *)vec->val;
				vec->val = NULL;
				continue;
			}
			if (CMP_EQUAL == strncmp(vec->key, "comment-utf8", KEY_LEN) && vec->type == 's') {
				ptr->comment_utf8 = (struct b_string *)vec->val;
				vec->val = NULL;
				continue;
			}
			if (CMP_EQUAL == strncmp(vec->key, "comment", KEY_LEN) && vec->type == 's') {
				ptr->comment = (struct b_string *)vec->val;
				vec->val = NULL;
				continue;
			}
			if (CMP_EQUAL == strncmp(vec->key, "create by", KEY_LEN) && vec->type == 's') {
				ptr->create_by = (struct b_string *)vec->val;
				vec->val = NULL;
				continue;
			}
			if (CMP_EQUAL == strncmp(vec->key, "creation date", KEY_LEN) && vec->type == 'i') {
				ptr->create_date = b_int_get((struct b_int *)(vec->val));
				continue;
			}
			if (CMP_EQUAL == strncmp(vec->key, "comment", KEY_LEN) && vec->type == 's') {
				ptr->comment = (struct b_string *)vec->val;
				vec->val = NULL;
				continue;
			}
			if (CMP_EQUAL == strncmp(vec->key, "comment-utf8", KEY_LEN) && vec->type == 's') {
				ptr->comment_utf8 = (struct b_string *)vec->val;
				vec->val = NULL;
				continue;
			}
			if (CMP_EQUAL == strncmp(vec->key, "info", KEY_LEN) && vec->type == 'd') {
				if (0 == torrent_info_parse((struct b_dict *)vec->val, &ptr->info))
					continue;
				else {
					b_dict_free(dict_ptr);
					return -1;
				}
			}
		}
		b_dict_free(dict_ptr);
		return 0;
	}
	else
		return -1;
}

void torrent_info_print(struct torren_info *ptr)
{

}
void torrent_print(struct torrent *ptr)
{ 
	time_t t = (time_t )(ptr->create_date);

	TRACE(INFO, "[ Torrent Info BEGIN ]\n");
	if (ptr->announce)
		TRACE(INFO, "[ announce ]:\n%s\n", b_string_get(ptr->announce));
	//TRACE(INFO, "[ announce list ]:\n%s\n", b_string_get(ptr->announce_list));
	if (ptr->announce_list)
		b_list_print(ptr->announce_list);
	if (ptr->comment)
		TRACE(INFO, "[ comment ]:\n%s\n", b_string_get(ptr->comment));
	if (ptr->comment_utf8)
		TRACE(INFO, "[ comment(utf8) ]:\n%s\n", b_string_get(ptr->comment_utf8));
	if (ptr->create_by)
		TRACE(INFO, "[ create by ]:\n%s\n", b_string_get(ptr->create_by));
	if (t)
		TRACE(INFO, "[ create date ]:\n%s\n", ctime(&t));
	if (ptr->encoding)
		TRACE(INFO, "[ encoding ]:\n%d\n", ptr->encoding);
	torrent_info_print(&ptr->info);
	TRACE(INFO, "[ Torrent Info END ]\n");
}

#ifdef _UNIT_TEST

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include "unittest/cmockery.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/* case 1: torrent parse test */
void torrent_test_1(void **state) {
	int fd = 0;
	char *buf = NULL;
	struct torrent * t = NULL;
	struct stat t_stat;

	fd = open("./src/test.torrent", O_RDONLY);
	if (fd < 0) {
		return;
	}
	fstat(fd, &t_stat);

	buf = malloc(t_stat.st_size);
	memset(buf, 0, t_stat.st_size);
	read(fd, buf, t_stat.st_size);
	t = torrent_alloc();
	if (t) {
		torrent_parse(buf, t);
		torrent_print(t);
		torrent_free(t);
	}
	free(buf);
}

int main(int argc, char **argv)
{
	const UnitTest tests[] = {
	    unit_test(torrent_test_1),
	};

	run_tests(tests);
	exit(EXIT_SUCCESS);
}

#endif /* _UNIT_TEST */
