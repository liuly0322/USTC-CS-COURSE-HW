#include <stdio.h>
#include <stdlib.h>

struct node {
	struct node* next;
	int num;
};

struct node* create(int length) {								//��������Ϊn������
	struct node* head, * p;
	head = (struct node*)malloc(sizeof(struct node));
	p = head;
	for (int i = 1; i <= length; i++) {
		p->next = (struct node*)malloc(sizeof(struct node));
		p = p->next;
		if (p == NULL) {
			printf("No enough memory!");
			return NULL;
		}
		scanf("%d", &(p->num));
		p->next = NULL;
	}
	return head;
}

void del(struct node* a, int num) {								//ɾ������ָ��ֵ������
	struct node* temp;
	while (a != NULL && a->next != NULL) {
		if ((a->next)->num == num) {
			temp = a->next;
			a->next = temp->next;
			free(temp);
		}
		else {
			a = a->next;
		}
	}
}

void print(struct node* a) {									//�������
	a = a->next;
	if (a == NULL) {
		printf("NULL");
	}
	while (a != NULL) {
		printf("%d ", a->num);
		a = a->next;
	}
}

int main() {
	int n, num;
	scanf("%d%d", &n, &num);
	struct node* a = create(n);
	del(a, num);
	print(a);
}