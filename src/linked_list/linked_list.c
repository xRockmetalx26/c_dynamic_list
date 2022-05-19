#include <linked_list/linked_list.h>

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// linked list implements

LinkedList LL_new_list() {
    LinkedList list = (LinkedList) calloc(1, LINKED_LIST_SIZE);
    if(!list) {
        fprintf(stderr, "\nERROR in LL_new_list(), calloc() is null.\n");
        return NULL;
    }

    return list;
}

// node implements

Node new_node(void *data) {
    Node node = (Node) calloc(1, NODE_SIZE);
    if(!node) {
        fprintf(stderr, "\nERROR in new_node(), calloc() is null.\n");
        return NULL;
    }

    node->data = data;

    return node;
}

Node get_node(LinkedList list, const size_t index) {
    if(!LL_is_valid_index(list, index)) {
        return NULL;
    }

    Node iterator = list->first;
    for(size_t i = 0; i < index; i++) {
        iterator = iterator->next;
    }

    return iterator;
}

void* LL_get_data(LinkedList list, const size_t index) {
    Node node = get_node(list, index);
    if(!node) {
        return NULL;
    }

    return node->data;
}

void LL_swap_data(LinkedList list, size_t index_1, size_t index_2) {
    if(LL_is_valid_index(list, index_1) && LL_is_valid_index(list, index_2)) {
        Node node_1 = get_node(list, index_1);
        void *data_copy = node_1->data;
        Node node_2 = get_node(list, index_2);
        node_1->data = node_2->data;
        node_2->data = data_copy;
    }
}

// add implements

Node LL_add_first(LinkedList list, void *data) {
    if(!LL_is_valid_add(list)) {
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

Node LL_add_last(LinkedList list, void *data) {
    if(!LL_is_valid_add(list)) {
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

Node LL_insert_at(LinkedList list, void *data, const size_t index) {
    if(!LL_is_valid_insert(list, index)) {
        return NULL;
    }

    if(!index) {
        return LL_add_first(list, data);
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

void LL_soft_delete_list(LinkedList list) {
    if(LL_is_valid_list(list)) {
        LL_soft_delete_all(list);
        free(list);
    }
}

void* LL_soft_delete_first(LinkedList list) {
    if(LL_is_empty_list(list)) {
        return NULL;
    }

    Node to_delete = list->first;
    void *data = to_delete->data;
    list->first = list->first->next;
    list->size--;

    free(to_delete);

    return data;
}

void* LL_soft_delete_last(LinkedList list) {
    if(LL_is_empty_list(list)) {
        return NULL;
    }

    if(list->size == 1) {
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

void* LL_soft_delete_at(LinkedList list, const size_t index) {
    if(!LL_is_valid_index(list, index)) {
        return NULL;
    }

    if(!index) {
        return LL_soft_delete_at(list, index);
    }

    Node previous = get_node(list, index - 1);
    Node to_delete = previous->next;
    void *data = to_delete->data;
    previous->next = to_delete->next;
    list->size--;

    free(to_delete);

    return data;
}

void LL_soft_delete_all(LinkedList list) {
    if(LL_is_empty_list(list)) {
        return;
    }

    Node it = list->first;
    Node to_delete;
    while(it) {
        to_delete = it;
        it = it->next;

        free(to_delete);
    }

    list->last = list->first = NULL;
    list->size = 0;
}

// hard delete implements

void LL_hard_delete_list(LinkedList list, void (*deleter)(void*)) {
    if(LL_is_valid_list(list)) {
        LL_hard_delete_all(list, deleter);
        free(list);
    }
}

void LL_hard_delete_first(LinkedList list, void (*deleter)(void*)) {
    delete_data(LL_soft_delete_first(list), deleter);
}

void LL_hard_delete_last(LinkedList list, void (*deleter)(void*)) {
    delete_data(LL_soft_delete_last(list), deleter);
}

void LL_hard_delete_at(LinkedList list, const size_t index, void (*deleter)(void*)) {
    delete_data(LL_soft_delete_at(list, index), deleter);
}

void LL_hard_delete_all(LinkedList list, void (*deleter)(void*)) {
    if(LL_is_empty_list(list)) {
        return;
    }

    Node it = list->first;
    Node to_delete;
    while(it) {
        to_delete = it;
        it = it->next;

        delete_data(to_delete->data, deleter);
        free(to_delete);
    }

    list->last = list->first = NULL;
    list->size = 0;
}

// validations list implements

bool LL_is_valid_list(LinkedList list) {
    if(!list) {
        fprintf(stderr, "\nERROR, linked list is null.\n");
        return false;
    }

    return true;
}

bool LL_is_empty_list(LinkedList list) {
    if(!LL_is_valid_list(list)) {
        return false;
    }

    return list->size ? false : true;
}

bool LL_is_full_list(LinkedList list) {
    if(!LL_is_valid_list(list)) {
        return false;
    }

    return list->size == ULONG_LONG_MAX ? true : false;
}

// index, add, insert validations implements

bool LL_contains(LinkedList list, void *data) {
    if(LL_is_empty_list(list)) {
        return false;
    }

    for(Node it = list->first; it; it = it->next) {
        if(it->data == data) {
            return true;
        }
    }

    return false;
}

bool LL_is_valid_index(LinkedList list, const size_t index) {
    if(LL_is_empty_list(list)) {
        return false;
    }

    if(index >= list->size) {
        fprintf(stderr, "\nERROR in LL_is_valid_index(), index >= linked list size.\n");
        return false;
    }

    return true;
}

bool LL_is_valid_add(LinkedList list) {
    return LL_is_full_list(list) ? false : true;
}

bool LL_is_valid_insert(LinkedList list, const size_t index) {
    return (LL_is_valid_add(list) && LL_is_valid_index(list, index)) ? true : false;
}

// utilities

LinkedList LL_copy(LinkedList list, const size_t size_data) {
    if(LL_is_empty_list(list)) {
        return list;
    }

    LinkedList list_copy = LL_new_list();
    if(!list_copy) {
        fprintf(stderr, "\nERROR in LL_copy(), LL_new_list() is null.\n");
        return NULL;
    }

    void *data;
    for(Node it = list->first; it; it = it->next) {
        data = malloc(size_data);
        if(!data) {
            fprintf(stderr, "\nERROR in LL_copy(), malloc() is null.\n");
            return NULL;
        }

        memcpy(data, it->data, size_data);
        LL_add_last(list_copy, data);
    }

    return list_copy;
}

LinkedList LL_reverse(LinkedList list) {
    if(LL_is_empty_list(list) || list->size == 1) {
        return list;
    }

    const size_t mid_data = list->size / 2;

    for(size_t i = 0; i < mid_data; i++) {
        LL_swap_data(list, i, list->size - 1 - i);
    }

    return list;
}

LinkedList LL_randomize(LinkedList list) {
    if(LL_is_empty_list(list) || list->size == 1) {
        return list;
    }

    srand(time(NULL));

    for(size_t j = 0; j < list->size; j++) {
        LL_swap_data(list, j, rand() % list->size);
    }

    return list;
}
