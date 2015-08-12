/* Description:	Use to parse the seed for torrent info: includes basical
 *		B-coding info parsing.
 * Author:	Tristan Xiao
 * Data:	28/7/2015
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

//#include "seed.h"
#include "bttrace.h"
#include "list.h"

struct b_string {
	unsigned int len;
	char *string;
};

char *parse_b_string(char *buf, struct b_string *target)
{
	char *ptr = buf;
	char *s = strchr(ptr, ':');
	char *endptr = NULL;
	long val = 0;
	
	if (s) {
		*s = 0;
		val = strtol(ptr, &endptr, 10);
		
		if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
                   || (errno != 0 && val == 0)) {
			TRACE(ERROR, "strtol"); 
			return NULL;
           	}

		if (endptr == ptr) {
			TRACE(ERROR, "No digits were found\n");
			return NULL;
		}
		target->len = val;
	}
	else {
		TRACE(ERROR, "B-coding string format error: %s\n", buf);
		return NULL;
	}
	return 0;
}

