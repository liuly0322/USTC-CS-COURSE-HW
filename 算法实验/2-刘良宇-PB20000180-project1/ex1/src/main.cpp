#include "counting.hpp"
#include "functional"
#include "heap.hpp"
#include "merge.hpp"
#include "quick.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <vector>

int main(int argc, char *argv[]) {
    bool generate_number =
        false; // do we need to generate random numbers and save to file

    // sorting methods supplied and tested
    std::map<std::string, std::function<void(std::vector<int> &)>> methods_map =
        {{"heap", heap_sort<int>},
         {"quick", quick_sort<int>},
         {"merge", merge_sort<int>},
         {"counting", counting_sort}};
    const std::set<std::string> sorting_methods{"heap", "quick", "merge",
                                                "counting"};
    std::set<std::string> testing_methods{};

    // testing n scale
    unsigned n;
    std::set<unsigned> testing_scales{};

    // input file location
    auto file_path = std::string();

    // parse opts (unix system only)
    for (;;) {
        auto param = std::string();
        switch (getopt(argc, argv, "gn:m:i:h")) {
        case 'g':
            std::cout << "Will generate new random numbers since parameter g "
                         "specified"
                      << std::endl;
            generate_number = true;
            continue;

        case 'n':
            n = atoi(optarg);
            param = std::to_string(n);
            if (n <= 18) {
                testing_scales.insert(n);
            } else {
                std::cout << "Invalid scale 'n' specified: " << param
                          << std::endl;
            }
            continue;

        case 'm':
            param = std::string(optarg);
            if (sorting_methods.count(param) != 0U) {
                testing_methods.insert(param);
            } else {
                std::cout << "Invalid sorting method 'm' specified: " << param
                          << std::endl;
            }
            continue;

        case 'i':
            file_path = std::string(optarg);
            continue;

        case '?':
        case 'h':
        default:
            std::cout << "Help/Usage Example" << std::endl;
            break;

        case -1:
            break;
        }

        break;
    }

    if (file_path.empty()) {
        std::cout << "Error: no input file path given with -i" << std::endl;
        exit(1);
    }

    // generate 2 ** 18 unsigned in [0,  2 ** 15 - 1]
    srand(static_cast<unsigned>(time(nullptr)));
    if (generate_number) {
        auto gen_rand_file = std::ofstream(file_path);
        for (auto i = 0; i < (1 << 18); i++) {
            auto gen_number = rand() & ((1 << 15) - 1);
            gen_rand_file << gen_number << std::endl;
        }
        gen_rand_file.close();
    }

    // read random data into memory
    auto rand_file = std::ifstream(file_path);
    auto nums = std::vector<int>(1 << 18);
    int read_num;
    for (auto i = 0; i < (1 << 18); i++) {
        rand_file >> read_num;
        nums[i] = read_num;
    }

    // test according to specified methods and scales
    struct timeval t1;
    struct timeval t2;
    double timeuse_ms;
    for (const auto &scale : testing_scales) {
        for (const auto &method : testing_methods) {
            auto test_vector =
                std::vector<int>(nums.begin(), nums.begin() + (1 << scale));
            auto test_function = methods_map[method];

            gettimeofday(&t1, nullptr);
            test_function(test_vector);
            gettimeofday(&t2, nullptr);

            timeuse_ms = static_cast<double>(t2.tv_sec - t1.tv_sec) * 1000.0 +
                         static_cast<double>(t2.tv_usec - t1.tv_usec) / 1000.0;

            std::cout << "Scale: " << scale << " Method: " << method
                      << " sort time usage: " << timeuse_ms << "ms"
                      << std::endl;

            // save to output file
            auto output_path = std::string("output/");
            output_path += method;
            output_path += "_sort/";
            auto result_file = std::ofstream(output_path + "result_" +
                                             std::to_string(scale) + ".txt");
            for (const auto &num : test_vector) {
                result_file << num << std::endl;
            }
            result_file.close();
            auto time_file =
                std::ofstream(output_path + "time.txt",
                              std::ios_base::out | std::ios_base::app);
            time_file << scale << "," << timeuse_ms << std::endl;
            time_file.close();
        }
    }
}