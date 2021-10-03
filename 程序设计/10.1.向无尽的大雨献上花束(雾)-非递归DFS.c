#include <stdio.h>
#include <stdlib.h>

struct node {
	int n;
	struct node* next;
};

struct node* createPoint(int x) {
	struct node* head;
	head = malloc(sizeof(struct node));
	head->next = NULL;
	head->n = x;
	return head;
}

void add(struct node* p, int x) {
	struct node* new;
	new = malloc(sizeof(struct node));
	new->n = x;
	new->next = p->next;
	p->next = new;
}

int main() {
	int N, M, x, y;
	struct node* p[100000] = { 0 }, * d[100000];
	char mark[100001];
	int depth = 0;
	scanf("%d%d", &N, &M);
	for (int i = 1; i <= M; i++) {
		scanf("%d%d", &x, &y);
		if (!p[x]) p[x] = createPoint(x);
		if (!p[y]) p[y] = createPoint(y);
		add(p[x], y);
		add(p[y], x);
	}
	while (1) {
	restart:
		scanf("%d%d", &x, &y);
		if (!x && !y) break;
		if (!p[x] || !p[y]) {
			printf("No\n");
			continue;
		}
		for (int i = 1; i <= N; i++) {
			mark[i] = 0; 
		}
		depth = 0; d[0] = p[x]; mark[x] = 1;
		while (1) {
		recal:
			depth++; d[depth] = p[d[depth - 1]->n];
			for (; depth >= 1; depth--) {
				while (d[depth]->next != NULL) {
					d[depth] = d[depth]->next;
					if (mark[d[depth]->n]) continue;
					mark[d[depth]->n] = 1;
					if (d[depth]->n == y) {
						printf("Yes\n");
						goto restart;
					}
					goto recal;
				}
			}
			printf("No\n");
			break;
		}
	}
}