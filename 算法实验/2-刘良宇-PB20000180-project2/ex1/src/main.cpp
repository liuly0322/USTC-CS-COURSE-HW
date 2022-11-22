#include "matrix_chain_multiply.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <vector>

int main() {
    // testing n scale
    const unsigned testcase_num = 5;

    // input file location
    const auto *in_file_path = "./input/2_1_input.txt";

    // read data into memory
    auto in_file = std::ifstream(in_file_path);
    std::vector<std::vector<u64>> testcases;
    for (auto testcase = 0; testcase < testcase_num; testcase++) {
        u64 matrix_num;
        in_file >> matrix_num;
        std::vector<u64> p;
        for (auto j = 0; j <= matrix_num; j++) {
            u64 temp_p;
            in_file >> temp_p;
            p.push_back(temp_p);
        }
        testcases.push_back(std::move(p));
    }

    // test according to specified methods and scales
    struct timeval t1;
    struct timeval t2;
    double timeuse_ms;

    auto time_file = std::ofstream("output/time.txt", std::ios_base::out);
    auto result_file = std::ofstream("output/result.txt", std::ios_base::out);

    for (const auto &testcase : testcases) {

        gettimeofday(&t1, nullptr);
        for (auto i = 0; i < 99; i++)
            auto result = matrix_chain_multiply(testcase);
        auto result = matrix_chain_multiply(testcase);
        gettimeofday(&t2, nullptr);

        auto min_cost = std::get<0>(result);
        auto m = std::get<1>(result);
        auto s = std::get<2>(result);

        timeuse_ms = static_cast<double>(t2.tv_sec - t1.tv_sec) * 1000.0 +
                     static_cast<double>(t2.tv_usec - t1.tv_usec) / 1000.0;

        // print m table
        auto n = m.size();
        if (n < 10) {
            for (int j = n - 1; j >= 0; j--) {
                for (int i = 0; i <= j; i++) {
                    std::cout << std::setw(20) << m.get(i, j);
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;

            // print s table
            for (int j = n - 1; j >= 1; j--) {
                for (int i = 0; i < j; i++) {
                    std::cout << std::setw(20) << s.get(i, j) + 1;
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

        // save to files
        time_file << timeuse_ms << std::endl;
        result_file << min_cost << std::endl;
        result_file << paren_repr(s) << std::endl;
    }

    result_file.close();
    time_file.close();
}