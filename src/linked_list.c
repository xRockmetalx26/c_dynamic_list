#include <linked_list.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// ============================= PRIVATE STRUCTS ==============================

typedef struct node {
  void *data;
  struct node *next;
} *Node;

struct linked_list {
  Node first;
  Node last;
  size_t size;
  Node lock; // nullptr: FREE, A NODE: ONLY IT CAN BE DELETED, LOCKED: NOTHING
};

#define NODE_SIZE sizeof(struct node)
#define LINKED_LIST_SIZE sizeof(struct linked_list)

// ================================== ERROR ===================================

// UNIQUE ADDRESS: NO VALID DATA CAN EVER POINT HERE
static char error;
void *const LL_ERROR = &error;

// ============================== LOCK (PRIVATE) ==============================

// SENTINEL: NEVER A NODE OF A LIST, SO NO NODE CAN BE DELETED
static struct node locked;
#define LOCKED (&locked)

// WHILE A USER CALLBACK RUNS, THE LIST CAN'T BE MODIFIED...
static bool is_locked(LinkedList list) {
  return list->lock != nullptr;
}

// ...EXCEPT FOR THE NODE THAT for_each IS VISITING, WHICH CAN BE DELETED
static bool can_delete(LinkedList list, Node node) {
  return !is_locked(list) || node == list->lock;
}

// =============================== LINKED LIST ================================

LinkedList LL_new_list() {
  return (LinkedList)calloc(1, LINKED_LIST_SIZE);
}

size_t LL_size(LinkedList list) {
  return list ? list->size : 0;
}

// ============================== NODE (PRIVATE) ==============================

static Node new_node(void *data) {
  if (data == LL_ERROR) return nullptr;

  Node node = (Node)calloc(1, NODE_SIZE);
  if (!node) return nullptr;

  node->data = data;

  return node;
}

static Node get_node(LinkedList list, const size_t index) {
  if (!LL_is_valid_index(list, index)) return nullptr;

  Node it = list->first;
  for (size_t i = 0; i < index; i++) {
    it = it->next;
  }

  return it;
}

// =================================== DATA ===================================

void *LL_get_data(LinkedList list, const size_t index) {
  Node node = get_node(list, index);
  if (!node) return LL_ERROR;

  return node->data;
}

void LL_swap_data(LinkedList list, size_t index_1, size_t index_2) {
  if (!LL_is_valid_index(list, index_1)) return;
  if (!LL_is_valid_index(list, index_2)) return;
  if (index_1 == index_2) return;

  Node node_1 = nullptr;
  Node node_2 = nullptr;
  size_t i = 0;
  for (Node it = list->first; it && (!node_1 || !node_2); it = it->next) {
    if (i == index_1) node_1 = it;
    if (i == index_2) node_2 = it;

    i++;
  }

  void *data_copy = node_1->data;
  node_1->data = node_2->data;
  node_2->data = data_copy;
}

// =================================== ADD ====================================

void *LL_add_first(LinkedList list, void *data) {
  if (!LL_is_valid_add(list)) return LL_ERROR;

  Node node = new_node(data);
  if (!node) return LL_ERROR;
  if (!list->first) {
    list->first = list->last = node;
  } else {
    node->next = list->first;
    list->first = node;
  }

  list->size++;

  return data;
}

void *LL_add_last(LinkedList list, void *data) {
  if (!LL_is_valid_add(list)) return LL_ERROR;

  Node node = new_node(data);
  if (!node) return LL_ERROR;
  if (!list->first) {
    list->first = list->last = node;
  } else {
    list->last = list->last->next = node;
  }

  list->size++;

  return data;
}

void *LL_insert_at(LinkedList list, void *data, const size_t index) {
  if (!LL_is_valid_insert(list, index)) return LL_ERROR;
  if (!index) return LL_add_first(list, data);
  if (index == list->size) return LL_add_last(list, data);

  Node node = new_node(data);
  if (!node) return LL_ERROR;

  Node previous = get_node(list, index - 1);
  node->next = previous->next;
  previous->next = node;
  list->size++;

  return data;
}

// ============================= DELETE (PRIVATE) =============================

static void delete_data(void *data, LL_Deleter deleter) {
  if (data && data != LL_ERROR) {
    if (deleter) {
      deleter(data);
    } else {
      free(data);
    }
  }
}

// UNLINK AND FREE THE NODE AFTER previous (nullptr: THE FIRST NODE)
// THE ONLY PLACE WHERE A SINGLE NODE IS DELETED (AND THE LOCK IS CHECKED)
static void *unlink_node(LinkedList list, Node previous) {
  Node node = previous ? previous->next : list->first;
  if (!can_delete(list, node)) return LL_ERROR;
  if (previous) {
    previous->next = node->next;
  } else {
    list->first = node->next;
  }
  if (node == list->last) list->last = previous;

  list->size--;

  if (node == list->lock) list->lock = LOCKED;

  void *data = node->data;

  free(node);

  return data;
}

// =============================== SOFT DELETE ================================

void LL_soft_delete_list(LinkedList list) {
  if (LL_is_valid_list(list) && !is_locked(list)) {
    LL_soft_delete_all(list);
    free(list);
  }
}

