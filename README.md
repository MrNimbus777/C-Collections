# C-Collections

A small collection of generic data structures implemented in C using macros
to generate type-specific structures and functions.

---

# [darr.h](https://github.com/MrNimbus777/C-Collections/blob/main/darr.h) (Dynamic Array)

Implementation of a generic dynamic array in C. Originally inspired by a
similar structure seen in Tsoding's video:

https://www.youtube.com/watch?v=95M6V3mZgrI

This version is based on a macro that generates the struct and related
functions for the provided type.

## How to Use

Suppose you need a dynamic array of integers. (Why would you use lib if you didn't? ._.)
Call the implementation macro like this: 
`DARR_IMPLEMENT(int)` _(or `DARR_IMPLEMENT_EXPLICIT(int, ints)` for custom naming.)_
This will generate the int_darr structure and all related functions for that type
If you want to get into details analyze carefully the [sourcode](https://github.com/MrNimbus777/C-Collections/blob/main/darr.h).

## Example

```c
#include <stdio.h>
#include "darr.h"

DARR_IMPLEMENT(int)

int main() {
    int_darr ints;

    int_darr_init(&ints);
    int_darr_push(&ints, 69);

    printf("%d\n", int_darr_pop(&ints));

    int_darr_free(&ints);   // very important to free the array to prevent memory leaks
    return 0;
}
```
>> ## IMPORTANT NOTE: 
>>   performing push might do a resize action which might invalidate the previous 
>>   position of 'elements' pointer. This means you should never rely on a previously saved
>>   pointer to a certain object, only on its index relatively to 'elements' pointer

# [llist.h](https://github.com/MrNimbus777/C-Collections/blob/main/llist.h) (Single Linked List)

Implementation of a generic single linked list in C using a macro for creating the set of function tied to a TYPE.

## How to use
If you need a linked list of integers, call the implementation macro:
`LLIST_IMPLEMENT(int)` _(or `LLIST_IMPLEMENT_EXPLICIT(int, ints)` for custom naming.)_
This will generate the struct int_llist and all the related functions that you can analize yourself in the below.

## Example
```
#include <stdio.h>
#include "llist.h"

LLIST_IMPLEMENT(int)

int main() {
    int_llist list;

    int_llist_init(&list);
    int_llist_add(&list, 69);

    printf("%d\n", int_llist_pop(&list));

    int_llist_clear(&list);   // important to clear the list to prevent memory leaks
    return 0;
}
```
