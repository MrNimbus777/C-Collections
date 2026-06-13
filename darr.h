// ################################################################################################
// Implementation of a generic dynamic array in C. Originally was inspired by a similar structure 
// seen in Tsoding's video (https://www.youtube.com/watch?v=95M6V3mZgrI).
// Though this version is based on a macro that generates structs and related functions based on
// the provided type.
// 
// ??? HOW TO USE ???  - Very simply actually
// Let's say you need an dynamic array of Integers, then you call the implementation macro like
// this: 
// DARR_IMPLEMENT(int) - this will generate the struct int_darr and all the related functions that 
//                       you can analize yourself in the below. 
// 
//
// Use example:
//
// .... (other code)
// DARR_IMPLEMENT(int)
// int main() {
//     int_darr ints;
//     int_darr_init(&ints);
//     darr_error_t err;
//     int_darr_push(&ints, 69, &err);
//     if(err.code != DARR_OK){
//         printf("%s\n", err.message);
//         return 1;
//     }
//     printf("%d", int_darr_pop(&ints));
//     int_darr_free(&ints); // Don't forget to free it when it is not needed anymore.
//     return 0;
// }
//
//
// !!! IMPORTANT NOTE: performing push might do a resize action which might invalidate the previous 
//     position of 'elements' pointer. This means you should never rely on a previously saved
//     pointer to a certain object, only on its index relatively to 'elements' pointer
//
//
// ################################################################################################

// ################################################################################################
// 11.06.2026 "SUGAR API" UPDATE
// Now you can use runtime dispatched functions for all array operations.
// You still need to explicitly initiate a map object before using those:
//     int_darr ints;
//     int_darr_init(&ints);
// then you can use:
//     darr_push(&ints, 69, &err);
// or if you strip the prefixes:
//     #define DARR_STRIP_PREFIXES
//     #include "darr.h"
// then you can simply write:
//     push(&ints, 69, &err);
//
// But surely I still recommend to use the explicit function calls, as it is less expensive than a runtime dispatch!
// ################################################################################################

#ifndef DARR_H
#define DARR_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>


// THE SUGARY API:


#define darr_reserve(arr_ptr, reserved_capacity, out_error_ptr) ((arr_ptr)-> op->_reserve(arr_ptr, reserved_capacity, out_error_ptr))
#define darr_free(arr_ptr)                                      ((arr_ptr)-> op->_free(arr_ptr))
#define darr_push(arr_ptr, value, out_error_ptr)                ((arr_ptr)-> op->_push(arr_ptr, value, out_error_ptr))
#define darr_insert(arr_ptr, value, index, out_error_ptr)       ((arr_ptr)-> op->_insert(arr_ptr, value, index, out_error_ptr))
#define darr_pop(arr_ptr)                                       ((arr_ptr)-> op->_pop(arr_ptr))
#define darr_remove(arr_ptr, index)                             ((arr_ptr)-> op->_remove(arr_ptr, index))
#define darr_try_shrink(arr_ptr, out_error_ptr)                 ((arr_ptr)-> op->_try_shrink(arr_ptr, out_error_ptr))
#define darr_resize(arr_ptr, new_capacity, out_error_ptr)       ((arr_ptr)-> op->_resize(arr_ptr, new_capacity, out_error_ptr))
#define darr_clear(arr_ptr)                                     ((arr_ptr)-> op->_clear(arr_ptr))
#define darr_clone(arr1_ptr, arr2_ptr, out_error_ptr)           ((arr1_ptr)->op->_clone(arr1_ptr, arr2_ptr, out_error_ptr))

#ifdef DARR_STRIP_PREFIXES

#define reserve(arr_ptr, reserved_capacity, out_error_ptr) darr_reserve(arr_ptr, reserved_capacity, out_error_ptr)
#define push(arr_ptr, value, out_error_ptr)                darr_push(arr_ptr, value, out_error_ptr)
#define insert(arr_ptr, value, index, out_error_ptr)       darr_insert(arr_ptr, value, index, out_error_ptr)
#define pop(arr_ptr)                                       darr_pop(arr_ptr)
#define remove(arr_ptr, index)                             darr_remove(arr_ptr, index)
#define try_shrink(arr_ptr, out_error_ptr)                 darr_try_shrink(arr_ptr, out_error_ptr)
#define resize(arr_ptr, new_capacity, out_error_ptr)       darr_resize(arr_ptr, new_capacity, out_error_ptr)
#define clear(arr_ptr)                                     darr_clear(arr_ptr)
#define clone(arr1_ptr, arr2_ptr, out_error_ptr)           darr_clone(arr1_ptr, arr2_ptr, out_error_ptr)

