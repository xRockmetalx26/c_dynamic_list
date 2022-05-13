#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linked_list/dll.h>

typedef struct Student {
    char name[128];
    size_t id;
} Student;

Student* new_student(const char *name, const size_t id) {
    Student *student = (Student*) calloc(1, sizeof(Student));

    strcpy(student->name, name);

    student->id = id;

    return student;
}

void print_students(LinkedList list) {
    for(Node it = list->first; it; it = it->next) {
        Student *student = (Student*) it->data;

        printf("Name: %s, Age: %llu\n", student->name, student->id);
    }
}

int main() {
    LinkedList list = new_list();

    add_first_node(list, new_student("Ronald", 1));
    add_last_node(list, new_student("Jose", 2));
    insert_a_node(list, new_student("jesus", 3), 1);
    add_last_node(list, new_student("Alan", 4));

    print_students(list);

    delete_dll(list);

    return EXIT_SUCCESS;
}