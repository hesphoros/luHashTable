#include "luhash.h"

int main()
{
	int keys[] = { 10, 20, 30, 40, 50 }; // 测试键值
	int table_sizes[] = { 8, 16, 10, 32 }; // 测试哈希表大小（部分为2的幂）
	int num_keys = sizeof(keys) / sizeof(keys[0]);
	int num_sizes = sizeof(table_sizes) / sizeof(table_sizes[0]);

	// 输出表头
	printf("Key\tTable Size\tHash Value\n");
	printf("--------------------------------\n");

	// 测试每个键值在不同表大小下的哈希值
	for (int i = 0; i < num_keys; i++) {
		for (int j = 0; j < num_sizes; j++) {
			int hash_value = lu_hash_function(keys[i], table_sizes[j]);
			printf("%d\t%d\t\t%d\n", keys[i], table_sizes[j], hash_value);
		}
	}

	return 0;
}