void *LL_soft_delete_first(LinkedList list) {
  if (LL_is_empty_list(list)) return LL_ERROR;

  return unlink_node(list, nullptr);
}

void *LL_soft_delete_last(LinkedList list) {
  if (LL_is_empty_list(list)) return LL_ERROR;

  const size_t size = list->size;

  return unlink_node(list, size == 1 ? nullptr : get_node(list, size - 2));
}

void *LL_soft_delete_at(LinkedList list, const size_t index) {
  if (!LL_is_valid_index(list, index)) return LL_ERROR;

  return unlink_node(list, index ? get_node(list, index - 1) : nullptr);
}

void LL_soft_delete_all(LinkedList list) {
  if (LL_is_empty_list(list) || is_locked(list)) return;

  Node it = list->first;
  Node to_delete;

  while (it) {
    to_delete = it;
    it = it->next;

    free(to_delete);
  }

  list->last = list->first = nullptr;
  list->size = 0;
}

// =============================== HARD DELETE ================================

void LL_hard_delete_list(LinkedList list, LL_Deleter deleter) {
  if (LL_is_valid_list(list) && !is_locked(list)) {
    LL_hard_delete_all(list, deleter);
    free(list);
  }
}

void LL_hard_delete_first(LinkedList list, LL_Deleter deleter) {
  delete_data(LL_soft_delete_first(list), deleter);
}

void LL_hard_delete_last(LinkedList list, LL_Deleter deleter) {
  delete_data(LL_soft_delete_last(list), deleter);
}

void LL_hard_delete_at(LinkedList list, const size_t index,
                       LL_Deleter deleter) {
  delete_data(LL_soft_delete_at(list, index), deleter);
}

void LL_hard_delete_all(LinkedList list, LL_Deleter deleter) {
  if (LL_is_empty_list(list) || is_locked(list)) return;

  list->lock = LOCKED;
  while (list->first) {
    Node to_delete = list->first;
    list->first = to_delete->next;
    list->size--;

    if (!list->first) list->last = nullptr;

    delete_data(to_delete->data, deleter);
    free(to_delete);
  }
  list->lock = nullptr;
}

// ============================= LIST VALIDATIONS =============================

bool LL_is_valid_list(LinkedList list) {
  return list != nullptr;
}

bool LL_is_empty_list(LinkedList list) {
  if (!LL_is_valid_list(list)) return true;

  return list->size == 0;
}

bool LL_is_full_list(LinkedList list) {
  if (!LL_is_valid_list(list)) return false;

  return list->size == SIZE_MAX;
}

// ====================== INDEX, ADD, INSERT VALIDATIONS ======================

bool LL_contains(LinkedList list, void *data) {
  if (LL_is_empty_list(list)) return false;

  Node it = list->first;
  while (it && it->data != data) {
    it = it->next;
  }

  return it != nullptr;
}

bool LL_is_valid_index(LinkedList list, const size_t index) {
  if (LL_is_empty_list(list)) return false;

  return index < list->size;
}

bool LL_is_valid_add(LinkedList list) {
  if (!LL_is_valid_list(list) || is_locked(list)) return false;

  return !LL_is_full_list(list);
}

bool LL_is_valid_insert(LinkedList list, const size_t index) {
  if (!LL_is_valid_add(list)) return false;

  return index <= list->size;
}

// ================================ UTILITIES =================================

LinkedList LL_copy(LinkedList list, const size_t size_data, LL_Copier copier,
                   LL_Deleter deleter) {
  if (!LL_is_valid_list(list)) return nullptr;

  LinkedList list_copy = LL_new_list();
  if (!list_copy) return nullptr;

  bool failed = false;
  const Node saved_lock = list->lock;
  list->lock = LOCKED;

  for (Node it = list->first; it; it = it->next) {
    void *data = nullptr;
    if (it->data) {
      data = copier ? copier(it->data) : malloc(size_data);
      if (!data) {
        failed = true;
        break;
      }
      if (!copier) memcpy(data, it->data, size_data);
    }

    if (LL_add_last(list_copy, data) == LL_ERROR) {
      delete_data(data, deleter);

      failed = true;

      break;
    }
  }

  list->lock = saved_lock;

  if (failed) {
    LL_hard_delete_list(list_copy, deleter);

    return nullptr;
  }

  return list_copy;
}

LinkedList LL_reverse(LinkedList list) {
  if (LL_is_empty_list(list) || list->size == 1 || is_locked(list)) {
    return list;
  }

  Node previous = nullptr;
  Node current = list->first;
  list->last = list->first;

  while (current) {
    Node next = current->next;
    current->next = previous;
    previous = current;
    current = next;
  }

  list->first = previous;

  return list;
}

void LL_for_each(LinkedList list, LL_Action action, void *context) {
  if (LL_is_empty_list(list) || !action) return;

  const Node saved_lock = list->lock;

  for (Node it = list->first; it;) {
    Node next = it->next;
    list->lock = saved_lock ? LOCKED : it;

    action(it->data, context);

    it = next;
  }

  list->lock = saved_lock;
}
