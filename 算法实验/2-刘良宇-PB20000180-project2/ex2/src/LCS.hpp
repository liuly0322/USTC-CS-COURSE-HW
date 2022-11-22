
#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

enum CHOICE { L, T, LT };

template <typename T> class Table {
  public:
    explicit Table(size_t m, size_t n)
        : m(m), n(n), data(std::vector<T>(m * n)) {}
    inline T get(size_t x, size_t y) const { return data[x * n + y]; }
    inline void set(size_t x, size_t y, T value) { data[x * n + y] = value; }
    inline size_t size() const { return n; }

  private:
    size_t m;
    size_t n;
    std::vector<T> data;
};

/**
 * @brief Return LCS of two strings
 *
 * @param x The first string
 * @param y The second string
 * @return std::string The LCS
 */
inline std::string longest_common_sequence(std::string &x, std::string &y) {
    auto m = x.length();
    auto n = y.length();

    // b and c are initialized with 0
    auto b = Table<CHOICE>(m, n);
    auto c = Table<unsigned>(m + 1, n + 1);

    // bottom up iteration
    for (auto i = 0; i < m; i++) {
        for (auto j = 0; j < n; j++) {
            if (x[i] == y[j]) {
                c.set(i + 1, j + 1, c.get(i, j) + 1);
                b.set(i, j, CHOICE::LT);
            } else if (c.get(i, j + 1) >= c.get(i + 1, j)) {
                c.set(i + 1, j + 1, c.get(i, j + 1));
                b.set(i, j, CHOICE::T);
            } else {
                c.set(i + 1, j + 1, c.get(i + 1, j));
                b.set(i, j, CHOICE::L);
            }
        }
    }

    // collect final string
    auto i = m - 1;
    auto j = n - 1;
    auto result = std::string();
    while ((i + 1 != 0U) && (j + 1 != 0U)) {
        if (b.get(i, j) == CHOICE::LT) {
            result += x[i];
            i -= 1;
            j -= 1;
        } else if (b.get(i, j) == CHOICE::T) {
            i -= 1;
        } else {
            j -= 1;
        }
    }
    std::reverse(result.begin(), result.end());
    return result;
}