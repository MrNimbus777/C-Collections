#ifndef LLIST_H
#define LLIST_H

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

// This macro is used to DECLARE the "is_equal" function related to TYPE. You must lately provide an DEFINITION (block of code) for this function, in order to use the linked list!
#define DECLARE_IS_EQUAL_FOR_TYPE(TYPE) bool is_equal_##TYPE (TYPE t1, TYPE t2);

// TYPE: for pointers provide a wrapping type (e.g. char* -> typedef char* my_string)
#define LLIST_IMPLEMENT_EXPLICIT(TYPE, LIST_STRUCTURE_NAME)                                                           \
                                                                                                                      \
typedef struct {                                                                                                      \
    struct LIST_STRUCTURE_NAME##_node* next;                                                                          \
    TYPE value;                                                                                                       \
} LIST_STRUCTURE_NAME##_node;                                                                                         \
                                                                                                                      \
typedef struct {                                                                                                      \
    LIST_STRUCTURE_NAME##_node* head;                                                                                 \
    LIST_STRUCTURE_NAME##_node* tail;                                                                                 \
    size_t size;                                                                                                      \
} LIST_STRUCTURE_NAME;                                                                                                \
                                                                                                                      \
static void LIST_STRUCTURE_NAME##_init(LIST_STRUCTURE_NAME* list){                                                    \
    assert(list != NULL && "A valid list expected");                                                                  \
    list->head = NULL;                                                                                                \
    list->tail = NULL;                                                                                                \
    list->size = 0;                                                                                                   \
}                                                                                                                     \
                                                                                                                      \
static void LIST_STRUCTURE_NAME##_add(LIST_STRUCTURE_NAME* list, TYPE element){                                       \
    assert(list != NULL && "A valid list expected");                                                                  \
    if(list->size == 0) {                                                                                             \
        list->head = (LIST_STRUCTURE_NAME##_node*) malloc(sizeof(LIST_STRUCTURE_NAME##_node));                        \
        if(!list->head) return;                                                                                       \
        list->tail = list->head;                                                                                      \
        list->head->next = NULL;                                                                                      \
        list->head->value = element;                                                                                  \
    } else {                                                                                                          \
        list->tail->next = (LIST_STRUCTURE_NAME##_node*) malloc(sizeof(LIST_STRUCTURE_NAME##_node));                  \
        if(!list->tail->next) return;                                                                                 \
        list->tail->next->next = NULL;                                                                                \
        list->tail->next->value = element;                                                                            \
        list->tail = list->tail->next;                                                                                \
    }                                                                                                                 \
    list->size++;                                                                                                     \
}                                                                                                                     \
                                                                                                                      \
static void LIST_STRUCTURE_NAME##_insert(LIST_STRUCTURE_NAME* list, TYPE element, size_t index){                      \
    assert(list != NULL && "A valid list expected");                                                                  \
    assert(list->size >= index && "index out of range");                                                              \
    if(index == list->size) {                                                                                         \
        LIST_STRUCTURE_NAME##_add(list, element);                                                                     \
        return;                                                                                                       \
    }                                                                                                                 \
    LIST_STRUCTURE_NAME##_node* new_node = (LIST_STRUCTURE_NAME##_node*) malloc(sizeof(LIST_STRUCTURE_NAME##_node));  \
    if(!new_node) return;                                                                                             \
    new_node->value = element;                                                                                        \
    list->size++;                                                                                                     \
    if(index == 0){                                                                                                   \
        new_node->next = list->head;                                                                                  \
        list->head = new_node;                                                                                        \
        return;                                                                                                       \
    }                                                                                                                 \
    LIST_STRUCTURE_NAME##_node* p = list->head;                                                                       \
    while(--index) p = p->next;                                                                                       \
    new_node->next = p->next;                                                                                         \
    p->next = new_node;                                                                                               \
}                                                                                                                     \
                                                                                                                      \
static void LIST_STRUCTURE_NAME##_remove(LIST_STRUCTURE_NAME* list, TYPE element){                                    \
    assert(list != NULL && "A valid list expected");                                                                  \
    if(list->size == 0) return;                                                                                       \
    LIST_STRUCTURE_NAME##_node* p = list->head;                                                                       \
    while(p->next){                                                                                                   \
        if(is_equal_##TYPE(p->next->value, element)) break;                                                           \
        p = p->next;                                                                                                  \
    }                                                                                                                 \
    if(!p->next) return;                                                                                              \
    LIST_STRUCTURE_NAME##_node* tmp = p->next;                                                                        \
    p->next = tmp->next;                                                                                              \
    free(tmp);                                                                                                        \
    list->size--;                                                                                                     \
    if(!p->next) list->tail = p;                                                                                      \
}                                                                                                                     \
                                                                                                                      \
static void LIST_STRUCTURE_NAME##_remove_start(LIST_STRUCTURE_NAME* list){                                            \
    assert(list != NULL && "A valid list expected");                                                                  \
    if(list->size == 0) return;                                                                                       \
    LIST_STRUCTURE_NAME##_node* tmp = list->head;                                                                     \
    list->head = list->head->next;                                                                                    \
    free(tmp);                                                                                                        \
    list->size--;                                                                                                     \
    if(!list->head) list->tail = NULL;                                                                                \
}                                                                                                                     \
                                                                                                                      \
static void LIST_STRUCTURE_NAME##_remove_index(LIST_STRUCTURE_NAME* list, size_t index){                              \
    assert(list != NULL && "A valid list expected");                                                                  \
    assert(index < list->size && "index out of range");                                                               \
    if(index == 0){                                                                                                   \
        LIST_STRUCTURE_NAME##_remove_start(list);                                                                     \
        return;                                                                                                       \
    }                                                                                                                 \
    LIST_STRUCTURE_NAME##_node* p = list->head;                                                                       \
    while(--index) p = p->next;                                                                                       \
    LIST_STRUCTURE_NAME##_node* tmp = p->next;                                                                        \
    p->next = tmp->next;                                                                                              \
    free(tmp);                                                                                                        \
    if(!p->next) list->tail = p;                                                                                      \
    list->size--;                                                                                                     \
}                                                                                                                     \
                                                                                                                      \
