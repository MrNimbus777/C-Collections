// ################################################################################################
// Implementation of a generic dynamic array in C. Originally was inspired by a similar structure seen in Tsoding's video (https://www.youtube.com/watch?v=95M6V3mZgrI).
// Though this version is based on a macro that generates structs and related functions based on the provided type.
//
//     HOW TO USE ??? - Very simply actually
// Let's say you need an dynamic array of Integers, then you call the implementation macro like this:
// DARR_IMPLEMENT(int) - this will generate the struct int_darr and all the related functions that you can
// analize yourself in the below. 
// Use example:
//
// .... (other code)
// DARR_IMPLEMENT(int)
// int main() {
//     int_darr ints;
//     int_darr_init(&ints);
//     int_darr_push(&ints, 69);
//     printf("%d", int_darr_pop(&ints));
//     int_darr_free(&ints);
//     return 0;
// }
//
//
// !!! IMPORTANT NOTE: performing push might invalidate the previous position of 'elements' pointer. 
//     This means you should never rely on a previously saved pointer to a certain object, 
//     only on its index relatively to 'elements' pointer
//
//
// ################################################################################################
#ifndef DARR_H
#define DARR_H

#include <stdlib.h>

// TYPE: for pointers provide a wrapping type (e.g. char* -> typedef char* my_string)
#define DARR_IMPLEMENT_EXPLICIT(TYPE, ARR_STRUCT_NAME)                                             \
                                                                                                   \
typedef struct {                                                                                   \
    TYPE* elements;                                                                                \
    size_t size;                                                                                   \
    size_t capacity;                                                                               \
} ARR_STRUCT_NAME;                                                                                 \
                                                                                                   \
static void ARR_STRUCT_NAME##_init(ARR_STRUCT_NAME* arr) {                                         \
    assert(arr != NULL && "A valid array is expected");                                            \
    arr->elements = NULL;                                                                          \
    arr->size = 0;                                                                                 \
    arr->capacity = 0;                                                                             \
}                                                                                                  \
static void ARR_STRUCT_NAME##_init_with_capacity(ARR_STRUCT_NAME* arr, size_t initial_capacity) {  \
    assert(arr != NULL && "A valid array is expected")                                             \
    arr->elements = (TYPE*)malloc(initial_capacity * sizeof(TYPE));                                \
    arr->size = 0;                                                                                 \
    arr->capacity = initial_capacity;                                                              \
}                                                                                                  \
static void ARR_STRUCT_NAME##_reserve(ARR_STRUCT_NAME* arr, size_t reserved_capacity) {            \
    assert(arr != NULL && "A valid array is expected")                                             \
    assert(reserved_capacity > arr->capacity && "reserve must not perform a shrink action");       \
    if(reserved_capacity == arr->capacity) return;                                                 \
    TYPE* tmp = (TYPE*)realloc(arr->elements, reserved_capacity * sizeof(TYPE));                   \
    if(!tmp) return;                                                                               \
    arr->elements = tmp;                                                                           \
    arr->capacity = reserved_capacity;                                                             \
}                                                                                                  \
                                                                                                   \
static void ARR_STRUCT_NAME##_free(ARR_STRUCT_NAME* arr) {                                         \
    assert(arr != NULL && "A valid array is expected");                                            \
    if(arr->elements) free(arr->elements);                                                         \
    arr->elements = NULL;                                                                          \
    arr->size = 0;                                                                                 \
    arr->capacity = 0;                                                                             \
}                                                                                                  \
                                                                                                   \
static void ARR_STRUCT_NAME##_push(ARR_STRUCT_NAME* arr, TYPE value) {                             \
    assert(arr != NULL && "A valid array is expected");                                            \
    if (arr->size >= arr->capacity) {                                                              \
        size_t new_capacity = arr->capacity == 0 ? 4 : arr->capacity * 2;                          \
        TYPE* tmp = (TYPE*)realloc(arr->elements, new_capacity * sizeof(TYPE));                    \
        if (!tmp) return;                                                                          \
        arr->elements = tmp;                                                                       \
        arr->capacity = new_capacity;                                                              \
    }                                                                                              \
    arr->elements[arr->size++] = value;                                                            \
}                                                                                                  \
                                                                                                   \
static void ARR_STRUCT_NAME##_insert(ARR_STRUCT_NAME* arr, TYPE value, size_t index) {             \
    assert(arr != NULL && "A valid array is expected");                                            \
    assert(index >= 0 && index <= arr->size && "index out of range");                              \
    if (arr->size >= arr->capacity) {                                                              \
        size_t new_capacity = arr->capacity == 0 ? 4 : arr->capacity * 2;                          \
        TYPE* tmp = (TYPE*)realloc(arr->elements, new_capacity * sizeof(TYPE));                    \
        if (!tmp) return;                                                                          \
        arr->elements = tmp;                                                                       \
        arr->capacity = new_capacity;                                                              \
    }                                                                                              \
    for(size_t i = arr->size++; i > index; i--){                                                   \
        arr->elements[i] = arr->elements[i-1];                                                     \
    }                                                                                              \
    arr->elements[index] = value;                                                                  \
}                                                                                                  \
                                                                                                   \
static TYPE ARR_STRUCT_NAME##_pop(ARR_STRUCT_NAME* arr) {                                          \
    assert(arr != NULL && "A valid array is expected");                                            \
    assert(arr->size >= 0 && "An array with positive size is expected");                           \
    return arr->elements[--arr->size];                                                             \
}                                                                                                  \
                                                                                                   \
static void ARR_STRUCT_NAME##_remove(ARR_STRUCT_NAME* arr, size_t index){                          \
    assert(arr != NULL && "A valid array is expected");                                            \
    assert(index >= 0 && index <= arr->size && "index out of range");                              \
    arr->size--;                                                                                   \
    for(size_t i = index; i < arr->size; i++) {                                                    \
        arr->elements[i] = arr->elements[i+1];                                                     \
    }                                                                                              \
}                                                                                                  \
                                                                                                   \
static void ARR_STRUCT_NAME##_shrink(ARR_STRUCT_NAME* arr){                                        \
    assert(arr != NULL && "A valid array is expected");                                            \
    size_t new_capacity = arr->capacity/4;                                                         \
    if(new_capacity = 0) return;                                                                   \
    if(arr->size < new_capacity) {                                                                 \
        TYPE* tmp = (TYPE*)realloc(arr->elements, new_capacity * sizeof(TYPE));                    \
        if (!tmp) return;                                                                          \
        arr->elements = tmp;                                                                       \
        arr->capacity = new_capacity;                                                              \
    }                                                                                              \
}                                                                                                  \
                                                                                                   \
static void ARR_STRUCT_NAME##_clear(ARR_STRUCT_NAME* arr){                                         \
    assert(arr != NULL && "A valid array is expected");                                            \
    arr->size = 0;                                                                                 \
}                                                                                                  \


#define DARR_IMPLEMENT(TYPE) DARR_IMPLEMENT_EXPLICIT(TYPE, TYPE##_darr)


#endif
