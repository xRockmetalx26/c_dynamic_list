#include <linked_list/dll.h>

#include <stdlib.h>

#include <linked_list/node.h>

// delete implements

void delete_node(Node node, void (*deleter)(void*)) {
    if(!node) {
        return;
    }

    if(node->data) {
        if(deleter) {
            deleter(node->data);
        }
        else {
            free(node->data);
        }
    }

    free(node);
}

void delete_dll(DinamycLinkedList list, void (*deleter)(void*)) {
    if(is_valid_list(list)) {
        delete_all_dll(list, deleter);
        free(list);
    }
}

void delete_first_dll(DinamycLinkedList list, void (*deleter)(void*)) {
    if(is_empty_list(list)) {
        return;
    }

    Node to_delete = list->first;
    list->first = list->first->next;
    list->size--;

    delete_node(to_delete, deleter);
}

void delete_last_dll(DinamycLinkedList list, void (*deleter)(void*)) {
    if(is_empty_list(list)) {
        return;
    }

    if(list->size == 1) {
        delete_first_dll(list, deleter);
        return;
    }

    Node previous = get_node(list, list->size - 2);
    Node to_delete = previous->next;
    previous->next = NULL;
    list->last = previous;
    list->size--;

    delete_node(to_delete, deleter);
}

void delete_at_dll(DinamycLinkedList list, const size_t index, void (*deleter)(void*)) {
    if(!is_valid_index(list, index)) {
        return;
    }

    if(!index) {
        delete_first_dll(list, deleter);
        return;
    }

    Node previous = get_node(list, index - 1);
    Node to_delete = previous->next;
    previous->next = to_delete->next;
    list->size--;

    delete_node(to_delete, deleter);
}

void delete_all_dll(DinamycLinkedList list, void (*deleter)(void*)) {
    if(is_empty_list(list)) {
        return;
    }

    Node iterator = list->first;

    while(iterator) {
        Node to_delete = iterator;
        iterator = iterator->next;

        delete_node(to_delete, deleter);
    }

    list->last = list->first = NULL;
    list->size = 0;
}
