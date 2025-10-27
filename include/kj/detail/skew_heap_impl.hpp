#pragma once
#include <utility>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <kj/detail/object_pool.hpp>

namespace kj::detail {

	/**
	 * @brief Skew heap (mergeable heap) with optional internal node pool.
	 *
	 * By default this is a min-heap via @p std::less<T>. Use @p std::greater<T> for a max-heap.
	 * If @p UsePool is true, node allocations are served from an internal @ref ObjectPool,
	 * which can dramatically reduce allocation overhead for heavy workloads.
	 *
	 * @tparam T       Key type.
	 * @tparam Comp    Comparator (StrictWeakOrder), defaults to @c std::less<T>.
	 * @tparam UsePool If true, use @ref ObjectPool for node storage (default: false).
	 */
	template <class T, class Comp = std::less<T>, bool UsePool = false>
	class SkewHeap {
	private:
		/**
		 * @brief Internal node type.
		 */
		struct Node {
			T     key;
			Node* left;
			Node* right;

			template<class U>
			explicit Node(U&& v) : key(std::forward<U>(v)), left(nullptr), right(nullptr) {}
		};

		using PoolT = std::conditional_t<UsePool, ObjectPool<Node>, struct __kj_no_pool_tag>;

		Node* root_ = nullptr;
		std::size_t  sz_ = 0;
#if defined(__has_cpp_attribute) && __has_cpp_attribute(no_unique_address)
		[[no_unique_address]]
#endif
		Comp         cmp_{};
		// Pool present only if UsePool == true (otherwise no storage cost)
		[[no_unique_address]] std::conditional_t<UsePool, PoolT, char> pool_{};

		// ---- allocation helpers -------------------------------------------------
		template <class U>
		Node* make_node(U&& v) {
			if constexpr (UsePool) {
				return pool_.create(std::forward<U>(v));
			}
			else {
				return new Node(std::forward<U>(v));
			}
		}
		static void destroy_node(std::conditional_t<UsePool, PoolT&, void*>, Node* n) {
			if constexpr (UsePool) {
				// no-op here; we recycle nodes in pop/clear via pool_.destroy
			}
			else {
				delete n;
			}
		}

		// ---- core merge on nodes ------------------------------------------------
		static Node* merge_nodes(Node* a, Node* b, Comp& cmp) noexcept(
			noexcept(cmp(std::declval<const T&>(), std::declval<const T&>()))
			) {
			if (!a) return b;
			if (!b) return a;
			if (cmp(b->key, a->key)) std::swap(a, b);     // ensure 'a' has preferred root
			a->right = merge_nodes(a->right, b, cmp);     // merge b into a->right
			std::swap(a->left, a->right);                 // skew step
			return a;
		}

		static void destroy_subtree(Node* n) noexcept {
			if (!n) return;
			destroy_subtree(n->left);
			destroy_subtree(n->right);
			delete n;
		}
		static void destroy_subtree_pool(PoolT& pool, Node* n) noexcept {
			if (!n) return;
			destroy_subtree_pool(pool, n->left);
			destroy_subtree_pool(pool, n->right);
			pool.destroy(n);
		}

	public:
		//-------------------------------------------------------------------------
		// Construction / destruction
		//-------------------------------------------------------------------------

		/// Constructs an empty heap. If @p UsePool is true, an internal pool is created.
		SkewHeap() = default;

		/// Constructs an empty heap with a custom comparator.
		explicit SkewHeap(const Comp& c) : cmp_(c) {}

		/// Releases all nodes (O(n)). If @p UsePool, nodes return to the pool.
		~SkewHeap() { clear(); }

		SkewHeap(const SkewHeap&) = delete;
		SkewHeap& operator=(const SkewHeap&) = delete;

		SkewHeap(SkewHeap&& other) noexcept
			: root_(other.root_), sz_(other.sz_), cmp_(std::move(other.cmp_))
		{
			if constexpr (UsePool) {
				// Pools cannot be moved trivially here; keep separate pools per instance.
				// Steal only the tree; resulting nodes remain owned by this instance's pool.
				// For safety, forbid moving when UsePool is true (or implement pool move).
				static_assert(!UsePool, "Move-constructing SkewHeap with UsePool is intentionally disabled.");
			}
			other.root_ = nullptr; other.sz_ = 0;
		}
		SkewHeap& operator=(SkewHeap&& other) noexcept {
			if (this != &other) {
				clear();
				root_ = other.root_; sz_ = other.sz_;
				cmp_ = std::move(other.cmp_);
				if constexpr (UsePool) {
					static_assert(!UsePool, "Move-assigning SkewHeap with UsePool is intentionally disabled.");
				}
				other.root_ = nullptr; other.sz_ = 0;
			}
			return *this;
		}

		//-------------------------------------------------------------------------
		// Pool controls (no-op when UsePool == false)
		//-------------------------------------------------------------------------

		/**
		 * @brief Pre-allocates approximately @p n node slots in the internal pool.
		 * No-op if @p UsePool is false.
		 */
		void reserve_nodes(std::size_t n) {
			if constexpr (UsePool) pool_.reserve(n);
		}

		/**
		 * @brief Releases all pool blocks. The heap must be empty.
		 * No-op if @p UsePool is false.
		 */
		void release_all_to_pool() noexcept {
			if constexpr (UsePool) {
#ifndef NDEBUG
				// defensive: ensure heap is empty before releasing pool memory
				assert(root_ == nullptr && "release_all_to_pool() requires empty heap");
#endif
				pool_.release_all();
			}
		}

		//-------------------------------------------------------------------------
		// Observers
		//-------------------------------------------------------------------------

		void clear() noexcept {
			if constexpr (UsePool) {
				destroy_subtree_pool(pool_, root_);
			}
			else {
				destroy_subtree(root_);
			}
			root_ = nullptr; sz_ = 0;
		}

		[[nodiscard]] bool empty() const noexcept { return root_ == nullptr; }
		[[nodiscard]] std::size_t size() const noexcept { return sz_; }
		[[nodiscard]] const T& top() const noexcept { return root_->key; }
		[[nodiscard]] const Comp& comparator() const noexcept { return cmp_; }

		//-------------------------------------------------------------------------
		// Modifiers
		//-------------------------------------------------------------------------

		void push(const T& v) {
			root_ = merge_nodes(root_, make_node(v), cmp_);
			++sz_;
		}
		void push(T&& v) {
			root_ = merge_nodes(root_, make_node(std::move(v)), cmp_);
			++sz_;
		}
		template<class... Args>
		void emplace(Args&&... args) {
			root_ = merge_nodes(root_, make_node(T(std::forward<Args>(args)...)), cmp_);
			++sz_;
		}

		void pop() {
			Node* l = root_->left;
			Node* r = root_->right;
			if constexpr (UsePool) pool_.destroy(root_);
			else delete root_;
			root_ = merge_nodes(l, r, cmp_);
			--sz_;
		}

		void merge(SkewHeap& other) {
			if (this == &other) return;
			// NOTE: When UsePool==true, each heap owns its own pool. Merging trees across pools
			// is fine because nodes are just pointers; lifetime is bound to the destination heap.
			root_ = merge_nodes(root_, other.root_, cmp_);
			sz_ += other.sz_;
			other.root_ = nullptr;
			other.sz_ = 0;
		}
	};

} // namespace kj::detail
