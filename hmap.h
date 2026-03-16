// ####################################################################################################################
// Implementation of a generic hash map in C using a macro for creating the set of 
// functions tied to a KET TYPE -> VALUE TYPE.
//
// ??? HOW TO USE ???
// Let's say you need a map of string to int, then you call the implementation macros like this: 
// DECLARE_FUNCTIONS_FOR_KEY_T(int) - this will DECLARE the hash_int and is_equal_int functions that you must define 
//                                    later. They are separated from the main macro because you may need more maps 
//                                    types with same KEY type (e.g int to double, int to char ...) 
//
// HMAP_IMPLEMENT(int, string) - this will generate the struct int_to_string_hmap and all the related functions that 
//                               you can analize yourself in the below. 
//                               I actually recommed using HMAP_IMPLEMENT_EXPLICIT(int, string, i2s) where you can
//                               specify a certain name so you could have a "i2s" instead of int_to_string_hmap.
//                               The same with the functions: int_to_string_hmap_init becomes i2s_init.
//
// Use example:
//
// .... (other code)
// typedef char* string;
// DECLARE_FUNCTIONS_FOR_KEY_T(int)
// bool is_equal_int(int i1, int i2){ return i1 == i2; }
// uint64_t hash_int(int i){ return fmix64(i); }
// 
// HMAP_IMPLEMENT(int, string)
// int main() {
//     int_to_string_hmap map;
//     int_to_string_hmap_init(&map);
//     int_to_string_hmap_put(&map, 69, "Six Nine");
//     printf("%s", *int_to_string_hmap_get_or_null(&map, 69));
//     int_to_string_hmap_clear(&map);  // very important to clear the elements to prevent memory leaks!
//     return 0;
// }
//
//
// !!! IMPORTANT NOTE: performing push might do a resize action which might invalidate the previous 
//     position of 'elements' pointer. This means you should never rely on a previously saved
//     pointer to a certain object, only on its index relatively to 'elements' pointer
//
//
// There are also a lot of useful and interting functions so I definetely recommend you to read the lib till the end
// to have a better understanding!
//
// ####################################################################################################################



#ifndef HMAP_H
#define HMAP_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>

typedef unsigned char byte;

static const uint8_t SIZEOF_BITMAP = CHAR_BIT/2;

#define LOAD_FACTOR 0.7


#define HMAP_FOREACH(MAP_STRUCT_NAME, map_ptr, valid_iterator_ptr) for(MAP_STRUCT_NAME##_iterator_init(valid_iterator_ptr); MAP_STRUCT_NAME##_next(map_ptr, valid_iterator_ptr);)

//
//  You may use this function for generating hashes. 
//
uint64_t fmix64(uint64_t x) {
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdULL;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ULL;
    x ^= x >> 33;
    return x;
}

size_t nextPowerOfTwo(size_t n) {
    if (n == 0) return 1;

    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
#if SIZE_MAX > UINT32_MAX
    n |= n >> 32;
#endif

    return n + 1;
}

// This macro is used to DECLARE the "hash" and "is_equal" functions related to keys. You must lately provide an DEFINITION (block of code) for these functions, in order to use the hash map!
#define DECLARE_FUNCTIONS_FOR_KEY_T(KEY_T) uint64_t hash_##KEY_T (KEY_T key);                                     \
bool is_equal_##KEY_T (KEY_T key1, KEY_T key2);


