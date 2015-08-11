/*
 * =====================================================================================
 *
 *       Filename:  bttrace.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2015-08-11 19:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  kuang guiming <kuangguiming@tp-link.net>
 *   Organization:
 *
 * =====================================================================================
 */

#ifndef _BTTRACE_H
#define _BTTRACE_H

#ifdef _cplusplus
extern "C" {
#endif

void bt_inc_msglevel(void);
void bt_set_msglevel(int level);
unsigned int bt_get_msglevel(void);

enum MSGLEVEL {
	MSG_ERROR = 0x0001,
	MSG_INFO  = 0x0002,
	MSG_DUMP  = 0x0004
};

#define ERROR	MSG_ERROR, __FUNCTION__, __LINE__
#define INFO	MSG_INFO, __FUNCTION__, __LINE__
#define DUMP	MSG_DUMP, __FUNCTION__, __LINE__

#ifdef _BTDEBUG

#define BTTRACELEVEL	(MSG_ERROR | MSG_INFO | MSG_DUMP)

#define TRACE	print_msg

void print_msg(int level, const char *func, int line, char *format, ...);

#else

#define TRACE(VARGLST) ((void)0)

#endif

#ifdef _cplusplus
}
#endif

#endif	/* _BTTRACE_H */
