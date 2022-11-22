#include "LCS.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <vector>

int main() {
    // testing n scale
    const unsigned testcase_num = 5;

    // input file location
    const auto *in_file_path = "./input/2_2_input.txt";

    // read data into memory
    auto in_file = std::ifstream(in_file_path);
    std::vector<std::pair<std::string, std::string>> testcases;
    for (auto testcase = 0; testcase < testcase_num; testcase++) {
        unsigned length;
        in_file >> length;
        std::string x;
        std::string y;
        in_file >> x >> y;
        testcases.emplace_back(x, y);
    }

    // test according to specified methods and scales
    struct timeval t1;
    struct timeval t2;
    double timeuse_ms;

    auto time_file = std::ofstream("output/time.txt", std::ios_base::out);

    for (const auto &testcase : testcases) {
        auto x = testcase.first;
        auto y = testcase.second;

        gettimeofday(&t1, nullptr);
        for (auto i = 0; i < 99; i++)
            auto LCS_string = longest_common_sequence(x, y);
        auto LCS_string = longest_common_sequence(x, y);
        gettimeofday(&t2, nullptr);

        timeuse_ms = static_cast<double>(t2.tv_sec - t1.tv_sec) * 1000.0 +
                     static_cast<double>(t2.tv_usec - t1.tv_usec) / 1000.0;

        // print LCS string and size
        std::cout << std::setw(5) << LCS_string.length() << ": " << LCS_string
                  << std::endl;

        // save to files
        time_file << timeuse_ms << std::endl;

        auto result_file = std::ofstream(
            "output/result_" + std::to_string(x.length()) + ".txt",
            std::ios_base::out);
        result_file << LCS_string.length() << std::endl;
        result_file << LCS_string << std::endl;
        result_file.close();
    }

    time_file.close();
}