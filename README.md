# C-Collections


## darr.h (Dynamic Array)

Implementation of a generic dynamic array in C. Originally was inspired by a similar structure 
seen in Tsoding's video (https:www.youtube.com/watch?v=95M6V3mZgrI).
Though this version is based on a macro that generates structs and related functions based on
the provided type.

??? HOW TO USE ???  - Very simply actually
Let's say you need an dynamic array of Integers, then you call the implementation macro like
this: 
DARR_IMPLEMENT(int) - this will generate the struct int_darr and all the related functions that 
                     you can analize yourself in the below. 

Use example:
```
// .... (other code)
DARR_IMPLEMENT(int)
int main() {
   int_darr ints;
   int_darr_init(&ints);
   int_darr_push(&ints, 69);
   printf("%d", int_darr_pop(&ints));
   int_darr_free(&ints);  very important to free the array to prevent memory leaks!
   return 0;
}
```
!!! IMPORTANT NOTE: performing push might do a resize action which might invalidate the previous 
   position of 'elements' pointer. This means you should never rely on a previously saved
   pointer to a certain object, only on its index relatively to 'elements' pointer

## llist.h (Single Linked List)

Implementation of a generic single linked list in C using a macro for creating the set of function tied to a TYPE.

??? HOW TO USE ???  - Very simply actually
Let's say you need an linked list of Integers, then you call the implementation macro like
this: 
LLIST_IMPLEMENT(int) - this will generate the struct int_llist and all the related functions that 
                     you can analize yourself in the below.

Use example:
```
// .... (other code)
LLIST_IMPLEMENT(int)
int main() {
   int_llist list;
   int_llist_init(&list);
   int_llist_add(&ints, 69);
   printf("%d", int_llist_pop(&ints));
   int_llist_clear(&ints);   very important to clear the elements to prevent memory leaks!
   return 0;
}
```
