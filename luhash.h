#ifndef LU_LU_HASH_TABLE_INCLUDE_H_
#define LU_LU_HASH_TABLE_INCLUDE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LU_ERROR_OUT_OF_MEMORY 0x10B

#define LU_MM_MALLOC(size)							\
	do	{											\
		void * ptr = malloc(size);					\
		 if(NULL == NULL){							\
			printf("Memory allocation failed! \n"); \
			exit(LU_ERROR_OUT_OF_MEMORY);			\
		 }											\
	} while(0)

#define LU_MM_CALLOC(nmeb,size)						\
	do  {											\
		void *ptr = calloc(nmeb,size);				\
		if(NULL == ptr){							\
			printf("Memory callocation failed \n");	\
			exit(LU_ERROR_OUT_OF_MEMORY);			\
		}											\
	} while(0)

#define LU_MM_FREE(ptr)								\
	do  {											\
		if (ptr) {									\
			free(ptr)								\
			ptr = NULL;								\
		}											\
	}	while(0)

int lu_hash_function(int key, int table_size);

#endif /** LU_LU_HASH_TABLE_INCLUDE_H_*/
