#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct linked_list *LinkedList;

// returned when an operation fails (invalid index, empty list, not added)
// different from nullptr, which is valid data stored in the list
extern void *const LL_ERROR;

// ================================ CALLBACKS =================================

// frees the data of a node (nullptr: free is used)
typedef void (*LL_Deleter)(void *data);

// returns a deep copy of the data, or nullptr if it fails
typedef void *(*LL_Copier)(const void *data);

// receives the data of a node and the context given to LL_for_each
typedef void (*LL_Action)(void *data, void *context);

// while the list runs a callback (for_each action, hard_delete_all deleter,
// copy copier) it is locked: adding or deleting fails (LL_ERROR or no
// effect), so a callback can't break the list
// only exception: the for_each action may delete the node it is visiting

// =============================== LINKED LIST ================================

// create a linked list
LinkedList LL_new_list();

// number of nodes in the list (0 if the list is nullptr)
size_t LL_size(LinkedList list);

// =================================== DATA ===================================

// get the data of a node, given an index
// returns LL_ERROR if the index is not valid
void *LL_get_data(LinkedList list, size_t index);

// swap two values from the list
void LL_swap_data(LinkedList list, size_t index_1, size_t index_2);

// =================================== ADD ====================================

// add a node with data to the beginning of the list
// returns the data, or LL_ERROR if it could not be added
void *LL_add_first(LinkedList list, void *data);

// add a node with data to the end of the list
// returns the data, or LL_ERROR if it could not be added
void *LL_add_last(LinkedList list, void *data);

// insert a node with data at the given index (0 to size)
// returns the data, or LL_ERROR if it could not be inserted
void *LL_insert_at(LinkedList list, void *data, size_t index);

// =============================== SOFT DELETE ================================

// delete the linked list without data
void LL_soft_delete_list(LinkedList list);

// delete the first node without its data and return it
// returns LL_ERROR if the list is empty
void *LL_soft_delete_first(LinkedList list);

// delete the last node without its data and return it
// returns LL_ERROR if the list is empty
void *LL_soft_delete_last(LinkedList list);

// delete the node without its data, given an index, and return it
// returns LL_ERROR if the index is not valid
void *LL_soft_delete_at(LinkedList list, size_t index);

// delete all nodes without their data from the list
void LL_soft_delete_all(LinkedList list);

// =============================== HARD DELETE ================================

// delete a list with its nodes and their data
void LL_hard_delete_list(LinkedList list, LL_Deleter deleter);

// delete the first node with its data
void LL_hard_delete_first(LinkedList list, LL_Deleter deleter);

// delete the last node with its data
void LL_hard_delete_last(LinkedList list, LL_Deleter deleter);

// delete the node with its data, given an index
void LL_hard_delete_at(LinkedList list, size_t index, LL_Deleter deleter);

// delete all nodes with their data from the list
void LL_hard_delete_all(LinkedList list, LL_Deleter deleter);

// ============================= LIST VALIDATIONS =============================

// checks if the list is valid (not nullptr)
bool LL_is_valid_list(LinkedList list);

// checks if the list is empty
bool LL_is_empty_list(LinkedList list);

// checks if the list is full (size reached SIZE_MAX)
bool LL_is_full_list(LinkedList list);

// ====================== INDEX, ADD, INSERT VALIDATIONS ======================

// checks if the list contains the data
bool LL_contains(LinkedList list, void *data);

// checks if the list contains a node at the index
bool LL_is_valid_index(LinkedList list, size_t index);

// checks if a node can be added to the list
bool LL_is_valid_add(LinkedList list);

// checks if a node can be inserted at the index (0 to size)
bool LL_is_valid_insert(LinkedList list, size_t index);

// ================================ UTILITIES =================================

// copy a list (copier: deep copy; nullptr: memcpy of size_data bytes)
// (deleter: frees the copied data if the copy fails; nullptr: free)
LinkedList LL_copy(LinkedList list, size_t size_data, LL_Copier copier,
                   LL_Deleter deleter);

// reverse a list
LinkedList LL_reverse(LinkedList list);

// call action with the data of every node and the context (any pointer, or
// nullptr), from first to last (O(n))
// the action may delete the current node, any other change is blocked
void LL_for_each(LinkedList list, LL_Action action, void *context);

#ifdef __cplusplus
}
#endif

#endif // LINKED_LIST_H
