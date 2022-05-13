#include <linked_list/linked_list.h>

#include <stdlib.h>
#include <limits.h>

// linked list implements

StaticLinkedList new_list() {
    StaticLinkedList list = (StaticLinkedList) calloc(1, LINKED_LIST_SIZE);

    return list;
}

// node implements

Node new_node(void *data) {
    Node node = (Node) calloc(1, NODE_SIZE);

    if(!node) {
        return NULL;
    }

    node->data = data;

    return node;
}

Node get_node(StaticLinkedList list, const size_t index) {
    if(!is_valid_index(list, index)) {
        return NULL;
    }

    Node iterator = list->first;

    for(size_t i = 0; i < index; i++) {
        iterator = iterator->next;
    }

    return iterator;
}

// add implements

Node add_first_node(LinkedList list, void *data) {
    if(!is_valid_add(list)) {
        return NULL;
    }

    Node node = new_node(data);

    if(!node) {
        return NULL;
    }

    if(!list->first) {
        list->first = list->last = node;
    }
    else {
        node->next = list->first;
        list->first = node;
    }

    list->size++;

    return node;
}

Node add_last_node(LinkedList list, void *data) {
    if(!is_valid_add(list)) {
        return NULL;
    }

    Node node = new_node(data);

    if(!node) {
        return NULL;
    }

    if(!list->first) {
        list->first = list->last = node;
    }
    else {
        list->last = list->last->next = node;
    }

    list->size++;

    return node;
}

Node insert_a_node(LinkedList list, void *data, const size_t index) {
    if(!is_valid_insert(list, index)) {
        return NULL;
    }

    if(!index) {
        return add_first_node(list, data);
    }

    Node node = new_node(data);
    Node previous = get_node(list, index - 1);
    node->next = previous->next;
    previous->next = node;
    list->size++;

    return node;
}

// validations list implements

bool is_valid_list(LinkedList list) {
    return list ? true : false;
}

bool is_empty_list(LinkedList list) {
    if(!is_valid_list(list)) {
        return false;
    }

    return list->size ? false : true;
}

bool is_full_list(LinkedList list) {
    if(!is_valid_list(list)) {
        return false;
    }

    return list->size == ULONG_LONG_MAX ? true : false;
}

// index, add, insert validations implements

bool is_valid_index(LinkedList list, const size_t index) {
    if(!is_empty_list(list)) {
        return false;
    }

    return index < list->size ? true : false;
}

bool is_valid_add(LinkedList list) {
    return is_full_list(list) ? false : true;
}

bool is_valid_insert(LinkedList list, const size_t index) {
    return (is_valid_add(list) && is_valid_index(list, index)) ? true : false;
}

// reverse implements

void reverse_iterator_list(Node previous, Node first) {
    if(!first) {
        return;
    }

    Node next = first->next;
    first->next = previous;

    if(next) {
        reverse_iterator_list(first, next);
    }
}

LinkedList reverse_list(LinkedList list) {
    if(!list) {
        return NULL;
    }

    if(!list->size) {
        return list;
    }

    Node last = list->last;

    reverse_iterator_list(NULL, list->first);

    list->first = last;

    return list;
}
