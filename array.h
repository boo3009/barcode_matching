#ifndef ARRAY_SENTRY_H
#define ARRAY_SENTRY_H

#include <stdio.h>
#include <string.h>

enum { EXTENSION_TXT=4 	}; /* '.txt' */

typedef struct Code_pal {
	char* code;
	char* pal;
} Code_pal;

typedef struct Array {
	Code_pal** arr_ptr;
	size_t arr_size;
	size_t nearest_empty_index;
} Array;

int compare_pal(const void* a,const void* b);
Array* create_array(size_t size);
int free_array(Array* array);
int add_struct(const char* code,const char* pal,Array* array);
void sort_array(Array* array);
void write_array_to_file(Array* array,const char* filename);

#endif
