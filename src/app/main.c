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
    LinkedList list = LL_new_list();
    Data data = new_data("Ronald", 1);
    Data data2 = new_data("Ronald", 1);

    LL_add_last(list, data);
    LL_add_last(list, new_data("Jose", 2));
    LL_add_last(list, new_data("jesus", 3));
    LL_add_last(list, new_data("Alan", 4));
    LL_add_last(list, new_data("cinco", 5));

    puts("normal list:");
    print_students(list);

    puts("\nreverse list:");
    LL_reverse(list);
    print_students(list);

    puts("\nrandomize list:");
    LL_randomize(list);
    print_students(list);

    puts("\ncopy list:");
    LinkedList list_copy = LL_copy(list, DATA_SIZE);
    print_students(list_copy);

    printf("%i\n", LL_contains(list, data2));

    LL_hard_delete_list(list, delete_data2);
    LL_hard_delete_list(list_copy, delete_data2);

    return EXIT_SUCCESS;
}
