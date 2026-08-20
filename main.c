#include <stdio.h>
#include <stdlib.h>
#include "main_process.h"
#include "hashmap.h"


int main() {
	printf("---Number of packages in total (5-digit number equal or less then 9.999): ");
	char buf[PACKAGE_TOTAL_LEN];
	int bucket_count=0;
	if(fgets(buf,sizeof(buf),stdin)!=NULL) {
		if(sscanf(buf,"%d",&bucket_count)!=1) {
			fprintf(stderr,"---Error (main): can't convert user input into integer. Give an integer value.\n");
			return EXIT_FAILURE;
		}
		if(bucket_count<=0 || bucket_count>PACKAGE_TOTAL_SUM) {
			fprintf(stderr,"---Error (main): Give an integer from 1 to 9.999.\n");
			return EXIT_FAILURE;
		}
	}
	Hashmap* hashmap=create_hashmap((size_t)bucket_count);
	main_processing_function(hashmap);
	free_hashmap(hashmap);
	return 0;
}
