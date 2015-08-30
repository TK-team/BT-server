#ifndef _PEER_MESSAGE_H 
#define _PEER_MESSAGE_H 


#define HANDSHAKE_MESSAGE_LEN		68
#define	DEFAULT_PSTRLEN			19
#define DEFAULT_PSTR			"BitTorrent protocol" 
#define RESERVED_LEN			8
#define HASH_LEN			20
#define SLICE_LEN			(16 * 1024)
#define PEER_ID_LEN			20
#define MSG_ID_LEN			1
#define LEN_PREFIX			4

enum {
	PEER_MESSAGE_KEEP_ALIVE = 0,
	PEER_MESSAGE_CHOKE = 0,
	PEER_MESSAGE_UNCHOCKE,
	PEER_MESSAGE_INTERESTED,
	PEER_MESSAGE_NOT_INTERESTED,
	PEER_MESSAGE_HAVE,
	PEER_MESSAGE_BITFIELD,
	PEER_MESSAGE_REQUEST,
	PEER_MESSAGE_PIECE,
	PEER_MESSAGE_CANCEL,
	PEER_MESSAGE_PORT,
};
extern struct b_string *generate_handshake_message(char *info_hash, char *peer_id);
struct b_string *set_prefix_and_msgid(unsigned int len_prefix,
		unsigned char message_id, struct b_string *payload);
#endif  /* _PEER_MESSAGE_H */
