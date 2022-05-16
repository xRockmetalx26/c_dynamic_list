#include <linked_list/linked_list.h>

#include <stdlib.h>
#include <limits.h>

// linked list implements

LinkedList new_list() {
    LinkedList list = (LinkedList) calloc(1, LINKED_LIST_SIZE);

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

Node get_node(LinkedList list, const size_t index) {
    if(!is_valid_index(list, index)) {
        return NULL;
    }

    Node iterator = list->first;

    for(size_t i = 0; i < index; i++) {
        iterator = iterator->next;
    }

    return iterator;
}

void* get_data(LinkedList list, const size_t index) {
    return get_node(list, index)->data;
}

// add implements

Node add_first(LinkedList list, void *data) {
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

Node add_last(LinkedList list, void *data) {
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

Node insert_at(LinkedList list, void *data, const size_t index) {
    if(!is_valid_insert(list, index)) {
        return NULL;
    }

    if(!index) {
        return add_first(list, data);
    }

    Node node = new_node(data);
    Node previous = get_node(list, index - 1);
    node->next = previous->next;
    previous->next = node;
    list->size++;

    return node;
}

// delete implements

void delete_data(void *data, void (*deleter)(void*)) {
    if(data) {
        if(deleter) {
            deleter(data);
        }
        else {
            free(data);
        }
    }
}

// soft delete implements

void soft_delete_list(LinkedList list) {
    if(is_valid_list(list)) {
        soft_delete_all(list);
        free(list);
    }
}

void* soft_delete_first(LinkedList list) {
    if(is_empty_list(list)) {
        return NULL;
    }

    Node to_delete = list->first;
    void *data = to_delete->data;
    list->first = list->first->next;
    list->size--;

    free(to_delete);

    return data;
}

void* soft_delete_last(LinkedList list) {
    if(is_empty_list(list)) {
        return NULL;
    }

    if(list->size == 1) {
        return soft_delete_last(list);
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

void* soft_delete_at(LinkedList list, const size_t index) {
    if(!is_valid_index(list, index)) {
        return NULL;
    }

    if(!index) {
        return soft_delete_at(list, index);
    }

    Node previous = get_node(list, index - 1);
    Node to_delete = previous->next;
    void *data = to_delete->data;
    previous->next = to_delete->next;
    list->size--;

    free(to_delete);

    return data;
}

void soft_delete_all(LinkedList list) {
    if(is_empty_list(list)) {
        return;
    }

    Node iterator = list->first;

    while(iterator) {
        Node to_delete = iterator;
        iterator = iterator->next;

        free(to_delete);
    }

    list->last = list->first = NULL;
    list->size = 0;
}

// hard delete implements

void hard_delete_list(LinkedList list, void (*deleter)(void*)) {
    if(is_valid_list(list)) {
        hard_delete_all(list, deleter);
        free(list);
    }
}

void hard_delete_first(LinkedList list, void (*deleter)(void*)) {
    delete_data(soft_delete_first(list), deleter);
}

void hard_delete_last(LinkedList list, void (*deleter)(void*)) {
    delete_data(soft_delete_last(list), deleter);
}

void hard_delete_at(LinkedList list, const size_t index, void (*deleter)(void*)) {
    delete_data(soft_delete_at(list, index), deleter);
}

void hard_delete_all(LinkedList list, void (*deleter)(void*)) {
    if(is_empty_list(list)) {
        return;
    }

    Node iterator = list->first;

    while(iterator) {
        Node to_delete = iterator;
        iterator = iterator->next;

        delete_data(to_delete->data, deleter);
        free(to_delete);
    }

    list->last = list->first = NULL;
    list->size = 0;
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
    if(is_empty_list(list)) {
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
