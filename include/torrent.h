#define FILE_NAME_LEN		256
#define URL_LEN			512
#define COMMENT_LEN		1024
#define NAME_DEFAULT_LEN	256

#include "bttrace.h"
#include "b_parse.h"

/* Torrent relate structure */
//TODO: need to reconsitution
struct multi_files {
	struct list_head head;
	unsigned long long length;
	struct b_string *path;
	struct b_string *path_utf8;
};

/* point to multi_files */
struct info_multi_file {
	struct list_head files_list;
	struct b_string name;
};

struct torrent_info {
	unsigned int piece_len;
	struct b_string *piece_hash;
	unsigned char privated;
	struct info_multi_file multi;
	struct b_string *name;
	struct b_string *name_utf8;
	unsigned long long length;
	struct b_string *publisher;
	struct b_string *publisher_utf8;
	struct b_string *publisher_url;
	struct b_string *publisher_url_utf8;
};

struct torrent {
	struct b_string *announce;
	struct b_list *announce_list;
	struct b_string *comment;
	struct b_string *comment_utf8;
	unsigned int create_date;
	unsigned int encoding;
	struct torrent_info info;
};

