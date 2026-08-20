#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hashmap.h"

Hashmap* create_hashmap(size_t bucket_count) {
	Hashmap* hmap=malloc(sizeof(Hashmap));
	if(hmap==NULL) {
		fprintf(stderr,"---Error (create_hashmap): can't malloc struct Hashmap");
		return NULL;
	}
	if(bucket_count==0)
		return NULL;
	bucket_count=get_nearest_prime(bucket_count);
	hmap->arr_ptr=malloc(sizeof(Node*)*bucket_count);
	if(hmap->arr_ptr==NULL) {
		fprintf(stderr,"---Error (create_hashmap): can't malloc array of Node pointers");
		return NULL;
	}
	for(size_t i=0;i!=bucket_count;++i)
		hmap->arr_ptr[i]=NULL;
	hmap->bucket_count=bucket_count;
	hmap->node_count=0;
	hmap->occupied_count=0;
	hmap->load_factor=0;
	return hmap;
}

Hashmap* resize_hashmap(Hashmap* old_hmap) {
	if(old_hmap==NULL) {
		printf("---Info (resize_hashmap): provided NULL pointer on Hashmap\n");
		return NULL;
	}
	size_t new_bucket_count = old_hmap->bucket_count*2<MAX_SIZE ? 
												 old_hmap->bucket_count*2 : MAX_SIZE-1;
	new_bucket_count=get_nearest_prime(new_bucket_count);
	Hashmap* new_hmap=create_hashmap(new_bucket_count);
	if(new_hmap==NULL)
		return NULL;
	Node* node=NULL;
	for(size_t idx=0;idx!=old_hmap->bucket_count;++idx) {
		for(node=old_hmap->arr_ptr[idx];node!=NULL;node=old_hmap->arr_ptr[idx])
			if(move_node(old_hmap,idx,node,new_hmap)==EXIT_FAILURE)
				return NULL;
	}
	free_hashmap(old_hmap);
	old_hmap=NULL;
	return new_hmap;
}

void free_hashmap(Hashmap* hmap) {
	if(hmap==NULL)
		return;
	if(hmap->arr_ptr!=NULL) {
		for(size_t i=0;i!=hmap->bucket_count;++i) {
			for(Node* tmp=hmap->arr_ptr[i];tmp!=NULL;tmp=hmap->arr_ptr[i]) {
				hmap->arr_ptr[i]=tmp->next;
				free(tmp->key);
				free(tmp->value);
				free(tmp);
			}
		}
		free(hmap->arr_ptr);
	}
	free(hmap);
	hmap=NULL;
}

int insert_node(str key,str value,Hashmap* hmap) {
	if(hmap && (hmap->load_factor > MAX_LOAD_FACTOR)) {
		if(resize_hashmap(hmap)==NULL)
			return EXIT_FAILURE;
	}
	if(hmap==NULL) {
		printf("---Info (insert_node): provided NULL pointer on Hashmap\n");
		return EXIT_FAILURE;
	}
	Node* new_node=(Node*)malloc(sizeof(Node));
	if(new_node==NULL) {
		fprintf(stderr,"---Error (insert_node): can't malloc Node");
		return EXIT_FAILURE;
	}
	new_node->key=malloc(strlen(key)+1);
	new_node->value=malloc(strlen(value)+1);
	if(new_node->key==NULL || new_node->value==NULL) {
		fprintf(stderr,"---Error (insert_node): can't malloc key or value for Node");
		return EXIT_FAILURE;
	}
	size_t index=calculate_index(key,hmap->bucket_count);
	if(hmap->arr_ptr[index]==NULL)
		hmap->occupied_count++;
	else {
		for(Node* tmp=hmap->arr_ptr[index];tmp!=NULL;tmp=tmp->next)
			if(strcmp(tmp->key,key)==0) {
				printf("---Info (insert_node): key already exists. No insertion!: %s,%s\n",key,tmp->key);
				return EXIT_SUCCESS;
			}
	}
	strncpy(new_node->key,key,strlen(key));
	new_node->key[strlen(key)]='\0';
	strncpy(new_node->value,value,strlen(value));
	new_node->value[strlen(value)]='\0';
	new_node->next=hmap->arr_ptr[index];
	hmap->arr_ptr[index]=new_node;
	hmap->node_count++;
	if(hmap->bucket_count!=0)
		hmap->load_factor = (float)hmap->occupied_count / (float)hmap->bucket_count;
	//new_node=NULL;
	return EXIT_SUCCESS;
}

