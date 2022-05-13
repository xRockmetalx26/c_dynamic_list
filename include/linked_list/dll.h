#ifndef DINAMYC_LINKED_LIST_H
#define DINAMYC_LINKED_LIST_H

#include <stddef.h>

#include <linked_list/linked_list.h>

void delete_dll(DinamycLinkedList list);                    // delete a static linked list

void delete_first_dll(DinamycLinkedList list);              // delete the first node and return its data
void delete_last_dll(DinamycLinkedList list);               // delete the last node and return its data
void delete_at_dll(DinamycLinkedList list, size_t index);   // delete a node in the index of the list and return its data
void delete_all_dll(DinamycLinkedList list);                // delete all nodes in the list

#endif // DINAMYC_LINKED_LIST_H