//   #######################################################
//          TECHNICAL MACRO, PLEASE DO NOT USE IT !!!
//   #######################################################
#define PUT_WITHOUT_SIZE_CHECK(map, k, v, KEY_T) do {                                                             \
    size_t mask = map->capacity - 1;                                                                              \
    size_t start = hash_##KEY_T(k) & mask;                                                                        \
                                                                                                                  \
    size_t first_deleted = SIZE_MAX;                                                                              \
                                                                                                                  \
    for (size_t probe = 0; probe < map->capacity; probe++) {                                                      \
                                                                                                                  \
        size_t pos = (start + probe) & mask;                                                                      \
                                                                                                                  \
        size_t byte_index = pos / SIZEOF_BITMAP;                                                                  \
        uint8_t bit_index = (pos & (SIZEOF_BITMAP - 1)) << 1;                                                     \
        uint8_t state = (map->bitmap[byte_index] >> bit_index) & 3;                                               \
                                                                                                                  \
                                                                                                                  \
        if (state == 1) {                                                                                         \
            if (is_equal_##KEY_T(map->elements[pos].key, k)) {                                                    \
                map->elements[pos].value = v;                                                                     \
                break;                                                                                            \
            }                                                                                                     \
        }                                                                                                         \
        else if (state == 0) {                                                                                    \
            if (first_deleted != -1)                                                                              \
                pos = first_deleted;                                                                              \
                                                                                                                  \
            map->elements[pos].key = k;                                                                           \
            map->elements[pos].value = v;                                                                         \
                                                                                                                  \
            size_t bi = pos / SIZEOF_BITMAP;                                                                      \
            uint8_t bti = 2 * (pos & (SIZEOF_BITMAP - 1));                                                        \
                                                                                                                  \
            map->bitmap[bi] &= ~(3 << bti);                                                                       \
            map->bitmap[bi] |= (1 << bti);                                                                        \
                                                                                                                  \
            map->size++;                                                                                          \
            first_deleted = SIZE_MAX;                                                                             \
            break;                                                                                                \
        }                                                                                                         \
        else if (state == 2) {                                                                                    \
            if (first_deleted == SIZE_MAX)                                                                        \
                first_deleted = pos;                                                                              \
        }                                                                                                         \
        else {                                                                                                    \
            exit(EXIT_FAILURE);                                                                                   \
        }                                                                                                         \
    }                                                                                                             \
    if(first_deleted != SIZE_MAX) {                                                                               \
        map->elements[first_deleted].key = k;                                                                     \
        map->elements[first_deleted].value = v;                                                                   \
                                                                                                                  \
        size_t bi = first_deleted / SIZEOF_BITMAP;                                                                \
        uint8_t bti = 2 * (first_deleted & (SIZEOF_BITMAP - 1));                                                  \
                                                                                                                  \
        map->bitmap[bi] &= ~(3 << bti);                                                                           \
        map->bitmap[bi] |= (1 << bti);                                                                            \
                                                                                                                  \
        map->size++;                                                                                              \
    }                                                                                                             \
} while(0)


// KEY_T, VALUE_T: for pointers provide a wrapping type (e.g. char* -> typedef char* my_string)
#define HMAP_IMPLEMENT_EXPLICIT(KEY_T, VALUE_T, MAP_STRUCT_NAME)                                                  \
                                                                                                                  \
typedef struct {                                                                                                  \
    KEY_T key;                                                                                                    \
    VALUE_T value;                                                                                                \
} MAP_STRUCT_NAME##_entry;                                                                                        \
                                                                                                                  \
typedef struct {                                                                                                  \
    MAP_STRUCT_NAME##_entry* elements;                                                                            \
    size_t capacity;                                                                                              \
    size_t size;                                                                                                  \
    byte* bitmap; /*each element maps to 2 bits: 00 - free, 01 - occupied, 10 - deleted, 11 - invalid*/           \
} MAP_STRUCT_NAME;                                                                                                \
                                                                                                                  \
typedef struct {                                                                                                  \
    MAP_STRUCT_NAME##_entry* entry;                                                                               \
    size_t index;                                                                                                 \
} MAP_STRUCT_NAME##_iterator;                                                                                     \
                                                                                                                  \
static void MAP_STRUCT_NAME##_init(MAP_STRUCT_NAME* map) {                                                        \
    assert(map != NULL && "A valid map is expected");                                                             \
    map->elements = (MAP_STRUCT_NAME##_entry*)malloc(4 * sizeof(MAP_STRUCT_NAME##_entry));                        \
    if (!map->elements) exit(EXIT_FAILURE);                                                                       \
    map->bitmap = (byte*)calloc(1, sizeof(byte));                                                                 \
    if (!map->bitmap) exit(EXIT_FAILURE);                                                                         \
    map->size = 0;                                                                                                \
    map->capacity = 4;                                                                                            \
}                                                                                                                 \
                                                                                                                  \
/* capacity must be a power of 2 */                                                                               \
static void MAP_STRUCT_NAME##_init_with_capacity(MAP_STRUCT_NAME* map, size_t capacity) {                         \
    assert(map != NULL && "A valid map is expected");                                                             \
    assert(capacity != 0 && (capacity & (capacity - 1)) == 0 && "capacity must be a power of 2");                 \
    map->elements = (MAP_STRUCT_NAME##_entry*)malloc(capacity * sizeof(MAP_STRUCT_NAME##_entry));                 \
    if (!map->elements) exit(EXIT_FAILURE);                                                                       \
    map->bitmap = (byte*)calloc((capacity + SIZEOF_BITMAP - 1) / SIZEOF_BITMAP, sizeof(byte));                    \
    if (!map->bitmap) exit(EXIT_FAILURE);                                                                         \
    map->size = 0;                                                                                                \
    map->capacity = capacity;                                                                                     \
}                                                                                                                 \
                                                                                                                  \
