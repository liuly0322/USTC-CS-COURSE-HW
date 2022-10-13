#include <array>
#include <vector>

inline void counting_sort(std::vector<int> &A) {
    constexpr int value_max = 1 << 15;
    std::array<int, value_max> cnt{};
    auto n = A.size();
    auto B = std::vector<int>(n);
    // count
    for (auto i = 0; i < n; i++)
        cnt[A[i]]++;
    // prefix sum
    for (auto i = 1; i < value_max; i++)
        cnt[i] += cnt[i - 1];
    // sort
    for (auto i = n; i-- > 0;)
        B[--cnt[A[i]]] = A[i];
    // copy
    for (auto i = 0; i < n; i++)
        A[i] = B[i];
}