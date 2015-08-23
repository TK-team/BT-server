#ifndef _TORRENT_H_
#define _TORRENT_H_

#include "bttrace.h"
#include "b_parse.h"

#define CMP_EQUAL			0

struct multi_files {
	struct list_head head;
	unsigned long long length;
	struct b_string *path;
	struct b_string *path_utf8;
};

/* point to multi_files */
struct info_multi_file {
	struct list_head files_list;
	struct b_string *dir_name; /* name of the dirctory for the files. */
};

struct torrent_info {
	unsigned int piece_len;
	struct b_string *piece_hash;
	unsigned char privated;
	struct info_multi_file multi;
	struct b_string *name;
	struct b_string *name_utf8;
	unsigned long long length;
	struct b_string *single_md5;
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
	struct b_string *create_by;
	unsigned int create_date;
	unsigned int encoding;
	struct torrent_info info;
};

#endif /* _TORRENT_H_ */
