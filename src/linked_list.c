#include <linked_list.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// LINKED LIST IMPLEMENTS

LinkedList LL_new_list() {
  return (LinkedList)calloc(1, LINKED_LIST_SIZE);
}

// NODE IMPLEMENTS

Node new_node(void *data) {
  Node node = (Node)calloc(1, NODE_SIZE);
  if (!node) {
    return NULL;
  }

  node->data = data;

  return node;
}

Node get_node(LinkedList list, const size_t index) {
  if (!LL_is_valid_index(list, index)) {
    return NULL;
  }

  Node it = list->first;
  for (size_t i = 0; i < index; i++) {
    it = it->next;
  }

  return it;
}

void *LL_get_data(LinkedList list, const size_t index) {
  Node node = get_node(list, index);
  if (!node) {
    return NULL;
  }

  return node->data;
}

void LL_swap_data(LinkedList list, size_t index_1, size_t index_2) {
  if (!LL_is_valid_index(list, index_1) || !LL_is_valid_index(list, index_2)) {
    return;
  }

  if (index_1 == index_2) {
    return;
  }

  Node node_1 = NULL;
  Node node_2 = NULL;
  size_t i = 0;
  for (Node it = list->first; it && (!node_1 || !node_2); it = it->next) {
    if (i == index_1) {
      node_1 = it;
    }
    if (i == index_2) {
      node_2 = it;
    }

    i++;
  }

  void *data_copy = node_1->data;
  node_1->data = node_2->data;
  node_2->data = data_copy;
}

// ADD IMPLEMENTS

Node LL_add_first(LinkedList list, void *data) {
  if (!LL_is_valid_add(list)) {
    return NULL;
  }

  Node node = new_node(data);
  if (!node) {
    return NULL;
  }

  if (!list->first) {
    list->first = list->last = node;
  } else {
    node->next = list->first;
    list->first = node;
  }

  list->size++;

  return node;
}

Node LL_add_last(LinkedList list, void *data) {
  if (!LL_is_valid_add(list)) {
    return NULL;
  }

  Node node = new_node(data);
  if (!node) {
    return NULL;
  }

  if (!list->first) {
    list->first = list->last = node;
  } else {
    list->last = list->last->next = node;
  }

  list->size++;

  return node;
}

Node LL_insert_at(LinkedList list, void *data, const size_t index) {
  if (!LL_is_valid_insert(list, index)) {
    return NULL;
  }

  if (!index) {
    return LL_add_first(list, data);
  }

  if (index == list->size) {
    return LL_add_last(list, data);
  }

  Node node = new_node(data);
  if (!node) {
    return NULL;
  }

  Node previous = get_node(list, index - 1);
  node->next = previous->next;
  previous->next = node;
  list->size++;

  return node;
}

// DELETE IMPLEMENTS

void delete_data(void *data, void (*deleter)(void *)) {
  if (data) {
    if (deleter) {
      deleter(data);
    } else {
      free(data);
    }
  }
}

// SOFT DELETE IMPLEMENTS

void LL_soft_delete_list(LinkedList list) {
  if (LL_is_valid_list(list)) {
    LL_soft_delete_all(list);
    free(list);
  }
}

void *LL_soft_delete_first(LinkedList list) {
  if (LL_is_empty_list(list)) {
    return NULL;
  }

  Node to_delete = list->first;
  void *data = to_delete->data;
  list->first = list->first->next;
  list->size--;

  if (!list->first) {
    list->last = NULL;
  }

  free(to_delete);

  return data;
}

void *LL_soft_delete_last(LinkedList list) {
  if (LL_is_empty_list(list)) {
    return NULL;
  }

  if (list->size == 1) {
    return LL_soft_delete_first(list);
  }

  Node previous = get_node(list, list->size - 2);
  Node to_delete = previous->next;
  void *data = to_delete->data;
  previous->next = NULL;
  list->last = previous;
  list->size--;

  free(to_delete);

  return data;
}

