#ifndef __DF_PSM_FILE_H__
#define __DF_PSM_FILE_H__

#include "tag.h"
#include "email.h"
#include "account.h"

void load_file( const char *path );
void save_file( const char *path );
unsigned char verify( const char *path);
#endif
