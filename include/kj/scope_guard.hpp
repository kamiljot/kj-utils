#pragma once

#include <utility>
#include <functional>

namespace kj {

	/**
	 * @brief Executes a cleanup function at scope exit (RAII pattern).
	 *
	 * This is a minimal implementation of the scope guard idiom. It ensures that a user-provided
	 * callable is invoked when the object goes out of scope, making it suitable for managing
	 * cleanup actions even in the presence of early returns or exceptions.
	 *
	 * Example:
	 * @code
	 * auto guard = kj::scope_exit([] { cleanup(); });
	 * @endcode
	 *
	 * @tparam F Callable type (e.g., lambda, functor).
	 */
	template <typename F>
	class ScopeGuard {
	public:
		/**
		 * @brief Constructs a scope guard from a callable.
		 *
		 * @param f Callable to be invoked on scope exit unless dismissed.
		 */
		explicit ScopeGuard(F&& f) noexcept
			: fn_(std::forward<F>(f)), active_(true) {
		}

		/// Copy constructor is deleted.
		ScopeGuard(const ScopeGuard&) = delete;

		/// Copy assignment is deleted.
		ScopeGuard& operator=(const ScopeGuard&) = delete;

		/**
		 * @brief Move constructor.
		 *
		 * Transfers ownership of the cleanup function and disables the moved-from guard.
		 *
		 * @param other Rvalue reference to another ScopeGuard.
		 */
		ScopeGuard(ScopeGuard&& other) noexcept
			: fn_(std::move(other.fn_)), active_(other.active_) {
			other.dismiss();
		}

		/**
		 * @brief Move assignment operator.
		 *
		 * Transfers the cleanup logic from another ScopeGuard and disables the source.
		 *
		 * @param other Rvalue reference to another ScopeGuard.
		 * @return Reference to this guard.
		 */
		ScopeGuard& operator=(ScopeGuard&& other) noexcept {
			if (this != &other) {
				fn_ = std::move(other.fn_);
				active_ = other.active_;
				other.dismiss();
			}
			return *this;
		}

		/**
		 * @brief Destructor.
		 *
		 * If not dismissed, the stored cleanup function is invoked.
		 */
		~ScopeGuard() {
			if (active_) fn_();
		}

		/**
		 * @brief Prevents the cleanup function from executing on destruction.
		 */
		void dismiss() noexcept {
			active_ = false;
		}

	private:
		F fn_;        ///< The cleanup function.
		bool active_; ///< Whether the guard is currently active.
	};

	/**
	 * @brief Helper function to create a ScopeGuard with type deduction.
	 *
	 * @tparam F Callable type.
	 * @param f Callable to be executed on scope exit.
	 * @return A ScopeGuard<F> instance.
	 */
	template <typename F>
	ScopeGuard<F> scope_exit(F&& f) noexcept {
		return ScopeGuard<F>(std::forward<F>(f));
	}

} // namespace kj
