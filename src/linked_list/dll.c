#include <linked_list/dll.h>

#include <stdlib.h>

#include <linked_list/node.h>

// delete implements

void delete_dll(StaticLinkedList list) {
    if(is_valid_list(list)) {
        delete_all_dll(list);
        free(list);
    }
}

void delete_first_dll(StaticLinkedList list) {
    if(is_empty_list(list)) {
        return;
    }

    Node to_delete = list->first;
    list->first = list->first->next;
    list->size--;

    if(to_delete->data) {
        free(to_delete->data);
    }

    free(to_delete);
}

void delete_last_dll(StaticLinkedList list) {
    if(is_empty_list(list)) {
        return;
    }

    if(list->size == 1) {
        delete_first_dll(list);
        return;
    }

    Node previous = get_node(list, list->size - 2);
    Node to_delete = previous->next;
    previous->next = NULL;
    list->last = previous;
    list->size--;

    if(to_delete->data) {
        free(to_delete->data);
    }

    free(to_delete);
}

void delete_at_dll(StaticLinkedList list, const size_t index) {
    if(!is_valid_index(list, index)) {
        return;
    }

    if(!index) {
        delete_first_dll(list);
        return;
    }

    Node previous = get_node(list, index - 1);
    Node to_delete = previous->next;
    previous->next = to_delete->next;
    list->size--;

    if(to_delete->data) {
        free(to_delete->data);
    }

    free(to_delete);
}

void delete_all_dll(StaticLinkedList list) {
    if(is_empty_list(list)) {
        return;
    }

    Node iterator = list->first;

    while(iterator) {
        Node to_delete = iterator;
        iterator = iterator->next;

        if(to_delete->data) {
            free(to_delete->data);
        }

        free(to_delete);
    }

    list->last = list->first = NULL;
    list->size = 0;
}
