#ifndef LU_LU_HASH_TABLE_INCLUDE_H_
#define LU_LU_HASH_TABLE_INCLUDE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int lu_hash_erron_global_ = 0;

#define LU_ERROR_OUT_OF_MEMORY			0x10B	 // Error code for hash table memory allocation
#define LU_HASH_TABLE_DEFAULT_SIZE		16		 // Default size for hash tables
#define LU_HASH_TABLE_MAX_LOAD_FACTOR	0.75	 // Maximum allowed load factor

/**
 * Threshold for converting a hash bucket from a linked list to a red-black tree.
 * If the number of elements in a bucket exceeds this threshold, the bucket will
 * be converted to a red-black tree to improve performance for operations such
 * as search, insert, and delete.
 *
 * Recommended default value is 8, which balances memory usage and performance.
 * A smaller value may trigger earlier tree conversion, increasing memory overhead,
 * while a larger value may result in longer search times for highly populated buckets.
 */
#define LU_HASH_BUCKET_LIST_THRESHOLD 8

#define LU_MM_MALLOC(size)									\
	do	{													\
		void * ptr = malloc(size);							\
		 if(NULL == NULL){									\
			printf("Memory allocation failed! \n");			\
			lu_hash_erron_global_ = LU_ERROR_OUT_OF_MEMORY;	\
			exit(LU_ERROR_OUT_OF_MEMORY);					\
		 }													\
	} while(0)

#define LU_MM_CALLOC(nmemb,size)							\
	do  {													\
		void *ptr = calloc(nmemb,size);						\
		if(NULL == ptr){									\
			printf("Memory callocation failed \n");			\
			lu_hash_erron_global_ = LU_ERROR_OUT_OF_MEMORY;	\
			exit(LU_ERROR_OUT_OF_MEMORY);					\
		}													\
	} while(0)

#define LU_MM_FREE(ptr)										\
	do  {													\
		if (ptr) {											\
			free(ptr)										\
			ptr = NULL;										\
		}													\
	}	while(0)

#define MM_FREE(ptr)			LU_MM_FREE(ptr)
#define MM_MALLOC(size)			LU_MM_MALLOC(size)
#define MM_CALOC(nmemb,size)	LU_MM_CALLOC(nmemb,size)

 /** Two types of hash buckets: linked list and red-black tree */
typedef enum lu_hash_bucket_type_u {
	LU_HASH_BUCKET_LIST,	// Bucket implemented as a linked list
	LU_HASH_BUCKET_RBTREE,	// Bucket implemented as a red-black tree
}lu_hash_bucket_type_t;

/**
 * Structure representing a node in a hash bucket
 */
typedef struct lu_hash_bucket_node_s {
	struct lu_hash_bucket_node_s* next; // Pointer to the next node in the bucket
	int key;                            // Key of the node
	void* value;                        // Pointer to the value associated with the key
} lu_hash_bucket_node_t;

/*** Pointer to a hash bucket node.*/
typedef lu_hash_bucket_node_t* lu_hash_bucket_node_ptr_t;

/**
 * Enum representing the color of a red-black tree's node.
 */
typedef enum lu_node_color_u {
	RED,
	BLACK
} lu_node_color_t, lu_rb_tree_color_t;

/**
 * Structure representing a node in a red-black tree.
 */
typedef struct lu_rb_tree_node_s {
	struct lu_rb_tree_node_s* left;
	struct lu_rb_tree_node_s* right;
	struct lu_rb_tree_node_s* parent;
	lu_node_color_t			  color;
	int						  key;
	int						  value;//Value associated with the key
}lu_rb_tree_node_t;

/**
 * Structure representing a red-black tree.
 */
typedef struct lu_rb_tree_s {
	lu_rb_tree_node_t* root;// Pointer to the root node of the red-black tree
	lu_rb_tree_node_t* nil;	// Sentinel node representing "null"
	size_t             size_tree;// Number of nodes in the tree
}lu_rb_tree_t;

/**
 * Structure representing a hash bucket.
 * A hash bucket can be implemented as either a linked list or a red-black tree,
 * depending on the `bucket_type` field.
 */
typedef struct lu_hash_bucket_s {
	lu_hash_bucket_type_t bucket_type; // The type of bucket (linked list or red-black tree)

	// Union to store the bucket data, based on the bucket type
	union
	{
		lu_hash_bucket_node_ptr_t	list_head; // Pointer to the head of the linked list (if bucket_type is list)
		lu_rb_tree_t* rb_tree;   // Pointer to the red-black tree (if bucket_type is rb_tree)
	}data;

	size_t size_bucket; // Number of elements in the bucket
}lu_hash_bucket_t;

int lu_hash_function(int key, int table_size);

#endif /** LU_LU_HASH_TABLE_INCLUDE_H_*/
