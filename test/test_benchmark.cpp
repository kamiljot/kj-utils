/**
 * @file test_benchmark.cpp
 * @brief Unit test for kj::Benchmark utility.
 *
 * This test verifies that the kj::Benchmark class properly performs warmup iterations,
 * executes the benchmarked code the expected number of times, and computes timing results.
 */

#include <catch2/catch_all.hpp>
#include <kj/benchmark.hpp>

 /**
  * @brief Test case for kj::Benchmark behavior.
  *
  * This test ensures the following:
  * - The warmup iterations are executed.
  * - The benchmark function is called the correct total number of times.
  * - The timing samples are collected.
  * - The average duration is greater than zero.
  */
TEST_CASE("kj::Benchmark runs warmup and measures", "[benchmark]") {
	int counter = 0;

	kj::Benchmark bench;
	auto result = bench.run("IncrTest", [&] {
		++counter;
		}, 10);  // 5 warmups + 10 measured iterations

	REQUIRE(counter == 15);                  // Total invocations = 5 (warmup) + 10 (measured)
	REQUIRE(result.samples.size() == 10);    // Ensure 10 timing samples were collected
	REQUIRE(result.avg.count() > 0);         // Average should be greater than zero
}