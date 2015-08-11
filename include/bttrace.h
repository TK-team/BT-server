/*
 * =====================================================================================
 *
 *       Filename:  bttrace.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月11日 19时20分45秒
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

#define ERROR	0x0001
#define INFO	0x0002
#define DUMP	0x0004

#ifdef _BTDEBUG

#define BTTRACELEVEL	(ERROR | INFO | DUMP)

#define TRACE	print_msg

void print_msg(int level, char *format, ...);

#else

#define TRACE(VARGLST) ((void)0)

#endif

#ifdef _cplusplus
}
#endif

#endif	/* _BTTRACE_H */
