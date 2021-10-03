#include <stdio.h>
#define MAX_BUFFER 100
#define MAX_DIGIT_NUM 100

typedef struct {
	/*
	* to describe a digit number
	*/
	char num[MAX_DIGIT_NUM];
	int digit;
} NUMBER;

/*
* function: multiple num2 to num1.
*/
inline void multiple(NUMBER* num1, NUMBER* num2) {
	int temp[MAX_DIGIT_NUM] = { 0 };
	int now_length;
	char* p1 = num1->num, * p2 = num2->num;
	//calculate
	for (int i = 0; i < num1->digit; i++) {
		for (int j = 0; j < num2->digit; j++) {
			temp[i + j] += p1[i] * p2[j];
		}
	}
	//carry numbers
	for (now_length = 0; now_length < num1->digit + num2->digit - 1
		|| temp[now_length] != 0; now_length++) {
		temp[now_length + 1] += temp[now_length] / 10;
		temp[now_length] = temp[now_length] % 10;
	}
	//copy to num1
	for (int i = 0; i < now_length; i++) {
		p1[i] = temp[i];
	}
	num1->digit = now_length;
}

/*
* function: to pow base times,
*			and copy answer to ans.
*/
inline void qpow(NUMBER* base, int times, NUMBER* ans) {
	ans->digit = 1;
	ans->num[0] = 1;
	while (times != 0) {
		if (times % 2) {
			multiple(ans, base);
		}
		multiple(base, base);
		times /= 2;
	}
}

/*
* function: read the input.
* the last and meaningless '0' will be ignored.
* return value:how many numbers are after the dot.
*/
inline int read(char* str_in, NUMBER* base, int n) {
	int point_posi = 0, i = 0, digit = 0;
	char* p = base->num;
	while (*str_in++) {
		i++;
	}
	str_in -= (i + 1);
	//delete the last and meaningless '0'.
	for (i--; i >= 0; i--) {
		if (str_in[i] != '0') {
			break;
		}
	}
	//read numbers after the dot.
	for (; i >= 0; i--) {
		if (str_in[i] == '.') {
			i--;
			break;
		}
		p[digit] = str_in[i] - '0';
		digit++;
		point_posi++;
	}
	point_posi *= n;
	//read numbers before the dot.
	for (; i >= 0; i--) {
		p[digit] = str_in[i] - '0';
		digit++;
	}
	//delete the meaningless and first '0'.
	//(if there is)
	for (int i = digit - 1; i >= 0; i--) {
		if (p[i] != 0) {
			break;
		}
		digit--;
	}
	base->digit = digit;
	return point_posi;
}

/*
* function: to print answer with ans.
*/
inline void print(NUMBER* ans, int point_posi) {
	char* p = ans->num;
	//print enough '0' after the dot if need.
	if (point_posi >= ans->digit) {
		putchar('.');
		for (int i = 1; i <= point_posi - ans->digit; i++) {
			putchar('0');
		}
	}
	//print number with dot.
	for (int i = ans->digit - 1; i >= 0; i--) {
		putchar(p[i] + '0');
		if (i == point_posi && i != 0) {
			putchar('.');
		}
	}
	printf("\n");
}

int main() {
	char str_in[MAX_BUFFER];
	NUMBER n1, n2, * base = &n1, * ans = &n2;
	int n;
	while (scanf("%s%d", str_in, &n) == 2) {
		int point_posi = read(str_in, base, n);
		qpow(base, n, ans);
		print(ans, point_posi);
	}
}