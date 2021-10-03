#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct student_info {
	struct student_info* next;
	char name[20];
	char id[20];
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
		scanf("%s", p->name);
		scanf("%s", p->id);
		p->next = NULL;
	}
	return head;
}

int main() {
	int n, m;
	scanf("%d%d", &n, &m);
	//加下来分别创建两个链表
	struct student_info* a, * b, * p, * q, * temp;
	a = create(n);
	b = create(m);
	//从链表a中删除链表b
	p = a;
	int del_flag = 0;
	while (p != NULL && p->next != NULL) {
		q = b;
		del_flag = 0;
		for (int i = 1; i <= m; i++) {
			if (strcmp((p->next)->id, (q->next)->id) == 0) {
				temp = p->next;
				p->next = temp->next;
				free(temp);
				del_flag = 1;
				break;
			}
			q = q->next;
		}
		if (del_flag == 0) {
			p = p->next;
		}
	}
	//输出链表a
	p = a->next;
	if (p == NULL) {
		printf("NULL");
	}
	while (p != NULL) {
		printf("%s %s\n", p->name, p->id);
		p = p->next;
	}
}