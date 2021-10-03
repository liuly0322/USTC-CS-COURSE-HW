#include <stdio.h>
#include <string.h>

/*
 * Description: to print a reverse-polish-notation
 *				in a usual way.
 * Parameters:  s is the string of a 
 *				reverse-polish-notation and
 *				s_length is the length of it.
 */
void printRPN(char* s, int s_length) {

	if (s_length == 1) {	//recursion exit
		putchar(s[0]);
		return;
	}

	//divide into 2 parts for an operation
	int count_num = 0, i;
	for (i = s_length - 1; i >= 0; i--) {
		if (s[i] < '0' || s[i] > '9') {
			count_num++;
		} else {
			count_num--;
		}
		if (count_num == 0) {
			break;
		}
	}

	//decide if the first part need parentheses and print it
	if (i==1||
		s[s_length - 1] == '+' || s[s_length - 1] == '-'||
		s[i - 1] == '*' || s[i - 1] == '/') {
		printRPN(s, i);
	} else {
		putchar('(');
		printRPN(s, i);
		putchar(')');
	}

	//print the operator
	putchar(s[s_length - 1]);

	//decide if the second part need parentheses and print it
	if (s_length - i - 1 == 1 ||
		((s[s_length - 1] == '+' || s[s_length - 1] == '-') &&
		(s[s_length - 2] == '*' || s[s_length - 2] == '/'))) {
		printRPN(s + i, s_length - i - 1);
	}
	else {
		putchar('(');
		printRPN(s + i, s_length - i - 1);
		putchar(')');
	}
}

int main() {
	char s[50];
	scanf("%s", s);
	printRPN(s, strlen(s));
}