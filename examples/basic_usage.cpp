/**
 * @file basic_usage.cpp
 * @brief Demonstrates usage of the kj-utils header-only utility library.
 *
 * This file showcases example usage of the provided utilities:
 * - Aligned buffers
 * - Span-based views
 * - Scope guards
 * - RAII-style timers
 * - Benchmarking helpers
 * - Lightweight result-based error handling
 * - Aligned memory allocation
 */

#include <kj/buffer.hpp>
#include <kj/view.hpp>
#include <kj/timer.hpp>
#include <kj/scope_guard.hpp>
#include <kj/benchmark.hpp>
#include <kj/result.hpp>
#include <kj/memory.hpp>

#include <iostream>
#include <string>
#include <thread>

 /**
  * @brief Demonstrates basic usage of kj-utils components.
  *
  * This function constructs and utilizes various kj-utils modules including:
  * - kj::Buffer and kj::View for managing and accessing data
  * - kj::ScopeGuard for RAII-based cleanup
  * - kj::ScopedTimer for scoped timing
  * - kj::Benchmark for performance measurements
  * - kj::Result for explicit error handling
  * - kj::aligned_new / aligned_delete for aligned object allocation
  *
  * @return int Exit status code.
  */
int main() {
	std::cout << "kj-utils demo\n";

	// Section: kj::Buffer and kj::View
	kj::Buffer<int> buf(8);
	for (int i = 0; i < static_cast<int>(buf.size()); ++i) {
		buf[i] = i * 10;
	}

	kj::View<int> view = buf.span();
	for (auto x : view)
		std::cout << "value: " << x << '\n';

	// Section: kj::ScopeGuard
	auto guard = kj::scope_exit([] {
		std::cout << "[exit] cleaning up...\n";
		});

	// Section: kj::ScopedTimer
	{
		kj::ScopedTimer timer("sleep");
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	// Section: kj::Benchmark
	kj::Benchmark bench("Loop x1000");
	bench.run("simple-loop", [] {
		volatile int x = 0;
		for (int i = 0; i < 1000; ++i) {
			x += i;
		}
		}, 10); // 10 measurements

	// Section: kj::Result<T>
	auto res_ok = kj::ok<int>(42);
	auto res_err = kj::error<int>(std::string("error"));

	if (res_ok.has_value())
		std::cout << "Result OK: " << res_ok.value() << '\n';
	if (res_err.has_error())
		std::cout << "Result ERROR: " << res_err.error() << '\n';

	// Section: kj::aligned_new / aligned_delete
	auto* obj = kj::aligned_new<std::string>(64, "hello aligned");
	std::cout << "Aligned object: " << *obj << '\n';
	kj::aligned_delete(obj);

	return 0;
}