#include <linked_list/sll.h>

#include <stdlib.h>

#include <linked_list/node.h>

// delete implements

void delete_sll(StaticLinkedList list) {
    if(is_valid_list(list)) {
        delete_all_sll(list);
        free(list);
    }
}

void* delete_first_sll(StaticLinkedList list) {
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

void* delete_last_sll(StaticLinkedList list) {
    if(is_empty_list(list)) {
        return NULL;
    }

    if(list->size == 1) {
        return delete_first_sll(list);
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

void* delete_at_sll(StaticLinkedList list, const size_t index) {
    if(!is_valid_index(list, index)) {
        return NULL;
    }

    if(!index) {
        return delete_first_sll(list);
    }

    Node previous = get_node(list, index - 1);
    Node to_delete = previous->next;
    void *data = to_delete->data;
    previous->next = to_delete->next;
    list->size--;

    free(to_delete);

    return data;
}

void delete_all_sll(StaticLinkedList list) {
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
