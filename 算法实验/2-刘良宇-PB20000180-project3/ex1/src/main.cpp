#include "IntervalTree.hpp"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <random>
#include <unordered_set>
#include <utility>
#include <vector>

bool cmdOptionExists(char **begin, char **end, const std::string &option) {
    return std::find(begin, end, option) != end;
}

int main(int argc, char **argv) {
    // Print out help information
    if (cmdOptionExists(argv, argv + argc, "-h")) {
        std::cout << "This is a simple CLI interval tree test program."
                  << std::endl
                  << std::endl;
        std::cout << "\e[1mUsage\e[0m" << std::endl;
        std::cout << "./main \e[1m[OPTION]\e[0m" << std::endl << std::endl;
        std::cout << "\e[1mOptions\e[0m" << std::endl;
        std::cout << "-g : generator new random numbers and test" << std::endl;
        return 0;
    }

    if (cmdOptionExists(argv, argv + argc, "-g")) {
        srand(static_cast<unsigned>(time(nullptr)));
        auto file = std::ofstream("./input/input.txt");
        std::unordered_set<int> set;
        std::vector<std::pair<int, int>> intervals;
        // 要求：这里每行两个随机数据，表示区间的左右端点，至少 30 行
        //      所有区间要么是 [0, 25] 的子区间，要么是 [30, 50] 的子区间
        //      每个区间左端点互不相同
        // 实现：我们生成 15 个 [0, 25] 的子区间和 15 个 [30, 50] 的子区间
        for (auto i = 0; i < 15; i++) {
            while (true) {
                const int int_low = rand() % 25;
                if (set.count(int_low) != 0U) {
                    continue;
                }
                set.insert(int_low);
                const int int_high = rand() % (25 - int_low) + int_low;
                intervals.emplace_back(int_low, int_high);
                break;
            }
        }
        for (auto i = 0; i < 15; i++) {
            while (true) {
                const int int_low = rand() % 20 + 30;
                if (set.count(int_low) != 0U) {
                    continue;
                }
                set.insert(int_low);
                const int int_high = rand() % (50 - int_low) + int_low;
                intervals.emplace_back(int_low, int_high);
                break;
            }
        }
        // 出于准确性考虑，不妨先打乱数组
        std::shuffle(intervals.begin(), intervals.end(),
                     std::mt19937(std::random_device()()));
        for (const auto &interval : intervals) {
            file << interval.first << ' ' << interval.second << std::endl;
        }
        file.close();
    }

    // 打开文件，读取数据，建立区间树
    IntervalTree interval_tree;
    auto file = std::ifstream("./input/input.txt");
    for (auto i = 0; i < 30; i++) {
        int int_low;
        int int_high;
        file >> int_low >> int_high;
        interval_tree.intervalInsert(int_low, int_high);
    }
    file.close();

    // 首先输出中序遍历的结果，并且保存每个结点
    std::vector<IntervalTree::shared> nodes;
    auto inorder_file = std::ofstream("./output/inorder.txt");
    auto traverse = [&inorder_file, &nodes](const IntervalTree::shared &node) {
        inorder_file << node->int_low << ' ' << node->int_high << ' '
                     << node->max << std::endl;
        nodes.push_back(node);
    };
    interval_tree.traverse(traverse);
    inorder_file.close();
    std::cout << "中序遍历结果已经保存在文件" << std::endl;

    // 随机选择 3 个区间删除，并存储删除后结果。先打乱
    std::shuffle(nodes.begin(), nodes.end(),
                 std::mt19937(std::random_device()()));
    // 删除
    interval_tree.intervalDelete(nodes[0]);
    interval_tree.intervalDelete(nodes[1]);
    interval_tree.intervalDelete(nodes[2]);
    // 写入文件，先是删除的区间
    auto delete_file = std::ofstream("./output/delete_data.txt");
    delete_file << nodes[0]->int_low << ' ' << nodes[0]->int_high << std::endl;
    delete_file << nodes[1]->int_low << ' ' << nodes[0]->int_high << std::endl;
    delete_file << nodes[2]->int_low << ' ' << nodes[0]->int_high << std::endl;
    // 再写入删除后的结果
    auto after_delete_traverse =
        [&delete_file](const IntervalTree::shared &node) {
            delete_file << node->int_low << ' ' << node->int_high << ' '
                        << node->max << std::endl;
        };
    interval_tree.traverse(after_delete_traverse);
    delete_file.close();
    std::cout << "删除结果已经保存在文件" << std::endl;

    // 最后对随机生成的三个区间进行搜索
    const int low_1 = rand() % 5 + 25;
    const int high_1 = rand() % (30 - low_1) + low_1;
    const int low_2 = rand() % 50;
    const int high_2 = rand() % (50 - low_2) + low_2;
    const int low_3 = rand() % 50;
    const int high_3 = rand() % (50 - low_3) + low_3;
    std::cout << "搜索: " << low_1 << ' ' << high_1 << std::endl;
    std::cout << "搜索: " << low_2 << ' ' << high_2 << std::endl;
    std::cout << "搜索: " << low_3 << ' ' << high_3 << std::endl;
    auto search_file = std::ofstream("./output/search.txt");
    auto res1 = interval_tree.intervalSearch(low_1, high_1);
    if (res1 == interval_tree.getNil()) {
        search_file << "Null" << std::endl;
    } else {
        search_file << res1->int_low << ' ' << res1->int_high << std::endl;
    }
    auto res2 = interval_tree.intervalSearch(low_2, high_2);
    if (res2 == interval_tree.getNil()) {
        search_file << "Null" << std::endl;
    } else {
        search_file << res2->int_low << ' ' << res2->int_high << std::endl;
    }
    auto res3 = interval_tree.intervalSearch(low_3, high_3);
    if (res3 == interval_tree.getNil()) {
        search_file << "Null" << std::endl;
    } else {
        search_file << res3->int_low << ' ' << res3->int_high << std::endl;
    }
    search_file.close();
    std::cout << "搜索结果已经保存在文件" << std::endl;
}