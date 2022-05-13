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

typedef LinkedList StaticLinkedList;                            // does not delete the data
typedef LinkedList DinamycLinkedList;                           // delete the data

LinkedList new_list();                                          // create a linked list

Node get_node(LinkedList list, size_t index);                   // get a node from the list

Node add_first_node(LinkedList list, void *data);               // add a node to the beginning of the list
Node add_last_node(LinkedList list, void *data);                // add a node to the last of the list
Node insert_a_node(LinkedList list, void *data, size_t index);  // insert a node in the index of the list

bool is_valid_list(LinkedList list);                            // checks if the list is valid
bool is_empty_list(LinkedList list);                            // checks if the list is empty
bool is_full_list(LinkedList list);                             // checks if the list is full (size_t max) is max

bool is_valid_index(LinkedList list, size_t index);             // checks if the list contains a node at the index
bool is_valid_add(LinkedList list);                             // checks if valid insert in list
bool is_valid_insert(LinkedList list, size_t index);            // checks if valid insert in list

LinkedList reverse_list(LinkedList list);                       // reverse a list

#endif // LINKED_LIST_H
