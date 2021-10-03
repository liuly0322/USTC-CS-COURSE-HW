#include <stdio.h>
int main() {
	int sizeofMemory, numberofInstructions, protectStart, protectEnd;
	int nowStart, nowEnd, allocateStart[1000] = { 0 }, allocateEnd[1000] = { 0 };
	int nowAllocatePosi = 0;
	char op, opTemp;
	scanf("%d%d", &sizeofMemory, &numberofInstructions);
	do {
		op = getchar();
	} while (op != ' ');
	scanf("%d%d", &protectStart, &protectEnd);
	for (int i = 1; i <= numberofInstructions - 1; i++) {
		do {
			op = getchar();
		} while (op != 'a' && op != 's');
		do {
			opTemp = getchar();
		} while (opTemp != ' ');
		scanf("%d%d", &nowStart, &nowEnd);
		//if (nowStart < 1) nowStart = 1;
		//if (nowEnd > sizeofMemory) nowEnd = sizeofMemory;
		if (op == 'a') {
			//如果指令无效则continue
			if (protectEnd >=nowStart && nowEnd >= protectStart) {
				continue;
			}
			int flag = 0;
			for (int j = 1; j <= nowAllocatePosi; j++) {
				if (allocateEnd[j] >= nowStart && nowEnd >= allocateStart[j]) {
					flag = 1;
					break;
				}
			}
			if (flag == 1) continue;
		}
		//接下来已经确定有重复，考虑取并集即可
		for (int j = 1; j <= nowAllocatePosi; j++) {
			if (allocateEnd[j] >= nowStart - 1 && nowEnd >= allocateStart[j] - 1) {
				//下面进行合并！
				nowStart = nowStart < allocateStart[j] ? nowStart : allocateStart[j];
				nowEnd = nowEnd > allocateEnd[j] ? nowEnd : allocateEnd[j];
				//同时该位置free了，考虑把最后一位挪过来检测（如果这一位不是最后一位）
				if (j != nowAllocatePosi) {
					allocateStart[j] = allocateStart[nowAllocatePosi];
					allocateEnd[j] = allocateEnd[nowAllocatePosi];
					j--;
				}
				nowAllocatePosi--;
			}
		}
		nowAllocatePosi += 1;
		allocateStart[nowAllocatePosi] = nowStart;
		allocateEnd[nowAllocatePosi] = nowEnd;
	}
	for (int i = 1; i <= nowAllocatePosi; i++) sizeofMemory -= (allocateEnd[i] - allocateStart[i] + 1);
	printf("%d", sizeofMemory);
}
