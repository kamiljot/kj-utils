#pragma once

#include <chrono>
#include <string>
#include <iostream>

namespace kj {

	/**
	 * @brief Measures and logs the time elapsed between construction and destruction.
	 *
	 * ScopedTimer starts timing upon construction and prints the elapsed time (in milliseconds)
	 * to stderr when the object goes out of scope.
	 *
	 * This is useful for profiling small code blocks using the RAII pattern.
	 *
	 * Example:
	 * @code
	 * {
	 *   kj::ScopedTimer timer("task-A");
	 *   do_work();
	 * } // Logs: [Timer] task-A: XX.XXX ms
	 * @endcode
	 */
	class ScopedTimer {
	public:
		/**
		 * @brief Constructs the timer and records the start time.
		 *
		 * @param label A label to identify the timed block in the log (optional).
		 */
		explicit ScopedTimer(std::string label = "")
			: label_(std::move(label)), start_(std::chrono::high_resolution_clock::now()) {
		}

		/**
		 * @brief Destructor automatically stops the timer and logs the elapsed time.
		 */
		~ScopedTimer() {
			const auto end = std::chrono::high_resolution_clock::now();
			const auto ms = std::chrono::duration<double, std::milli>(end - start_).count();
			std::cerr << "[Timer] " << label_ << ": " << ms << " ms\n";
		}

	private:
		std::string label_;  ///< Label used for identification in logs.
		std::chrono::high_resolution_clock::time_point start_;  ///< Start time recorded at construction.
	};

	/**
	 * @brief Manual stopwatch-style timer for measuring elapsed time.
	 *
	 * The Timer class allows explicit control over when timing starts and stops.
	 * It can also be queried for current elapsed time without stopping it.
	 *
	 * Example:
	 * @code
	 * kj::Timer timer;
	 * timer.start();
	 * compute();
	 * double ms = timer.stop();
	 * @endcode
	 */
	class Timer {
	public:
		/**
		 * @brief Starts or restarts the timer by recording the current time.
		 */
		void start() {
			start_ = std::chrono::high_resolution_clock::now();
		}

		/**
		 * @brief Stops the timer and returns the duration since start in milliseconds.
		 *
		 * @return Elapsed time in milliseconds as a double.
		 */
		double stop() {
			const auto end = std::chrono::high_resolution_clock::now();
			return std::chrono::duration<double, std::milli>(end - start_).count();
		}

		/**
		 * @brief Returns the elapsed time since the last call to start().
		 *
		 * @return Elapsed duration as a std::chrono::milliseconds object.
		 */
		std::chrono::milliseconds elapsed() const {
			const auto now = std::chrono::high_resolution_clock::now();
			return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_);
		}

	private:
		std::chrono::high_resolution_clock::time_point start_;  ///< Time point when the timer was started.
	};

} // namespace kj