#endif

// THE IMPLEMENTATION:
enum DARR_ERROR_CODES {
    DARR_OK,
    DARR_ALLOC,
    DARR_OUT_OF_BOUNDS
};
typedef struct darr_error_t {
    const char* message;
    enum DARR_ERROR_CODES code;
} darr_error_t;

#define STR_(x) #x
#define STR(x) STR_(x)

#define darr_err(code_, string_literal, func) ((darr_error_t){.message = (__FILE__"(line: "STR(__LINE__)") in "STR(func)"(...): "string_literal), .code = (code_)})
inline darr_error_t darr_ok() { return (darr_error_t){.message = 0, .code = DARR_OK}; }

#define INITIAL_CAPACITY 4


// TYPE: for pointers provide a wrapping type (e.g. char* -> typedef char* my_string)
#define DARR_IMPLEMENT_EXPLICIT(TYPE, ARR_STRUCT_NAME)                                                                    \
                                                                                                                          \
typedef struct ARR_STRUCT_NAME##_operators ARR_STRUCT_NAME##_operators;                                                   \
                                                                                                                          \
typedef struct {                                                                                                          \
    TYPE* elements;                                                                                                       \
    size_t size;                                                                                                          \
    size_t capacity;                                                                                                      \
    ARR_STRUCT_NAME##_operators* op;                                                                                      \
} ARR_STRUCT_NAME;                                                                                                        \
                                                                                                                          \
struct ARR_STRUCT_NAME##_operators {                                                                                      \
    void(*const _reserve)(ARR_STRUCT_NAME*, size_t, darr_error_t*);                                                       \
    void(*const _free)(ARR_STRUCT_NAME*);                                                                                 \
    void(*const _push)(ARR_STRUCT_NAME*, TYPE, darr_error_t*);                                                            \
    void(*const _insert)(ARR_STRUCT_NAME*, TYPE, size_t, darr_error_t*);                                                  \
    TYPE(*const _pop)(ARR_STRUCT_NAME*);                                                                                  \
    void(*const _remove)(ARR_STRUCT_NAME*, size_t);                                                                       \
    bool(*const _try_shrink)(ARR_STRUCT_NAME*, darr_error_t*);                                                            \
    bool(*const _resize)(ARR_STRUCT_NAME*, size_t, darr_error_t*);                                                        \
    void(*const _clear)(ARR_STRUCT_NAME*);                                                                                \
    void(*const _clone)(ARR_STRUCT_NAME*, ARR_STRUCT_NAME*, darr_error_t*);                                               \
};                                                                                                                        \
                                                                                                                          \
static void ARR_STRUCT_NAME##_init(ARR_STRUCT_NAME* arr);                                                                 \
static void ARR_STRUCT_NAME##_init_with_capacity(ARR_STRUCT_NAME* arr, size_t initial_capacity, darr_error_t* out_error); \
                                                                                                                          \
static void ARR_STRUCT_NAME##_reserve(ARR_STRUCT_NAME* arr, size_t reserved_capacity, darr_error_t* out_error) {          \
    assert(arr != NULL && "A valid array is expected");                                                                   \
    if(reserved_capacity >= (arr->capacity-arr->size)) {                                                                  \
        *out_error = darr_ok();                                                                                           \
        return;                                                                                                           \
    }                                                                                                                     \
    TYPE* tmp = (TYPE*)realloc(arr->elements, reserved_capacity * sizeof(TYPE));                                          \
    if(!tmp) {                                                                                                            \
        *out_error = darr_err(DARR_ALLOC, "Failed to realloc(arr->elements)", ARR_STRUCT_NAME##_reserve);                 \
        return;                                                                                                           \
    }                                                                                                                     \
    arr->elements = tmp;                                                                                                  \
    arr->capacity = reserved_capacity;                                                                                    \
    *out_error = darr_ok();                                                                                               \
}                                                                                                                         \
                                                                                                                          \
static void ARR_STRUCT_NAME##_free(ARR_STRUCT_NAME* arr) {                                                                \
    assert(arr != NULL && "A valid array is expected");                                                                   \
    if(arr->elements) free(arr->elements);                                                                                \
    arr->elements = NULL;                                                                                                 \
    arr->size = 0;                                                                                                        \
    arr->capacity = 0;                                                                                                    \
}                                                                                                                         \
                                                                                                                          \
