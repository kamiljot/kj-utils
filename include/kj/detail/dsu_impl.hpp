#pragma once
#include <vector>
#include <utility>
#include <cstddef>

namespace kj::detail {

	/**
	 * @brief Disjoint Set Union (Union-Find) with union-by-size and path compression.
	 *
	 * Stores a partition of {0..n-1} and supports near-O(1) amortized operations:
	 * - @ref find : get the representative of a set,
	 * - @ref unite : merge two sets,
	 * - @ref same : check if two elements are in the same set,
	 * - @ref size : get the size of the set containing a given element.
	 *
	 * Implementation uses a single vector<int> `p`, where:
	 * - `p[x] < 0` encodes that x is a root and `-p[x]` is the size of the set,
	 * - `p[x] >= 0` encodes that `p[x]` is the parent of x.
	 */
	struct DSU {
		/// Parent/size array (see class description).
		std::vector<int> p;

		/**
		 * @brief Constructs a DSU of @p n singleton sets (0..n-1).
		 * @param n Number of elements (defaults to 0).
		 */
		explicit DSU(int n = 0) : p(n, -1) {}

		/**
		 * @brief Resets the structure to @p n singleton sets.
		 * @param n Number of elements.
		 */
		void reset(int n) { p.assign(n, -1); }

		/**
		 * @brief Finds the representative (root) of the set containing @p x.
		 *
		 * Uses path compression to flatten the tree and speed up future queries.
		 *
		 * @param x Element id in [0, size()).
		 * @return The index of the root representative of @p x.
		 */
		int find(int x) {
			int r = x;
			while (p[r] >= 0) r = p[r];        // climb to root
			while (x != r) {                   // path compression
				int up = p[x];
				p[x] = r;
				x = up;
			}
			return r;
		}

		/**
		 * @brief Merges the sets containing @p a and @p b.
		 *
		 * Uses union-by-size: smaller tree is attached under the larger tree's root.
		 *
		 * @param a Element id.
		 * @param b Element id.
		 * @return @c true if a merge actually happened (different sets), @c false otherwise.
		 */
		bool unite(int a, int b) {
			a = find(a); b = find(b);
			if (a == b) return false;
			// p[root] is negative size: "greater" means smaller absolute size.
			if (p[a] > p[b]) std::swap(a, b);
			p[a] += p[b];   // increase size at root a
			p[b] = a;      // make a the parent of b
			return true;
		}

		/**
		 * @brief Checks if @p a and @p b belong to the same set.
		 */
		bool same(int a, int b) { return find(a) == find(b); }

		/**
		 * @brief Returns the size of the set containing @p x.
		 */
		int size(int x) { return -p[find(x)]; }

		/**
		 * @brief Returns the current universe size (number of elements).
		 */
		std::size_t universe() const { return p.size(); }
	};


	/**
	 * @brief Rollback-able Disjoint Set Union (no path compression).
	 *
	 * This variant allows taking snapshots and rolling back to them. It is intended
	 * for offline algorithms (e.g., divide & conquer on time, segment tree of queries).
	 * Path compression is intentionally omitted to keep rollback simple/cheap.
	 *
	 * API:
	 * - @ref snapshot to mark a point in time,
	 * - @ref rollback to revert to a previous snapshot,
	 * - @ref unite / @ref same / @ref size similar to @ref DSU (without compression).
	 */
	struct RollbackDSU {
		/// Parent/size array: roots store negative sizes, non-roots store parent index.
		std::vector<int> p;
		/// Modification stack: (index, previous_value) pairs to restore on rollback.
		std::vector<std::pair<int, int>> stk;

		/**
		 * @brief Constructs a rollback DSU of @p n singleton sets (0..n-1).
		 * @param n Number of elements (defaults to 0).
		 */
		explicit RollbackDSU(int n = 0) : p(n, -1) {}

		/**
		 * @brief Resets to @p n singleton sets and clears the modification stack.
		 */
		void reset(int n) { p.assign(n, -1); stk.clear(); }

		/**
		 * @brief Returns a snapshot token representing the current stack size.
		 *
		 * Use this token with @ref rollback to revert all changes done since this snapshot.
		 * @return Opaque snapshot token (stack size).
		 */
		int snapshot() const { return static_cast<int>(stk.size()); }

		/**
		 * @brief Rolls back all changes pushed after snapshot @p t.
		 * @param t Snapshot token previously obtained from @ref snapshot.
		 */
		void rollback(int t) {
			while (static_cast<int>(stk.size()) > t) {
				auto [i, old] = stk.back();
				stk.pop_back();
				p[i] = old;
			}
		}

		/**
		 * @brief Finds the root representative of @p x (no path compression).
		 */
		int find(int x) const {
			while (p[x] >= 0) x = p[x];
			return x;
		}

		/**
		 * @brief Merges the sets containing @p a and @p b and records changes for rollback.
		 * @return @c true if merged, @c false if both were already in the same set.
		 */
		bool unite(int a, int b) {
			a = find(a); b = find(b);
			if (a == b) return false;
			if (p[a] > p[b]) std::swap(a, b);     // attach smaller (by size) under larger
			// Save previous states:
			stk.emplace_back(a, p[a]);
			stk.emplace_back(b, p[b]);
			p[a] += p[b];  // increase size
			p[b] = a;     // parent link
			return true;
		}

		/**
		 * @brief Checks if @p a and @p b are in the same set.
		 */
		bool same(int a, int b) const { return find(a) == find(b); }

		/**
		 * @brief Returns the size of the set containing @p x.
		 */
		int size(int x) const { return -p[find(x)]; }

		/**
		 * @brief Returns the current universe size (number of elements).
		 */
		std::size_t universe() const { return p.size(); }
	};

} // namespace kj::detail
