#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char eng[11];
	char ale[11];
} ENTRY;
ENTRY word[100000];

int Comp(const void* p1, const void* p2) {
	return strcmp((*(ENTRY*)p1).ale, (*(ENTRY*)p2).ale);
}

int main() {
	int n = 0;
	char s[30];
	for (; fgets(s, 29, stdin) && s[0] != '\n'; n++) {
		sscanf(s, "%s%s", word[n].eng, word[n].ale);
	}
	qsort(word, n, sizeof(ENTRY), Comp);
	ENTRY input, * ans = NULL;
	while (scanf("%s", input.ale) != EOF) {
		if ((ans = bsearch(&input, word, n, sizeof(ENTRY), Comp)) != NULL) {
			printf("%s\n", ans->eng);
		}
		else {
			printf("eh\n");
		}
	}
}