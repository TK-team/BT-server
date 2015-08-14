#ifndef _SEED_PARSE_H
#define _SEED_PARSE_H

#include "bttrace.h"
#include "list.h"

#define INTEGER_LEN		20
#define FILE_NAME_LEN		256
#define URL_LEN			512
#define COMMENT_LEN		1024
#define NAME_DEFAULT_LEN	256
#define KEY_LEN			128

struct b_string {
	struct list_head head;
	unsigned int len;
	char *string;
};

struct b_int {
	struct list_head head;
	unsigned int val;
};

struct l_entry {
	struct list_head head;
	void *entry;
	char type; /* 'l': list; 's': string; 'i': integer; 'd': dictionary */
};

struct b_list {
	struct list_head head;
	struct list_head l_list; /* list of l_entry */
};

struct d_entry {
	struct list_head head;
	char key[KEY_LEN];
	void *val;
	char type; /* 'l': list; 's': string; 'i': integer; 'd': dictionary */
};

struct b_dict {
	void *prev; /* point to the l_entry or dentry */
	struct list_head d_list; /* list of d_entry */
};

#if 0
struct b_code_operation {
	(void *)(*alloc) (void),
	(void *)(*free) (void *),
	void (*add) (void *, void *),
	void (*del) (void *)
	void (*free_all) (void),
	char *(*parse) (char *, void *),
};
#endif

/* Torrent relate structure */
//TODO: need to reconsitution
struct multi_files {
	struct list_head head;
	unsigned long long length;
	char path[FILE_NAME_LEN];
	char path_utf8[FILE_NAME_LEN];
};

struct info_multi_file {
	struct list_head files_list;
};

struct torrent_info {
	unsigned int piece_len;
	char *piece_hash;
	unsigned char privated;
	struct info_multi_file multi;
	char name[FILE_NAME_LEN];
	char name_utf8[FILE_NAME_LEN];
	unsigned long long length;
	char publisher[NAME_DEFAULT_LEN];
	char publisher_utf8[NAME_DEFAULT_LEN];
	char publisher_url[URL_LEN];
	char publisher_url_utf8[URL_LEN];
};

struct torrent {
	char announce[URL_LEN];
	struct list_head *announce_list;
	char comment[COMMENT_LEN];
	char comment_utf8[COMMENT_LEN];
	unsigned int create_date;
	unsigned int encoding;
	struct torrent_info info;
};

extern struct b_string *b_string_alloc(void);
extern void b_string_free(struct b_string *ptr);
extern void b_string_set(struct b_string *ptr, char *buf);
extern char *b_string_get(struct b_string *ptr);
extern void b_string_print(struct b_string *ptr);
extern char *b_string_parse(char *buf, struct b_string *target);
extern struct b_int *b_int_alloc(void);
extern void b_int_free(struct b_int *ptr);
extern void b_int_set(struct b_int *ptr, unsigned int val);
extern unsigned int b_int_get(struct b_int *ptr);
extern void b_int_print(struct b_int *ptr);
extern char *b_int_parse(char *buf, struct b_int *ret);
extern struct l_entry *l_entry_alloc(char type);
extern void l_entry_free(struct l_entry *ptr);
extern void l_entry_print(struct l_entry *ptr);
extern struct b_list *b_list_alloc(void);
extern void b_list_add(void *entry, struct b_list *list, char type);
extern void b_list_del(void *entry);
extern void b_list_free(struct b_list *ptr);
extern char *b_list_parse(char *buf, struct b_list *ret);
extern void b_list_print(struct b_list *ret);
extern struct d_entry *d_entry_alloc(char *key, char type);
extern void d_entry_free(struct d_entry *ptr);
extern void d_entry_print(struct d_entry *ptr);
extern struct b_dict *b_dict_alloc(void);
extern void b_dict_add(void *ptr, char *key, struct b_dict *d_ptr, char type);
extern void b_dict_del(void *entry);
extern char *b_dict_parse(char *buf, struct b_dict *ret);
extern void b_dict_free(struct b_dict *ptr);
extern void b_dict_print(struct b_dict *ptr);

#endif /* _SEED_PARSE_H */
