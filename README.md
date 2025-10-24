# kj-utils

kj-utils is a modern, header-only C++23 utility library that provides foundational building blocks for high-performance and clean C++ applications. It focuses on RAII, modularity, and cross-platform usability - with zero dependencies and maximum portability.

## Features

This library provides a curated set of reusable components:

- `kj::Buffer<T>` - RAII-managed aligned buffer with move semantics
- `kj::View<T>` - friendly aliases and helpers around `std::span`
- `kj::Timer` / `kj::ScopedTimer` - precise wall-clock timing tools
- `kj::ScopeGuard` - deterministic resource cleanup (like `scope_exit`)
- `kj::Benchmark` - minimalistic benchmarking loop with warmup and stats
- `kj::Result<T>` - lightweight `expected<T, E>` implementation
- `kj::memory` - aligned `new`/`delete` helpers (cross-platform)

All components are header-only, live in the `kj::` namespace, and follow modern C++20/23 idioms.

## Purpose

This library was created to:

- Eliminate boilerplate in performance-critical C++ apps
- Unify helpers used across GPU/kernel benchmarking and ML scenarios
- Promote modular design using clean, well-scoped utilities
- Enable reuse across tools like `gpu-kernel-benchmark` and future infrastructure

It emerged from practical needs in benchmarking GPU workloads and building fast, clean abstractions in C++.

## Example Use

```cpp
#include <kj/buffer.hpp>
#include <kj/benchmark.hpp>
#include <kj/result.hpp>
#include <kj/scope_guard.hpp>

int main() {
    kj::Buffer<int> data(100, 64);         // aligned buffer
    kj::ScopedTimer timer("Sleep");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // RAII guarantees cleanup on scope exit
}
```

## Projects Using kj-utils

<<<<<<< HEAD
- gpu-kernel-benchmark – modular tool for GPU kernel measurement (CUDA)
- ml-scenario-benchmark (planned) – for benchmarking ML workloads with consistent methodology
=======
- gpu-kernel-benchmark - modular tool for GPU kernel measurement (CUDA)
- ml-scenario-benchmark (planned) - for benchmarking ML workloads with consistent methodology
>>>>>>> bd68443716ba43e579af9720ca62c8f0cfd612d6

kj-utils can also be reused in embedded systems, HPC pipelines, game engines, or any modern C++ project.

## Documentation

All headers use Doxygen-style comments. You can generate documentation locally:

```bash
doxygen Doxyfile
```

This will create `docs/html/index.html` with full API reference.

## Header-Only Design

No build steps. Just:

```bash
git clone https://github.com/yourname/kj-utils
```

<<<<<<< HEAD
Then include what you need. That’s it.
=======
Then include what you need. That's it.
>>>>>>> bd68443716ba43e579af9720ca62c8f0cfd612d6

## License

Licensed under the MIT License. Free for open source and commercial use.

## Author

Developed by Kamil Jatkowski to support clean and modular C++ systems built around performance, simplicity, and reuse.
<<<<<<< HEAD
=======

>>>>>>> bd68443716ba43e579af9720ca62c8f0cfd612d6
