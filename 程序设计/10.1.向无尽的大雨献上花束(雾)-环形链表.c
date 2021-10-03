#include <stdio.h>
#include <stdlib.h>

struct node {
	int num;
	struct node* before, * after;
};

inline int read()
{
	register char ch = getchar();
	register int x = 0;
	while (ch < '0' || ch>'9')ch = getchar();
	while (ch >= '0' && ch <= '9')x = x * 10 + ch - 48, ch = getchar();
	return x;
}

inline struct node* createRing(int x, int y) {
	struct node* head, * p;
	head = malloc(sizeof(struct node));
	head->num = x;
	head->after = malloc(sizeof(struct node));
	head->before = head->after;
	p = head->after;
	p->before = head; p->after = head; p->num = y;
	return head;
}

void add(struct node* ring, int x) {
	struct node* new;
	new = malloc(sizeof(struct node));
	new->num = x; new->before = ring; new->after = ring->after;
	(ring->after)->before = new; ring->after = new;
}

void combine(struct node* ring1, struct node* ring2) {
	(ring1->after)->before = ring2->before;
	(ring2->before)->after = ring1->after;
	ring1->after = ring2; ring2->before = ring1;
}

inline int search(struct node* ring, int n) {
	int start = ring->num;
	int i = start;
	do {
		if (n == i) {
			return 1;
		}
		ring = ring->after;
		i = ring->num;
	} while (i!=start);
	return 0;
}

int main() {
	int numsofRings = 0, M, x, y, xinRing, yinRing;
	struct node* ring[10000];
	char isinRing[100001] = { 0 };
	M = read(); M = read();
	for (int i = 1; i <= M; i++) {
		x = read(); y = read();
		isinRing[x] = 1; isinRing[y] = 1;
		xinRing = 0; yinRing = 0;
		if (isinRing[x] == 1) {
			for (int j = 1; j <= numsofRings; j++) {
				if (search(ring[j], x)) {
					xinRing = j; break;
				}
			}
		}
		if (isinRing[y] == 1) {
			for (int j = 1; j <= numsofRings; j++) {
				if (search(ring[j], y)) {
					yinRing = j; break;
				}
			}
		}
		if ((!xinRing) || (!yinRing)) {
			if ((!xinRing) && (!yinRing)) {
				numsofRings++;
				ring[numsofRings] = createRing(x, y);
			}
			else {
				if (xinRing) {
					add(ring[xinRing], y);
				}
				if (yinRing) {
					add(ring[yinRing], x);
				}
			}
		}
		else {
			if (xinRing != yinRing) {
				if (xinRing > yinRing) {
					int temp = xinRing;
					xinRing = yinRing;
					yinRing = temp;
				}
				combine(ring[xinRing], ring[yinRing]);
				ring[yinRing] = ring[numsofRings];
				numsofRings--;
			}
		}
	}
	while (1) {
		x = read(); y = read();
		if (!x) break;
		if (isinRing[x] == 0 || isinRing[y] == 0) {
			printf("No\n");
			continue;
		}
		xinRing = 0; yinRing = 0;
		for (int j = 1; j <= numsofRings; j++) {
			if (search(ring[j], x)) {
				xinRing = j; break;
			}
		}
		for (int j = 1; j <= numsofRings; j++) {
			if (search(ring[j], y)) {
				yinRing = j; break;
			}
		}
		if (xinRing && xinRing == yinRing) {
			printf("Yes\n");
		}
		else {
			printf("No\n");
		}
	}
}