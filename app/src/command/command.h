#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "add.h"
#include "del.h"
#include "show.h"

int leave(void *arg);
int passwd(void *arg);

int load_entry(void *arg);
int save_entry(void *arg);
void show_secret(const char *name);

#endif