/* capacity must be a power of 2 */                                                                               \
static void MAP_STRUCT_NAME##_resize(MAP_STRUCT_NAME* map, size_t new_capacity) {                                 \
    assert(map != NULL && "A valid map is expected");                                                             \
    assert(new_capacity != 0 && (new_capacity & (new_capacity - 1)) == 0 && "new_capacity must be a power of 2"); \
    assert(new_capacity > map->size && "cannot shrink the map");                                                  \
                                                                                                                  \
    size_t old_capacity = map->capacity;                                                                          \
                                                                                                                  \
    if(old_capacity == new_capacity) return;                                                                      \
                                                                                                                  \
    MAP_STRUCT_NAME##_entry* old_elements = map->elements;                                                        \
    byte* old_bitmap = map->bitmap;                                                                               \
                                                                                                                  \
    map->elements = malloc(new_capacity * sizeof(MAP_STRUCT_NAME##_entry));                                       \
    if (!map->elements) exit(EXIT_FAILURE);                                                                       \
                                                                                                                  \
    map->bitmap = calloc((new_capacity + SIZEOF_BITMAP - 1) / SIZEOF_BITMAP, sizeof(byte));                       \
    if (!map->bitmap) exit(EXIT_FAILURE);                                                                         \
                                                                                                                  \
    map->capacity = new_capacity;                                                                                 \
    map->size = 0;                                                                                                \
                                                                                                                  \
    for (size_t i = 0; i < old_capacity; i++) {                                                                   \
                                                                                                                  \
        size_t byte_index = i / SIZEOF_BITMAP;                                                                    \
        uint8_t bit_index = (i & (SIZEOF_BITMAP - 1)) << 1;                                                       \
        uint8_t state = (old_bitmap[byte_index] >> bit_index) & 3;                                                \
                                                                                                                  \
        if (state == 1) {                                                                                         \
            KEY_T k = old_elements[i].key;                                                                        \
            VALUE_T v = old_elements[i].value;                                                                    \
            PUT_WITHOUT_SIZE_CHECK(map, k, v, KEY_T);                                                             \
        }                                                                                                         \
    }                                                                                                             \
                                                                                                                  \
    free(old_elements);                                                                                           \
    free(old_bitmap);                                                                                             \
}                                                                                                                 \
                                                                                                                  \
static void MAP_STRUCT_NAME##_reserve(MAP_STRUCT_NAME* map, size_t new_capacity) {                                \
    assert(map != NULL && "A valid map is expected");                                                             \
    if(map->capacity < new_capacity) MAP_STRUCT_NAME##_resize(map, nextPowerOfTwo(new_capacity));                 \
}                                                                                                                 \
                                                                                                                  \
static bool MAP_STRUCT_NAME##_try_shrink(MAP_STRUCT_NAME* map) {                                                  \
    assert(map != NULL && "A valid map is expected");                                                             \
    size_t new_capacity = nextPowerOfTwo(map->size/LOAD_FACTOR);                                                  \
    if(map->capacity > new_capacity) {                                                                            \
        MAP_STRUCT_NAME##_resize(map, new_capacity);                                                              \
        return true;                                                                                              \
    }                                                                                                             \
    return false;                                                                                                 \
}                                                                                                                 \
                                                                                                                  \
static void MAP_STRUCT_NAME##_free(MAP_STRUCT_NAME* map) {                                                        \
    assert(map != NULL && "A valid map is expected");                                                             \
    if(map->elements) free(map->elements);                                                                        \
    if(map->bitmap) free(map->bitmap);                                                                            \
    map->elements = NULL;                                                                                         \
    map->bitmap = NULL;                                                                                           \
    map->size = 0;                                                                                                \
    map->capacity = 0;                                                                                            \
}                                                                                                                 \
                                                                                                                  \
