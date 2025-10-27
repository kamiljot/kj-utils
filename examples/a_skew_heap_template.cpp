/**
 * @file a_skew_heap_template.cpp
 * @brief Minimal ICPC template showcasing kj::SkewHeap and fast I/O.
 *
 * Reads N, then N integers, pushes into a heap, and pops in order.
 * Switch between kj::SkewHeap<int> and kj::SkewHeapArena<int> as needed.
 */

#include <kj/io/fast_io.hpp>
#include <kj/skew_heap.hpp>
 // #include <functional> // for std::greater if you want a max-heap

int main() {
    kj::io::FastInput  in;
    kj::io::FastOutput out;

    int n;
    if (!in.read(n)) return 0;

    // Min-heap (default):
    kj::SkewHeap<int> pq;
    // Or arena-backed heap:
    // kj::SkewHeapArena<int> pq; pq.reserve_nodes(n);

    for (int i = 0; i < n; ++i) {
        int x; in.read(x);
        pq.push(x);
    }

    while (!pq.empty()) {
        out.write_int(pq.top()).put_char(' ');
        pq.pop();
    }
    out.write_line();
    return 0;
}
