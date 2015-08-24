#ifndef _PEER_MESSAGE_H 
#define _PEER_MESSAGE_H 
#include "b_parse.h"

#define HANDSHAKE_MESSAGE_LEN		68
#define	DEFAULT_PSTRLEN			19
#define DEFAULT_PSTR			"BitTorrent protocol" 
#define RESERVED_LEN			8
#define HASH_LEN			20

extern struct b_string *make_handshake_message(char *info_hash);
extern struct b_string *generate_common_message(unsigned int len_prefix, unsigned char message_id, char *payload);
#endif  /* _PEER_MESSAGE_H */
