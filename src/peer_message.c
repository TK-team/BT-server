#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

#include "peer_message.h"
#include "b_parse.h"
#include "list.h"

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

struct b_string *generate_handshake_message(char *info_hash, char *peer_id)
{
	struct b_string *ptr = NULL;
	char *buf = NULL;

	if (info_hash && peer_id) {
		ptr = b_string_alloc();
		if (ptr) {
			buf = malloc((HANDSHAKE_MESSAGE_LEN) * sizeof(char));
			b_string_set_head(ptr, buf);
			b_string_set(ptr, buf);
			memset(buf, 0, HANDSHAKE_MESSAGE_LEN);
			*buf = DEFAULT_PSTRLEN;
			memcpy(buf + 1, DEFAULT_PSTR, DEFAULT_PSTRLEN); 
			memset(buf + 1 + DEFAULT_PSTRLEN, 0, RESERVED_LEN);
			memcpy(buf + 1 + DEFAULT_PSTRLEN + RESERVED_LEN, 
					info_hash, HASH_LEN); 
			memcpy(buf + 1 + DEFAULT_PSTRLEN + RESERVED_LEN + HASH_LEN, 
					peer_id, PEER_ID_LEN); 
			b_string_set_length(ptr, HANDSHAKE_MESSAGE_LEN); 
			return ptr;
		}
	}
		return NULL;
}

/* need relloc: for piece message */
struct b_string *set_prefix_and_msgid(unsigned int len_prefix, 
		unsigned char message_id, struct b_string *payload)
{
	struct b_string *ptr = NULL;
	char *buf = NULL;
	unsigned int msg_id_len = 0;
	unsigned int tmp;

	if (len_prefix)
		msg_id_len = MSG_ID_LEN;

	if (payload) {
		ptr = payload;
		if ((b_string_get(ptr) - b_string_get_head(ptr)) > (LEN_PREFIX + msg_id_len))
			buf = b_string_get(ptr) - (LEN_PREFIX + msg_id_len);
		/* the reserved of string is not enough, realloc */
		else {
			char *b = NULL;

			b = b_string_get(ptr);
			buf = malloc((len_prefix + LEN_PREFIX) * sizeof(char));
			memcpy((buf + LEN_PREFIX + msg_id_len), b, len_prefix);
			b = b_string_get_head(ptr);
			free(b);
			b_string_set_head(ptr, buf);
			b_string_set(ptr, buf + LEN_PREFIX + msg_id_len);
		}
	}
	else {
		buf = malloc((len_prefix + LEN_PREFIX) * sizeof(char));
		ptr = b_string_alloc_reserved(buf, (len_prefix + LEN_PREFIX), 0);
	}
	tmp = htonl(len_prefix);
	memcpy(buf, &tmp, LEN_PREFIX);
	if (len_prefix)
		*(buf + LEN_PREFIX) = message_id;
	b_string_set(ptr, buf);
	b_string_set_length(ptr, len_prefix + LEN_PREFIX);

	return ptr;
}

struct b_string *generate_keep_alive_message(void)
{
	return set_prefix_and_msgid(0, PEER_MESSAGE_KEEP_ALIVE, NULL);
}

struct b_string *generate_choke_message(void)
{
	return set_prefix_and_msgid(MSG_ID_LEN, PEER_MESSAGE_CHOKE, NULL);
}

struct b_string *generate_unchoke_message(void)
{
	return set_prefix_and_msgid(MSG_ID_LEN, PEER_MESSAGE_UNCHOCKE, NULL);
}

struct b_string *generate_interested_message(void)
{
	return set_prefix_and_msgid(MSG_ID_LEN, PEER_MESSAGE_INTERESTED, NULL);
}

struct b_string *generate_not_interested_message(void)
{
	return set_prefix_and_msgid(MSG_ID_LEN, PEER_MESSAGE_NOT_INTERESTED, NULL);
}

struct b_string *generate_have_message(unsigned int piece_index)
{
	char *buf = malloc((LEN_PREFIX + MSG_ID_LEN + 4) * sizeof(char));
	int tmp = htonl(piece_index);
	struct b_string *ptr = NULL; 

	ptr = b_string_alloc_reserved(buf, 4, LEN_PREFIX + MSG_ID_LEN);
	buf = b_string_get(ptr);
	memcpy(buf, &tmp, 4);
	return set_prefix_and_msgid(MSG_ID_LEN + 4, PEER_MESSAGE_HAVE, ptr);
}

struct b_string *generate_bitfield_message(unsigned int bitfield_len, char *bitfield)
{
	char *buf = malloc((LEN_PREFIX + MSG_ID_LEN + bitfield_len) * sizeof(char));
	struct b_string *ptr = NULL;

	ptr = b_string_alloc_reserved(buf, bitfield_len, LEN_PREFIX +MSG_ID_LEN);
	buf = b_string_get(ptr);
	memcpy(buf, bitfield, bitfield_len);
	return set_prefix_and_msgid(MSG_ID_LEN + bitfield_len, PEER_MESSAGE_BITFIELD, ptr);
}

