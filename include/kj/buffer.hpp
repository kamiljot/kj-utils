#pragma once

#include <cstddef>
#include <memory>
#include <utility>
#include <span>
#include <stdexcept>
#include <cstdlib>

#if defined(_MSC_VER)
#include <malloc.h>  // _aligned_malloc / _aligned_free for MSVC
#endif

namespace kj {

	/**
	 * @brief A simple RAII wrapper around aligned heap-allocated memory.
	 *
	 * This class manages a typed, contiguous block of memory allocated with
	 * a specific alignment requirement. It cannot be copied but can be moved.
	 * Memory is automatically released on destruction.
	 *
	 * @tparam T The element type stored in the buffer.
	 */
	template <typename T>
	class Buffer {
	public:
		/**
		 * @brief Constructs a buffer with the given size and alignment.
		 *
		 * Allocates memory for `size` elements of type `T`, aligned to the given alignment.
		 *
		 * @param size Number of elements to allocate.
		 * @param alignment Desired byte alignment. Defaults to `alignof(T)`.
		 *
		 * @throws std::bad_alloc if memory allocation fails.
		 */
		Buffer(std::size_t size, std::size_t alignment = alignof(T)) : size_(size) {
			void* raw = nullptr;

#if defined(_MSC_VER)
			raw = _aligned_malloc(size * sizeof(T), alignment);
			if (!raw) throw std::bad_alloc{};
#else
			if (posix_memalign(&raw, alignment, size * sizeof(T)) != 0)
				throw std::bad_alloc{};
#endif
			data_ = static_cast<T*>(raw);
		}

		/// Deleted copy constructor.
		Buffer(const Buffer&) = delete;

		/// Deleted copy assignment.
		Buffer& operator=(const Buffer&) = delete;

		/**
		 * @brief Move constructor.
		 *
		 * Transfers ownership of the buffer from another instance.
		 */
		Buffer(Buffer&& other) noexcept : data_(other.data_), size_(other.size_) {
			other.data_ = nullptr;
			other.size_ = 0;
		}

		/**
		 * @brief Move assignment operator.
		 *
		 * Releases current memory and takes ownership of another buffer's memory.
		 */
		Buffer& operator=(Buffer&& other) noexcept {
			if (this != &other) {
				cleanup();
				data_ = other.data_;
				size_ = other.size_;
				other.data_ = nullptr;
				other.size_ = 0;
			}
			return *this;
		}

		/// Destructor. Frees the allocated memory if any.
		~Buffer() { cleanup(); }

		/// @return Pointer to the beginning of the buffer (mutable).
		T* data() noexcept { return data_; }

		/// @return Pointer to the beginning of the buffer (const).
		const T* data() const noexcept { return data_; }

		/// @return Number of elements in the buffer.
		std::size_t size() const noexcept { return size_; }

		/**
		 * @brief Element access operator.
		 * @param idx Index into the buffer (no bounds checking).
		 * @return Reference to the element at the given index.
		 */
		T& operator[](std::size_t idx) noexcept { return data_[idx]; }

		/// @copydoc operator[](std::size_t)
		const T& operator[](std::size_t idx) const noexcept { return data_[idx]; }

		/// @return A mutable std::span representing the buffer.
		std::span<T> span() noexcept { return { data_, size_ }; }

		/// @return A read-only std::span representing the buffer.
		std::span<const T> span() const noexcept { return { data_, size_ }; }

	private:
		T* data_ = nullptr;              ///< Pointer to the allocated memory.
		std::size_t size_ = 0;           ///< Number of elements allocated.

		/// Frees the buffer memory if allocated.
		void cleanup() {
			if (data_) {
#if defined(_MSC_VER)
				_aligned_free(data_);
#else
				std::free(data_);
#endif
			}
			data_ = nullptr;
			size_ = 0;
		}
	};

} // namespace kj