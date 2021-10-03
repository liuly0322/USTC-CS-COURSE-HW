#include <stdio.h>
#include <stdlib.h>

int t = 0;

struct node {
	int n;
	struct node* next;
};
typedef struct node NODE;

NODE* new(int x) {
	NODE* head;
	head = malloc(sizeof(NODE));
	head->next = NULL;
	head->n = x;
	return head;
}

void add(NODE* p, int x) {
	NODE* new;
	new = malloc(sizeof(NODE));
	new->n = x;
	new->next = p->next;
	p->next = new;
}

void search(NODE** p, NODE* pnow, int y, char* mark) {
	NODE* pnext = p[pnow->n]->next;
	if (t) {
		return;
	}
	while (pnext != NULL) {
		if (!mark[pnext->n]) {
			mark[pnext->n] = 1;
			if (pnext->n == y) {
				t = 1;
				break;
			}
			search(p, pnext, y, mark);
		}
		pnext = pnext->next;
	}
}

int main() {
	int N, M, x, y;
	NODE* p[100000] = { 0 };
	char mark[100001];
	scanf("%d%d", &N, &M);
	for (int i = 1; i <= M; i++) {
		scanf("%d%d", &x, &y);
		if (!p[x]) p[x] = new(x);
		if (!p[y]) p[y] = new(y);
		add(p[x], y);
		add(p[y], x);
	}
	while (1) {
		scanf("%d%d", &x, &y);
		if (!x && !y) break;
		if (!p[x] || !p[y]) {
			printf("No\n");
			continue;
		}
		t = 0;
		for (int i = 1; i <= N; i++) {
			mark[i] = 0;
		}
		mark[x] = 1;
		search(p, p[x], y, mark);
		if (t) {
			printf("Yes\n");
			continue;
		}
		printf("No\n");
	}
}