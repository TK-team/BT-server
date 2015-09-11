/*
 * =====================================================================================
 *
 *       Filename:  conn_tracker.h
 *
 *    Description:  connect tracker server
 *
 *        Version:  1.0
 *        Created:  2015-09-10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lawrence kuang
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _CONN_TRACKER_
#define _CONN_TRAKCER_

struct bt_request {
	unsigned char info_hash[20];
	unsigned char peer_id[20];
};

int conn_tracker(const char *url);

#endif
