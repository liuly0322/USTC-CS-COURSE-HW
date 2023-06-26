#include <stdio.h>
char* insert(char* string, char c) {
	int i = 0;
	for (; string[i] != 0; i++) {
		if (string[i] > c) {
			break;
		}
	}
	//c 应该放在 i 位
	for (int j = 50; j >= i; j--) {
		string[j + 1] = string[j];
	}
	string[i] = c;
	return string;
}
int main() {
	char string[52], c;
	char* afterInsert;
	int i = 0;
	while ((string[i] = getchar()) != '\n') {
		i++;
	}
	string[i] = 0;
	c = getchar();
	afterInsert = insert(string, c);
	printf("%s", afterInsert);
}