static TYPE LIST_STRUCTURE_NAME##_pop(LIST_STRUCTURE_NAME* list){                                                     \
    assert(list != NULL && "A valid list expected");                                                                  \
    assert(list->size > 0 && "Cannot pop from empty list");                                                           \
    TYPE value = list->tail->value;                                                                                   \
    if(list->size == 1){                                                                                              \
        free(list->head);                                                                                             \
        list->head = list->tail = NULL;                                                                               \
        list->size = 0;                                                                                               \
        return value;                                                                                                 \
    }                                                                                                                 \
    LIST_STRUCTURE_NAME##_node* p = list->head;                                                                       \
    while(p->next != list->tail) p = p->next;                                                                         \
    free(list->tail);                                                                                                 \
    list->tail = p;                                                                                                   \
    list->tail->next = NULL;                                                                                          \
    list->size--;                                                                                                     \
    return value;                                                                                                     \
}                                                                                                                     \
                                                                                                                      \
static TYPE LIST_STRUCTURE_NAME##_pop_start(LIST_STRUCTURE_NAME* list){                                               \
    assert(list != NULL && "A valid list expected");                                                                  \
    assert(list->size > 0 && "Cannot pop from empty list");                                                           \
    TYPE value = list->head->value;                                                                                   \
    LIST_STRUCTURE_NAME##_node* tmp = list->head;                                                                     \
    list->head = list->head->next;                                                                                    \
    free(tmp);                                                                                                        \
    list->size--;                                                                                                     \
    if(!list->head) list->tail = NULL;                                                                                \
    return value;                                                                                                     \
}                                                                                                                     \
                                                                                                                      \
static TYPE LIST_STRUCTURE_NAME##_pop_index(LIST_STRUCTURE_NAME* list, size_t index){                                 \
    assert(list != NULL && "A valid list expected");                                                                  \
    assert(index < list->size && "index out of range");                                                               \
    if(index == 0) return LIST_STRUCTURE_NAME##_pop_start(list);                                                      \
    if(index == list->size - 1) return LIST_STRUCTURE_NAME##_pop(list);                                               \
    LIST_STRUCTURE_NAME##_node* p = list->head;                                                                       \
    while(--index) p = p->next;                                                                                       \
    LIST_STRUCTURE_NAME##_node* tmp = p->next;                                                                        \
    TYPE value = tmp->value;                                                                                          \
    p->next = tmp->next;                                                                                              \
    free(tmp);                                                                                                        \
    list->size--;                                                                                                     \
    return value;                                                                                                     \
}                                                                                                                     \
                                                                                                                      \
static TYPE LIST_STRUCTURE_NAME##_at(LIST_STRUCTURE_NAME* list, size_t index){                                        \
    assert(list != NULL && "A valid list expected");                                                                  \
    assert(index < list->size && "index out of range");                                                               \
    LIST_STRUCTURE_NAME##_node* p = list->head;                                                                       \
    for(size_t i = 0; i < index; i++) p = p->next;                                                                    \
    return p->value;                                                                                                  \
}                                                                                                                     \
                                                                                                                      \
static LIST_STRUCTURE_NAME##_node* LIST_STRUCTURE_NAME##_find(LIST_STRUCTURE_NAME* list, TYPE value){                 \
    assert(list != NULL && "A valid list expected");                                                                  \
    if(list->size == 0) return NULL;                                                                                  \
    LIST_STRUCTURE_NAME##_node* p = list->head;                                                                       \
    while(p->next){                                                                                                   \
        if(is_equal_##TYPE(p->next->value, element)) break;                                                           \
        p = p->next;                                                                                                  \
    }                                                                                                                 \
    if(!p->next) return NULL;                                                                                         \
    return p->next;                                                                                                   \
}                                                                                                                     \
                                                                                                                      \
static void LIST_STRUCTURE_NAME##_clear(LIST_STRUCTURE_NAME* list){                                                   \
    assert(list != NULL && "A valid list expected");                                                                  \
    LIST_STRUCTURE_NAME##_node* p = list->head;                                                                       \
    while(p){                                                                                                         \
        LIST_STRUCTURE_NAME##_node* tmp = p;                                                                          \
        p = p->next;                                                                                                  \
        free(tmp);                                                                                                    \
    }                                                                                                                 \
    list->head = list->tail = NULL;                                                                                   \
    list->size = 0;                                                                                                   \
}                                                                                                                     \
                                                                                                                      \
static void LIST_STRUCTURE_NAME##_clone(LIST_STRUCTURE_NAME* list1, LIST_STRUCTURE_NAME* list2){                      \
    assert(list1 != NULL && "A valid list expected");                                                                 \
    LIST_STRUCTURE_NAME##_init(list2);                                                                                \
    if(list1->size == 0) return;                                                                                      \
    LIST_STRUCTURE_NAME##_node* p = list1->head;                                                                      \
    while(p){                                                                                                         \
        LIST_STRUCTURE_NAME##_add(list2, p->value);                                                                   \
        p = p->next;                                                                                                  \
    }                                                                                                                 \
}

// TYPE: for pointers provide a wrapping type (e.g. char* -> typedef char* my_string)
#define LLIST_IMPLEMENT(TYPE) LLIST_IMPLEMENT_EXPLICIT(TYPE, llist_##TYPE)

#endif // !LLIST_H