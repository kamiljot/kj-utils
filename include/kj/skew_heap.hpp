#pragma once
#include <functional>
#include <kj/detail/skew_heap_impl.hpp>

namespace kj {

	/**
	 * @brief Public alias for a standard skew heap (min-heap by default).
	 */
	template<class T, class Comp = std::less<T>>
	using SkewHeap = ::kj::detail::SkewHeap<T, Comp, /*UsePool=*/false>;

	/**
	 * @brief Public alias for a skew heap backed by an internal object pool.
	 *
	 * Use this when you expect a very large number of node allocations (push/pop/merge),
	 * and want to reduce allocation overhead. You can pre-reserve nodes via @c reserve_nodes.
	 */
	template<class T, class Comp = std::less<T>>
	using SkewHeapArena = ::kj::detail::SkewHeap<T, Comp, /*UsePool=*/true>;

} // namespace kj
