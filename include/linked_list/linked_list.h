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

LinkedList new_list();                                                       // create a linked list

void* get_data(LinkedList list, size_t index);                               // get the data of a node, given an index

Node add_first(LinkedList list, void *data);                                 // add a node with data to the beginning of the list
Node add_last(LinkedList list, void *data);                                  // add a node with data to the end of the list
Node insert_at(LinkedList list, void *data, size_t index);                   // insert a node with data at the end of the list

void soft_delete_list(LinkedList list);                                      // delete the linked list without data
void* soft_delete_first(LinkedList list);                                    // delete the first node without its data
void* soft_delete_last(LinkedList list);                                     // delete the last node without its data
void* soft_delete_at(LinkedList list, size_t index);                         // delete the node without its data, given an index
void soft_delete_all(LinkedList list);                                       // delete all nodes without their data from the list

void hard_delete_list(LinkedList list, void (*deleter)(void*));              // delete a list with its nodes and their data
void hard_delete_first(LinkedList list, void (*deleter)(void*));             // delete the first node with its data
void hard_delete_last(LinkedList list, void (*deleter)(void*));              // delete the last node with its data
void hard_delete_at(LinkedList list, size_t index, void (*deleter)(void*));  // delete the node with its data, given an index
void hard_delete_all(LinkedList list, void (*deleter)(void*));               // delete all nodes with their data from the list

bool is_valid_list(LinkedList list);                                         // checks if the list is valid
bool is_empty_list(LinkedList list);                                         // checks if the list is empty
bool is_full_list(LinkedList list);                                          // checks if the list is full (size_t max) is max

bool is_valid_index(LinkedList list, size_t index);                          // checks if the list contains a node at the index
bool is_valid_add(LinkedList list);                                          // checks if valid insert in list
bool is_valid_insert(LinkedList list, size_t index);                         // checks if valid insert in list

LinkedList reverse_list(LinkedList list);                                    // reverse a list

#endif // LINKED_LIST_H
