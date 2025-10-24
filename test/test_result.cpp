/**
 * @file test_result.cpp
 * @brief Unit tests for the kj::Result<T, E> class.
 *
 * Validates behavior of success and error handling, including move semantics.
 */

#include <catch2/catch_all.hpp>
#include <kj/result.hpp>
#include <string>

 /**
  * @test Verifies that kj::Result correctly holds a successful value.
  */
TEST_CASE("kj::Result holds value", "[result]") {
	auto res = kj::ok<int>(42);
	REQUIRE(res.has_value());
	REQUIRE(res.value() == 42);
}

/**
 * @test Verifies that kj::Result correctly holds an error value.
 */
TEST_CASE("kj::Result holds error", "[result]") {
	auto res = kj::error<int>(std::string("fail"));
	REQUIRE(res.has_error());
	REQUIRE(res.error() == "fail");
}

/**
 * @test Verifies that kj::Result can be move-constructed while preserving its content.
 */
TEST_CASE("kj::Result can be moved", "[result][move]") {
	auto res = kj::ok<std::string, int>("test");
	auto moved = std::move(res);

	REQUIRE(moved.has_value());
	REQUIRE(moved.value() == "test");
}