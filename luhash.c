#include "luhash.h"

static int			  lu_convert_bucket_to_rbtree(lu_hash_bucket_t* bucket);
static lu_rb_tree_t* lu_rb_tree_init();
static void			  lu_rb_tree_insert(lu_rb_tree_t* tree, int key, void* value);

static void lu_rb_tree_insert_fixup(lu_rb_tree_t* tree, lu_rb_tree_node_t* node);

/**
 * @brief Computes a hash value for a given key using the multiplication method.
 *
 * This function implements a hash function based on the multiplication method,
 * using the fractional part of the product between the key and the constant
 * A (the reciprocal of the golden ratio). If the table size is a power of two,
 * the modulo operation is optimized using bitwise operations. Otherwise, a
 * standard modulo operation is applied.
 *
 * @param key The integer key to be hashed.
 * @param table_size The size of the hash table (number of buckets).
 * @return The computed hash value, ranging from 0 to table_size - 1.
 */
int lu_hash_function(int key, int table_size)
{
	static const float golden_rate_reciprocal = 0.6180339887; // Reciprocal of the golden ratio

	double temp = key * golden_rate_reciprocal;
	double fractional_part = temp - (int)temp; // Extract fractional part
	int hash = (int)(table_size * fractional_part);

	// Optimize modulo operation if table_size is a power of two
	if ((table_size & (table_size - 1)) == 0) {
		return hash & (table_size - 1); // Use bitwise AND for power-of-two table sizes
	}

	// Fallback to standard modulo operation
	return hash % table_size;
}

/**
 * Initializes a hash table with the specified number of buckets.
 * If the specified `table_size` is invalid (less than or equal to 0),
 * a default size is used instead (`LU_HASH_TABLE_DEFAULT_SIZE`).
 *
 * Memory is allocated for the hash table structure and the buckets.
 * Each bucket is initialized as a linked list by default.
 *
 * @param table_size The number of buckets in the hash table. If <= 0, default size is used.
 * @return A pointer to the newly initialized hash table, or exits the program if memory allocation fails.
 *
 * Usage example:
 *     lu_hash_table_t* hash_table = lu_hash_table_init(16); // Creates a hash table with 16 buckets.
 */
static lu_hash_table_t* lu_hash_table_init(int table_size)
{
	if (table_size <= 0) {
		table_size = LU_HASH_TABLE_DEFAULT_SIZE;
	}
	lu_hash_table_t* table = (lu_hash_table_t*)LU_MM_MALLOC(sizeof(lu_hash_table_t));
	table->element_count = 0;
	table->buckets = (lu_hash_bucket_t*)LU_MM_CALLOC(table_size, sizeof(lu_hash_bucket_t));
	table->table_size = table_size;

	for (size_t i = 0; i < table_size; i++) {
		table->buckets[i].type = LU_HASH_BUCKET_LIST;
		table->buckets[i].data.list_head = NULL;
	}

	return table;
}

/**
 * Inserts a key-value pair into the hash table.
 *
 * This function calculates the bucket index using the provided key and inserts
 * the key-value pair into the corresponding bucket. If the bucket is implemented
 * as a linked list, it checks for existing keys and updates their values if found;
 * otherwise, it creates a new node and inserts it at the head of the list.
 *
 * @param table A pointer to the hash table.
 * @param key   The key to be inserted or updated in the hash table.
 * @param value A pointer to the value associated with the key.
 *
 * Usage:
 *     lu_hash_table_insert(hash_table, 42, value_ptr);
 */

