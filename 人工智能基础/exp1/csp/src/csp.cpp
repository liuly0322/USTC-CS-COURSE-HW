#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <tuple>

using std::string;
using std::vector;

using Requests = vector<vector<vector<bool>>>;
using Schedule = vector<int>;

class IO {
  public:
    static std::tuple<Requests, int, int, int> read(const string &input_file) {
        std::ifstream in(input_file);
        int n;
        int d;
        int s;
        char tmp_c;
        in >> n >> tmp_c >> d >> tmp_c >> s;
        auto requests = Requests(n, vector<vector<bool>>(d, vector<bool>(s)));
        auto line = string{};
        for (auto i = 0; i < n; i++) {
            for (auto j = 0; j < d; j++) {
                in >> line;
                for (auto k = 0; k < s; k++) {
                    requests[i][j][k] = line[2 * k] == '1';
                }
            }
        }
        return {requests, n, d, s};
    }

    static void output(int sat_count, int d, int s, const Schedule &schedule,
                       const string &output_file) {
        std::ofstream out(output_file);
        for (int i = 0; i < d; ++i) {
            for (int j = 0; j < s; ++j) {
                out << schedule[i * s + j];
                if (j != s - 1)
                    out << ',';
            }
            out << std::endl;
        }
        out << sat_count << std::endl;
    }
};

class CSP {
  public:
    CSP(Requests requests, int n, int d, int s)
        : requests(std::move(requests)), schedule(d * s),
          staff_shifts_count(n, 0), n(n), d(d), s(s) {}

    int solve() {
        for (auto shift = 0; shift < d * s; shift++)
            assign_shift(shift / s, shift % s);

        while (!is_fair())
            adjust();

        auto sat_count = 0;
        for (int i = 0; i < d; ++i)
            for (int j = 0; j < s; ++j)
                if (requests[schedule[i * s + j] - 1][i][j])
                    sat_count++;

        return sat_count;
    }

    [[nodiscard]] const Schedule &get_schedule() const { return schedule; }

  private:
    Requests requests;
    Schedule schedule;
    vector<unsigned> staff_shifts_count;
    int n;
    int d;
    int s;
    const unsigned MAX = 1e7;

    bool is_fair() {
        auto min_shift =
            *min_element(staff_shifts_count.begin(), staff_shifts_count.end());
        return static_cast<int>(min_shift) >= (d * s / n);
    }

    void adjust() {
        // 不公平，需要调整，挑出上的最少的去替别人的班
        auto min_shift_staff = 0;
        auto min_shift_staff_times = MAX;
        for (auto staff = 0; staff < n; staff++) {
            if (staff_shifts_count[staff] < min_shift_staff_times) {
                min_shift_staff_times = staff_shifts_count[staff];
                min_shift_staff = staff;
            }
        }

        auto final_replaced_staff = 0;
        auto final_replaced_shift = -1;
        for (auto shift = 0; shift < d * s; shift++) {
            auto day = shift / s;
            auto day_shift = shift % s;
            auto staff = schedule[shift];
            if (!requests[min_shift_staff][day][day_shift] ||
                staff == min_shift_staff + 1)
                continue;
            if (shift != 0 && schedule[shift - 1] == min_shift_staff + 1) {
                continue;
            }
            if (shift != d * s - 1 &&
                schedule[shift + 1] == min_shift_staff + 1) {
                continue;
            }

            if (final_replaced_staff == 0) {
                final_replaced_staff = staff;
                final_replaced_shift = shift;
            } else {
                if (staff_shifts_count[staff - 1] >
                    staff_shifts_count[final_replaced_staff - 1]) {
                    final_replaced_staff = staff;
                    final_replaced_shift = shift;
                }
            }
        }

        schedule[final_replaced_shift] = min_shift_staff + 1;
        staff_shifts_count[final_replaced_staff - 1]--;
        staff_shifts_count[min_shift_staff]++;
    }

    void assign_shift(int i, int j) {
        auto min_shift_staff_shifts = MAX;
        auto min_shift_staff = 0;
        for (auto staff : match_requests(i, j)) {
            if (not_consistent(i, j, staff) &&
                staff_shifts_count[staff - 1] < min_shift_staff_shifts) {
                min_shift_staff_shifts = staff_shifts_count[staff - 1];
                min_shift_staff = staff;
            }
        }

        if (min_shift_staff != 0) {
            schedule[i * s + j] = min_shift_staff;
            staff_shifts_count[min_shift_staff - 1]++;
        }
    }

    [[nodiscard]] bool not_consistent(int i, int j, int val) const {
        auto shift_index = i * s + j;
        if (shift_index != 0 && schedule[shift_index - 1] == val)
            return false;
        if (shift_index != d * s - 1 && schedule[shift_index + 1] == val)
            return false;
        return true;
    }

    [[nodiscard]] vector<int> match_requests(int i, int j) const {
        vector<int> request_staffs;
        for (int k = 0; k < n; ++k)
            if (requests[k][i][j])
                request_staffs.push_back(k + 1);

        if (request_staffs.empty())
            for (int k = 0; k < n; ++k)
                request_staffs.push_back(k + 1);

        return request_staffs;
    }
};

int main() {
    constexpr auto start = 0;
    constexpr auto end = 10;

    for (auto i = start; i < end; i++) {
        auto input_file = "input/input" + std::to_string(i) + ".txt";
        auto output_file = "output/output" + std::to_string(i) + ".txt";

        auto [requests, n, d, s] = IO::read(input_file);
        CSP csp(requests, n, d, s);
        auto cnt = csp.solve();
        IO::output(cnt, d, s, csp.get_schedule(), output_file);
    }
}
