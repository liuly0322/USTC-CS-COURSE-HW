/**
 * @file gen_input.cpp
 * @author liuly (me@liuly.moe)
 * @brief gen input data. Compiled with flag -std=c++17
 * @version 0.1
 * @date 2023-01-11
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using std::endl;
using std::log;

auto N_Scales = std::vector<int>{27, 81, 243, 729};

auto log_5(int n) -> int { return log(n) / log(5); }
auto log_7(int n) -> int { return log(n) / log(7); }

/**
 * @brief 检测负环，如果有则删除
 *
 * @param nodes 图
 * @return true 有负环
 * @return false 没有负环
 */
bool detectNegativeCycle(std::vector<std::unordered_map<int, int>> &nodes) {
    // 首先运行一遍 Bellman-Ford 算法，以第一个点为源点
    auto n = nodes.size() - 1;
    // d 需要初始化为 inf 数组，除了源点为 0
    auto d = std::vector<int>(n + 1, std::numeric_limits<int>::max() / 2);
    d[1] = 0;
    // 前驱数组
    auto pi = std::vector<int>(n + 1, 0);

    // 循环 |V| - 1 次，松弛每一条边
    for (auto i = 1; i <= n - 1; i++) {
        for (auto u = 1; u <= n; u++) {
            for (auto [v, weight] : nodes[u]) {
                if (d[v] > d[u] + weight) {
                    d[v] = d[u] + weight;
                    pi[v] = u;
                }
            }
        }
    }

    // 判断是否有负环
    auto found = false;
    auto path = std::unordered_set<int>{};
    auto runner = 0;
    for (auto u = 1; u <= n; u++) {
        for (auto [v, weight] : nodes[u]) {
            if (d[v] > d[u] + weight) {
                found = true;
                path.insert(v);
                runner = v;
            }
        }
    }

    // 如果没找到，不需要进一步处理了
    if (!found)
        return false;

    // 否则不断寻找前驱，直到重复
    while ((runner = pi[runner]) != 0) {
        if (path.count(runner) == 0) {
            path.insert(runner);
        } else {
            auto pre = pi[runner];
            // 删去边 <pre, runner>
            nodes[pre].erase(runner);
            break;
        }
    }

    return true;
}

int main() {
    // initialize RNG
    srand(static_cast<unsigned>(time(nullptr)));

    for (auto i = 0; i < N_Scales.size(); i++) {
        auto n = N_Scales[i];
        auto edge_nums = std::vector<int>{log_5(n), log_7(n)};
        for (auto j = 0; j < edge_nums.size(); j++) {
            auto edge_num = edge_nums[j];
            auto file = std::ofstream("./input/input" + std::to_string(i + 1) +
                                      std::to_string(j + 1) + ".txt");

            // graph
            auto nodes = std::vector<std::unordered_map<int, int>>(n + 1);

            // gen input data
            for (auto i = 1; i <= n; i++) {
                // gen edge_num edges
                auto gened = std::unordered_set<int>{};
                while (gened.size() < edge_num) {
                    // try gen one
                    auto dst = (rand() % n) + 1;
                    if (dst == i)
                        continue;
                    gened.insert(dst);
                }
                // insert to graph first
                for (const auto &dst : gened) {
                    auto weight = (rand() % 61) - 10;
                    nodes[i][dst] = weight;
                }
            }

            // delete negative weight cycle
            while (detectNegativeCycle(nodes))
                ;

            // write to file
            for (auto i = 1; i <= n; i++) {
                for (auto [j, weight] : nodes[i]) {
                    file << i << ' ' << j << ' ' << weight << endl;
                }
            }
            file.close();
        }
    }
}