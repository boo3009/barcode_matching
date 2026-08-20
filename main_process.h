#ifndef SENTRY_MAIN__H
#define SENTRY_MAIN__H
#include "hashmap.h"
#include "array.h"


enum { PACKAGE_TOTAL_LEN=6, PACKAGE_TOTAL_SUM=9999, LINES_FOR_ANALYZE=16, LINES_FOR_RAW_FILE=4,
			 BUF_AVG_LEN=50, BUF_MAX_LEN=100, FIRST_LETTERS=3, /*EXTENSION_TXT=4*/};
enum { RAW_DATA_FILE, RAW_WITHOUT_PREFIX, CODES_FILE, OTHER };

typedef struct Lengths {
	size_t code_len;
	size_t pal_len;
	char* code_example;
} Lengths;

int is_file_from_web(FILE* unknown_file,const char* filename);
char* get_pal_num(FILE *raw_file);
Lengths* get_lengths(FILE* raw_file,const char* pal_num);
size_t get_lines_count(FILE* code_file);
size_t get_code_len(FILE* codes_file);
size_t fill_hashmap(FILE* raw_file,Hashmap* hashmap,Lengths* lengths,const char* pal_num);
size_t fill_array(FILE* code_file,Hashmap* hashmap,Array* arr,size_t code_len);
void main_processing_function(Hashmap* hashmap);

#endif