static void MAP_STRUCT_NAME##_put(MAP_STRUCT_NAME* map, KEY_T key, VALUE_T value) {                               \
    assert(map != NULL && "A valid map is expected");                                                             \
    if (map->size >= (size_t)(map->capacity * LOAD_FACTOR)) {                                                     \
        MAP_STRUCT_NAME##_resize(map, map->capacity * 2);                                                         \
    }                                                                                                             \
    PUT_WITHOUT_SIZE_CHECK(map, key, value, KEY_T);                                                               \
}                                                                                                                 \
                                                                                                                  \
static VALUE_T* MAP_STRUCT_NAME##_get_or_null(MAP_STRUCT_NAME* map, KEY_T key) {                                  \
    assert(map != NULL && "A valid map is expected");                                                             \
    if (map->size == 0) return NULL;                                                                              \
    size_t pos = hash_##KEY_T(key) & (map->capacity - 1);                                                         \
    size_t byte_index = pos / SIZEOF_BITMAP;                                                                      \
    uint8_t bit_index =  (pos & (SIZEOF_BITMAP - 1)) << 1;                                                        \
    size_t counter = map->capacity;                                                                               \
    while (counter--) {                                                                                           \
        uint8_t state = (map->bitmap[byte_index] >> bit_index) & 3;                                               \
        if (state == 1) {                                                                                         \
            pos = byte_index*SIZEOF_BITMAP + bit_index/2;                                                         \
            if (is_equal_##KEY_T(map->elements[pos].key, key))                                                    \
                return &map->elements[pos].value;                                                                 \
        } else if(state == 0) {                                                                                   \
            return NULL;                                                                                          \
        } else if(state == 3) exit(EXIT_FAILURE);                                                                 \
                                                                                                                  \
        bit_index += 2;                                                                                           \
        if(bit_index >= CHAR_BIT) {                                                                               \
            bit_index = 0;                                                                                        \
            byte_index = (byte_index + 1) & (map->capacity / SIZEOF_BITMAP - 1);                                  \
        }                                                                                                         \
    }                                                                                                             \
    return NULL;                                                                                                  \
}                                                                                                                 \
                                                                                                                  \
static void MAP_STRUCT_NAME##_remove(MAP_STRUCT_NAME* map, KEY_T key) {                                           \
    assert(map != NULL && "A valid map is expected");                                                             \
    if (map->size == 0)                                                                                           \
        return;                                                                                                   \
    size_t pos = hash_##KEY_T(key) & (map->capacity - 1);                                                         \
    size_t byte_index = pos / SIZEOF_BITMAP;                                                                      \
    uint8_t bit_index = (pos & (SIZEOF_BITMAP - 1)) << 1;                                                         \
    size_t counter = map->capacity;                                                                               \
    while (counter--) {                                                                                           \
        uint8_t state = (map->bitmap[byte_index] >> bit_index) & 3;                                               \
        size_t real_pos = byte_index * SIZEOF_BITMAP + bit_index / 2;                                             \
                                                                                                                  \
        if (state == 1) {                                                                                         \
            if (is_equal_##KEY_T(map->elements[real_pos].key, key)) {                                             \
                map->bitmap[byte_index] &= ~(3 << bit_index);                                                     \
                map->bitmap[byte_index] |= (2 << bit_index);                                                      \
                map->size--;                                                                                      \
                return;                                                                                           \
            }                                                                                                     \
        }                                                                                                         \
        else if (state == 0) {                                                                                    \
            return;                                                                                               \
        }                                                                                                         \
        else if (state == 3) {                                                                                    \
            exit(EXIT_FAILURE);                                                                                   \
        }                                                                                                         \
                                                                                                                  \
        bit_index += 2;                                                                                           \
        if (bit_index >= CHAR_BIT) {                                                                              \
            bit_index = 0;                                                                                        \
            byte_index = (byte_index + 1) & (map->capacity / SIZEOF_BITMAP - 1);                                  \
        }                                                                                                         \
    }                                                                                                             \
}                                                                                                                 \
                                                                                                                  \
