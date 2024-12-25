#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "luhash.h"

// 定义一个学生结构体
typedef struct {
	int id;         // 学生ID
	char name[50];  // 学生姓名
	int age;        // 学生年龄
	float grade;    // 学生成绩
} student_t;

// 哈希表相关函数声明

// 创建一个虚拟的学生数据
student_t generate_student(int id) {
	student_t s;
	s.id = id;
	snprintf(s.name, sizeof(s.name), "Student%d", id);  // 为每个学生生成唯一名字
	s.age = 18 + (id % 10);  // 假设年龄在18到27之间
	s.grade = 60 + (id % 40);  // 假设成绩在60到100之间
	return s;
}

// 测试函数
void test_hash_table_with_students() {
	int table_size = 10;
	lu_hash_table_t* table = lu_hash_table_init(table_size);
	assert(table != NULL);  // 确保表初始化成功

	// 插入100个学生数据
	for (int i = 1; i <= 100; i++) {
		student_t student = generate_student(i);
		lu_hash_table_insert(table, student.id, &student);
	}

	// 查找并验证学生数据
	for (int i = 1; i <= 100; i++) {
		student_t* found_student = (student_t*)lu_hash_table_find(table, i);
		assert(found_student != NULL);  // 确保学生存在
		assert(found_student->id == i); // 验证ID
		printf("Found student: %d, Name: %s, Age: %d, Grade: %.2f\n",
			found_student->id, found_student->name, found_student->age, found_student->grade);
	}

	// 删除部分学生数据并验证
	for (int i = 1; i <= 50; i++) {
		lu_hash_table_delete(table, i);
		student_t* found_student = (student_t*)lu_hash_table_find(table, i);
		assert(found_student == NULL);  // 确保已删除
	}

	// 检查未删除的学生数据是否仍然存在
	for (int i = 51; i <= 100; i++) {
		student_t* found_student = (student_t*)lu_hash_table_find(table, i);
		assert(found_student != NULL);  // 确保未删除的学生仍然存在
	}

	// 销毁哈希表
	lu_hash_table_destroy(table);

	printf("Student hash table test passed!\n");
}

int main() {
	test_hash_table_with_students(); // 执行测试
	return 0;
}