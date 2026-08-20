#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include "main_process.h"


int is_file_from_web(FILE* unknown_file,const char* const filename) {
	fseek(unknown_file,0,SEEK_SET);
	char extension[EXTENSION_TXT+1];
	memset(extension,0,sizeof(extension));
	for(size_t i=strlen(filename),j=sizeof(extension)-1;/* */;i--,j--) {
		extension[j]=filename[i];
		if(i==0 || j==0)
			break;
	}
	if(strcmp(extension,".txt")!=0)
		return OTHER;
	size_t count=0,cur=0;
	char buf[BUF_MAX_LEN];
	for( ; fgets(buf,sizeof(buf),unknown_file)!=NULL && count!=LINES_FOR_ANALYZE ;++count) {
		bool newline_found=false;
		char* ptr=buf;
		for(	;*ptr!='\0';++ptr) {
			if(*ptr=='\n') {
				newline_found=true;
				break;
			}
		}
		if(!newline_found) {
			printf("---Info (is_file_from_web): symbol count of line above max, it's not ok.\n");
			printf("---so file \"%s\" won't be processed.\n",filename);
			return OTHER;
		}
		if((buf[0]=='P' || buf[0]=='p') && count==0)
			return RAW_DATA_FILE;
		else if((buf[0]<'0' || buf[0]>'9') && count==0) {
			cur++;
			if(cur==LINES_FOR_RAW_FILE) {
				printf("---Info (is_file_from_web): it seems that file \"%s\" is raw\n",filename);
			  printf("---and you forgot to add \"PAL-011...\" at the beginning of the file.\n");
				printf("---By that file won't be processed at all. Add prefix and run again.\n");
				return RAW_WITHOUT_PREFIX;
			}
		}
	}
	return CODES_FILE;
}

char* get_pal_num(FILE *raw_file) {
	fseek(raw_file,0,SEEK_SET);
	char* pal_num=NULL;
	char buf[BUF_MAX_LEN];
	if(fgets(buf,sizeof(buf),raw_file) != NULL) {
		size_t len=0;
		for(char* tmp=buf;*tmp!='\n' && *tmp!='\0';tmp++) {
			if(*tmp>='0' && *tmp<='9')
				len++;
			if(len!=0 && (*tmp<'0' || *tmp>'9'))
				break;
		}
		pal_num=malloc(sizeof(char)*(len+1));
		size_t i=0;
		for(char* tmp=buf;i!=len && *tmp!='\0';tmp++) {
			if(*tmp>='0' && *tmp<='9') {
				pal_num[i]=*tmp;
				i++;
			}
			if(i!=0 && (*tmp<'0' || *tmp>'9'))
				break;
		}
		pal_num[len]='\0';
	}
	return pal_num;
}

Lengths* get_lengths(FILE* raw_file,const char* const pal_num) {
	fseek(raw_file,0,SEEK_SET);
	Lengths* lengths=malloc(sizeof(Lengths));
	if(lengths==NULL) {
		fprintf(stderr,"---Error (get_lengths): can't malloc struct Lengths!\n");
		return NULL;
	}
	lengths->pal_len=strlen(pal_num);
	char buf[BUF_MAX_LEN];
	for(size_t count=0; fgets(buf,sizeof(buf),raw_file)!=NULL ;++count) {
		if(count==1) {
			if(buf[0]>='0' && buf[0]<='9') {
				size_t len=0;
				for(char* tmp=buf;*tmp!='\n' && *tmp!='\0';++tmp)
					len++;
				lengths->code_len=len;
				break;
			} else {
				printf("---Info (get_lengths): second line of file don't contain code.\n");
				return NULL;
			}
		}
	}
	lengths->code_example=malloc(lengths->code_len+1);
	if(lengths->code_example==NULL) {
		fprintf(stderr,"---Error (get_lengths): can't malloc char* for code_example.\n");
		return NULL;
	}
	strncpy(lengths->code_example,buf,lengths->code_len);
	lengths->code_example[lengths->code_len]='\0';
	return lengths;
}

size_t fill_hashmap(FILE* raw_file,Hashmap* hashmap,Lengths* lengths,const char* const pal_num) {
	fseek(raw_file,0,SEEK_SET);
	size_t count=0;
	char code[lengths->code_len+1];
	char buf[BUF_MAX_LEN];
	while(fgets(buf,sizeof(buf),raw_file)!=NULL) {
		if(strncmp(buf,lengths->code_example,FIRST_LETTERS)==0) {
			char* ptr=code;
			for(char* tmp=buf;*tmp!='\n' && *tmp!='\0' && ptr-code!=lengths->code_len;tmp++,ptr++)
				*ptr=*tmp;
			if(ptr-code==lengths->code_len)
				*ptr='\0';
			else
				printf("WHAT THE FUCK MAN\n");
			insert_node(code,pal_num,hashmap);
			count++;
		}
	}
	return count;
}

