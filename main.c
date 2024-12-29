#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "luhash.h"
#include <Windows.h>

#define LU_HASH_DEBUG

// 定义包含姓名、ID号、性别等信息的结构体
typedef struct {
	char name[50];       // 姓名
	int id;              // ID号
	char gender;         // 性别 ('M' = 男, 'F' = 女)
} Person;

// 定义一个包含 `lu_hash_table_handle_t` 的结构体
typedef struct {
	lu_hash_table_t* hash_table;  // 哈希表句柄，用于存储 Person 数据
} PersonDatabase;

// 创建并初始化 Person 实例并插入哈希表
void add_person_to_db(PersonDatabase* db, Person* person) {
	// 在哈希表中插入 Person 数据，使用 id 作为 key
	lu_hash_table_insert(db->hash_table, person->id, person);
}

// 查找指定 ID 的 Person 数据
Person* find_person_by_id(PersonDatabase* db, int id) {
	return (Person*)lu_hash_table_find(db->hash_table, id);
}

// 销毁数据库并释放内存
void destroy_person_db(PersonDatabase* db) {
	lu_hash_table_destroy(db->hash_table);
}

void test_hash() {
	// 初始化哈希表数据库
	PersonDatabase db;
	db.hash_table = lu_hash_table_init(8);
	Person people[100] = {
		   {"Zhang San", 1001, 'M'}, {"Li Si", 1002, 'F'}, {"Wang Wu", 1003, 'M'},
		   {"Zhao Liu", 1004, 'F'}, {"Sun Qi", 1005, 'M'}, {"Zhou Ba", 1006, 'F'},
		   {"Wu Jiu", 1007, 'M'}, {"Zheng Shi", 1008, 'F'}, {"Qian Shiyi", 1009, 'M'},
		   {"Liu Shier", 1010, 'F'}, {"Chen Shisan", 1011, 'M'}, {"Yang Shisi", 1012, 'F'},
		   {"Huang Shiwu", 1013, 'M'}, {"Wu Shiliu", 1014, 'F'}, {"Shen Shiqi", 1015, 'M'},
		   {"Jiang Shiba", 1016, 'F'}, {"Wei Shijiu", 1017, 'M'}, {"Song Ershi", 1018, 'F'},
		   {"Zhang Ershi", 1019, 'M'}, {"Wang Ershi Er", 1020, 'F'}, {"Li Ershi San", 1021, 'M'},
		   {"Zhou Ershi Si", 1022, 'F'}, {"Zheng Ershi Wu", 1023, 'M'}, {"Zhao Ershi Liu", 1024, 'F'},
		   {"Qian Ershi Qi", 1025, 'M'}, {"Sun Ershi Ba", 1026, 'F'}, {"Chen Ershi Jiu", 1027, 'M'},
		   {"Liu San Shi", 1028, 'F'}, {"Jiang San Shi Yi", 1029, 'M'}, {"Wei San Shi Er", 1030, 'F'},
		   {"Yang San Shi San", 1031, 'M'}, {"Huang San Shi Si", 1032, 'F'}, {"Shen San Shi Wu", 1033, 'M'},
		   {"Song San Shi Liu", 1034, 'F'}, {"Zhu San Shi Qi", 1035, 'M'}, {"Wang San Shi Ba", 1036, 'F'},
		   {"Li San Shi Jiu", 1037, 'M'}, {"Zhou Si Shi", 1038, 'F'}, {"Zhao Si Shi Yi", 1039, 'M'},
		   {"Sun Si Shi Er", 1040, 'F'}, {"Chen Si Shi San", 1041, 'M'}, {"Liu Si Shi Si", 1042, 'F'},
		   {"Jiang Si Shi Wu", 1043, 'M'}, {"Wei Si Shi Liu", 1044, 'F'}, {"Yang Si Shi Qi", 1045, 'M'},
		   {"Huang Si Shi Ba", 1046, 'F'}, {"Shen Si Shi Jiu", 1047, 'M'}, {"Song Wu Shi", 1048, 'F'},
		   {"Zhang Wu Shi Yi", 1049, 'M'}, {"Wang Wu Shi Er", 1050, 'F'}, {"Li Wu Shi San", 1051, 'M'},
		   {"Zhou Wu Shi Si", 1052, 'F'}, {"Zheng Wu Shi Wu", 1053, 'M'}, {"Zhao Wu Shi Liu", 1054, 'F'},
		   {"Qian Wu Shi Qi", 1055, 'M'}, {"Sun Wu Shi Ba", 1056, 'F'}, {"Chen Wu Shi Jiu", 1057, 'M'},
		   {"Liu Liu Shi", 1058, 'F'}, {"Jiang Liu Shi Yi", 1059, 'M'}, {"Wei Liu Shi Er", 1060, 'F'},
		   {"Yang Liu Shi San", 1061, 'M'}, {"Huang Liu Shi Si", 1062, 'F'}, {"Shen Liu Shi Wu", 1063, 'M'},
		   {"Song Liu Shi Liu", 1064, 'F'}, {"Zhang Liu Shi Qi", 1065, 'M'}, {"Wang Liu Shi Ba", 1066, 'F'},
		   {"Li Liu Shi Jiu", 1067, 'M'}, {"Zhou Qi Shi", 1068, 'F'}, {"Zhao Qi Shi Yi", 1069, 'M'},
		   {"Sun Qi Shi Er", 1070, 'F'}, {"Chen Qi Shi San", 1071, 'M'}, {"Liu Qi Shi Si", 1072, 'F'},
		   {"Jiang Qi Shi Wu", 1073, 'M'}, {"Wei Qi Shi Liu", 1074, 'F'}, {"Yang Qi Shi Qi", 1075, 'M'},
		   {"Huang Qi Shi Ba", 1076, 'F'}, {"Shen Qi Shi Jiu", 1077, 'M'}, {"Song Ba Shi", 1078, 'F'},
		   {"Zhang Ba Shi Yi", 1079, 'M'}, {"Wang Ba Shi Er", 1080, 'F'}, {"Li Ba Shi San", 1081, 'M'},
		   {"Zhou Ba Shi Si", 1082, 'F'}, {"Zheng Ba Shi Wu", 1083, 'M'}, {"Zhao Ba Shi Liu", 1084, 'F'},
		   {"Qian Ba Shi Qi", 1085, 'M'}, {"Sun Ba Shi Ba", 1086, 'F'}, {"Chen Ba Shi Jiu", 1087, 'M'},
		   {"Liu Jiu Shi", 1088, 'F'}, {"Jiang Jiu Shi Yi", 1089, 'M'}, {"Wei Jiu Shi Er", 1090, 'F'},
		   {"Yang Jiu Shi San", 1091, 'M'}, {"Huang Jiu Shi Si", 1092, 'F'}, {"Shen Jiu Shi Wu", 1093, 'M'},
		   {"Song Jiu Shi Liu", 1094, 'F'}, {"Zhang Jiu Shi Qi", 1095, 'M'}, {"Wang Jiu Shi Ba", 1096, 'F'},
		   {"Li Jiu Shi Jiu", 1097, 'M'}, {"Zhou Yi Bai", 1098, 'F'}, {"Zheng Yi Bai Ling Yi", 1099, 'M'},
		   {"Zhao Yi Bai Ling Er", 1100, 'F'}
	};
	// 将 Person 数据添加到哈希表中
	for (int i = 0; i < 100; ++i) {
		add_person_to_db(&db, &people[i]);
	}

	//// 查找指定 ID 的 Person 数据
	//for (int i = 1001; i <= 1100; ++i) {
	//	Person* found_person = (Person*)find_person_by_id(&db, i);
	//	if (found_person) {
	//		printf("Found person: Name: %s, ID: %d, Gender: %c\n", found_person->name, found_person->id, found_person->gender);
	//	}
	//	else {
	//		printf("Person with ID %d not found\n", i);
	//	}
	//}
	printf("Statr to del person in hash_table\n");
	//// 删除指定 ID 的 Person 数据
	//for (int i = 1001; i <= 1100; ++i) {
	//	lu_hash_table_delete(db.hash_table, i);
	//}
	lu_hash_table_delete(db.hash_table, 1004);
	lu_hash_table_delete(db.hash_table, 1005);
	for (int i = 1001; i <= 1100; ++i) {
		Person* found_person = (Person*)find_person_by_id(&db, i);
		if (found_person) {
			printf("Found person: Name: %s, ID: %d, Gender: %c\n", found_person->name, found_person->id, found_person->gender);
		}
		else {
			printf("Person with ID %d not found\n", i);
		}
	}

	printf("Delete elements finished\n");
	int found_count = 0;
	for (int i = 1001; i <= 1100; ++i) {
		Person* found_person = (Person*)find_person_by_id(&db, i);
		if (found_person) {
			found_count++;
		}
	}
	printf("Find person after del operation is : %d", found_count);
	// 销毁哈希表数据库，释放内存
	destroy_person_db(&db);
}

int main() {
	//system("chcp 65001");

	test_hash();
	return 0;
}