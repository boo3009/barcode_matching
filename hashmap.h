#ifndef SENTRY_HASHMAP_H
#define SENTRY_HASHMAP_H
#include <stdint.h>

#define MAX_SIZE 65536  // 2^16 bytes
#define MAX_LOAD_FACTOR 0.75  // comfort load_factor for resizing hashmap

typedef struct Node {
	char* key;
	char* value;
	struct Node* next;
} Node;

typedef struct Hashmap {
	Node** arr_ptr;
	size_t bucket_count;			// indexes count (size of array of pointers)
	size_t node_count;				// all the nodes (from every linked list sumed)
	size_t occupied_count;		// all the occupied indexes
	float load_factor;					// which % of indexes is occupied(=occ/buck)
} Hashmap;

typedef const char* str;

Hashmap* create_hashmap(size_t bucket_count);
Hashmap* resize_hashmap(Hashmap* old_hmap);
void free_hashmap(Hashmap* hmap);

int insert_node(str key,str value,Hashmap* hmap);
int move_node(Hashmap* old_hmap,size_t old_index,Node* node,Hashmap* new_hmap);
Node* search_node(str key,const Hashmap* hmap);
void free_node(str key,Hashmap* hmap);

size_t get_nearest_prime(size_t number);
size_t calculate_index(str key,size_t bucket_count);  
size_t get_collisions_count(const Hashmap* hmap);  // (=node_count-occ)
void print_hashmap(const Hashmap* hmap);

#endif
