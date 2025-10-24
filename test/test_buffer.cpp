/**
 * @file test_buffer.cpp
 * @brief Unit tests for the kj::Buffer class.
 *
 * These tests verify that kj::Buffer allocates the correct amount of memory,
 * supports move semantics, and respects alignment constraints.
 */

#include <catch2/catch_all.hpp>
#include <kj/buffer.hpp>

 /**
  * @brief Verifies that kj::Buffer allocates the requested number of elements.
  *
  * This test checks:
  * - That the buffer reports the correct size.
  * - That values can be written to and read from each element.
  */
TEST_CASE("kj::Buffer allocates correct size", "[buffer]") {
	const std::size_t size = 16;
	kj::Buffer<int> buffer(size);

	REQUIRE(buffer.size() == size);
	for (std::size_t i = 0; i < size; ++i) {
		buffer[i] = static_cast<int>(i);
	}

	for (std::size_t i = 0; i < size; ++i) {
		REQUIRE(buffer[i] == static_cast<int>(i));
	}
}

/**
 * @brief Verifies that kj::Buffer supports move semantics correctly.
 *
 * This test ensures that:
 * - A moved-from buffer transfers ownership of data.
 * - The destination buffer retains content and size.
 */
TEST_CASE("kj::Buffer supports move semantics", "[buffer][move]") {
	kj::Buffer<int> a(4);
	a[0] = 42;

	kj::Buffer<int> b = std::move(a);
	REQUIRE(b.size() == 4);
	REQUIRE(b[0] == 42);
}

/**
 * @brief Verifies that kj::Buffer respects alignment constraints.
 *
 * This test ensures that the memory pointer returned from the buffer
 * is aligned to the specified number of bytes.
 */
TEST_CASE("kj::Buffer alignment is respected", "[buffer][alignment]") {
	constexpr std::size_t alignment = 64;
	kj::Buffer<float> buffer(8, alignment);

	void* ptr = static_cast<void*>(buffer.data());
	REQUIRE(reinterpret_cast<std::uintptr_t>(ptr) % alignment == 0);
}