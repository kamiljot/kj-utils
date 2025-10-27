/**
 * @file test_skew_heap.cpp
 * @brief Unit tests for kj::SkewHeap<T, Comp>.
 *
 * Verifies ordering, merge behavior, comparator customization (max-heap),
 * emplace/Move semantics, and basic size/empty observers.
 */

#include <catch2/catch_all.hpp>
#include <kj/skew_heap.hpp>
#include <vector>
#include <algorithm>
#include <utility>
#include <string>

 /**
  * @test Verifies min-heap ordering with push/pop.
  */
TEST_CASE("kj::SkewHeap basic min-heap ordering", "[skew_heap][min]") {
	kj::SkewHeap<int> h;
	for (int x : {5, 3, 7, 2, 9, 1, 8}) h.push(x);

	std::vector<int> got;
	while (!h.empty()) {
		got.push_back(h.top());
		h.pop();
	}
	REQUIRE(got == std::vector<int>({ 1, 2, 3, 5, 7, 8, 9 }));
}

/**
 * @test Verifies merge operation and that the donor heap becomes empty.
 */
TEST_CASE("kj::SkewHeap merge and donor emptied", "[skew_heap][merge]") {
	kj::SkewHeap<int> a, b;
	for (int x : {5, 1, 9}) a.push(x);
	for (int x : {2, 6, 3}) b.push(x);

	a.merge(b);
	REQUIRE(b.empty());

	std::vector<int> got;
	while (!a.empty()) {
		got.push_back(a.top());
		a.pop();
	}
	REQUIRE(got == std::vector<int>({ 1, 2, 3, 5, 6, 9 }));
}

/**
 * @test Verifies that merging with self is a no-op and does not corrupt state.
 */
TEST_CASE("kj::SkewHeap self-merge is no-op", "[skew_heap][merge]") {
	kj::SkewHeap<int> h;
	for (int x : {4, 1, 7}) h.push(x);

	// Capture state
	std::vector<int> before;
	{
		kj::SkewHeap<int> tmp = kj::SkewHeap<int>();
		// Copy state by popping into buffer (we’ll rebuild to check later).
		kj::SkewHeap<int> mirror;
		// Make a shallow snapshot by popping (we’ll use 'before', then rebuild 'h').
		while (!h.empty()) { before.push_back(h.top()); h.pop(); }
		// Rebuild h
		for (int v : before) h.push(v);
	}

	// Self-merge
	h.merge(h); // should be no-op

	std::vector<int> after;
	while (!h.empty()) { after.push_back(h.top()); h.pop(); }

	REQUIRE(before == after);
}

/**
 * @test Verifies max-heap behavior via custom comparator.
 */
TEST_CASE("kj::SkewHeap max-heap comparator", "[skew_heap][max]") {
	kj::SkewHeap<int, std::greater<int>> h; // max-heap
	for (int x : {5, 3, 7, 2, 9, 1, 8}) h.push(x);

	std::vector<int> got;
	while (!h.empty()) { got.push_back(h.top()); h.pop(); }
	REQUIRE(got == std::vector<int>({ 9, 8, 7, 5, 3, 2, 1 }));
}

/**
 * @test Verifies emplace and move-push on non-trivial types.
 */
TEST_CASE("kj::SkewHeap emplace and move semantics", "[skew_heap][emplace][move]") {
	using P = std::pair<int, std::string>;
	// Min-heap by first, then lexicographic by second (std::pair has operator<)
	kj::SkewHeap<P> h;

	h.emplace(3, "ccc");
	std::string s = "bbb";
	h.push(std::make_pair(1, "aaa"));
	h.push(P{ 2, std::move(s) });
	h.emplace(1, "zzz");

	// Expected order by pair<first,second>
	std::vector<P> got;
	while (!h.empty()) { got.push_back(h.top()); h.pop(); }

	REQUIRE(got.size() == 4);
	REQUIRE(got[0] == P(1, "aaa"));
	REQUIRE(got[1] == P(1, "zzz"));
	REQUIRE(got[2] == P(2, "bbb"));  // moved-from 's' ends up in heap; its content is "bbb"
	REQUIRE(got[3] == P(3, "ccc"));
}

/**
 * @test Verifies size() and empty() change consistently.
 */
TEST_CASE("kj::SkewHeap size and empty observers", "[skew_heap][observers]") {
	kj::SkewHeap<int> h;
	REQUIRE(h.empty());
	REQUIRE(h.size() == 0);

	h.push(10);
	h.push(5);
	REQUIRE_FALSE(h.empty());
	REQUIRE(h.size() == 2);
	REQUIRE(h.top() == 5);

	h.pop();
	REQUIRE(h.size() == 1);
	REQUIRE(h.top() == 10);

	h.pop();
	REQUIRE(h.empty());
	REQUIRE(h.size() == 0);
}

/**
 * @test Verifies pool-backed variant behaves identically and supports reservation.
 */
TEST_CASE("kj::SkewHeapArena behaves like SkewHeap and supports reserve/release", "[skew_heap][pool]") {
	kj::SkewHeapArena<int> h;
	h.reserve_nodes(1024);

	for (int x : {5, 3, 7, 2, 9, 1, 8}) h.push(x);
	std::vector<int> got;
	while (!h.empty()) { got.push_back(h.top()); h.pop(); }
	REQUIRE(got == std::vector<int>({ 1, 2, 3, 5, 7, 8, 9 }));

	// Heap is empty now; we can release pool memory.
	h.release_all_to_pool();
}
