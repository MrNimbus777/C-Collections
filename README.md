# C-Collections

A small collection of generic data structures implemented in C using macros
to generate type-specific structures and functions.

---

### [darr.h](https://github.com/MrNimbus777/C-Collections/blob/main/darr.h) (Dynamic Array)

Implementation of a generic dynamic array in C. Originally inspired by a
similar structure seen in Tsoding's video: <a href="https://www.youtube.com/watch?v=95M6V3mZgrI" target="_blank">https://www.youtube.com/watch?v=95M6V3mZgrI</a>

#### Example

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

### [llist.h](https://github.com/MrNimbus777/C-Collections/blob/main/llist.h) (Single Linked List)

Implementation of a generic single linked list in C using a macro for creating the set of function tied to a TYPE.

#### Example
```
#include <stdio.h>
#include "llist.h"

LLIST_IMPLEMENT(int)
bool is_equal_int(int i1, int i2){ return i1 == i2; }

int main() {
    int_llist list;

    int_llist_init(&list);
    int_llist_add(&list, 69);

    printf("%d\n", int_llist_pop(&list));

    int_llist_clear(&list);   // important to clear the list to prevent memory leaks
    return 0;
}
```
