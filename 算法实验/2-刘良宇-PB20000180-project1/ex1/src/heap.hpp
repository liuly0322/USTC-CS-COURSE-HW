#include <vector>

template <typename T> void heap_sort(std::vector<T> &A) {
    auto heap_size = A.size();
    auto max_heapify = [&](std::size_t index) {
        while (true) {
            auto l = index * 2 + 1;
            auto r = index * 2 + 2;
            auto largest = index;
            if (l < heap_size && A[l] > A[index]) {
                largest = l;
            }
            if (r < heap_size && A[r] > A[largest]) {
                largest = r;
            }
            if (largest == index) {
                return;
            }
            std::swap(A[index], A[largest]);
            index = largest;
        }
    };
    // build max heap
    for (auto i = A.size() / 2; i-- > 0;) {
        max_heapify(i);
    }
    // sort by poping the biggest and continue
    for (auto i = A.size() - 1; i > 0; i--) {
        std::swap(A[0], A[i]);
        heap_size -= 1;
        max_heapify(0);
    }
}