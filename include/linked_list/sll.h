#ifndef STATIC_LINKED_LIST_H
#define STATIC_LINKED_LIST_H

#include <stddef.h>

#include <linked_list/linked_list.h>

void delete_sll(StaticLinkedList list);                    // delete a static linked list

void* delete_first_sll(StaticLinkedList list);              // delete the first node and return its data
void* delete_last_sll(StaticLinkedList list);               // delete the last node and return its data
void* delete_at_sll(StaticLinkedList list, size_t index);   // delete a node in the index of the list and return its data
void delete_all_sll(StaticLinkedList list);                // delete all nodes in the list

#endif // STATIC_LINKED_LIST_H
