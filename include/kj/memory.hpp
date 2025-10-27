#pragma once

#include <cstddef>
#include <cstdlib>
#include <new>

#if defined(_WIN32)
#include <malloc.h> // _aligned_malloc/_aligned_free on Windows (MSVC/MinGW)
#endif

namespace kj {

	/**
	 * @brief Portable aligned allocation.
	 *
	 * - On Windows (_WIN32): uses `_aligned_malloc(size, alignment)`.
	 * - On POSIX: uses `posix_memalign(&ptr, alignment, size)`.
	 *
	 * @param alignment Alignment in bytes (power of two, >= sizeof(void*)).
	 * @param size      Number of bytes to allocate.
	 * @return pointer to aligned memory, or nullptr on failure.
	 */
	inline void* aligned_alloc(std::size_t alignment, std::size_t size) noexcept {
#if defined(_WIN32)
		// Windows path (MSVC/MinGW): returns nullptr on failure.
		return _aligned_malloc(size, alignment);
#else
		if (size == 0) return nullptr;
		void* ptr = nullptr;
		const int rc = ::posix_memalign(&ptr, alignment, size);
		if (rc != 0) return nullptr;
		return ptr;
#endif
	}

	/**
	 * @brief Portable aligned free matching kj::aligned_alloc.
	 */
	inline void aligned_free(void* p) noexcept {
#if defined(_WIN32)
		_aligned_free(p);
#else
		std::free(p);
#endif
	}

	/**
	 * @brief Allocates aligned storage and constructs an object in-place.
	 *
	 * @tparam T Object type to construct.
	 * @tparam Args Argument types forwarded to T's constructor.
	 * @param alignment Desired alignment in bytes (defaults to alignof(T)).
	 * @param args Constructor arguments.
	 * @return Pointer to constructed object, or throws std::bad_alloc on failure.
	 */
	template <typename T, typename... Args>
	T* aligned_new(std::size_t alignment = alignof(T), Args&&... args) {
		void* mem = kj::aligned_alloc(alignment, sizeof(T));
		if (!mem) throw std::bad_alloc{};
		return new (mem) T(std::forward<Args>(args)...);
	}

	/**
	 * @brief Destroys the object and frees aligned storage acquired by aligned_new.
	 */
	template <typename T>
	void aligned_delete(T* ptr) noexcept {
		if (!ptr) return;
		ptr->~T();
		kj::aligned_free(static_cast<void*>(ptr));
	}

} // namespace kj
