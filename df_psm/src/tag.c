#include "tag.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// allocate tag
tag_t *tag_create(const char *name, uint8_t length) {
  if (name) {
    tag_t *tag = (tag_t *)malloc(sizeof(tag_t));

    if (!tag) {
      fprintf(stderr, "can't allocate tag\n");
      return NULL;
    }

    tag->name = (char *)malloc(length);
    strncpy(tag->name, name, length);

    tag->id = 0;
    tag->next = NULL;
    tag->nl = length;

    return tag;
  }

  return NULL;
}

// allocate tag list
tag_list_t *tag_list_create() {
  tag_list_t *list = (tag_list_t *)malloc(sizeof(tag_list_t));

  if (!list) {
    fprintf(stderr, "can't allocate tag list\n");
    return NULL;
  }

  list->head = NULL;
  list->count = 0;
  return list;
}

// deallocate tag
void tag_destroy(tag_t *tag) {
  if (tag) {
    memset(tag->name, 0xff, tag->nl);
    free(tag->name);
    tag->name = NULL;
    tag->next = NULL;

    free(tag);

    tag = NULL;
  }
}

void tag_list_reset(tag_list_t *list) {
  if (list) {
    tag_t *temp;

    while (list->head) {
      temp = list->head;
      list->head = temp->next;
      tag_destroy(temp);
    }
  }

  list->head = NULL;
  list->count = 0;
}

// deallocate tag list and all tag in the list
void tag_list_destroy(tag_list_t *list) {
  tag_list_reset(list);
  free(list);
}

// add tag to list
void tag_add(tag_t *tag, tag_list_t *list) {
  uint8_t id = 1;
  if (list) {
    tag_t **trace = &(list->head);

    while (*trace) {
      if ((*trace)->id != id) {
        tag->next = *trace;
        *trace = tag;
        break;
      }
      trace = &((*trace)->next);
      ++id;
    }

    tag->id = id;
    (*trace) = tag;
    list->count++;
  }
}

// remove tag from list
void tag_remove(tag_t *tag, tag_list_t *list) {
  if (list && tag) {
    tag_t **trace = &(list->head), *temp;

    while (*trace) {
      if (*trace == tag) { // target
        temp = *trace;
        (*trace) = temp->next;
        tag_destroy(temp);
        list->count--;
        break;
      }

      trace = &((*trace)->next);
    }
  }
}

void tag_remove_id(const int id, tag_list_t *list) {
  if (list) {
    tag_t **trace = &(list->head), *temp;

    while (*trace) {
      if ((*trace)->id == id) { // target
        temp = *trace;
        (*trace) = temp->next;
        tag_destroy(temp);
        list->count--;
        break;
      }

      trace = &((*trace)->next);
    }
  }
}

// remove tag name from list
void tag_remove_str(const char *name, tag_list_t *list) {
  if (list && name) {
    tag_t **trace = &(list->head), *temp;

    while (*trace) {
      if (!strncmp(name, (*trace)->name, (*trace)->nl)) { // target
        temp = *trace;
        (*trace) = temp->next;
        tag_destroy(temp);
        list->count--;
        break;
      }

      trace = &((*trace)->next);
    }
  }
}

// check if tag id already extis
int8_t tag_exits(const int8_t id, tag_list_t *list) {
  if (list) {
    tag_t **trace = &list->head;

    while (*trace) {
      if ((*trace)->id == id)
        return 1;
      trace = &(*trace)->next;
    }
  }

  return 0;
}

// check if tag name is already exits
int8_t tag_exits_name(const char *name, tag_list_t *list) {
  if (list) {
    tag_t **trace = &list->head;

    while (*trace) {
      if (!strncmp(name, (*trace)->name, (*trace)->nl))
        return 1;
      trace = &(*trace)->next;
    }
  }

  return 0;
}

// use tag name to get tag id
uint8_t tag_get_id(const char *name, tag_list_t *list) {
  if (list) {
    tag_t **trace = &list->head;

    while (*trace) {
      if (!strncmp(name, (*trace)->name, (*trace)->nl))
        return (*trace)->id;
      trace = &(*trace)->next;
    }
  }

  return 0;
}

tag_t *tag_find_name(const char *name, tag_list_t *list) {
  if (list) {
    tag_t **trace = &list->head;

    while (*trace) {
      if (!strncmp(name, (*trace)->name, (*trace)->nl))
        return *trace;
      trace = &(*trace)->next;
    }
  }

  return NULL;
}