static void ARR_STRUCT_NAME##_push(ARR_STRUCT_NAME* arr, TYPE value, darr_error_t* out_error) {                           \
    assert(arr != NULL && "A valid array is expected");                                                                   \
    if (arr->size >= arr->capacity) {                                                                                     \
        size_t new_capacity = arr->capacity == 0 ? INITIAL_CAPACITY : arr->capacity * 2;                                  \
        TYPE* tmp = (TYPE*)realloc(arr->elements, new_capacity * sizeof(TYPE));                                           \
        if(!tmp) {                                                                                                        \
            *out_error = darr_err(DARR_ALLOC, "Failed to realloc(arr->elements)", ARR_STRUCT_NAME##_push);                \
            return;                                                                                                       \
        }                                                                                                                 \
        arr->elements = tmp;                                                                                              \
        arr->capacity = new_capacity;                                                                                     \
    }                                                                                                                     \
    arr->elements[arr->size++] = value;                                                                                   \
    *out_error = darr_ok();                                                                                               \
}                                                                                                                         \
                                                                                                                          \
static void ARR_STRUCT_NAME##_insert(ARR_STRUCT_NAME* arr, TYPE value, size_t index, darr_error_t* out_error) {           \
    assert(arr != NULL && "A valid array is expected");                                                                   \
    assert(index <= arr->size && "index out of range");                                                                   \
    if (arr->size >= arr->capacity) {                                                                                     \
        size_t new_capacity = arr->capacity == 0 ? INITIAL_CAPACITY : arr->capacity * 2;                                  \
        TYPE* tmp = (TYPE*)realloc(arr->elements, new_capacity * sizeof(TYPE));                                           \
        if(!tmp) {                                                                                                        \
            *out_error = darr_err(DARR_ALLOC, "Failed to realloc(arr->elements)", ARR_STRUCT_NAME##_insert);              \
            return;                                                                                                       \
        }                                                                                                                 \
        arr->elements = tmp;                                                                                              \
        arr->capacity = new_capacity;                                                                                     \
    }                                                                                                                     \
    for(size_t i = arr->size++; i > index; i--){                                                                          \
        arr->elements[i] = arr->elements[i-1];                                                                            \
    }                                                                                                                     \
    arr->elements[index] = value;                                                                                         \
    *out_error = darr_ok();                                                                                               \
}                                                                                                                         \
                                                                                                                          \
static TYPE ARR_STRUCT_NAME##_pop(ARR_STRUCT_NAME* arr) {                                                                 \
    assert(arr != NULL && "A valid array is expected");                                                                   \
    assert(arr->size > 0 && "Nothing to pop!");                                                                           \
    return arr->elements[--arr->size];                                                                                    \
}                                                                                                                         \
                                                                                                                          \
static void ARR_STRUCT_NAME##_remove(ARR_STRUCT_NAME* arr, size_t index){                                                 \
    assert(arr != NULL && "A valid array is expected");                                                                   \
    assert((0 < index) && (index < arr->size) && "Nothing to pop!");                                                      \
    arr->size--;                                                                                                          \
    for(size_t i = index; i < arr->size; i++) {                                                                           \
        arr->elements[i] = arr->elements[i+1];                                                                            \
    }                                                                                                                     \
}                                                                                                                         \
                                                                                                                          \
static bool ARR_STRUCT_NAME##_try_shrink(ARR_STRUCT_NAME* arr, darr_error_t* out_error){                                  \
    assert(arr != NULL && "A valid array is expected");                                                                   \
    size_t new_capacity = arr->capacity/4;                                                                                \
    if(new_capacity == 0) return false;                                                                                   \
    if(arr->size <= new_capacity) {                                                                                       \
        TYPE* tmp = (TYPE*)realloc(arr->elements, new_capacity * sizeof(TYPE));                                           \
        if(!tmp) {                                                                                                        \
            *out_error = darr_err(DARR_ALLOC, "Failed to realloc(arr->elements)", ARR_STRUCT_NAME##_try_shrink);          \
            return false;                                                                                                 \
        }                                                                                                                 \
        arr->elements = tmp;                                                                                              \
        arr->capacity = new_capacity;                                                                                     \
        *out_error = darr_ok();                                                                                           \
        return true;                                                                                                      \
    }                                                                                                                     \
    *out_error = darr_ok();                                                                                               \
    return false;                                                                                                         \
}                                                                                                                         \
                                                                                                                          \
