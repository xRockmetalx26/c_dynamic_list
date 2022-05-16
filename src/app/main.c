#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <linked_list/linked_list.h>

#define STUDENT_SIZE sizeof (struct _Student)
#define DATA_SIZE sizeof (struct _Data)

typedef struct _Student {
    char name[128];
    size_t id;
} *Student;
//136
typedef struct _Data {
    Student student;
} *Data;
// 8
Student new_student(const char *name, const size_t id) {
    Student student = (Student) malloc(STUDENT_SIZE);
    student->id = id;

    strcpy(student->name, name);

    return student;
}

Data new_data(const char *name, const size_t id) {
    Data data = (Data) malloc(DATA_SIZE);
    data->student = new_student(name, id);

    return data;
}

void delete_student(void *student) {
    Student to_delete = (Student) student;

    if(to_delete) {
        free(to_delete);
    }
}

void delete_data2(void *data) {
    Data to_delete = (Data) data;

    if(to_delete) {
        delete_student(to_delete->student);
        free(to_delete);
    }
}

void print_students(LinkedList list) {
    for(Node it = list->first; it; it = it->next) {
        Data data = (Data) it->data;

        printf("Name: %s, Age: %llu\n", data->student->name, data->student->id);
    }
}

int main() {
    LinkedList list = new_list();
    Data data = new_data("Ronald", 1);

    add_first(list, data);

    printf("list: %llu, node: %llu, student: %llu, data: %llu", LINKED_LIST_SIZE, NODE_SIZE, STUDENT_SIZE, DATA_SIZE);
    add_last(list, new_data("Jose", 2));
    insert_at(list, new_data("jesus", 3), 1);
    add_last(list, new_data("Alan", 4));

    print_students(list);

    hard_delete_list(list, delete_data2);

    return EXIT_SUCCESS;
}
