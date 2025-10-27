#pragma once
#include <vector>
#include <cstddef>
#include <new>
#include <utility>
#include <algorithm>
#include <cassert>

namespace kj::detail {

	/**
	 * @brief Fixed-size object pool with free-list and geometric block growth.
	 *
	 * This pool provides fast allocation/deallocation for trivially relocatable
	 * node-like objects. It does not call constructors at block allocation time;
	 * objects are constructed with placement-new in @ref create and destroyed in @ref destroy.
	 *
	 * @tparam T Object type managed by this pool.
	 *
	 * @note All outstanding objects must be destroyed before calling @ref release_all.
	 *       In DEBUG builds an assertion will fire if the pool still owns live objects.
	 */
	template <class T>
	class ObjectPool {
	public:
		/// Constructs an empty pool. Initial block capacity grows geometrically.
		explicit ObjectPool(std::size_t initial_block_capacity = 4096)
			: next_block_cap_(initial_block_capacity) {
		}

		ObjectPool(const ObjectPool&) = delete;
		ObjectPool& operator=(const ObjectPool&) = delete;

		ObjectPool(ObjectPool&&) = delete;
		ObjectPool& operator=(ObjectPool&&) = delete;

		/// Destroys the pool and releases all raw memory. All objects must be destroyed beforehand.
		~ObjectPool() { release_all(); }

		/**
		 * @brief Ensures at least @p n free slots are available.
		 *        Allocates one or more blocks and pushes their slots to the free-list.
		 */
		void reserve(std::size_t n) {
			if (free_.size() >= n) return;
			const std::size_t need = n - free_.size();
			grow_(need);
		}

		/**
		 * @brief Creates a new object with forwarded arguments and returns its pointer.
		 */
		template <class... Args>
		T* create(Args&&... args) {
			if (free_.empty()) grow_(1);
			T* p = free_.back(); free_.pop_back();
			::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
			++live_;
			return p;
		}

		/**
		 * @brief Destroys the object and returns its slot to the free-list.
		 */
		void destroy(T* p) noexcept {
			if (!p) return;
			p->~T();
			free_.push_back(p);
			--live_;
		}

		/**
		 * @brief Releases all blocks back to the OS.
		 *
		 * @warning All objects must have been @ref destroy 'ed prior to this call.
		 *          In DEBUG builds, this is asserted.
		 */
		void release_all() noexcept {
#ifndef NDEBUG
			assert(live_ == 0 && "ObjectPool::release_all(): there are live objects");
#endif
			for (auto& blk : blocks_) {
				::operator delete[](blk.ptr, std::align_val_t{ alignof(T) });
			}
			blocks_.clear();
			free_.clear();
			// keep next_block_cap_ as-is to preserve growth pattern on re-use
		}

		/// @return Number of currently allocated (live) objects.
		std::size_t live() const noexcept { return live_; }

		/// @return Total number of slots ever made available by this pool.
		std::size_t capacity_slots() const noexcept { return total_slots_; }

	private:
		struct Block {
			T* ptr;
			std::size_t count;
		};

		std::vector<Block> blocks_;   // raw memory blocks of T slots (uninitialized)
		std::vector<T*>    free_;     // free-list of available slots
		std::size_t        next_block_cap_ = 4096;
		std::size_t        total_slots_ = 0;
		std::size_t        live_ = 0;

		void grow_(std::size_t min_new_slots) {
			// Allocate enough blocks to satisfy min_new_slots, with geometric growth.
			while (min_new_slots > 0) {
				const std::size_t count = std::max(next_block_cap_, min_new_slots);
				T* mem = static_cast<T*>(::operator new[](sizeof(T)* count, std::align_val_t{ alignof(T) }));
				blocks_.push_back(Block{ mem, count });
				// push each slot to the free list (no construction yet)
				for (std::size_t i = 0; i < count; ++i) free_.push_back(mem + i);
				total_slots_ += count;
				min_new_slots = (min_new_slots > count ? min_new_slots - count : 0);
				// next block grows geometrically
				if (next_block_cap_ < (static_cast<std::size_t>(1) << 28)) next_block_cap_ *= 2;
			}
		}
	};

} // namespace kj::detail
