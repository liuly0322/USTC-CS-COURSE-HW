#include <stdio.h>
#define MAX(x, y) x > y ? x : y

/*
 * function: fill all num[x][y] in the same room with (i, j) with num_now.
 * parameters: num[i][j] is simply a mapping from (i, j),
 *			   num_now is the number to be used to fill.
 *			   block[i][j] shows the wall around (i, j).
 * return value: number of successfully filled blocks, include itself.
 *				 when reaching the border, it should be 0.
 * caution: assuming all num[x][y] is different from num_now.
 */
int fillnum(int num[][50], char block[][50],
	int i, int j, int n, int m, int num_now) {
	if (i < 0 || j < 0 || i > n - 1 || i > m - 1) {	//recursion exit
		return 0;
	}
	num[i][j] = num_now;	//fill num[i][j] with num_now
	int sum = 1, wall = block[i][j], x[4] = { 0,-1,0,1 }, y[4] = { -1,0,1,0 };
	for (int k = 0; k < 4; k++) {
		// DFS 4 directions to fill the room and get the size
		if (wall % 2 == 0 && num_now != num[i + x[k]][j + y[k]]) {
			sum += fillnum(num, block, i + x[k], j + y[k], n, m, num_now);
		}
		wall /= 2;
	}
	return sum;
}

int main() {
	char block[50][50] = { {0} };	//the input
	int num[50][50] = { {0} },	//num can be seen as a mapping from (i, j)
		room_num = 0, //number of rooms before breking a wall
		room_maxsize = 0, new_max = 0,//max size of rooms before and after
		new_i = 0, new_j = 0;	//describe the wall need broken
	char wall_direction = '\0';

	int n, m;
	scanf("%d%d", &m, &n);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			scanf("%hhd", &block[i][j]);
		}
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if (num[i][j]) continue;
			room_num++;	//a new room, then get the room size.
			num[i][j] = fillnum(num, block, i, j, n, m, 1);
			room_maxsize = MAX(num[i][j], room_maxsize);
			// 2 ^ 12 = 4096 > 2500 = 50 ^ 2, so do a state compression
			// the last 12 binary numbers of num[i][j] shows which room
			// (i, j) is in and the other shows size of the room.
			num[i][j] = (num[i][j] << 12) + room_num;
			// fill it into all blocks in this room
			fillnum(num, block, i, j, n, m, num[i][j]);
		}
	}
	printf("%d\n%d\n", room_num, room_maxsize);

	for (int j = 0; j < m; j++) {
		for (int i = n - 1; i >= 0; i--) {
			//compare two blocks near (i, j) (North and East) with (i, j)
			//if in different room, then see if it can be largest new room.
			if (i >= 1 &&
				((num[i][j] ^ num[i - 1][j]) & ((1 << 12) - 1)) &&
				(num[i][j] >> 12) + (num[i - 1][j] >> 12) > new_max) {
				new_max = (num[i][j] >> 12) + (num[i - 1][j] >> 12);
				new_i = i + 1; new_j = j + 1; wall_direction = 'N';
			}
			if (j < m - 1 &&
				((num[i][j] ^ num[i][j + 1]) & ((1 << 12) - 1)) &&
				(num[i][j] >> 12) + (num[i][j + 1] >> 12) > new_max) {
				new_max = (num[i][j] >> 12) + (num[i][j + 1] >> 12);
				new_i = i + 1; new_j = j + 1; wall_direction = 'E';
			}
		}
	}
	printf("%d\n%d %d %c\n", new_max, new_i, new_j, wall_direction);
}