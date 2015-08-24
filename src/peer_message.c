#include <arpa/inet.h>

#include "peer_message.h"

#undef _UNIT_TEST
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

struct b_string *make_handshake_message(char *info_hash)
{
	struct b_string *ptr = b_string_alloc();

	if (ptr && buf) {
		char *buf = malloc(HANDSHAKE_MESSAGE_LEN * sizeof(char));
		if (buf) {
			b_string_set(ptr, buf);
			*buf = DEFAULT_PSTRLEN;
			strncpy(buf + 1; DEFAULT_PSTR, DEFAULT_PSTRLEN); 
			memset(buf + 1 + DEFAULT_PSTRLEN, 0, RESERVED_LEN);
			strncpy(buf + 1 + DEFAULT_PSTRLEN + RESERVED_LEN, 
					info_hash, HASH_LEN); 
			b_string_set_length(ptr, HANDSHAKE_MESSAGE_LEN); 
			return ptr;
		}
		else {
			b_string_free(ptr);
			return NULL;
		}
	} else
		return NULL;
}

struct b_string *generate_common_message(unsigned int len_prefix, 
				unsigned char message_id, char *payload)
{
	struct b_string *ptr = b_string_alloc();

	if (ptr) {
		char *buf = malloc((len_prefix + 4) * sizeof(char));

		if (buf) {
			unsigned int tmp = htonl(len_prefix);

			b_string_set(ptr, buf);
			b_string_set_length(ptr, len_prefix + 4);
			memcpy(buf, &tmp, 4);
			if (len_prefix > 0)
				*(buf + 4) = message_id;
			if (len_prefix > 1)
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
	return generate_common_message(0, 0, NULL)
}

struct b_string *generate_choke_message(void)
{
	return generate_choke_message(1, 0, NULL);
}

struct b_string *generate_unchoke_message(void)
{
	return generate_choke_message(1, 1, NULL);
}

struct b_string *generate_interested_message(void)
{
	return generate_choke_message(1, 2, NULL);
}

struct b_string *generate_not_interested_message(void)
{
	return generate_choke_message(1, 3, NULL);
}

struct b_string *generate_have_message(unsigned int piece_index)
{
	int tmp = htonl(piece_index);
	return generate_choke_message(5, 4, (char *)&tmp);
}

struct b_string *generate_have_message(unsigned int bitmap_len, char *bitmap)
{
	return generate_choke_message(1 + bitmap_len, 5, bitmap);
}


