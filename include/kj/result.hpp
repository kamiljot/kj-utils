#pragma once

#include <variant>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <string>

namespace kj {

	/**
	 * @brief A lightweight alternative to std::expected for value-or-error return semantics.
	 *
	 * This template class represents a result that either contains a valid value (`T`) or an error (`E`),
	 * implemented using `std::variant`. It is designed to simplify error handling without relying on exceptions.
	 *
	 * @tparam T The type of the expected value.
	 * @tparam E The type of the error (default: std::string).
	 *
	 * @note The value type `T` and error type `E` must be distinct.
	 */
	template <typename T, typename E = std::string>
	class Result {
		static_assert(!std::is_same_v<T, E>, "T and E must be different types");

	public:
		/**
		 * @brief Constructs a result holding a value.
		 *
		 * @param value A constant reference to the value to store.
		 */
		Result(const T& value) : storage_(value) {}

		/**
		 * @brief Constructs a result holding a value (move version).
		 *
		 * @param value An rvalue reference to the value to move into the result.
		 */
		Result(T&& value) : storage_(std::move(value)) {}

		/**
		 * @brief Constructs a result holding an error.
		 *
		 * @param error A constant reference to the error to store.
		 */
		Result(const E& error) : storage_(error) {}

		/**
		 * @brief Constructs a result holding an error (move version).
		 *
		 * @param error An rvalue reference to the error to move into the result.
		 */
		Result(E&& error) : storage_(std::move(error)) {}

		/**
		 * @brief Checks whether the result holds a value.
		 * @return true if the result contains a value, false if it contains an error.
		 */
		bool has_value() const noexcept {
			return std::holds_alternative<T>(storage_);
		}

		/**
		 * @brief Checks whether the result holds an error.
		 * @return true if the result contains an error, false if it contains a value.
		 */
		bool has_error() const noexcept {
			return std::holds_alternative<E>(storage_);
		}

		/**
		 * @brief Retrieves the value stored in the result.
		 * @return A reference to the contained value.
		 * @throws std::bad_variant_access if the result does not contain a value.
		 */
		T& value() { return std::get<T>(storage_); }

		/**
		 * @brief Retrieves the value stored in the result (const overload).
		 * @return A const reference to the contained value.
		 * @throws std::bad_variant_access if the result does not contain a value.
		 */
		const T& value() const { return std::get<T>(storage_); }

		/**
		 * @brief Retrieves the error stored in the result.
		 * @return A reference to the contained error.
		 * @throws std::bad_variant_access if the result does not contain an error.
		 */
		E& error() { return std::get<E>(storage_); }

		/**
		 * @brief Retrieves the error stored in the result (const overload).
		 * @return A const reference to the contained error.
		 * @throws std::bad_variant_access if the result does not contain an error.
		 */
		const E& error() const { return std::get<E>(storage_); }

	private:
		std::variant<T, E> storage_;
	};

	/**
	 * @brief Creates a `Result` representing a successful outcome.
	 *
	 * @tparam T The value type.
	 * @tparam E The error type (defaults to std::string).
	 * @param value The value to wrap in a successful result.
	 * @return A `Result<T, E>` holding the value.
	 */
	template <typename T, typename E = std::string>
	Result<T, E> ok(T&& value) {
		return Result<T, E>(std::forward<T>(value));
	}

	/**
	 * @brief Creates a `Result` representing a failed outcome.
	 *
	 * @tparam T The value type.
	 * @tparam E The error type (defaults to std::string).
	 * @param err The error to wrap in the result.
	 * @return A `Result<T, E>` holding the error.
	 */
	template <typename T, typename E = std::string>
	Result<T, E> error(E&& err) {
		return Result<T, E>(std::forward<E>(err));
	}

} // namespace kj
