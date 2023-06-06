/**
 * @file validate.cpp
 * @author liuly (me@liuly.moe)
 * @brief valid if the output solution file is valid
 * @version 0.1
 * @date 2023-05-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <fstream>
#include <iostream>
#include <vector>

void valid(int index) {
    std::ifstream input("input/input" + std::to_string(index) + ".txt");
    // build board
    int board_size;
    input >> board_size;
    std::vector<std::vector<bool>> board(board_size,
                                         std::vector<bool>(board_size));
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            int temp;
            input >> temp;
            board[i][j] = temp != 0;
        }
    }
    std::ifstream output("output/output" + std::to_string(index) + ".txt");
    int ops;
    output >> ops;
    for (int i = 0; i < ops; i++) {
        int x;
        int y;
        int op;
        // 读入 1,0,1 形式的 x,y,op
        char tmp_c;
        output >> x >> tmp_c >> y >> tmp_c >> op;
        board[x][y] = !board[x][y];
        switch (op) {
        case 1:
            board[x - 1][y] = !board[x - 1][y];
            board[x][y + 1] = !board[x][y + 1];
            break;
        case 2:
            board[x - 1][y] = !board[x - 1][y];
            board[x][y - 1] = !board[x][y - 1];
            break;
        case 3:
            board[x][y - 1] = !board[x][y - 1];
            board[x + 1][y] = !board[x + 1][y];
            break;
        case 4:
            board[x][y + 1] = !board[x][y + 1];
            board[x + 1][y] = !board[x + 1][y];
            break;
        }
    }
    // all 0?
    bool valid = true;
    for (auto &row : board) {
        for (auto cell : row) {
            if (cell) {
                valid = false;
                break;
            }
        }
    }
    std::cout << "output" << index << ".txt is "
              << (valid ? "valid" : "invalid") << std::endl;
}

int main() {
    for (int i = 0; i < 10; i++) {
        valid(i);
    }
}