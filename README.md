# C-Collections

A small collection of generic data structures implemented in C (С23) using macros
to generate type-specific structures and functions.

---

### [darr.h](https://github.com/MrNimbus777/C-Collections/blob/main/darr.h) (Dynamic Array)

Implementation of a generic dynamic array in C. Originally inspired by a
similar structure seen in Tsoding's video: <a href="https://www.youtube.com/watch?v=95M6V3mZgrI" target="_blank">https://www.youtube.com/watch?v=95M6V3mZgrI</a>

#### Example

```c
#include <stdio.h>

#define DARR_STRIP_PREFIXES
#include "darr.h"

DARR_IMPLEMENT(int)

int main() {
    int_darr ints;
    int_darr_init(&ints);

    darr_error_t err;

    push(&ints, 69, &err);
    if(err.code != DARR_OK){
        printf("%s\n", err.message);
        return 1;
    }
    printf("%d", pop(&ints));
    darr_free(&ints);   // very important to free the array to prevent memory leaks
    return 0;
}
```
---
### [hmap.h](https://github.com/MrNimbus777/C-Collections/blob/main/hmap.h) (Hash Map)

Implementation of a generic hash map in C using a macro for creating the set of functions tied to a KET TYPE -> VALUE TYPE.

#### Example

```c
typedef char* string;

DECLARE_FUNCTIONS_FOR_KEY_T(int)
bool is_equal_int(int i1, int i2){ return i1 == i2; }
uint64_t hash_int(int i){ return fmix64(i); }

HMAP_IMPLEMENT(int, string)

int main() {
    int_to_string_hmap map;

    hmap_error_t err;
    
    int_to_string_hmap_init(&map, &err);
    if(err.code != HMAP_OK) {
        fprintf(stderr, "%s\n", err.message);
        return err.code;
    }
    put(&map, 69, "Six Nine", &err);
    printf("%s\n", *get_or_null(&map, 69, &err));
    clear(&map);   // very important to free the array to prevent memory leaks
    return 0;
}
```
---
### [llist.h](https://github.com/MrNimbus777/C-Collections/blob/main/llist.h) (Single Linked List)

Implementation of a generic single linked list in C using a macro for creating the set of function tied to a TYPE.

#### Example
```c
#include <stdio.h>
#include "llist.h"

LLIST_IMPLEMENT(int)
bool is_equal_int(int i1, int i2){ return i1 == i2; }

int main() {
    int_llist list;

    int_llist_init(&list);
    int_llist_add(&list, 69);

    printf("%d\n", int_llist_pop(&list));

    int_llist_clear(&list);   // very important to free the array to prevent memory leaks
    return 0;
}
```

