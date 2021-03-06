#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#define LINKED_LIST_SIZE sizeof (struct _LinkedList)

#include <stddef.h>
#include <stdbool.h>

#include <linked_list/node.h>

typedef struct _LinkedList {
    Node first;
    Node last;
    size_t size;
} *LinkedList;

LinkedList LL_new_list();                                                       // create a linked list

void* LL_get_data(LinkedList list, size_t index);                               // get the data of a node, given an index
void LL_swap_data(LinkedList list, size_t index_1, size_t index_2);             // swap two values from the list

Node LL_add_first(LinkedList list, void *data);                                 // add a node with data to the beginning of the list
Node LL_add_last(LinkedList list, void *data);                                  // add a node with data to the end of the list
Node LL_insert_at(LinkedList list, void *data, size_t index);                   // insert a node with data at the end of the list

void LL_soft_delete_list(LinkedList list);                                      // delete the linked list without data
void* LL_soft_delete_first(LinkedList list);                                    // delete the first node without its data
void* LL_soft_delete_last(LinkedList list);                                     // delete the last node without its data
void* LL_soft_delete_at(LinkedList list, size_t index);                         // delete the node without its data, given an index
void LL_soft_delete_all(LinkedList list);                                       // delete all nodes without their data from the list

void LL_hard_delete_list(LinkedList list, void (*deleter)(void*));              // delete a list with its nodes and their data
void LL_hard_delete_first(LinkedList list, void (*deleter)(void*));             // delete the first node with its data
void LL_hard_delete_last(LinkedList list, void (*deleter)(void*));              // delete the last node with its data
void LL_hard_delete_at(LinkedList list, size_t index, void (*deleter)(void*));  // delete the node with its data, given an index
void LL_hard_delete_all(LinkedList list, void (*deleter)(void*));               // delete all nodes with their data from the list

bool LL_is_valid_list(LinkedList list);                                         // checks if the list is valid
bool LL_is_empty_list(LinkedList list);                                         // checks if the list is empty
bool LL_is_full_list(LinkedList list);                                          // checks if the list is full (size_t max) is max

bool LL_contains(LinkedList list, void *data);                                  // checks if the list contains the data
bool LL_is_valid_index(LinkedList list, size_t index);                          // checks if the list contains a node at the index
bool LL_is_valid_add(LinkedList list);                                          // checks if valid insert in list
bool LL_is_valid_insert(LinkedList list, size_t index);                         // checks if valid insert in list

LinkedList LL_copy(LinkedList list, size_t size_data);                          // copy a list
LinkedList LL_reverse(LinkedList list);                                         // reverse a list
LinkedList LL_randomize(LinkedList list);                                       // randomize a list

#endif // LINKED_LIST_H
