#ifndef __COMMON_H__
#define __COMMON_H__

#define BIT(x) 	(1<<x)

#include "psm.h"
#include <stdio.h>

extern account_list_t *account_list;
extern tag_list_t *tag_list;
extern email_list_t *email_list;
extern struct chacha20 *cypher;
extern byte hash[32];

extern int authorized;
extern void notallow();

#define AUTH if (!authorized) {notallow();return FAILED;}

enum __RESPONSE {
	OK,
	FAILED,
	LOGGEDIN,
	LEAVE,
	COUNT,
};

#endif