void lu_hash_table_insert(lu_hash_table_t* table, int key, void* value)
{
	int index = lu_hash_function(key, table->table_size);
	lu_hash_bucket_t* bucket = &table->buckets[index];
	if (LU_HASH_BUCKET_LIST == bucket->type) {
		lu_hash_bucket_node_ptr_t new_node = (lu_hash_bucket_node_ptr_t)LU_MM_MALLOC(sizeof(lu_hash_bucket_node_t));
		if (!new_node) {
#ifdef LU_HASH_DEBUG
			printf("Memory allocation  failed for new code\n");
#endif // LU_HASH_DEBUG
			return;
		}

		// Check if the key already exists and update the value
		lu_hash_bucket_node_t* current = bucket->data.list_head;
		while (current) {
			if (current->key == key) {
				current->value = value; // Update value if key exists
				return;
			}
			current = current->next;
		}

		// Assign the value to the new node
		new_node->value = value;

		// Assign the key to the new nod
		new_node->key = key;

		// Link the new node to the existing linked list
		new_node->next = bucket->data.list_head;

		// Update the head of the linked list to the new node
		bucket->data.list_head = new_node;

		// Increment the global element count and local bucket size
		table->element_count++;
		bucket->esize_bucket++;

		// Check if the bucket's linked list length exceeds the threshold
		if (bucket->esize_bucket > LU_HASH_BUCKET_LIST_THRESHOLD) {
#ifdef LU_HASH_DEBUG
			printf("Bucket size exceeded threshold. Converting to red-black tree...\n");
#endif // LU_HASH_DEBUG
			//Convert the internal structure of bucket from list to rb_tree
			if (lu_convert_bucket_to_rbtree(bucket) != 0) {
#ifdef LU_HASH_DEBUG
				printf("Error: Failed to convert bucket to red-black tree.\n");
#endif // LU_HASH_DEBUG
			}
		}
	}
	else if (LU_HASH_BUCKET_RBTREE == bucket->type) {
		/**Insert into the red-black tree*/

		//Check the rb_tree and nil
		if (NULL == bucket->data.rb_tree || NULL == bucket->data.rb_tree->nil) {
#ifdef LU_HASH_DEBUG
			printf("Inserting key %d into red-black tree \n", key);
			printf("Error: RB-tree or tree->nil is not initialized\n");
#endif // LU_HASH_DEBUG
			lu_hash_erron_global_ = LU_ERROR_TREE_OR_NIL_NOT_INIT;
			return;
		}

		lu_rb_tree_insert(bucket->data.rb_tree, key, value);
		bucket->esize_bucket++;
		table->element_count++;
	}
}

/**
 * Converts a hash bucket's linked list to a red-black tree.
 *
 * This function takes a hash bucket that is implemented as a linked list,
 * initializes a new red-black tree, and transfers all elements from the linked
 * list to the red-black tree. Once the transfer is complete, it updates the bucket
 * to use the red-black tree as its underlying data structure.
 *
 * @param bucket Pointer to the hash bucket to be converted.
 * @return 0 on success, -1 on failure (e.g., memory allocation error or invalid bucket).
 */
static int lu_convert_bucket_to_rbtree(lu_hash_bucket_t* bucket)
{
	// Check if the bucket is valid and of the correct type
	if (!bucket || bucket->type != LU_HASH_BUCKET_LIST) {
#ifdef LU_HASH_DEBUG
		printf("Error: Invalid bucket or bucket is not a linked list.\n");
#endif //LU_HASH_DEBUG
		return -1; // Return error if the bucket is invalid or not a linked list
	}

	// Initialize the new red-black tree
	lu_rb_tree_t* new_tree = lu_rb_tree_init();
	if (!new_tree) {
#ifdef LU_HASH_DEBUG
		printf("Error: Memory allocation failed for red-black tree.\n");
#endif //LU_HASH_DEBUG
		return -1;// Return error if memory allocation for the red-black tree fails
	}

	// Transfer elements from the linked list to the red-black tree
	lu_hash_bucket_node_ptr_t node = bucket->data.list_head;// Start with the head of the list

	// Transfer all elements from the linked list to the red-black tree
	while (node)
	{
		lu_rb_tree_insert(new_tree, node->key, node->value); // Insert key-value pair into the red-black tree
		lu_hash_bucket_node_ptr_t temp = node; // Save current node pointer
		node = node->next; // Move to the next node
		free(temp); // Free the memory of the linked list node
	}

	// Update the bucket to use the red-black tree
	bucket->data.list_head = NULL;			// Clear the linked list head
	bucket->type = LU_HASH_BUCKET_RBTREE;	// Update the bucket type
	bucket->data.rb_tree = new_tree;		// Point to the new red-black tree
#ifdef LU_HASH_DEBUG
	printf("Bucket(list chain) successfully converted to red-black tree.\n");
#endif
	return 0; // Indicate successful conversion
}