int move_node(Hashmap* old_hmap,size_t old_index, Node* cur_node,Hashmap* new_hmap) {
	if(!old_hmap || !cur_node || !new_hmap ) {
		printf("---Info (move_node): provided NULL pointer on Hashmaps or Node\n");
		return EXIT_FAILURE;
	}
	size_t new_index=calculate_index(cur_node->key,new_hmap->bucket_count);
	if(new_hmap->arr_ptr[new_index]==NULL)
		new_hmap->occupied_count++;
	old_hmap->arr_ptr[old_index]=cur_node->next;
	cur_node->next=new_hmap->arr_ptr[new_index];
	new_hmap->arr_ptr[new_index]=cur_node;
	
	new_hmap->node_count++;
	if(new_hmap->bucket_count!=0)
		new_hmap->load_factor = (float)new_hmap->occupied_count / (float)new_hmap->bucket_count;
	return EXIT_SUCCESS;
}

Node* search_node(str key,const Hashmap* hmap) {
	if(hmap==NULL || hmap->arr_ptr==NULL) {
		printf("---Info (search_node): provided NULL pointer on Hashmap or on array of it\n");
		return NULL;
	}
	size_t index=calculate_index(key,hmap->bucket_count);
	for(Node* tmp=hmap->arr_ptr[index];tmp!=NULL;tmp=tmp->next) {
		if(strcmp(tmp->key,key)==0)
			return tmp;
	}
	printf("---Info (search_node): the key \"%s\" not exists\n",key);
	return NULL;
}

void free_node(str key,Hashmap* hmap) {
	if(hmap==NULL) {
		printf("---Info (free_node): provided NULL pointer on Hashmap\n");
		return;
	}
	size_t index=calculate_index(key,hmap->bucket_count);
	for(Node** tmp=&(hmap->arr_ptr[index]);(*tmp)!=NULL;tmp=&(*tmp)->next) {
		if(strcmp((*tmp)->key,key)==0) {
			Node* ptr=*tmp;	
			*tmp=(*tmp)->next;
			free(ptr->key);
			free(ptr->value);
			free(ptr);
			printf("---Info (free_node): key \"%s\" was freed\n",key);
			hmap->node_count--;
			if(hmap->arr_ptr[index]==NULL) {
				hmap->occupied_count--;
				if(hmap->bucket_count!=0)
					hmap->load_factor = (float)hmap->occupied_count / (float)hmap->bucket_count;
			}
			return;
		}
	}
}

size_t calculate_index(const char* s,size_t bucket_count) {
	size_t hash=5381;
	int c;
	for(const char* tmp=s;*tmp!='\0';tmp++) {
		c=*tmp;
		hash=((hash<<5)+hash)+c;
	}
	return hash % bucket_count;
}

size_t get_collisions_count(const Hashmap* hmap) {
	return hmap->node_count - hmap->occupied_count;
}

void print_hashmap(const Hashmap* hmap) {
	for(size_t idx=0;idx!=hmap->bucket_count;++idx) {
		if(hmap->arr_ptr[idx]==NULL) {
			printf("---empty---\n");
			continue;
		}	
		for(Node* tmp=hmap->arr_ptr[idx];tmp!=NULL;tmp=tmp->next)
			printf("|---%s, %s---|",tmp->key,tmp->value);
		printf("\n");
	}
}

size_t get_nearest_prime(size_t number) {
	size_t res=0;
	size_t offset = number==MAX_SIZE-1 ? -1 : 1;
	size_t max_min = number==MAX_SIZE-1 ? 0 : MAX_SIZE-1;
	for(res=number;res!=max_min;res+=offset) {
		if(res==1 || (res!=2 && res%2==0) || (res!=3 && res%3==0) || 
		  (res!=5 && res%5==0) || (res!=7 && res%7==0))
			continue;
		break;
	}
	return res;
}
