#include <stdio.h>
#include <stdlib.h>

struct student_info {
	struct student_info* next;
	char id[20];
	char name[20];
	char sex;
	int age;
};

struct student_info* create(int length) {
	struct student_info* head, * p;
	head = (struct student_info*)malloc(sizeof(struct student_info));
	p = head;
	head->next = NULL;
	for (int i = 1; i <= length; i++) {
		p->next = (struct student_info*)malloc(sizeof(struct student_info));
		p = p->next;
		if (p == NULL) {
			printf("No enough memory!");
			return NULL;
		}
		scanf("%s", p->id);
		scanf("%s", p->name);
		getchar();
		scanf("%c", &(p->sex));
		scanf("%d", &(p->age));
		p->next = NULL;
	}
	return head;
}

int main() {
	int n, age;
	scanf("%d%d", &n, &age);
	struct student_info* a, * p, * temp;
	a = create(n);
	//去除带有指定年龄的
	p = a;
	while (p != NULL && p->next != NULL) {
		if ((p->next)->age == age) {
			temp = p->next;
			p->next = temp->next;
			free(temp);
		}
		else {
			p = p->next;
		}
	}
	//输出链表 a
	p = a->next;
	if (p == NULL) {
		printf("NULL");
	}
	while (p != NULL) {
		printf("%s %s %c %d\n", p->id, p->name, p->sex, p->age);
		p = p->next;
	}
}