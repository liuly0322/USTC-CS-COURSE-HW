// 接收文件，交给 sysYFParser，打印返回值

#include "subprocess.h"
#include <iostream>

const int TEST_INTERNAL_ERROR_CODE = 255;

auto main(int argc, char *argv[]) -> int {
    if (argc != 3) {
        std::cout << "error format for testing!" << std::endl;
        return TEST_INTERNAL_ERROR_CODE;
    }
    auto *program = argv[1];
    auto *filename = argv[2];
    char *command[] = {program, filename, nullptr};
    struct subprocess_s subprocess;
    if (subprocess_create(command, 0, &subprocess) != 0) {
        std::cout << "error spawning subprocess for testing!" << std::endl;
        return TEST_INTERNAL_ERROR_CODE;
    }
    int process_return;
    if (subprocess_join(&subprocess, &process_return) != 0) {
        std::cout << "error waiting subprocess for testing!" << std::endl;
        return TEST_INTERNAL_ERROR_CODE;
    }
    std::cout << process_return;
    return 0;
}
