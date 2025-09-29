#ifndef DYNARRAY_H
#define DYNARRAY_H

typedef struct {
    void *arr;
    size_t used;
    size_t size;
    size_t type_size;
} DynArr;

void initArray(DynArr *a, size_t initSize, size_t typeSize);
void insertArray(DynArr *a, void *item);
void removeItem(DynArr *a, int32_t index);
void shrinkArray(DynArr *a);
void freeArray(DynArr *a);

#endif