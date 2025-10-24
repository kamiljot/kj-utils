/**
 * @file test_scope_exit.cpp
 * @brief Unit tests for the kj::ScopeGuard utility.
 *
 * Verifies behavior of scope-based cleanup execution and dismissal.
 */

#include <catch2/catch_all.hpp>
#include <kj/scope_guard.hpp>

 /**
  * @test Ensures that the cleanup function is called when the guard goes out of scope.
  */
TEST_CASE("kj::ScopeGuard executes on scope exit", "[scope_exit]") {
	bool cleaned = false;

	{
		auto guard = kj::scope_exit([&] {
			cleaned = true;
			});
		REQUIRE_FALSE(cleaned);
	}

	REQUIRE(cleaned);
}

/**
 * @test Ensures that calling dismiss() disables cleanup execution.
 */
TEST_CASE("kj::ScopeGuard releases early when requested", "[scope_exit]") {
	bool cleaned = false;

	{
		auto guard = kj::scope_exit([&] {
			cleaned = true;
			});
		guard.dismiss();  // manually cancel cleanup
	}

	REQUIRE_FALSE(cleaned);
}