#pragma once
#include <kj/detail/dsu_impl.hpp>

namespace kj {

	/**
	 * @brief Public alias for the classic (path-compressing) DSU implementation.
	 *
	 * @see kj::detail::DSU
	 */
	using DSU = ::kj::detail::DSU;

	/**
	 * @brief Public alias for the rollback-capable DSU (no path compression).
	 *
	 * @see kj::detail::RollbackDSU
	 */
	using RollbackDSU = ::kj::detail::RollbackDSU;

} // namespace kj
