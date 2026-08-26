#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <dirent.h>
#include "array.h"

int compare_pal(const void* a,const void* b) {
	const Code_pal* node_a=*(const Code_pal**)a;
	const Code_pal* node_b=*(const Code_pal**)b;
	return strcmp(node_a->pal,node_b->pal);
}

Array* create_array(size_t size) {
	if(size==0)
		return NULL;
	Array* array=malloc(sizeof(Array));
	if(array==NULL) {
		fprintf(stderr,"---Error (create_array): can't malloc struct Array.\n");
		return NULL;
	}
	array->arr_ptr=malloc(sizeof(Code_pal*)*size);
	if(array->arr_ptr==NULL) {
		fprintf(stderr,"---Error (create_array): can't malloc array of Code_pal*.\n");
		return NULL;
	}
	for(size_t i=0;i!=size;++i)
		array->arr_ptr[i]=NULL;
	array->arr_size=size;
	array->nearest_empty_index=0;
	return array;
}

int free_array(Array* array) {
	if(array==NULL) {
		printf("---Info (free_array): provided NULL pointer on Array, nothing to free.\n");
		return EXIT_FAILURE;
	}
	if(array->arr_ptr!=NULL) {
		for(size_t i=0;i!=array->arr_size;++i) {
			if(array->arr_ptr[i]!=NULL) {
				free(array->arr_ptr[i]->code);
				free(array->arr_ptr[i]->pal);
				free(array->arr_ptr[i]);
			}
		}
	}
	free(array->arr_ptr);
	free(array);
	array=NULL;
	return EXIT_SUCCESS;
}

int add_struct(const char* code,const char* pal,Array* array) {
	if(array==NULL) {
		printf("---Info (add_struct): provided NULL pointer on Array, nowhere to add struct.\n");
		return EXIT_FAILURE;
	}
	if(array->nearest_empty_index==array->arr_size) {
		printf("---Info (add_struct): array is full of data, can't add any node.\n");
		return EXIT_FAILURE;
	}
	Code_pal* node=malloc(sizeof(Code_pal));
	if(node==NULL) {
		fprintf(stderr,"---Error (add_struct): can't malloc struct Code_pal.\n");
		return EXIT_FAILURE;
	}
	node->code=malloc(strlen(code)+1);
	node->pal=malloc(strlen(pal)+1);
	if(node->code==NULL || node->pal==NULL) {
		fprintf(stderr,"---Error (add_struct): can't malloc code or pal.\n");
		return EXIT_FAILURE;
	}
	strncpy(node->code,code,strlen(code));
	node->code[strlen(code)]='\0';
	strncpy(node->pal,pal,strlen(pal));
	node->pal[strlen(pal)]='\0';
	array->arr_ptr[array->nearest_empty_index]=node;
	array->nearest_empty_index++;
	node=NULL;
	return EXIT_SUCCESS;
}

void sort_array(Array* array,size_t occ_size) {
	if(array==NULL) {
		printf("---Info (sort_array): provided NULL pointer on Array, nothing to sort.\n");
		return;
	}
	qsort(array->arr_ptr,occ_size,sizeof(Code_pal*),compare_pal);
}

void write_array_to_file(Array* array,const char* filename) {
	if(array==NULL) {
		printf("---Info (sort_array): provided NULL pointer on Array, nothing to write in file.\n");
		return;
	}
	const char* suffix="_FINAL.txt";
	char prefix[strlen(filename)-EXTENSION_TXT+1];
	strncpy(prefix,filename,strlen(filename)-EXTENSION_TXT);
	prefix[strlen(filename)-EXTENSION_TXT]='\0';
	char new_filename[strlen(filename)-EXTENSION_TXT+strlen(suffix)+1];
	snprintf(new_filename,sizeof(new_filename),"%s%s",prefix,suffix);	
	
	FILE* file=fopen(new_filename,"w");
	if(file==NULL) {
		fprintf(stderr,"---Error (sort_array): can't open file \"%s.txt\"\n",new_filename);
		return;
	}
	for(size_t i=0;i!=array->nearest_empty_index;i++) {
		if(array->arr_ptr[i]==NULL || array->arr_ptr[i]->code==NULL || array->arr_ptr[i]->pal==NULL)
			continue;
		if(i!=0 && strcmp(array->arr_ptr[i]->pal,array->arr_ptr[i-1]->pal)!=0)
			fprintf(file,"\n");
		fprintf(file,"%s   %s\n",array->arr_ptr[i]->code,array->arr_ptr[i]->pal);
	}
	fclose(file);
}
