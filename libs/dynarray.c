#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "dynarray.h"

// Example ussage:
// DynArr a;
// initArray(&a, 5, sizeof(char));
// char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
// for (size_t i = 0; i < 26; i++) {
//     insertArray(&a, &alphabet[i]);
// }
// char *data = (char*)a.arr;
// for (size_t i = 0; i < a.used; i++) {
//     printf("%c\n", data[i]);
// }
// printf("Array Size: %d\n", a.size);
// printf("Array Size Used: %d\n", a.used);;
// freeArray(&a);

void initArray(DynArr *a, size_t initSize, size_t typeSize) {
    a->arr = malloc(initSize * typeSize);
    a->used = 0;
    a->size = initSize;
    a->type_size = typeSize;

    if (a == NULL) {
        fprintf(stderr, "Memory not allocated\n");
        exit(EXIT_FAILURE);
    }
}

void insertArray(DynArr *a, void *item) {
    if (a->used == a->size) {
        a->size += 2;
        a->arr = realloc(a->arr, a->size * a->type_size);
    }
    void *target = (char*)a->arr + (a->used * a->type_size);
    memcpy(target, item, a->type_size);
    a->used++;
}

void removeItem(DynArr *a, int32_t index) {
    if (index < 0 || index >= a->used) {
        fprintf(stderr, "Index out of bounds\n");
        return;
    }

    char *base = (char*)a->arr;
    void *dest = base + index * a->type_size;
    void *src = base + (index + 1) * a->type_size;

    size_t numToMove = a->used - index - 1;

    memmove(dest, src, numToMove * a->type_size);
    a->used--;
    // Shrink array if possible
    shrinkArray(a);
}

// Shrinks array by two type sizes
void shrinkArray(DynArr *a) {
    if (a->used > a->size - 2 || a->size <= 2) {
        return;
    }

    size_t new_size = a->size - 2;
    void *tmp = realloc(a->arr, new_size * a->type_size);
    
    if (tmp == NULL) {
        return;
    }

    a->arr = tmp;
    a->size = new_size;
}

void freeArray(DynArr *a) {
    free(a->arr);
    a->arr = NULL;
    a->used = a->size = a->type_size = 0;
}