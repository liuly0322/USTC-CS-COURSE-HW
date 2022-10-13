#include <vector>

template <typename T> void merge_sort(std::vector<T> &A) {
    // temp vector for merge, B.size() ==  A.size()
    std::vector<T> B(A.size());
    auto _merge_sort = [&](auto &&self, std::size_t lo, std::size_t hi) {
        if (lo >= hi)
            return;
        auto mid = (lo + hi) / 2 + 1;

        // divide into 2 subproblems
        self(self, lo, mid - 1);
        self(self, mid, hi);

        // merge with temp vector B
        auto left = lo;
        auto right = mid;
        for (auto ans = lo; ans <= hi; ans++) {
            if (left >= mid || (right <= hi && A[left] > A[right])) {
                B[ans] = A[right++];
            } else {
                B[ans] = A[left++];
            }
        }
        // copy B into A
        for (auto i = lo; i <= hi; i++)
            A[i] = B[i];
    };
    _merge_sort(_merge_sort, 0, A.size() - 1);
}