static bool ARR_STRUCT_NAME##_resize(ARR_STRUCT_NAME* arr, size_t new_capacity, darr_error_t* out_error){                 \
    assert(arr != NULL && "A valid array is expected");                                                                   \
    assert(new_capacity > arr->size && "new_capacity cannot be smaller than actual size");                                \
    TYPE* tmp = (TYPE*)realloc(arr->elements, new_capacity * sizeof(TYPE));                                               \
    if(!tmp) {                                                                                                            \
        *out_error = darr_err(DARR_ALLOC, "Failed to realloc(arr->elements)", ARR_STRUCT_NAME##_resize);                  \
        return false;                                                                                                     \
    }                                                                                                                     \
    arr->elements = tmp;                                                                                                  \
    arr->capacity = new_capacity;                                                                                         \
    *out_error = darr_ok();                                                                                               \
    return true;                                                                                                          \
}                                                                                                                         \
                                                                                                                          \
static void ARR_STRUCT_NAME##_clear(ARR_STRUCT_NAME* arr){                                                                \
    assert(arr != NULL && "A valid array is expected");                                                                   \
    arr->size = 0;                                                                                                        \
}                                                                                                                         \
                                                                                                                          \
static void ARR_STRUCT_NAME##_clone(ARR_STRUCT_NAME* arr1, ARR_STRUCT_NAME* arr2, darr_error_t* out_error){               \
    assert(arr1 != NULL && "A valid array is expected");                                                                  \
    assert(arr2 != NULL && "A valid array is expected");                                                                  \
    ARR_STRUCT_NAME##_init_with_capacity(arr2, arr1->capacity, out_error);                                                \
    if(out_error->code != DARR_OK) return;                                                                                \
    arr2->size = arr1->size;                                                                                              \
    mempcpy(arr2->elements, arr1->elements, arr1->size * sizeof(TYPE));                                                   \
    return;                                                                                                               \
}                                                                                                                         \
                                                                                                                          \
static ARR_STRUCT_NAME##_operators ARR_STRUCT_NAME##_op = (ARR_STRUCT_NAME##_operators) {                                 \
    ._reserve     = ARR_STRUCT_NAME##_reserve,                                                                            \
    ._free        = ARR_STRUCT_NAME##_free,                                                                               \
    ._push        = ARR_STRUCT_NAME##_push,                                                                               \
    ._insert      = ARR_STRUCT_NAME##_insert,                                                                             \
    ._pop         = ARR_STRUCT_NAME##_pop,                                                                                \
    ._remove      = ARR_STRUCT_NAME##_remove,                                                                             \
    ._try_shrink  = ARR_STRUCT_NAME##_try_shrink,                                                                         \
    ._resize      = ARR_STRUCT_NAME##_resize,                                                                             \
    ._clear       = ARR_STRUCT_NAME##_clear,                                                                              \
    ._clone       = ARR_STRUCT_NAME##_clone                                                                               \
};                                                                                                                        \
                                                                                                                          \
static void ARR_STRUCT_NAME##_init(ARR_STRUCT_NAME* arr) {                                                                \
    assert(arr != NULL && "A valid array is expected");                                                                   \
    arr->size = 0;                                                                                                        \
    arr->capacity = 0;                                                                                                    \
    arr->op = &ARR_STRUCT_NAME##_op;                                                                                      \
    arr->elements = NULL;                                                                                                 \
}                                                                                                                         \
static void ARR_STRUCT_NAME##_init_with_capacity(ARR_STRUCT_NAME* arr, size_t initial_capacity, darr_error_t* out_error){ \
    assert(arr != NULL && "A valid array is expected");                                                                   \
    arr->size = 0;                                                                                                        \
    arr->capacity = initial_capacity;                                                                                     \
    arr->op = &ARR_STRUCT_NAME##_op;                                                                                      \
    arr->elements = (TYPE*)malloc(initial_capacity * sizeof(TYPE));                                                       \
    if(!arr->elements) {                                                                                                  \
        *out_error = darr_err(DARR_ALLOC, "Failed to allocate arr->elements.", ARR_STRUCT_NAME##_init_with_capacity);     \
        return;                                                                                                           \
    }                                                                                                                     \
    *out_error = darr_ok();                                                                                               \
}                                                                                                                         \


#define DARR_IMPLEMENT(TYPE) DARR_IMPLEMENT_EXPLICIT(TYPE, TYPE##_darr)


#endif