/**
 * Initializes a red-black tree.
 * Allocates memory for the tree and its sentinel node (`nil`), and sets
 * up the tree structure with `nil` as its root and children.
 *
 * @return Pointer to the initialized red-black tree, or exits the program if memory allocation fails.
 */
static lu_rb_tree_t* lu_rb_tree_init()
{
	lu_rb_tree_t* rb_tree = (lu_rb_tree_t*)LU_MM_MALLOC(sizeof(lu_rb_tree_t));
	if (NULL == rb_tree) {
#ifdef LU_HASH_DEBUG
		printf("Error ops! rb_tree is NULL in lu_rb_tree_init function\n");
#endif // LU_HASH_DEBUG
		lu_hash_erron_global_ = LU_ERROR_OUT_OF_MEMORY;
		exit(lu_hash_erron_global_);
	}

	// Allocate memory for the sentinel node (`nil`)
	rb_tree->nil = (lu_rb_tree_node_t*)LU_MM_MALLOC(sizeof(lu_rb_tree_node_t));
	if (NULL == rb_tree->nil) {
#ifdef LU_HASH_DEBUG
		printf("Error ops! rb_tree->nil is NULL in lu_rb_tree_init function\n");
#endif // LU_HASH_DEBUG
		free(rb_tree);
		lu_hash_erron_global_ = LU_ERROR_OUT_OF_MEMORY;
		exit(lu_hash_erron_global_);
	}

	/**Initialize the `nil` sentinel node*/
	// Sentinel node is always black
	rb_tree->nil->color = BLACK;
	rb_tree->nil->left = rb_tree->nil;
	rb_tree->nil->right = rb_tree->nil;
	rb_tree->nil->parent = rb_tree->nil;
	// Set the root to point to `nil`
	rb_tree->root = rb_tree->nil;

#ifdef LU_HASH_DEBUG
	printf("Red-black tree initialized successfully. Root: %p, Nil: %p\n", rb_tree->root, rb_tree->nil);
#endif // LU_HASH_DEBUG

	return rb_tree;
}

/**
 * @brief Inserts a new node into the red-black tree with the specified key and value.
 *
 * This function inserts a new node into the red-black tree, maintaining the properties of the
 * red-black tree. If the tree or its nil sentinel node is uninitialized, or if memory allocation
 * for the new node fails, the function will exit early with an error message (if debugging is enabled).
 *
 * @param tree Pointer to the red-black tree.
 * @param key The key for the new node.
 * @param value The value associated with the key in the new node.
 */
static void lu_rb_tree_insert(lu_rb_tree_t* tree, int key, void* value)
{
	if (NULL == tree || NULL == tree->nil) {
#ifdef LU_HASH_DEBUG
		printf("Error: RB-tree or tree->nil is not initialized\n");
#endif // LU_HASH_DEBUG
		lu_hash_erron_global_ = LU_ERROR_TREE_OR_NIL_NOT_INIT;
		return;
	}
	lu_rb_tree_node_t* new_node = (lu_rb_tree_node_t*)LU_MM_MALLOC(sizeof(lu_rb_tree_node_t));
	if (NULL == new_node) {
#ifdef LU_HASH_DEBUG
		printf("Error: Memory allocation failed in not initialized!(lu_rb_tree_node_t)\n");
#endif // LU_HASH_DEBUG
		return;
	}

	// Initialize the new node with the given key and value.
	new_node->key = key;
	new_node->value = value;
	new_node->color = RED; // New nodes are always inserted as RED.
	new_node->left = tree->nil;// Left child is set to the nil sentinel
	new_node->right = tree->nil;// Right child is set to the nil sentinel.
	new_node->parent = tree->nil; // Parent is set to the nil sentinel.

	if (tree->root == tree->nil) {
		// Case 1:The tree is empty, so the new node becomes the root.
		tree->root = new_node;
		new_node->color = BLACK; // Root is always black.
	}
	else {
		// Case 2: Find the correct position for the new node.
		lu_rb_tree_node_t* parent = tree->root;// Pointer to track the parent of the new node.
		lu_rb_tree_node_t* current = tree->root;// Pointer to traverse the tree.

		// Traverse the tree to find the insertion point.
		while (current != tree->nil) {
			parent = current;
			if (key < current->key) {
				current = current->left;
			}
			else {
				current = current->right;
			}
		}

		// Set the parent of the new node and attach it as a child of the parent.
		new_node->parent = parent;
		if (key < parent->key) {
			parent->left = new_node;
		}
		else {
			parent->right = new_node;
		}
		// Sanity check: Ensure new node and tree are valid before fixing violations.
		if (new_node == NULL || tree == NULL) {
#ifdef LU_HASH_DEBUG
			printf("Error: Invalid node or tree during fixup\n");
#endif
			return;
		}

		// Fix any violations of the red-black tree properties.
		lu_rb_tree_insert_fixup(tree, new_node);
	}
}

