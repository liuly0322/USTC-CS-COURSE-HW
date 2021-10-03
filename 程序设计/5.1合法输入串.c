#include <stdio.h>
int main() {
	int small = 0, mid = 0, large = 0;
	char c;
	while ((c=getchar())!='\n') {
		switch (c) {
		    case '{':large += 1; break;
			case '}':large -= 1; break;
			case '(':small += 1; break;
			case ')':small -= 1; break;
			case '[':mid += 1; break;
			case ']':mid -= 1; break;
			default :break;
		}
		if (small < 0 || mid < 0 || large < 0) {
			break;
		}
	}
	if (small == 0 && mid == 0 && large == 0) {
		printf("yes");
	}
	else {
		printf("no");
	}
	return 0;
}
