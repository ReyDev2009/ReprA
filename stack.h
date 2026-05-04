#ifndef STACK
#define STACK

#include<stdlib.h>

typedef struct stack_char{
    char* t;
    struct stack_char* sig;
} stack_char;

stack_char* create_stack ( ) {
    stack_char* stack = (stack_char*)malloc ( sizeof ( stack_char ));
    stack -> t = NULL;
    stack -> sig = NULL;

    return stack;
}

void add_element_to_stack ( stack_char** head, char* str ) {
    stack_char* nstack = (stack_char*) malloc ( sizeof ( stack_char ));
    nstack -> t = str;
    nstack -> sig = *head;
    *head = nstack;
    
}

void del_last_element_from_stack ( stack_char** head ) {
    stack_char* nstack = (*head) -> sig;
    free ( *head );
    (*head) = nstack;
}

#endif