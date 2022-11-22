#include <cstddef>
#include <cstdint>
#include <tuple>
#include <vector>

template <typename T> class Table {
  public:
    explicit Table(size_t n) : n(n), data(std::vector<T>(n * n)) {}
    inline T get(size_t x, size_t y) const { return data[x * n + y]; }
    inline void set(size_t x, size_t y, T value) { data[x * n + y] = value; }
    inline size_t size() const { return n; }

  private:
    size_t n;
    std::vector<T> data;
};

using u64 = uint64_t;

/**
 * @brief Compute the best matrix chain multiplication plan
 *
 * @param p Dimensions for matrices
 * @return std::tuple<u64, Table<u64>, Table<u64>>
 *         min_cost, m, s
 */
inline std::tuple<u64, Table<u64>, Table<u64>>
matrix_chain_multiply(const std::vector<u64> &p) {
    const int n = p.size() - 1;

    // Initialize n * n matrix m & s (with all 0)
    Table<u64> m(n);
    Table<u64> s(n);

    // do a bottom-up search
    // iter over chain length
    for (size_t l = 2; l <= n; l++) {
        // iter over (i, j)
        for (size_t i = 0; i <= n - l; i++) {
            const size_t j = i + l - 1;
            // set to +\inf
            m.set(i, j, UINT64_MAX);
            // split point k
            // (A[i]...A[k]) * (A[k+1]...A[j])
            for (size_t k = i; k < j; k++) {
                const u64 q =
                    m.get(i, k) + m.get(k + 1, j) + p[i] * p[k + 1] * p[j + 1];
                if (q < m.get(i, j)) {
                    // better solution
                    m.set(i, j, q);
                    s.set(i, j, k);
                }
            }
        }
    }

    return {m.get(0, n - 1), m, s};
}

/**
 * @brief Return the paren repr of matrix chain multiplication plan in string
 *
 * @param s Table s of matrix chain multiplication
 * @return std::string paren repr in string
 */
inline std::string paren_repr(const Table<u64> &s) {
    auto recur_paren_repr = [&](auto &&self, u64 i, u64 j) {
        if (i == j)
            return std::string("A");
        auto repr = std::string("(");
        repr += self(self, i, s.get(i, j));
        repr += self(self, s.get(i, j) + 1, j);
        repr += ")";
        return repr;
    };
    return recur_paren_repr(recur_paren_repr, 0, s.size() - 1);
}