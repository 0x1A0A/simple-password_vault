#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <cmocka.h>

#include "../src/tag.h"

static void tag_list_create_tests(void **state) {
  tag_list_t *list = *state;

  assert_null(list->head);
  assert_true(list->count == 0);
}

static void tag_create_tests(void **state) {
  tag_t *tag = tag_create("main", 4);

  assert_int_equal(tag->id, 0);
  assert_int_equal(tag->nl, 4);
  assert_null(tag->next);
  assert_string_equal("main", tag->name);

  tag_destroy(tag);
}

static void tag_add_tests(void **state) {
  tag_list_t *list = *state;
  tag_t *tag = tag_create("main", 4);
  tag_t *tag1 = tag_create("test", 4);
  tag_t *tag2 = tag_create("last", 4);

  tag_add(tag, list);
  assert_true(list->head == tag);
  assert_int_equal(list->count, 1);

  tag_add(tag1, list);
  tag_add(tag2, list);

  tag_t *head = list->head;

  tag_t *tags[] = {tag, tag1, tag2};
  int i = 0;

  while (head) {
    assert_true(head == tags[i++]);
    head = head->next;
  }

  assert_int_equal(list->count, 3);
}

static void tag_exits_tests(void **state) {
  tag_list_t *list = *state;

  assert_true(tag_exits(1, list));
  assert_true(tag_exits_name("last", list));
  assert_false(tag_exits_name("false", list));
}

static void tag_get_id_tests(void **state) {
  tag_list_t *list = *state;

  assert_int_equal(tag_get_id("main", list), 1);
  assert_int_equal(tag_get_id("last", list), 3);
  assert_int_equal(tag_get_id("yo", list), 0);
}

static void tag_find_by_name_tests(void **state) {
  tag_list_t *list = *state;

  tag_t *tag = tag_create("new_tag", 6);

  tag_add(tag, list);

  assert_true(tag == tag_find_name("new_tag", list));
  assert_true(NULL == tag_find_name("none_exists_tag", list));
}

static void tag_list_reset_tests(void **state) {
  tag_list_t *list = *state;

  tag_list_reset(list);

  assert_null(list->head);
  assert_true(list->count == 0);
}

int main(void) {
  tag_list_t *list = tag_list_create();

  const struct CMUnitTest tests[] = {
      cmocka_unit_test(tag_create_tests),
      cmocka_unit_test_prestate(tag_list_create_tests, list),
      cmocka_unit_test_prestate(tag_add_tests, list),
      cmocka_unit_test_prestate(tag_exits_tests, list),
      cmocka_unit_test_prestate(tag_get_id_tests, list),
      cmocka_unit_test_prestate(tag_find_by_name_tests, list),
      cmocka_unit_test_prestate(tag_list_reset_tests, list),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
