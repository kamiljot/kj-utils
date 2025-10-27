#pragma once

#include <cstddef>
#include <span>
#include <utility>
#include <stdexcept>
#include <algorithm> // std::max

#include <kj/memory.hpp>

namespace kj {

	/**
	 * @brief A simple RAII wrapper around aligned heap-allocated memory.
	 *
	 * Manages a typed, contiguous block allocated with a specific alignment.
	 * Non-copyable, movable. Memory is released automatically on destruction.
	 *
	 * @tparam T Element type stored in the buffer.
	 */
	template <typename T>
	class Buffer {
	public:
		/**
		 * @brief Constructs a buffer with the given size and alignment.
		 *
		 * Allocates memory for `size` elements of type `T`, aligned to `alignment`.
		 * If `size == 0`, no allocation is performed and `data() == nullptr`.
		 *
		 * @param size      Number of elements to allocate.
		 * @param alignment Desired byte alignment. Defaults to max(alignof(T), alignof(void*)).
		 *
		 * @throws std::bad_alloc if allocation fails.
		 */
		explicit Buffer(std::size_t size,
			std::size_t alignment = std::max<std::size_t>(alignof(T), alignof(void*)))
			: data_(nullptr), size_(size)
		{
			if (size_ == 0) return;

			// Ensure alignment is at least alignof(T)
			if (alignment < alignof(T)) alignment = alignof(T);

			const std::size_t bytes = size_ * sizeof(T);
			void* raw = kj::aligned_alloc(alignment, bytes);
			if (!raw) throw std::bad_alloc{};
			data_ = static_cast<T*>(raw);
		}

		// Non-copyable
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

		// Movable
		Buffer(Buffer&& other) noexcept
			: data_(other.data_), size_(other.size_) {
			other.data_ = nullptr;
			other.size_ = 0;
		}

		Buffer& operator=(Buffer&& other) noexcept {
			if (this != &other) {
				cleanup_();
				data_ = other.data_;
				size_ = other.size_;
				other.data_ = nullptr;
				other.size_ = 0;
			}
			return *this;
		}

		~Buffer() { cleanup_(); }

		/// @return Pointer to the beginning of the buffer (mutable).
		T* data() noexcept { return data_; }

		/// @return Pointer to the beginning of the buffer (const).
		const T* data() const noexcept { return data_; }

		/// @return Number of elements in the buffer.
		std::size_t size() const noexcept { return size_; }

		/// Element access (no bounds checking).
		T& operator[](std::size_t i) noexcept { return data_[i]; }
		const T& operator[](std::size_t i) const noexcept { return data_[i]; }

		/// Views as std::span
		std::span<T> span() noexcept { return { data_, size_ }; }
		std::span<const T> span() const noexcept { return { data_, size_ }; }

	private:
		T* data_ = nullptr;
		std::size_t  size_ = 0;

		void cleanup_() noexcept {
			if (data_) {
				kj::aligned_free(static_cast<void*>(data_));
				data_ = nullptr;
				size_ = 0;
			}
		}
	};

} // namespace kj
