/*
 * =====================================================================================
 *
 *       Filename:  bttrace.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月11日 19时38分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  kuang guiming <kuangguiming@tp-link.net>
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "bttrace.h"

#ifdef _BTDEBUG
static unsigned int msglevel = ERROR;
#else
static unsigned int msglevel = 0;
#endif

void bt_inc_msglevel(void)
{
	msglevel++;
}

void bt_set_msglevel(int level)
{
	msglevel = level;
}

unsigned int bt_get_msglevel(void)
{
	return msglevel;
}

#ifdef _BTDEBUG
void print_msg(int level, char *format, ...)
{
	char buffer[2000];
	int  nLen;
	va_list args;

	if (!(level & BTTRACELEVEL))
		return;

	nLen = sprintf(buffer, "BT-Server: %s(%d)", __FUNCTION__, __LINE__);
	va_start(args, format);
	nLen = vsprintf(buffer + nLen, format, args);
	va_end(args);

	if (level & msglevel)
		fprintf(stdout, "%s", buffer);

}
#endif
