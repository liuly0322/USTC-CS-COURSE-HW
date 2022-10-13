#include <vector>

template <typename T>
inline std::size_t partition(std::vector<T> &A, std::size_t lo,
                             std::size_t hi) {
    T pivot = A[(lo + hi) / 2];
    while (true) {
        while (A[lo] < pivot)
            lo++;
        while (A[hi] > pivot)
            hi--;
        if (lo >= hi)
            return hi;
        std::swap(A[lo], A[hi]);
        lo++, hi--;
    }
}

template <typename T>
void quick_sort_(std::vector<T> &A, std::size_t lo, std::size_t hi) {
    if (lo >= hi || lo < 0)
        return;
    auto pivot = partition(A, lo, hi);
    quick_sort_(A, lo, pivot); // pivot is not the actual pivot position
    quick_sort_(A, pivot + 1, hi);
}

// for main.cpp
template <typename T> void quick_sort(std::vector<T> &A) {
    quick_sort_(A, 0, A.size() - 1);
}