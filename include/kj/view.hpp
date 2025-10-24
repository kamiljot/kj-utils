#pragma once

#include <span>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace kj {

	/**
	 * @brief Alias for a mutable view over a contiguous range of elements.
	 *
	 * A convenient shorthand for `std::span<T>`, improves code readability in function
	 * signatures and templates involving memory views or slices.
	 *
	 * @tparam T Element type.
	 */
	template <typename T>
	using View = std::span<T>;

	/**
	 * @brief Alias for an immutable view over a contiguous range of elements.
	 *
	 * This is equivalent to `std::span<const T>`. It improves clarity in APIs
	 * that accept read-only memory regions.
	 *
	 * @tparam T Element type.
	 */
	template <typename T>
	using ConstView = std::span<const T>;

	/**
	 * @brief Returns the total size in bytes of the given span.
	 *
	 * Useful when performing low-level operations such as serialization or
	 * interfacing with raw memory.
	 *
	 * @tparam T Element type.
	 * @param s The span to measure.
	 * @return Total number of bytes in the span.
	 */
	template <typename T>
	std::size_t byte_size(std::span<T> s) noexcept {
		return s.size_bytes();
	}

	/**
	 * @brief Checks whether a span is empty.
	 *
	 * This function provides a more expressive alternative to `s.empty()`.
	 *
	 * @tparam T Element type.
	 * @param s The span to examine.
	 * @return true if the span contains no elements, false otherwise.
	 */
	template <typename T>
	bool empty(std::span<T> s) noexcept {
		return s.empty();
	}

} // namespace kj
