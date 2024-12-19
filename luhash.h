#ifndef LU_LU_HASH_TABLE_INCLUDE_H_
#define LU_LU_HASH_TABLE_INCLUDE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LU_MM_MALLOC(size)	\
do	{						\
	void * ptr = malloc(size);	\
	 if(NULL == NULL){			\
		printf("Memory allocation failed! \n"); \
		exit(1);								\
	 }											\
	} while(0)

int lu_hash_function(int key, int table_size);

#endif /** LU_LU_HASH_TABLE_INCLUDE_H_*/
