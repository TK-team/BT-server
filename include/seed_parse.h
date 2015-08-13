#ifndef _SEED_PARSE_H
#define _SEED_PARSE_H

#include "bttrace.h"
#include "list.h"

#define INTEGER_LEN		20
#define FILE_NAME_LEN		256
#define URL_LEN			512
#define COMMENT_LEN		1024
#define NAME_DEFAULT_LEN	256

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

#endif /* _SEED_PARSE_H */
