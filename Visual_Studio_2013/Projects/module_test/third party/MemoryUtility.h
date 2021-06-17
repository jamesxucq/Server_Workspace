#pragma once


#define MALLOC_MAX 0x40000000
void *_new_array(unsigned long num, unsigned int size, int use_calloc);
void *_realloc_array(void *ptr, unsigned int size, size_t num);

/* Convenient wrappers for malloc and realloc.  Use them. */
#define newm(type) ((type*)malloc(sizeof (type)))
#define newc(type) ((type*)calloc(1, sizeof (type)))
#define new_array(type, num) ((type*)_new_array((num), sizeof (type), 0))
#define new_arrayc(type, num) ((type*)_new_array((num), sizeof (type), 1))
#define realloc_array(ptr, type, num) ((type*)_realloc_array((ptr), sizeof(type), (num)))