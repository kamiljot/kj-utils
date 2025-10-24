/**
 * @file test_timer.cpp
 * @brief Unit tests for the kj::Timer and kj::ScopedTimer utilities.
 *
 * Tests verify time measurement behavior and correct construction/destruction of scoped timers.
 */

#include <catch2/catch_all.hpp>
#include <kj/timer.hpp>
#include <thread>
#include <chrono>

 /**
  * @test Measures a short time span using kj::Timer and checks that the elapsed time is within bounds.
  *
  * This test allows some tolerance due to potential system scheduling noise.
  */
TEST_CASE("kj::Timer measures time span", "[timer]") {
	using namespace std::chrono_literals;

	kj::Timer timer;
	timer.start();
	std::this_thread::sleep_for(50ms);
	auto elapsed = timer.elapsed();

	REQUIRE(elapsed >= 40ms);
	REQUIRE(elapsed <= 200ms); // tolerance for system noise
}

/**
 * @test Verifies that kj::ScopedTimer can be constructed and destroyed without error.
 *
 * This test does not verify actual output to std::cerr; it ensures correct RAII behavior.
 */
TEST_CASE("kj::ScopedTimer prints label on destruction", "[scoped_timer]") {
	{
		kj::ScopedTimer scoped("TestScope");
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	SUCCEED("ScopedTimer constructed and destroyed without error.");
}