static bool MAP_STRUCT_NAME##_contains(MAP_STRUCT_NAME* map, KEY_T key){                                          \
    assert(map != NULL && "A valid map is expected");                                                             \
    if (map->size == 0) return false;                                                                             \
    size_t pos = hash_##KEY_T(key) & (map->capacity - 1);                                                         \
    size_t byte_index = pos / SIZEOF_BITMAP;                                                                      \
    uint8_t bit_index = (pos & (SIZEOF_BITMAP - 1)) << 1;                                                         \
    size_t counter = map->capacity;                                                                               \
    while (counter--) {                                                                                           \
        uint8_t state = (map->bitmap[byte_index] >> bit_index) & 3;                                               \
        if (state == 1) {                                                                                         \
            pos = byte_index*SIZEOF_BITMAP + bit_index/2;                                                         \
            if (is_equal_##KEY_T(map->elements[pos].key, key))                                                    \
                return true;                                                                                      \
        } else if(state == 0) {                                                                                   \
            return false;                                                                                         \
        } else if(state == 3) exit(EXIT_FAILURE);                                                                 \
                                                                                                                  \
        bit_index += 2;                                                                                           \
        if (bit_index >= CHAR_BIT) {                                                                              \
            bit_index = 0;                                                                                        \
            byte_index = (byte_index + 1) & (map->capacity / SIZEOF_BITMAP - 1);                                  \
        }                                                                                                         \
    }                                                                                                             \
    return false;                                                                                                 \
}                                                                                                                 \
                                                                                                                  \
static void MAP_STRUCT_NAME##_clear(MAP_STRUCT_NAME* map) {                                                       \
    assert(map != NULL && "A valid map is expected");                                                             \
    map->size = 0;                                                                                                \
    memset(map->bitmap, 0, (map->capacity + SIZEOF_BITMAP - 1) / SIZEOF_BITMAP);                                  \
}                                                                                                                 \
                                                                                                                  \
static void MAP_STRUCT_NAME##_clone(MAP_STRUCT_NAME* map1, MAP_STRUCT_NAME* map2) {                               \
    assert(map1 != NULL && "A valid map is expected");                                                            \
    assert(map2 != NULL && "A valid map is expected");                                                            \
    if(map1->size == 0) {                                                                                         \
        MAP_STRUCT_NAME##_init(map2);                                                                             \
        return;                                                                                                   \
    }                                                                                                             \
    MAP_STRUCT_NAME##_init_with_capacity(map2, map1->capacity);                                                   \
    map2->size = map1->size;                                                                                      \
    mempcpy(map2->bitmap, map1->bitmap, (map1->capacity + SIZEOF_BITMAP - 1) / SIZEOF_BITMAP);                    \
    mempcpy(map2->elements, map1->elements, map1->capacity * sizeof(MAP_STRUCT_NAME##_entry));                    \
}                                                                                                                 \
                                                                                                                  \
static void MAP_STRUCT_NAME##_iterator_init(MAP_STRUCT_NAME##_iterator* it) {                                     \
    assert(it != NULL && "A valid iterator is expected");                                                         \
    it->entry = NULL;                                                                                             \
    it->index = 0;                                                                                                \
}                                                                                                                 \
                                                                                                                  \
static MAP_STRUCT_NAME##_entry* MAP_STRUCT_NAME##_next(MAP_STRUCT_NAME* map, MAP_STRUCT_NAME##_iterator* it){     \
    assert(map != NULL && "A valid map is expected");                                                             \
    if(map->size == 0) return NULL;                                                                               \
    if(it == NULL) return NULL;                                                                                   \
    size_t start_position = it->entry == NULL ? 0 : it->entry-map->elements+1;                                    \
    size_t byte_index = start_position / SIZEOF_BITMAP;                                                           \
    uint8_t bit_index = (start_position & (SIZEOF_BITMAP - 1)) << 1;                                              \
    for(size_t i = start_position; i < map->capacity; i++){                                                       \
        uint8_t state = (map->bitmap[byte_index] >> bit_index) & 3;                                               \
        if (state == 1) {                                                                                         \
            it->entry = map->elements+i;                                                                          \
            it->index++;                                                                                          \
            return it->entry;                                                                                     \
        } else if(state == 3) exit(EXIT_FAILURE);                                                                 \
                                                                                                                  \
        bit_index += 2;                                                                                           \
        if (bit_index >= CHAR_BIT) {                                                                              \
            bit_index = 0;                                                                                        \
            byte_index = byte_index+1;                                                                            \
        }                                                                                                         \
    }                                                                                                             \
    return NULL;                                                                                                  \
}                                                                                                                 \

// KEY_T, VALUE_T: for pointers provide a wrapping type (e.g. char* -> typedef char* my_string)
#define HMAP_IMPLEMENT(KEY_T, VALUE_T) HMAP_IMPLEMENT_EXPLICIT(KEY_T, VALUE_T, KEY_T##_to_##VALUE_T##_hmap)

#endif