size_t fill_array(FILE* code_file,Hashmap* hashmap,Array* arr,size_t code_len) {
	fseek(code_file,0,SEEK_SET);
	size_t count=0;
	char code[code_len+1];
	char buf[BUF_MAX_LEN];
	while(fgets(buf,sizeof(buf),code_file)!=NULL) {
		if(buf[0]>='0' && buf[0]<='9') {
			char* ptr=code;
			for(char* tmp=buf;*tmp!='\n' && *tmp!='\0' && ptr-code!=code_len;tmp++,ptr++)
				*ptr=*tmp;
			if(ptr-code==code_len)
				*ptr='\0';
			else
				printf("WHAT THE FUCK MAN\n");
			Node* node=search_node(code,hashmap);
			if(node!=NULL) {
				add_struct(node->key,node->value,arr);
				count++;
			}
		}
	}
	return count;
}

size_t get_lines_count(FILE* codes_file) {
	fseek(codes_file,0,SEEK_SET);
	size_t lines=0;
	char buf[BUF_MAX_LEN];
	while(fgets(buf,sizeof(buf),codes_file) != NULL) {
		if(buf[0]>='0' && buf[0]<='9')
			lines++;
	}
	return lines;
}

size_t get_code_len(FILE* codes_file) {
	fseek(codes_file,0,SEEK_SET);
	size_t code_len=0;
	char buf[BUF_MAX_LEN];
	while(fgets(buf,sizeof(buf),codes_file) != NULL) {
		if(buf[0]>='0' && buf[0]<='9') {
			for(char* tmp=buf;*tmp!='\n' && *tmp!='\0';tmp++)
					code_len++;
			break;
		}
	}
	return code_len;
}

void main_processing_function(Hashmap* hashmap) {
	DIR* dir=opendir(".");
	if(dir==NULL) {
		fprintf(stderr,"---Error (main_processing_function): can't open the current directory!\n");
		return;
	}
	FILE* cur_file=NULL;
	struct dirent* ptr_to_files=NULL;
	while((ptr_to_files=readdir(dir)) != NULL) {
		if(ptr_to_files->d_name[0]=='.' || strcmp(ptr_to_files->d_name,"main")==0)
			continue;
		cur_file=fopen(ptr_to_files->d_name,"r");
		if(cur_file==NULL) {
			fprintf(stderr,"---Error (main_processing_function): can't open file: %s\n",ptr_to_files->d_name);
			return;
		}
		int res=is_file_from_web(cur_file,ptr_to_files->d_name);
		if(res==RAW_DATA_FILE) {
			char* pal_num=get_pal_num(cur_file);
			if(pal_num==NULL)
				continue;
			printf("---Pallet number for file \"%s\" is: %s\n",ptr_to_files->d_name,pal_num);
			
			Lengths* lengths=get_lengths(cur_file,pal_num);
			size_t count=fill_hashmap(cur_file,hashmap,lengths,pal_num);
			printf("---Codes filtered and added to hashmap: \"%s\", %ld\n",ptr_to_files->d_name,count);
			free(pal_num);
			free(lengths->code_example);
			free(lengths);
		}
		fclose(cur_file);
	}
	rewinddir(dir);
	while((ptr_to_files=readdir(dir)) != NULL) {
		if(ptr_to_files->d_name[0]=='.' || strcmp(ptr_to_files->d_name,"main")==0)
			continue;
		cur_file=fopen(ptr_to_files->d_name,"r");
		if(cur_file==NULL) {
			fprintf(stderr,"---Error (main_processing_function): can't open file: %s\n",ptr_to_files->d_name);
			return;
		}
		int res=is_file_from_web(cur_file,ptr_to_files->d_name);
		if(res==CODES_FILE) {
			size_t code_len=get_code_len(cur_file); 
			size_t lines_count=get_lines_count(cur_file);
			Array* array=create_array(lines_count);
			size_t count=fill_array(cur_file,hashmap,array,code_len);
			printf("---Codes filtered and added to array: \"%s\", %ld\n",ptr_to_files->d_name,count);
			sort_array(array);
			write_array_to_file(array,ptr_to_files->d_name);
			free_array(array);
		}
		fclose(cur_file);
	}
	closedir(dir);
}
