#pragma once

void *new_array(unsigned long num, unsigned int size, int use_calloc);
void *realloc_array(void *ptr, unsigned int size, size_t num);