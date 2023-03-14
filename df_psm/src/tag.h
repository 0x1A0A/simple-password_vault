#ifndef __DF_PSM_TAG_H__
#define	__DF_PSM_TAG_H__

#include <stdint.h>

typedef struct df_tag tag_t;
typedef struct df_tag_list tag_list_t;

struct df_tag {
	char *name;
	tag_t *next;
	uint8_t nl, id;
};

struct df_tag_list {
	tag_t *head;
	uint8_t count;
};

tag_t * tag_create(const char *name, uint8_t length);
tag_list_t *tag_list_create();
void tag_destroy ( tag_t *tag);
void tag_list_destroy(tag_list_t *list);
void tag_list_reset(tag_list_t *list);

void tag_add( tag_t *tag, tag_list_t *list );
void tag_remove( tag_t *tag, tag_list_t *list );
void tag_remove_str( const char *name, tag_list_t *list );

//---checking
int8_t tag_exits( const int8_t id, tag_list_t *list );
int8_t tag_exits_name( const char *name, tag_list_t *list );
uint8_t tag_get_id( const char *name, tag_list_t *list );

#endif