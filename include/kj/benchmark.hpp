#pragma once

#include <chrono>
#include <vector>
#include <numeric>
#include <functional>
#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>

namespace kj {

	/**
	 * @brief Holds the result of a benchmark run.
	 *
	 * Contains the individual timing samples as well as the average duration.
	 */
	struct BenchmarkResult {
		/// Measured execution times for each iteration in milliseconds.
		std::vector<std::chrono::duration<double, std::milli>> samples;

		/// Average execution time across all iterations in milliseconds.
		std::chrono::duration<double, std::milli> avg;
	};

	/**
	 * @brief Utility class for benchmarking callable operations.
	 *
	 * This class provides a way to perform warmup iterations (to stabilize CPU cache, JIT, etc.)
	 * and then collect repeated time measurements using `std::chrono::high_resolution_clock`.
	 *
	 * Results include individual samples and a computed average duration in milliseconds.
	 */
	class Benchmark {
	public:
		/**
		 * @brief Constructs a Benchmark instance.
		 *
		 * @param name Optional benchmark name (used for labeling output).
		 * @param warmup_iters Number of warmup iterations to execute before timing.
		 * @param repeat_iters Default number of measurement iterations.
		 */
		Benchmark(std::string name = "", int warmup_iters = 5, int repeat_iters = 10)
			: name_(std::move(name)), warmup_(warmup_iters), repeat_(repeat_iters) {
		}

		/**
		 * @brief Runs the benchmark for a given callable.
		 *
		 * Executes warmup iterations first (not timed), then measures execution time
		 * over the specified number of repetitions.
		 *
		 * @param label Descriptive label for this run (shown in output).
		 * @param fn The function or lambda to benchmark.
		 * @param repeat_count Optional override for number of timed measurements (defaults to constructor's repeat_iters).
		 * @return BenchmarkResult containing timing samples and average.
		 */
		BenchmarkResult run(const std::string& label, const std::function<void()>& fn, int repeat_count = -1) const {
			using namespace std::chrono;
			std::vector<duration<double, std::milli>> samples;

			if (repeat_count < 0) {
				repeat_count = repeat_;
			}

			// Warmup phase (not measured)
			for (int i = 0; i < warmup_; ++i) {
				fn();
			}

			// Measurement phase
			for (int i = 0; i < repeat_count; ++i) {
				const auto start = high_resolution_clock::now();
				fn();
				const auto end = high_resolution_clock::now();
				samples.push_back(end - start);
			}

			// Compute average
			const duration<double, std::milli> total = std::accumulate(samples.begin(), samples.end(), duration<double, std::milli>(0));
			const duration<double, std::milli> avg = total / samples.size();

			// Output summary
			std::cerr << "[Benchmark] " << label << ": avg = "
				<< std::fixed << std::setprecision(3) << avg.count()
				<< " ms over " << repeat_count << " runs\n"
				<< std::defaultfloat;

			return BenchmarkResult{ samples, avg };
		}

	private:
		std::string name_;  ///< Optional name for the benchmark context.
		int warmup_;        ///< Number of warmup iterations.
		int repeat_;        ///< Default number of measurement repetitions.
	};

} // namespace kj