#include <cstdio>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

using std::cout;
using std::ifstream;
using std::ios_base;
using std::string;
using std::unordered_map;
using std::vector;
using ScheduleTable = vector<int>;

bool check_not_continuous(ScheduleTable shifts) {
    auto n = shifts.size();
    for (int i = 1; i < n; i++) {
        if (shifts[i] == shifts[i - 1]) {
            return false;
        }
    }
    return true;
}

bool check_fairness(ScheduleTable shifts, int n) {
    unordered_map<int, int> mp;
    for (auto num : shifts) {
        mp[num]++;
    }

    for (auto [num, cnt] : mp) {
        if (cnt < shifts.size() / n) {
            cout << "num: " << num << "cnt: " << cnt << std::endl;
            return false;
        }
    }
    return true;
}

bool check_all(ScheduleTable shifts, int n) {
    if (not check_not_continuous(shifts)) {
        cout << "continuous wrong" << std::endl;
        return false;
    }
    if (not check_fairness(shifts, n)) {
        cout << "fairness wrong" << std::endl;
        return false;
    }
    return true;
}

int main() {
    for (int i = 0; i < 10; ++i) {
        const string output_file =
            "./output/output" + std::to_string(i) + ".txt";
        const string input_file = "./input/input" + std::to_string(i) + ".txt";
        auto in1 = ifstream(input_file, ios_base::in);
        int n = 0;
        int d = 0;
        int s = 0;
        in1 >> n;
        in1.get();
        in1 >> d;
        in1.get();
        in1 >> s;

        auto in = ifstream(output_file, ios_base::in);
        ScheduleTable shifts;
        string line;

        int j = 0;
        while (++j <= d && getline(in, line)) {
            if (line.size() == 1) {
                break;
            }
            std::stringstream ss(line);
            int num = 0;
            for (int i = 0; i < s; ++i) {
                ss >> num;
                shifts.push_back(num);
                ss.get();
            }
        }

        if (check_all(shifts, n)) {
            cout << "output" << i << " is legal" << std::endl;
        } else {
            cout << "output" << i << " is illegal" << std::endl;
        }
    }
}
