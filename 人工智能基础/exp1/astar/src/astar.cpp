#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <vector>

using std::deque;
using std::pair;
using std::priority_queue;
using std::unordered_set;
using std::vector;

class Action {
  public:
    Action(int i, int j, int not_op) { action = (i << 16) | (j << 8) | not_op; }
    [[nodiscard]] int8_t get_i() const { return action >> 16; }
    [[nodiscard]] int8_t get_j() const { return (action >> 8) & 0xff; }
    [[nodiscard]] int8_t get_not_op() const { return action & 0xff; }
    bool operator==(const Action &other) const {
        return action == other.action;
    }

  private:
    int action;
};
using Actions = vector<Action>;

struct State {
    explicit State(vector<int> state) : state(std::move(state)) {}
    [[nodiscard]] size_t size() const { return state.size(); }
    [[nodiscard]] bool get(int i, int j) const {
        return (state[i] & (1 << j)) != 0;
    }
    void set(int i, int j, bool v) {
        if (v)
            state[i] |= (1 << j);
        else
            state[i] &= ~(1 << j);
    }
    void neg(int i, int j) { state[i] ^= (1 << j); }
    [[nodiscard]] bool operator==(const State &other) const {
        return state == other.state;
    }
    vector<int> state;
};

struct StateHash {
    std::size_t operator()(const State &state) const {
        const auto &vec = state.state;
        std::size_t seed = vec.size();
        for (auto i : vec) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

class Node {
  public:
    Node(State state, int g, Actions actions)
        : state(std::move(state)), actions_so_far(std::move(actions)), g(g) {
        h = heuristic();
    }

    [[nodiscard]] const State &get_state() { return state; }
    [[nodiscard]] Actions get_actions() const { return actions_so_far; }
    [[nodiscard]] int get_h() const { return h; }

    bool operator<(const Node &other) const {
        return (g + h) > (other.g + other.h) ||
               ((g + h) == (other.g + other.h) && h > other.h);
    }

    [[nodiscard]] const vector<Node> &next_nodes(vector<Node> &next) const {
        next.clear();
        for (const auto &action : good_actions()) {
            auto next_state = take_action(state, action);
            auto actions = actions_so_far;
            actions.emplace_back(action);
            next.emplace_back(std::move(next_state), g + 1, std::move(actions));
        }
        return next;
    }

  private:
    State state;
    Actions actions_so_far;
    int g;
    int h;

    static State take_action(const State &state, const Action &action) {
        auto next_state = state;
        auto i = action.get_i();
        auto j = action.get_j();
        for (auto op = 0; op < 4; op++)
            if (op != action.get_not_op())
                next_state.neg(i + op / 2, j + op % 2);
        return next_state;
    }

    [[nodiscard]] pair<int, int> first_one_index() const {
        auto n = state.size();
        for (auto i = 0U; i < n; i++)
            for (auto j = 0U; j < n; j++)
                if (state.get(i, j))
                    return {i, j};
        return {-1, -1};
    }

    [[nodiscard]] const vector<Action> &good_actions() const {
        auto n = state.size();
        auto [first_one_i, first_one_j] = first_one_index();
        static auto good_actions = vector<Action>{};
        good_actions.clear();
        auto i = first_one_i == n - 1 ? first_one_i - 1 : first_one_i;
        for (auto j : {first_one_j - 1, first_one_j}) {
            if (j < 0 || j >= n - 1)
                continue;
            auto dx = first_one_i - i;
            auto dy = first_one_j - j;
            for (auto not_op = 0; not_op < 4; not_op++) {
                if (dx * 2 + dy == not_op)
                    continue;
                good_actions.emplace_back(i, j, not_op);
            }
        }
        return good_actions;
    }

    int search_eight_connected(int i, int j, bool marked[16][16]) const {
        if (marked[i][j] || !state.get(i, j))
            return 0;
        marked[i][j] = true;
        auto n = state.size();
        auto count = 0;
        static auto frontier = deque<pair<int, int>>{};
        frontier.clear();
        frontier.emplace_back(i, j);
        while (!frontier.empty()) {
            auto [i, j] = frontier.front();
            frontier.pop_front();
            count++;
            for (auto op = 0; op < 9; op++) {
                auto new_i = i - 1 + op / 3;
                auto new_j = j - 1 + op % 3;
                if (new_i < 0 || new_i >= n || new_j < 0 || new_j >= n)
                    continue;
                if (marked[new_i][new_j] || !state.get(new_i, new_j))
                    continue;
                marked[new_i][new_j] = true;
                frontier.emplace_back(new_i, new_j);
            }
        }
        return count;
    }

    [[nodiscard]] int heuristic() {
        auto n = state.size();
        auto cost = 0;
        auto total_count = 0;
        bool marked[16][16];
        for (auto i = 0U; i < n; i++)
            for (auto j = 0U; j < n; j++)
                marked[i][j] = false;
        for (auto i = 0U; i < n; i++) {
            for (auto j = 0U; j < n; j++) {
                auto count = search_eight_connected(i, j, marked);
                total_count += count;
                cost += (count + 2) / 3;
            }
        }
        if (cost % 2 != total_count % 2)
            cost++;
        return cost;
    }
};

class Solver {
  public:
    explicit Solver(Node start) : start(std::move(start)) {}

    Actions solve() {
        auto explored = unordered_set<State, StateHash>{};
        auto pq = priority_queue<Node>({}, {start});
        auto next_nodes = vector<Node>{};

        while (!pq.empty()) {
            auto top = pq.top();
            pq.pop();
            if (top.get_h() == 0) [[unlikely]]
                return top.get_actions();
            if (explored.contains(top.get_state()))
                continue;
            explored.emplace(top.get_state());
            for (const auto &next : top.next_nodes(next_nodes))
                pq.push(next);
        }
        return {};
    }

  private:
    Node start;
};

class IO {
  public:
    static State read(const std::string &filename) {
        std::ifstream input(filename);
        int n;
        input >> n;
        State state((vector<int>(n, 0)));
        for (auto i = 0; i < n; i++) {
            for (auto j = 0; j < n; j++) {
                int temp;
                input >> temp;
                state.set(i, j, temp != 0);
            }
        }
        return state;
    }
    static void write(const std::string &filename, const Actions &actions) {
        std::ofstream output(filename);
        if (actions.empty()) {
            output << "No valid solution." << std::endl;
            return;
        }
        output << actions.size() << std::endl;
        for (auto action : actions) {
            auto not_op = action.get_not_op();
            auto i = action.get_i();
            auto j = action.get_j();
            auto lut_map = vector<int>{2, 1, 3, 4};
            output << ((not_op / 2) != 0 ? i : i + 1) << ','
                   << ((not_op % 2) != 0 ? j : j + 1) << ',' << lut_map[not_op]
                   << std::endl;
        }
    }
};

int main() {
    for (int i = 0; i < 10; i++) {
        auto start_state = IO::read("input/input" + std::to_string(i) + ".txt");
        std::cout << "input" << i << ": ";
        auto start_time = std::chrono::high_resolution_clock::now();
        Solver solver({start_state, 0, {}});
        auto result = solver.solve();
        auto end_time = std::chrono::high_resolution_clock::now();
        std::cout << "solution g=" << result.size() << ", done in "
                  << std::chrono::duration_cast<std::chrono::microseconds>(
                         end_time - start_time)
                             .count() /
                         1000.0
                  << "ms" << std::endl;
        IO::write("output/output" + std::to_string(i) + ".txt", result);
    }
}
