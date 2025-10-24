#pragma once

#include <cstddef>
#include <cstdlib>
#include <new>
#include <stdexcept>
#include <utility>

#if defined(_MSC_VER)
#include <malloc.h> // For _aligned_malloc and _aligned_free
#endif

namespace kj {

	/**
	 * @brief Allocates memory with a specific byte alignment.
	 *
	 * This function provides platform-specific aligned allocation:
	 * - On Windows, it uses `_aligned_malloc`.
	 * - On POSIX systems, it uses `posix_memalign`.
	 *
	 * @param size The size of the memory block in bytes.
	 * @param alignment The alignment requirement in bytes (must be power of two).
	 * @return A pointer to the allocated memory block.
	 * @throws std::bad_alloc if allocation fails.
	 */
	inline void* aligned_alloc(std::size_t size, std::size_t alignment) {
		void* ptr = nullptr;
#if defined(_MSC_VER)
		ptr = _aligned_malloc(size, alignment);
		if (!ptr) throw std::bad_alloc{};
#else
		if (posix_memalign(&ptr, alignment, size) != 0)
			throw std::bad_alloc{};
#endif
		return ptr;
	}

	/**
	 * @brief Frees memory allocated with `aligned_alloc`.
	 *
	 * Uses platform-specific deallocation matching the allocation method:
	 * - `_aligned_free` on Windows.
	 * - `std::free` on POSIX.
	 *
	 * @param ptr Pointer to the previously allocated aligned memory.
	 */
	inline void aligned_free(void* ptr) noexcept {
#if defined(_MSC_VER)
		_aligned_free(ptr);
#else
		std::free(ptr);
#endif
	}

	/**
	 * @brief Allocates aligned memory and constructs an object in-place.
	 *
	 * Allocates memory aligned to `alignment` bytes and constructs an object
	 * of type `T` using placement new with the provided arguments.
	 *
	 * @tparam T The type of the object to construct.
	 * @tparam Args Argument types forwarded to T's constructor.
	 * @param alignment The desired memory alignment in bytes. Defaults to alignof(T).
	 * @param args Arguments forwarded to T's constructor.
	 * @return Pointer to the constructed object of type `T`.
	 * @throws std::bad_alloc if allocation fails.
	 */
	template <typename T, typename... Args>
	T* aligned_new(std::size_t alignment = alignof(T), Args&&... args) {
		void* mem = aligned_alloc(sizeof(T), alignment);
		return new (mem) T(std::forward<Args>(args)...);
	}

	/**
	 * @brief Calls the destructor and deallocates aligned memory for an object.
	 *
	 * Destroys the object pointed to by `ptr` and frees the aligned memory
	 * previously allocated by `aligned_new`.
	 *
	 * @tparam T The type of the object.
	 * @param ptr Pointer to the object to destroy and deallocate.
	 */
	template <typename T>
	void aligned_delete(T* ptr) noexcept {
		if (ptr) {
			ptr->~T();
			aligned_free(static_cast<void*>(ptr));
		}
	}

} // namespace kj