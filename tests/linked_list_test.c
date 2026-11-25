#include <linked_list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ================================ TEST TOOLS ================================

static int tests_run = 0;
static int tests_failed = 0;

#define CHECK(condition)                                                       \
  do {                                                                         \
    tests_run++;                                                               \
    if (!(condition)) {                                                        \
      tests_failed++;                                                          \
      printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition);            \
    }                                                                          \
  } while (0)

#define TEST(name)                                                             \
  static void name(void);                                                      \
  static void name(void)

#define RUN(name)                                                              \
  do {                                                                         \
    int failed_before = tests_failed;                                          \
    name();                                                                    \
    printf("%s %s\n", tests_failed == failed_before ? "[ OK ]" : "[FAIL]",     \
           #name);                                                             \
  } while (0)

// values used as data (their addresses are stored in the lists)
static int values[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

// checks that the list contains exactly the expected ints, in order
static bool list_equals(LinkedList list, const int *expected, size_t count) {
  if (LL_size(list) != count) return false;

  for (size_t i = 0; i < count; i++) {
    int *data = LL_get_data(list, i);
    if (!data || *data != expected[i]) return false;
  }

  return true;
}

// creates a list with values[0..count-1] (data NOT owned by the list)
static LinkedList new_list_of(size_t count) {
  LinkedList list = LL_new_list();
  for (size_t i = 0; i < count; i++) {
    LL_add_last(list, &values[i]);
  }

  return list;
}

// heap int, for the tests where the list owns (and frees) the data
static int *new_int(int value) {
  int *data = malloc(sizeof *data);
  *data = value;

  return data;
}

// deleter that counts how many times it was called
static int deleter_calls = 0;
static void counting_deleter(void *data) {
  deleter_calls++;
  free(data);
}

// action for LL_for_each that records the visited values
static int visited[10];
static size_t visited_count = 0;
static void record_action(void *data) {
  visited[visited_count++] = *(int *)data;
}

// data with an inner pointer, for deep copy tests
typedef struct {
  int *value;
} Box;

static void *box_copier(const void *data) {
  const Box *source = data;
  Box *copy = malloc(sizeof *copy);
  copy->value = new_int(*source->value);

  return copy;
}

static void box_deleter(void *data) {
  Box *box = data;
  free(box->value);
  free(box);
}

// copier that fails on its N-th call (to test LL_copy cleanup)
static int copier_calls = 0;
static int copier_fail_at = 0;
static void *failing_copier(const void *data) {
  if (++copier_calls == copier_fail_at) return nullptr;

  return new_int(*(const int *)data);
}

// =============================== LINKED LIST ================================

TEST(test_new_list) {
  LinkedList list = LL_new_list();

  CHECK(list != nullptr);
  CHECK(LL_size(list) == 0);
  CHECK(LL_is_empty_list(list));

  LL_soft_delete_list(list);
}

TEST(test_size) {
  LinkedList list = new_list_of(3);

  CHECK(LL_size(list) == 3);
  CHECK(LL_size(nullptr) == 0);

  LL_soft_delete_list(list);
}

// =================================== DATA ===================================

TEST(test_get_data) {
  LinkedList list = new_list_of(3);

  CHECK(*(int *)LL_get_data(list, 0) == 0);
  CHECK(*(int *)LL_get_data(list, 2) == 2);

  // errors
  CHECK(LL_get_data(list, 3) == LL_ERROR);
  CHECK(LL_get_data(list, 100) == LL_ERROR);
  CHECK(LL_get_data(nullptr, 0) == LL_ERROR);

  LinkedList empty = LL_new_list();
  CHECK(LL_get_data(empty, 0) == LL_ERROR);

  // stored nullptr data: valid index, the data itself is nullptr
  CHECK(LL_add_last(empty, nullptr) ==
        nullptr); // adding nullptr data is a success
  CHECK(LL_is_valid_index(empty, 0));
  CHECK(LL_get_data(empty, 0) == nullptr);

  LL_soft_delete_list(empty);
  LL_soft_delete_list(list);
}

TEST(test_swap_data) {
  LinkedList list = new_list_of(5);

  LL_swap_data(list, 0, 4);
  CHECK(list_equals(list, (int[]){4, 1, 2, 3, 0}, 5));

  LL_swap_data(list, 3, 1);
  CHECK(list_equals(list, (int[]){4, 3, 2, 1, 0}, 5));

  // same index: no change
  LL_swap_data(list, 2, 2);
  CHECK(list_equals(list, (int[]){4, 3, 2, 1, 0}, 5));

  // errors: invalid indexes or nullptr list, no change and no crash
  LL_swap_data(list, 0, 5);
  LL_swap_data(list, 9, 1);
  LL_swap_data(nullptr, 0, 1);
  CHECK(list_equals(list, (int[]){4, 3, 2, 1, 0}, 5));

  LL_soft_delete_list(list);
}

// =================================== ADD ====================================

TEST(test_add_first) {
  LinkedList list = LL_new_list();

  CHECK(LL_add_first(list, &values[1]) == &values[1]);
  CHECK(LL_add_first(list, &values[0]) == &values[0]);
  CHECK(list_equals(list, (int[]){0, 1}, 2));

  // errors
  CHECK(LL_add_first(nullptr, &values[0]) == LL_ERROR);

  LL_soft_delete_list(list);
}

TEST(test_add_last) {
  LinkedList list = LL_new_list();

  CHECK(LL_add_last(list, &values[0]) == &values[0]);
  CHECK(LL_add_last(list, &values[1]) == &values[1]);
  CHECK(list_equals(list, (int[]){0, 1}, 2));

  // add_last after add_first on an empty list (first == last)
  LinkedList other = LL_new_list();
  LL_add_first(other, &values[0]);
  LL_add_last(other, &values[1]);
  CHECK(list_equals(other, (int[]){0, 1}, 2));

  // errors
  CHECK(LL_add_last(nullptr, &values[0]) == LL_ERROR);

  // LL_ERROR can't be stored (it would be confused with an error)
  CHECK(LL_add_last(list, LL_ERROR) == LL_ERROR);
  CHECK(LL_add_first(list, LL_ERROR) == LL_ERROR);
  CHECK(LL_insert_at(list, LL_ERROR, 1) == LL_ERROR);
  CHECK(list_equals(list, (int[]){0, 1}, 2));

  LL_soft_delete_list(other);
  LL_soft_delete_list(list);
}

TEST(test_insert_at) {
  LinkedList list = LL_new_list();

  // BUG #5: inserting into an empty list and at the end was not allowed
  CHECK(LL_insert_at(list, &values[1], 0) == &values[1]); // empty list
  CHECK(LL_insert_at(list, &values[3], 1) == &values[3]); // end (index == size)
  CHECK(LL_insert_at(list, &values[2], 1) == &values[2]); // middle
  CHECK(LL_insert_at(list, &values[0], 0) == &values[0]); // beginning
  CHECK(list_equals(list, (int[]){0, 1, 2, 3}, 4));

  // last must be updated when inserting at the end
  LL_insert_at(list, &values[4], 4);
  LL_add_last(list, &values[5]);
  CHECK(list_equals(list, (int[]){0, 1, 2, 3, 4, 5}, 6));

  // errors
  CHECK(LL_insert_at(list, &values[9], 7) == LL_ERROR); // index > size
  CHECK(LL_insert_at(list, &values[9], 100) == LL_ERROR);
  CHECK(LL_insert_at(nullptr, &values[9], 0) == LL_ERROR);
  CHECK(LL_size(list) == 6);

  LL_soft_delete_list(list);
}

// =============================== SOFT DELETE ================================

TEST(test_soft_delete_first) {
  LinkedList list = new_list_of(3);

  CHECK(*(int *)LL_soft_delete_first(list) == 0);
  CHECK(list_equals(list, (int[]){1, 2}, 2));

  // BUG #2: emptying the list with delete_first left last dangling
  LL_soft_delete_first(list);
  LL_soft_delete_first(list);
  CHECK(LL_is_empty_list(list));
  LL_add_last(list, &values[7]);
  CHECK(list_equals(list, (int[]){7}, 1));

  // stored nullptr data: the list is not empty, the deleted data is nullptr
  LinkedList with_null = LL_new_list();
  LL_add_last(with_null, nullptr);
  CHECK(!LL_is_empty_list(with_null));
  CHECK(LL_soft_delete_first(with_null) == nullptr);
  CHECK(LL_is_empty_list(with_null));
  LL_soft_delete_list(with_null);

  // errors
  LinkedList empty = LL_new_list();
  CHECK(LL_soft_delete_first(empty) == LL_ERROR);
  CHECK(LL_soft_delete_first(nullptr) == LL_ERROR);

  LL_soft_delete_list(empty);
  LL_soft_delete_list(list);
}

TEST(test_soft_delete_last) {
  LinkedList list = new_list_of(3);

  CHECK(*(int *)LL_soft_delete_last(list) == 2);
  CHECK(list_equals(list, (int[]){0, 1}, 2));

  // last must be updated: add_last goes after the new last
  LL_add_last(list, &values[5]);
  CHECK(list_equals(list, (int[]){0, 1, 5}, 3));

  // single node list
  LinkedList one = new_list_of(1);
  CHECK(*(int *)LL_soft_delete_last(one) == 0);
  CHECK(LL_is_empty_list(one));
  LL_add_last(one, &values[3]);
  CHECK(list_equals(one, (int[]){3}, 1));

  // errors
  LinkedList empty = LL_new_list();
  CHECK(LL_soft_delete_last(empty) == LL_ERROR);
  CHECK(LL_soft_delete_last(nullptr) == LL_ERROR);

  LL_soft_delete_list(empty);
  LL_soft_delete_list(one);
  LL_soft_delete_list(list);
}

TEST(test_soft_delete_at) {
  LinkedList list = new_list_of(5);

  // middle
  CHECK(*(int *)LL_soft_delete_at(list, 2) == 2);
  CHECK(list_equals(list, (int[]){0, 1, 3, 4}, 4));

  // BUG #1: index 0 called itself forever (stack overflow)
  CHECK(*(int *)LL_soft_delete_at(list, 0) == 0);
  CHECK(list_equals(list, (int[]){1, 3, 4}, 3));

  // BUG #2: deleting the last node left last pointing to freed memory
  CHECK(*(int *)LL_soft_delete_at(list, 2) == 4);
  LL_add_last(list, &values[9]);
  CHECK(list_equals(list, (int[]){1, 3, 9}, 3));

  // errors
  CHECK(LL_soft_delete_at(list, 3) == LL_ERROR);
  CHECK(LL_soft_delete_at(list, 100) == LL_ERROR);
  CHECK(LL_soft_delete_at(nullptr, 0) == LL_ERROR);
  CHECK(LL_size(list) == 3);

  LinkedList empty = LL_new_list();
  CHECK(LL_soft_delete_at(empty, 0) == LL_ERROR);

  LL_soft_delete_list(empty);
  LL_soft_delete_list(list);
}

TEST(test_soft_delete_all) {
  LinkedList list = new_list_of(4);

  LL_soft_delete_all(list);
  CHECK(LL_is_empty_list(list));
  CHECK(LL_size(list) == 0);

  // the list is still usable
  LL_add_last(list, &values[1]);
  CHECK(list_equals(list, (int[]){1}, 1));

  // errors: no crash
  LinkedList empty = LL_new_list();
  LL_soft_delete_all(empty);
  LL_soft_delete_all(nullptr);
  CHECK(LL_is_empty_list(empty));

  LL_soft_delete_list(empty);
  LL_soft_delete_list(list);
}

TEST(test_soft_delete_list) {
  // the data is not freed (values is static)
  LL_soft_delete_list(new_list_of(3));
  LL_soft_delete_list(LL_new_list());

  // errors: no crash
  LL_soft_delete_list(nullptr);
  CHECK(true);
}

// =============================== HARD DELETE ================================

TEST(test_hard_delete_first_last_at) {
  LinkedList list = LL_new_list();
  for (int i = 0; i < 5; i++) {
    LL_add_last(list, new_int(i));
  }

  deleter_calls = 0;
  LL_hard_delete_first(list, counting_deleter);
  LL_hard_delete_last(list, counting_deleter);
  LL_hard_delete_at(list, 1, counting_deleter);
  CHECK(deleter_calls == 3);
  CHECK(list_equals(list, (int[]){1, 3}, 2));

  // errors: the deleter must NOT be called
  deleter_calls = 0;
  LL_hard_delete_at(list, 5, counting_deleter);
  LL_hard_delete_first(nullptr, counting_deleter);
  LL_hard_delete_last(nullptr, counting_deleter);
  LL_hard_delete_at(nullptr, 0, counting_deleter);
  CHECK(deleter_calls == 0);
  CHECK(LL_size(list) == 2);

  LinkedList empty = LL_new_list();
  LL_hard_delete_first(empty, counting_deleter);
  LL_hard_delete_last(empty, counting_deleter);
  CHECK(deleter_calls == 0);

  // nullptr deleter: uses free
  LL_hard_delete_first(list, nullptr);
  CHECK(list_equals(list, (int[]){3}, 1));

  LL_soft_delete_list(empty);
  LL_hard_delete_list(list, nullptr);
}

TEST(test_hard_delete_all_and_list) {
  LinkedList list = LL_new_list();
  for (int i = 0; i < 4; i++) {
    LL_add_last(list, new_int(i));
  }

  deleter_calls = 0;
  LL_hard_delete_all(list, counting_deleter);
  CHECK(deleter_calls == 4);
  CHECK(LL_is_empty_list(list));

  // stored nullptr data: the deleter is not called for it
  LL_add_last(list, nullptr);
  LL_add_last(list, new_int(9));
  deleter_calls = 0;
  LL_hard_delete_list(list, counting_deleter);
  CHECK(deleter_calls == 1);

  // errors: no crash
  LL_hard_delete_all(nullptr, counting_deleter);
  LL_hard_delete_list(nullptr, counting_deleter);
  CHECK(deleter_calls == 1);
}

// heap list with the ints 0..count-1 (owned by the list)
static LinkedList new_owned_list_of(int count) {
  LinkedList list = LL_new_list();
  for (int i = 0; i < count; i++) {
    LL_add_last(list, new_int(i));
  }

  return list;
}

// ============================= LIST VALIDATIONS =============================

TEST(test_list_validations) {
  LinkedList list = new_list_of(2);
  LinkedList empty = LL_new_list();

  CHECK(LL_is_valid_list(list));
  CHECK(!LL_is_valid_list(nullptr));

  CHECK(!LL_is_empty_list(list));
  CHECK(LL_is_empty_list(empty));
  CHECK(LL_is_empty_list(nullptr)); // BUG #3: nullptr was "not empty" -> crash

  CHECK(!LL_is_full_list(list));
  CHECK(!LL_is_full_list(empty));
  CHECK(!LL_is_full_list(nullptr));

  LL_soft_delete_list(empty);
  LL_soft_delete_list(list);
}

// ====================== INDEX, ADD, INSERT VALIDATIONS ======================

TEST(test_contains) {
  LinkedList list = new_list_of(3);
  int outside = 2; // same value, different address

  CHECK(LL_contains(list, &values[0]));
  CHECK(LL_contains(list, &values[2]));
  CHECK(!LL_contains(list, &values[5]));
  CHECK(!LL_contains(list, &outside)); // compares pointers, not values

  // errors
  CHECK(!LL_contains(nullptr, &values[0]));
  LinkedList empty = LL_new_list();
  CHECK(!LL_contains(empty, &values[0]));

  LL_soft_delete_list(empty);
  LL_soft_delete_list(list);
}

TEST(test_index_add_insert_validations) {
  LinkedList list = new_list_of(3);
  LinkedList empty = LL_new_list();

  CHECK(LL_is_valid_index(list, 0));
  CHECK(LL_is_valid_index(list, 2));
  CHECK(!LL_is_valid_index(list, 3));
  CHECK(!LL_is_valid_index(empty, 0));
  CHECK(!LL_is_valid_index(nullptr, 0));

  CHECK(LL_is_valid_add(list));
  CHECK(LL_is_valid_add(empty));
  CHECK(!LL_is_valid_add(nullptr));

  CHECK(LL_is_valid_insert(list, 0));
  CHECK(LL_is_valid_insert(list, 3)); // index == size: insert at the end
  CHECK(!LL_is_valid_insert(list, 4));
  CHECK(LL_is_valid_insert(empty, 0));
  CHECK(!LL_is_valid_insert(empty, 1));
  CHECK(!LL_is_valid_insert(nullptr, 0));

  LL_soft_delete_list(empty);
  LL_soft_delete_list(list);
}

// ================================ UTILITIES =================================

TEST(test_copy_memcpy) {
  LinkedList list = LL_new_list();
  for (int i = 0; i < 3; i++) {
    LL_add_last(list, new_int(i));
  }

  LinkedList copy = LL_copy(list, sizeof(int), nullptr, nullptr);
  CHECK(copy != nullptr && copy != list);
  CHECK(list_equals(copy, (int[]){0, 1, 2}, 3));

  // the copied data is independent
  *(int *)LL_get_data(list, 0) = 7;
  CHECK(*(int *)LL_get_data(copy, 0) == 0);
  CHECK(LL_get_data(list, 1) != LL_get_data(copy, 1));

  LL_hard_delete_list(copy, nullptr);
  LL_hard_delete_list(list, nullptr);
}

TEST(test_copy_with_copier) {
  LinkedList list = LL_new_list();
  for (int i = 0; i < 3; i++) {
    Box *box = malloc(sizeof *box);
    box->value = new_int(i);
    LL_add_last(list, box);
  }

  // BUG #4: a memcpy copy would share the inner pointer (double free)
  LinkedList copy = LL_copy(list, sizeof(Box), box_copier, box_deleter);
  CHECK(copy != nullptr && LL_size(copy) == 3);

  Box *original = LL_get_data(list, 1);
  Box *copied = LL_get_data(copy, 1);
  CHECK(original != copied);
  CHECK(original->value != copied->value);
  CHECK(*copied->value == 1);

  // both lists can be fully deleted without a double free
  LL_hard_delete_list(copy, box_deleter);
  LL_hard_delete_list(list, box_deleter);
}

TEST(test_copy_null_data) {
  // BUG #6: nullptr data was passed to memcpy (UB) / to the copier (copy
  // failed)
  LinkedList list = LL_new_list();
  LL_add_last(list, new_int(1));
  LL_add_last(list, nullptr);
  LL_add_last(list, new_int(3));

  // without copier (memcpy)
  LinkedList copy = LL_copy(list, sizeof(int), nullptr, nullptr);
  CHECK(copy != nullptr && LL_size(copy) == 3);
  CHECK(*(int *)LL_get_data(copy, 0) == 1);
  CHECK(LL_get_data(copy, 1) == nullptr);
  CHECK(*(int *)LL_get_data(copy, 2) == 3);
  LL_hard_delete_list(copy, nullptr);

  // with copier: the copier is never called with nullptr
  copier_calls = 0;
  copier_fail_at = 0;
  copy = LL_copy(list, 0, failing_copier, nullptr);
  CHECK(copy != nullptr && LL_size(copy) == 3);
  CHECK(copier_calls == 2);
  CHECK(LL_get_data(copy, 1) == nullptr);
  LL_hard_delete_list(copy, nullptr);

  LL_hard_delete_list(list, nullptr);
}

TEST(test_copy_errors) {
  // nullptr list
  CHECK(LL_copy(nullptr, sizeof(int), nullptr, nullptr) == nullptr);

  // BUG #4: an empty list returned the SAME list (double free)
  LinkedList empty = LL_new_list();
  LinkedList copy = LL_copy(empty, sizeof(int), nullptr, nullptr);
  CHECK(copy != nullptr && copy != empty);
  CHECK(LL_is_empty_list(copy));
  LL_soft_delete_list(copy);

  // copier fails on the 3rd node: returns nullptr and frees the 2 copied ones
  LinkedList list = new_list_of(5);
  copier_calls = 0;
  copier_fail_at = 3;
  deleter_calls = 0;
  CHECK(LL_copy(list, 0, failing_copier, counting_deleter) == nullptr);
  CHECK(deleter_calls == 2);

  // the original list is not modified
  CHECK(list_equals(list, (int[]){0, 1, 2, 3, 4}, 5));

  LL_soft_delete_list(list);
  LL_soft_delete_list(empty);
}

TEST(test_reverse) {
  LinkedList list = new_list_of(5);

  CHECK(LL_reverse(list) == list);
  CHECK(list_equals(list, (int[]){4, 3, 2, 1, 0}, 5));

  // last must be updated: add_last goes after the new last
  LL_add_last(list, &values[9]);
  CHECK(list_equals(list, (int[]){4, 3, 2, 1, 0, 9}, 6));

  // reverse twice: back to the original order
  LL_reverse(list);
  LL_reverse(list);
  CHECK(list_equals(list, (int[]){4, 3, 2, 1, 0, 9}, 6));

  // single node and two nodes
  LinkedList one = new_list_of(1);
  LL_reverse(one);
  CHECK(list_equals(one, (int[]){0}, 1));

  LinkedList two = new_list_of(2);
  LL_reverse(two);
  CHECK(list_equals(two, (int[]){1, 0}, 2));
  LL_add_first(two, &values[5]);
  LL_add_last(two, &values[6]);
  CHECK(list_equals(two, (int[]){5, 1, 0, 6}, 4));

  // errors: no crash
  LinkedList empty = LL_new_list();
  CHECK(LL_reverse(empty) == empty);
  CHECK(LL_reverse(nullptr) == nullptr);

  LL_soft_delete_list(empty);
  LL_soft_delete_list(two);
  LL_soft_delete_list(one);
  LL_soft_delete_list(list);
}

TEST(test_for_each) {
  LinkedList list = new_list_of(4);

  visited_count = 0;
  LL_for_each(list, record_action);
  CHECK(visited_count == 4);
  CHECK(memcmp(visited, (int[]){0, 1, 2, 3}, 4 * sizeof(int)) == 0);

  // errors: the action is not called
  LinkedList empty = LL_new_list();
  visited_count = 0;
  LL_for_each(empty, record_action);
  LL_for_each(nullptr, record_action);
  LL_for_each(list, nullptr);
  CHECK(visited_count == 0);

  LL_soft_delete_list(empty);
  LL_soft_delete_list(list);
}

// action that records the value and deletes the CURRENT node (always first)
static LinkedList for_each_list;
static void record_and_delete_action(void *data) {
  record_action(data);
  LL_soft_delete_first(for_each_list);
}

TEST(test_for_each_delete_current) {
  // AUDIT: the action deleted the current node and for_each read it after
  for_each_list = new_list_of(4);

  visited_count = 0;
  LL_for_each(for_each_list, record_and_delete_action);
  CHECK(visited_count == 4);
  CHECK(memcmp(visited, (int[]){0, 1, 2, 3}, 4 * sizeof(int)) == 0);
  CHECK(LL_is_empty_list(for_each_list));

  LL_soft_delete_list(for_each_list);
}

// counts forbidden operations that a callback could do on a locked list
static LinkedList locked_list;
static int lock_violations = 0;

static void nested_delete_action(void *data) {
  (void)data;
  // a nested for_each can't delete anything (not even its current node)
  if (LL_soft_delete_first(locked_list) != LL_ERROR) lock_violations++;
}

static void abusive_action(void *data) {
  if (data != &values[0]) return; // abuse only while visiting the first node

  // adding is blocked
  if (LL_add_first(locked_list, &values[9]) != LL_ERROR) lock_violations++;
  if (LL_add_last(locked_list, &values[9]) != LL_ERROR) lock_violations++;
  if (LL_insert_at(locked_list, &values[9], 1) != LL_ERROR) lock_violations++;

  // deleting any node other than the current one is blocked
  if (LL_soft_delete_last(locked_list) != LL_ERROR) lock_violations++;
  if (LL_soft_delete_at(locked_list, 1) != LL_ERROR) lock_violations++;
  LL_soft_delete_all(locked_list);
  LL_hard_delete_all(locked_list, nullptr);
  LL_reverse(locked_list);
  LL_soft_delete_list(locked_list);
  LL_hard_delete_list(locked_list, nullptr);
  LL_for_each(locked_list, nested_delete_action);
  if (!list_equals(locked_list, (int[]){0, 1, 2, 3}, 4)) lock_violations++;

  // reading is allowed
  if (LL_get_data(locked_list, 3) != &values[3]) lock_violations++;

  // the current node can be deleted, but only once
  if (LL_soft_delete_first(locked_list) != &values[0]) lock_violations++;
  if (LL_soft_delete_first(locked_list) != LL_ERROR) lock_violations++;
}

TEST(test_for_each_lock) {
  locked_list = new_list_of(4);
  lock_violations = 0;

  LL_for_each(locked_list, abusive_action);
  CHECK(lock_violations == 0);
  CHECK(list_equals(locked_list, (int[]){1, 2, 3}, 3));

  // the list is unlocked again after for_each
  CHECK(LL_add_last(locked_list, &values[4]) == &values[4]);
  CHECK(LL_soft_delete_first(locked_list) == &values[1]);
  CHECK(list_equals(locked_list, (int[]){2, 3, 4}, 3));

  LL_soft_delete_list(locked_list);
}

// deleter that checks the list is consistent and locked while it runs
static size_t expected_size = 0;
static void checking_deleter(void *data) {
  free(data);
  expected_size--;
  if (LL_size(locked_list) != expected_size) lock_violations++;
  if (expected_size && LL_get_data(locked_list, 0) == LL_ERROR) {
    lock_violations++;
  }
  if (LL_add_last(locked_list, &values[9]) != LL_ERROR) lock_violations++;
  if (LL_soft_delete_first(locked_list) != LL_ERROR) lock_violations++;
  LL_hard_delete_list(locked_list, nullptr); // must have no effect
}

static void *deleting_copier(const void *data) {
  if (LL_soft_delete_first(locked_list) != LL_ERROR) lock_violations++;
  return new_int(*(const int *)data);
}

TEST(test_callbacks_lock) {
  // AUDIT: hard_delete_all left the list corrupt while the deleter ran
  locked_list = new_owned_list_of(4);
  expected_size = 4;
  lock_violations = 0;
  LL_hard_delete_all(locked_list, checking_deleter);
  CHECK(lock_violations == 0);
  CHECK(LL_is_empty_list(locked_list));

  // copy: the copier can't modify the source list
  LL_hard_delete_list(locked_list, nullptr);
  locked_list = new_owned_list_of(2);
  LinkedList copy = LL_copy(locked_list, 0, deleting_copier, nullptr);
  CHECK(lock_violations == 0);
  CHECK(list_equals(copy, (int[]){0, 1}, 2));
  CHECK(list_equals(locked_list, (int[]){0, 1}, 2));

  LL_hard_delete_list(copy, nullptr);
  LL_hard_delete_list(locked_list, nullptr);
}

// =================================== MAIN ===================================

int main() {
  RUN(test_new_list);
  RUN(test_size);

  RUN(test_get_data);
  RUN(test_swap_data);

  RUN(test_add_first);
  RUN(test_add_last);
  RUN(test_insert_at);

  RUN(test_soft_delete_first);
  RUN(test_soft_delete_last);
  RUN(test_soft_delete_at);
  RUN(test_soft_delete_all);
  RUN(test_soft_delete_list);

  RUN(test_hard_delete_first_last_at);
  RUN(test_hard_delete_all_and_list);

  RUN(test_list_validations);

  RUN(test_contains);
  RUN(test_index_add_insert_validations);

  RUN(test_copy_memcpy);
  RUN(test_copy_with_copier);
  RUN(test_copy_null_data);
  RUN(test_copy_errors);
  RUN(test_reverse);
  RUN(test_for_each);
  RUN(test_for_each_delete_current);
  RUN(test_for_each_lock);
  RUN(test_callbacks_lock);

  printf("\n%d checks, %d failed\n", tests_run, tests_failed);

  return tests_failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
