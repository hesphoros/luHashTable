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

#define LU_MM_CALLOC(nmemb,size)					\
	do  {											\
		void *ptr = calloc(nmemb,size);				\
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

#define MM_FREE(ptr)			LU_MM_FREE(ptr)
#define MM_MALLOC(size)			LU_MM_MALLOC(size)
#define MM_CALOC(nmemb,size)	LU_MM_CALLOC(nmemb,size)

int lu_hash_function(int key, int table_size);

/**Two type of the hash buket (list and red black tree*/
typedef enum lu_hash_bucket_type_u {
	LU_BUCKET_LIST,
	LU_BUCKET_RBTREE,
}lu_hash_bucket_type_t;

#endif /** LU_LU_HASH_TABLE_INCLUDE_H_*/