/**
 * @brief Fixes violations of red-black tree properties after an insertion.
 *
 * This function ensures that the red-black tree properties are restored after
 * a node is inserted. It resolves cases where the tree may temporarily violate
 * properties such as the double-red rule (a red node cannot have a red parent).
 *
 * @param tree Pointer to the red-black tree.
 * @param node Pointer to the newly inserted node.
 */
void lu_rb_tree_insert_fixup(lu_rb_tree_t* tree, lu_rb_tree_node_t* node)
{
	// While the current node is not the root and its parent is red
	while (node != tree->root && node->parent != tree->nil && node->parent->color == RED) {
		// Ensure node->parent is not nil and has a parent
		lu_rb_tree_node_t* parent = node->parent; // Parent of the current node
		lu_rb_tree_node_t* grandparent = parent->parent; // Grandparent of the current node
		if (grandparent == NULL) {
			break;
		}
		// Case 1: Parent is the left child of the grandparent
		if (parent == grandparent->left) {
			// Uncle is the right child of the grandparent
			lu_rb_tree_node_t* uncle = grandparent->right;

			// Case 1.1: Uncle is RED
			if (uncle != tree->nil && uncle != NULL && uncle->color == RED) {
				// Case 1: Uncle is RED, recoloring required
				parent->color = BLACK;
				uncle->color = BLACK;
				grandparent->color = RED;
				node = grandparent; // Move up to the grandparent for further checks
			}
			else {
				// Uncle is BLACK or NULL, perform rotations
				if (node == parent->right) {
					node = parent;// Move node up to parent
					lu_rb_tree_left_rotate(tree, node); // Ensure node is valid
				}
				parent->color = BLACK;
				grandparent->color = RED;
				lu_rb_tree_right_rotate(tree, grandparent);
			}
		}
		else {
			// Case 2: Parent is the right child of the grandparent
#ifdef LU_HASH_DEBUG
			printf("Parent: %p, Grandparent: %p, Uncle: %p\n", parent, grandparent, grandparent->left);
#endif // LU_HASH_DEBUG

			// Symmetric case: node->parent is the right child of the grandparent
			lu_rb_tree_node_t* uncle = grandparent->left; // Uncle is the left child of the grandparent

			// Check if uncle is valid and not nil       // Case 2.1: Uncle is RED
			if (uncle != tree->nil && uncle != NULL && uncle->color == RED) {
				// Case 1: Uncle is RED, recoloring required
				parent->color = BLACK;
				uncle->color = BLACK;
				grandparent->color = RED;
				node = grandparent; // Move the node up
			}
			else {
				// Uncle is BLACK or NULL, perform rotations
				if (node == parent->left) {
					node = parent;
					lu_rb_tree_right_rotate(tree, node); // Ensure node is valid
				}
				parent->color = BLACK;
				grandparent->color = RED;
				lu_rb_tree_left_rotate(tree, grandparent); // Perform right rotation on parent
			}
		}
	}
	// Ensure the root is always black, as required by red-black tree properties
	tree->root->color = BLACK; // Ensure root is always black
}