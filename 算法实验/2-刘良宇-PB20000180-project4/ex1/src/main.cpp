/**
 * @file main.cpp
 * @author liuly (me@liuly.moe)
 * @brief 读取文件输入，运行 johnson 算法并统计时间
 * @version 0.1
 * @date 2023-01-11
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <fstream>
#include <iostream>
#include <limits>
#include <queue>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct dist {
    int dis, u;
    bool operator>(const dist &a) const { return dis > a.dis; }
};

std::vector<int> dijkstra(std::vector<std::unordered_map<int, int>> &nodes,
                          int src) {
    auto n = nodes.size() - 1;
    auto d = std::vector<int>(n + 1, std::numeric_limits<int>::max() / 2);
    d[src] = 0;

    // 前驱数组，即为返回的对象
    auto pi = std::vector<int>(n + 1, 0);

    // 优先队列
    auto p_q = std::priority_queue<dist, std::vector<dist>, std::greater<>>{};
    p_q.push({0, src});

    // 记录已确定的集合
    auto visited = std::unordered_set<int>{};

    while (!p_q.empty()) {
        int u = p_q.top().u;
        p_q.pop();
        if (visited.count(u) != 0)
            continue;
        visited.insert(u);
        auto &adjs = nodes[u];
        for (auto [v, weight] : adjs) {
            if (d[v] > d[u] + weight) {
                d[v] = d[u] + weight;
                pi[v] = u;
                p_q.push({d[v], v});
            }
        }
    }
    return pi;
}

/**
 * @brief 运行 johnson 算法，返回构造完毕的矩阵
 *
 * @param nodes 需要执行算法的图
 */
std::vector<std::vector<int>>
johnson(std::vector<std::unordered_map<int, int>> nodes) {
    // 首先运行一遍 Bellman-Ford 算法，以第 0 个点为源点（虚拟结点）
    auto n = nodes.size();
    // d 需要初始化为 inf 数组，除了源点为 0
    auto d = std::vector<int>(n, std::numeric_limits<int>::max() / 2);
    d[0] = 0;

    // 循环 |V| - 1 次，松弛每一条边
    for (auto i = 1; i <= n - 1; i++) {
        for (auto u = 0; u < n; u++) {
            for (auto [v, weight] : nodes[u]) {
                if (d[v] > d[u] + weight) {
                    d[v] = d[u] + weight;
                }
            }
        }
    }

    n--;
    // 边权重新设定为 w + h_u - h_v
    nodes[0].clear();
    for (auto u = 1; u <= n; u++) {
        auto &map = nodes[u];
        for (auto &[v, weight] : map) {
            weight += d[u] - d[v];
        }
    }

    // 以每个点为起点，运行 n 次 dijkstra 算法
    // dijkstra 返回的是 src 到 dst 最短路径上 dst 前面的点
    std::vector<std::vector<int>> result{{}};
    for (auto u = 1; u <= n; u++) {
        result.push_back(dijkstra(nodes, u));
    }

    return result;
}

int main() {
    auto worklist = std::vector<std::pair<std::string, int>>{
        {"11", 27},  {"12", 27},  {"21", 81},  {"22", 81},
        {"31", 243}, {"32", 243}, {"41", 729}, {"42", 729}};
    auto time_file = std::ofstream("./output/time.txt");
    struct timeval t1;
    struct timeval t2;
    for (auto [work, n] : worklist) {
        auto input_filename = "./input/input" + work + ".txt";
        auto output_filename = "./output/result" + work + ".txt";

        // 读取输入，建图
        auto nodes = std::vector<std::unordered_map<int, int>>(n + 1);
        auto input_fstream = std::ifstream(input_filename);
        auto line = std::string{};
        while (std::getline(input_fstream, line)) {
            auto line_sstream = std::stringstream{line};
            int u, v, weight;
            line_sstream >> u >> v >> weight;
            nodes[u][v] = weight;
        }

        // 执行 johnson 算法
        gettimeofday(&t1, nullptr);
        auto johnson_result = johnson(nodes);
        gettimeofday(&t2, nullptr);

        // johnson 算法的结果保存到输出文件
        auto output_fstream = std::ofstream(output_filename);
        for (auto u = 1; u <= n; u++) {
            for (auto v = 1; v <= n; v++) {
                if (u == v)
                    continue;
                auto pre_v = johnson_result[u][v];
                if (pre_v == 0) {
                    // 不连通
                    output_fstream << "Null!" << std::endl;
                    continue;
                }
                // 循环找 v 的前驱，直到找到 u
                auto path = std::vector<int>{v};
                auto length = 0;
                while (path.back() != u) {
                    length += nodes[pre_v][path.back()];
                    path.push_back(pre_v);
                    pre_v = johnson_result[u][pre_v];
                }
                // 此时 path 逆向保存了 u 到 v 的路径，输出即可
                output_fstream << '(';
                while (path.size() > 1) {
                    auto back = path.back();
                    output_fstream << back << ',';
                    path.pop_back();
                }
                output_fstream << v << ' ' << length << ')' << std::endl;
            }
        }

        // 输出用时
        auto timeuse_ms = static_cast<double>(t2.tv_sec - t1.tv_sec) * 1000.0 +
                          static_cast<double>(t2.tv_usec - t1.tv_usec) / 1000.0;
        time_file << timeuse_ms << "ms" << std::endl;
    }
    time_file.close();
}