/* begin: offset of piece */
struct b_string *generate_request_message(unsigned int piece_index, unsigned int begin, unsigned int slice_len)
{
	struct b_string *ptr = NULL;
	char *buf = malloc((LEN_PREFIX + MSG_ID_LEN + 12) * sizeof(char));
	unsigned int  payload[3] = {};

	ptr = b_string_alloc_reserved(buf, 12, LEN_PREFIX + MSG_ID_LEN);
	buf = b_string_get(ptr); 
	payload[0] = htonl(piece_index);
	payload[1] = htonl(begin);
	payload[2] = htonl(slice_len);
	memcpy(buf, (char *)payload, 12);
	return set_prefix_and_msgid(12 + MSG_ID_LEN, PEER_MESSAGE_REQUEST, ptr);
} 

/* Please reserved 13 bytes for overheads of piece message at least */
struct b_string *generate_piece_message(unsigned int piece_index, unsigned int begin, unsigned int slice_len, struct b_string *ptr)
{
	char *buf = NULL;
	unsigned int t = htonl(piece_index);

	if ((b_string_get(ptr) - b_string_get_head(ptr)) < (8 + MSG_ID_LEN + LEN_PREFIX)) {
		TRACE(ERROR, "No enough reserved space for overheads info\n");
		b_string_free(ptr);
		return NULL;
	}
	buf = b_string_get(ptr);
	buf -= 8;
	memcpy(buf, (char *)&t, 4);
	t = htonl(begin);
	memcpy(buf + 4, (char *)&t, 4);
	b_string_set(ptr, buf);
	b_string_set_length(ptr, b_string_get_length(ptr) + 8);
	return set_prefix_and_msgid(slice_len + MSG_ID_LEN + 8, PEER_MESSAGE_PIECE, ptr);
}

struct b_string *generate_cancel_message(unsigned int piece_index, unsigned int begin, unsigned int slice_len)
{
	char *buf = malloc((LEN_PREFIX + MSG_ID_LEN + 12) * sizeof(char));
	unsigned int  payload[3] = {};
	struct b_string *ptr = b_string_alloc_reserved(buf, 12, LEN_PREFIX + MSG_ID_LEN);

	buf = b_string_get(ptr); 
	payload[0] = htonl(piece_index);
	payload[1] = htonl(begin);
	payload[2] = htonl(slice_len);
	memcpy(buf, (char *)payload, 12);
	return set_prefix_and_msgid(12 + MSG_ID_LEN, PEER_MESSAGE_CANCEL, ptr);
}

struct b_string *generate_port_message(unsigned short port)
{
	unsigned short tmp = htons(port);
	char *buf = malloc((LEN_PREFIX + MSG_ID_LEN + 2) * sizeof(char));
	struct b_string *ptr = b_string_alloc_reserved(buf, 2, LEN_PREFIX + MSG_ID_LEN);

	buf = b_string_get(ptr);
	memcpy(buf, (char *)&tmp, 2);
	return set_prefix_and_msgid(MSG_ID_LEN + 2, PEER_MESSAGE_PORT, ptr);
}

#ifdef _UNIT_TEST

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include "unittest/cmockery.h"

/* case 1: torrent parse test */
void generate_test_1(void **state) {
	struct b_string *hand = generate_handshake_message("1234567890098765432", "-AZ2060-12345678901");
	struct b_string *keep_alive = generate_keep_alive_message();
	struct b_string *choke = generate_choke_message();
	struct b_string *unchoke = generate_unchoke_message();
	struct b_string *interested = generate_interested_message();
	struct b_string *non_interested = generate_not_interested_message();
	struct b_string *has_message = generate_have_message(100);
	struct b_string *bitfield = generate_bitfield_message(10, "1234567890");
	struct b_string *request = generate_request_message(100, 0x10000, SLICE_LEN); 
	struct b_string *cancel = generate_cancel_message(100, 0x10000, SLICE_LEN); 
	char *buf = malloc(13 + SLICE_LEN);
	memset(buf, '1', 13 + SLICE_LEN);
	struct b_string *piece = b_string_alloc_reserved(buf, SLICE_LEN, 13);
	piece = generate_piece_message(100, 0x10000, SLICE_LEN, piece);

	b_string_hex_print(hand);
	b_string_hex_print(keep_alive);
	b_string_hex_print(choke);
	b_string_hex_print(unchoke);
	b_string_hex_print(interested);
	b_string_hex_print(non_interested);
	b_string_hex_print(has_message);
	b_string_hex_print(bitfield);
	b_string_hex_print(request);
	b_string_hex_print(cancel);
	b_string_hex_print(piece);

	b_string_free(hand);
	b_string_free(keep_alive);
	b_string_free(choke);
	b_string_free(unchoke);
	b_string_free(interested);
	b_string_free(non_interested);
	b_string_free(has_message);
	b_string_free(bitfield);
	b_string_free(request);
	b_string_free(cancel);
	b_string_free(piece);
}

int main(int argc, char **argv)
{
	const UnitTest tests[] = {
	    unit_test(generate_test_1),
	};

	run_tests(tests);
	exit(EXIT_SUCCESS);
}

#endif /* _UNIT_TEST */