void *LL_soft_delete_at(LinkedList list, const size_t index) {
  if (!LL_is_valid_index(list, index)) {
    return NULL;
  }

  if (!index) {
    return LL_soft_delete_first(list);
  }

  Node previous = get_node(list, index - 1);
  Node to_delete = previous->next;
  void *data = to_delete->data;
  previous->next = to_delete->next;

  if (to_delete == list->last) {
    list->last = previous;
  }

  list->size--;

  free(to_delete);

  return data;
}

void LL_soft_delete_all(LinkedList list) {
  if (LL_is_empty_list(list)) {
    return;
  }

  Node it = list->first;
  Node to_delete;
  while (it) {
    to_delete = it;
    it = it->next;

    free(to_delete);
  }

  list->last = list->first = NULL;
  list->size = 0;
}

// HARD DELETE IMPLEMENTS

void LL_hard_delete_list(LinkedList list, void (*deleter)(void *)) {
  if (LL_is_valid_list(list)) {
    LL_hard_delete_all(list, deleter);
    free(list);
  }
}

void LL_hard_delete_first(LinkedList list, void (*deleter)(void *)) {
  delete_data(LL_soft_delete_first(list), deleter);
}

void LL_hard_delete_last(LinkedList list, void (*deleter)(void *)) {
  delete_data(LL_soft_delete_last(list), deleter);
}

void LL_hard_delete_at(LinkedList list, const size_t index,
                       void (*deleter)(void *)) {
  delete_data(LL_soft_delete_at(list, index), deleter);
}

void LL_hard_delete_all(LinkedList list, void (*deleter)(void *)) {
  if (LL_is_empty_list(list)) {
    return;
  }

  Node it = list->first;
  Node to_delete;
  while (it) {
    to_delete = it;
    it = it->next;

    delete_data(to_delete->data, deleter);
    free(to_delete);
  }

  list->last = list->first = NULL;
  list->size = 0;
}

// VALIDATIONS LIST IMPLEMENTS

bool LL_is_valid_list(LinkedList list) {
  return list ? true : false;
}

bool LL_is_empty_list(LinkedList list) {
  if (!LL_is_valid_list(list)) {
    return true;
  }

  return list->size ? false : true;
}

bool LL_is_full_list(LinkedList list) {
  if (!LL_is_valid_list(list)) {
    return false;
  }

  return list->size == SIZE_MAX ? true : false;
}

// INDEX, ADD, INSERT VALIDATIONS IMPLEMENTS

bool LL_contains(LinkedList list, void *data) {
  if (LL_is_empty_list(list)) {
    return false;
  }

  for (Node it = list->first; it; it = it->next) {
    if (it->data == data) {
      return true;
    }
  }

  return false;
}

bool LL_is_valid_index(LinkedList list, const size_t index) {
  if (LL_is_empty_list(list)) {
    return false;
  }

  return index < list->size;
}

bool LL_is_valid_add(LinkedList list) {
  if (!LL_is_valid_list(list)) {
    return false;
  }

  return LL_is_full_list(list) ? false : true;
}

bool LL_is_valid_insert(LinkedList list, const size_t index) {
  if (!LL_is_valid_add(list)) {
    return false;
  }

  return index <= list->size;
}

// UTILITIES

LinkedList LL_copy(LinkedList list, const size_t size_data,
                   void *(*copier)(const void *), void (*deleter)(void *)) {
  if (!LL_is_valid_list(list)) {
    return NULL;
  }

  LinkedList list_copy = LL_new_list();
  if (!list_copy) {
    return NULL;
  }

  void *data;
  for (Node it = list->first; it; it = it->next) {
    if (copier) {
      data = copier(it->data);
    } else {
      data = malloc(size_data);
      if (data) {
        memcpy(data, it->data, size_data);
      }
    }

    if (!data) {
      LL_hard_delete_list(list_copy, deleter);
      return NULL;
    }

    if (!LL_add_last(list_copy, data)) {
      delete_data(data, deleter);
      LL_hard_delete_list(list_copy, deleter);
      return NULL;
    }
  }

  return list_copy;
}

LinkedList LL_reverse(LinkedList list) {
  if (LL_is_empty_list(list) || list->size == 1) {
    return list;
  }

  Node previous = NULL;
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
