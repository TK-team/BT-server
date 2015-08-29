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

struct b_string *make_handshake_message(char *info_hash, char *peer_id)
{
	struct b_string *ptr = NULL;
	char *buf = NULL;

	if (info_hash && peer_id) {
		ptr = b_string_alloc();
		if (ptr) {
			buf = malloc((HANDSHAKE_MESSAGE_LEN + 1) * sizeof(char));
			b_string_set(ptr, buf);
			memset(buf, 0, HANDSHAKE_MESSAGE_LEN + 1);
			*buf = DEFAULT_PSTRLEN;
			memcpy(buf + 1, DEFAULT_PSTR, DEFAULT_PSTRLEN); 
			memset(buf + 1 + DEFAULT_PSTRLEN, 0, RESERVED_LEN);
			memcpy(buf + 1 + DEFAULT_PSTRLEN + RESERVED_LEN, 
					info_hash, HASH_LEN); 
			memcpy(buf + 1 + DEFAULT_PSTRLEN + RESERVED_LEN + HASH_LEN, 
					peer_id, PEER_ID_LEN); 
			*(buf + HANDSHAKE_MESSAGE_LEN) = '\0';
			b_string_set_length(ptr, HANDSHAKE_MESSAGE_LEN); 
			return ptr;
		}
	}
		return NULL;
}

/* need relloc should be piece message */
struct b_string *generate_common_message(unsigned int len_prefix, 
		unsigned char message_id, char *payload, unsigned int need_relloc)
{
	struct b_string *ptr = b_string_alloc();
	char *buf = NULL;

	if (ptr) {
		if (need_relloc)
			buf = malloc((len_prefix + 4) * sizeof(char));
		else
			buf = payload - 4 - 1;

		if (buf) {
			unsigned int tmp = htonl(len_prefix);

			b_string_set(ptr, buf);
			b_string_set_length(ptr, len_prefix + 4);
			memcpy(buf, &tmp, 4);
			if (len_prefix > 0)
				*(buf + 4) = message_id;
			if (need_relloc && len_prefix > 1)
				memcpy(buf + 5, payload, len_prefix - 1);
			return ptr;
		} else {
			b_string_free(ptr);
			return NULL;
		}
	}
	return NULL;
}

struct b_string *generate_keep_alive_message(void)
{
	return generate_common_message(0, 0, NULL, 1);
}

struct b_string *generate_choke_message(void)
{
	return generate_common_message(1, 0, NULL, 1);
}

struct b_string *generate_unchoke_message(void)
{
	return generate_common_message(1, 1, NULL, 1);
}

struct b_string *generate_interested_message(void)
{
	return generate_common_message(1, 2, NULL, 1);
}

struct b_string *generate_not_interested_message(void)
{
	return generate_common_message(1, 3, NULL, 1);
}

struct b_string *generate_have_message(unsigned int piece_index)
{
	int tmp = htonl(piece_index);
	return generate_common_message(5, 4, (char *)&tmp, 1);
}

struct b_string *generate_bitfield_message(unsigned int bitfield_len, char *bitfield)
{
	return generate_common_message(1 + bitfield_len, 5, bitfield, 1);
}

/* begin: offset of piece */
struct b_string *generate_request_message(unsigned int piece_index, unsigned int begin, unsigned int slice_len)
{
	unsigned int  payload[3] = {};

	payload[0] = htonl(piece_index);
	payload[1] = htonl(begin);
	payload[2] = htonl(slice_len);
	return generate_common_message(13, 6, (char *)payload, 1);
}

struct b_string *generate_piece_message(unsigned int piece_index, unsigned int begin, unsigned int slice_len,  char *block)
{
	char *payload = block - 8;
	unsigned int t = htonl(piece_index);

	memcpy(payload, &t, 4);
	t = htonl(begin);
	memcpy(payload + 4, &t, 4);
	return generate_common_message(slice_len + 13, 7, (char *)payload, 0);
}

struct b_string *generate_cancel_message(unsigned int piece_index, unsigned int begin, unsigned int slice_len)
{
	unsigned int  payload[3] = {};

	payload[0] = htonl(piece_index);
	payload[1] = htonl(begin);
	payload[2] = htonl(slice_len);
	return generate_common_message(13, 8, (char *)payload, 1);
}

struct b_string *generate_port_message(unsigned short port)
{
	unsigned short tmp = htons(port);
	return generate_common_message(3, 8, (char *)&tmp, 1);
}

#ifdef _UNIT_TEST

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include "unittest/cmockery.h"

/* case 1: torrent parse test */
void generate_test_1(void **state) {
	struct b_string *ptr = make_handshake_message("1234567890098765432", "-AZ2060-12345678901");
	if (ptr) {
		b_string_hex_print(ptr);
		b_string_free(ptr);
	}
}

void generate_test_2(void **state) {
	struct b_string *b = make_handshake_message("1234567890098765432", "-AZ2060-12345678901");

	b_string_hex_print(b);
	b_string_free(b);
	return;
}

int main(int argc, char **argv)
{
	const UnitTest tests[] = {
	    unit_test(generate_test_2),
	};

	run_tests(tests);
	exit(EXIT_SUCCESS);
}

#endif /* _UNIT_TEST */

