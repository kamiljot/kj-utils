# kj-utils

**kj-utils** is a modern, header-only C++ utility library (C++17/20/23) that provides foundational building blocks for high-performance and clean C++ applications. It focuses on RAII, modularity, and cross-platform usability - with zero external dependencies and maximum portability.

## Table of Contents

- [Features](#features)
- [Purpose](#purpose)
- [Quick Start](#quick-start)
  - [As a submodule](#as-a-submodule)
- [Build & Tests](#build--tests)
  - [Examples](#examples)
  - [Tested Toolchains](#tested-toolchains)
- [Documentation](#documentation)
- [Example Snippets](#example-snippets)
  - [Result + ScopeGuard + Timer](#result--scopeguard--timer)
  - [DSU](#dsu)
  - [Skew Heap (pool-backed)](#skew-heap-pool-backed)
- [Integration Notes](#integration-notes)
- [Projects Using kj-utils](#projects-using-kj-utils)
- [Roadmap](#roadmap)
- [License](#license)
- [Author](#author)

## Features

Reusable components, all in the `kj::` namespace:

- **Core**
  - `kj::Buffer<T>` - RAII-managed aligned buffer with move semantics
  - `kj::View<T>` - friendly aliases & helpers around `std::span`
  - `kj::ScopeGuard` - deterministic cleanup (scope_exit pattern)
  - `kj::Timer` / `kj::ScopedTimer` - precise wall-clock timing tools
  - `kj::Benchmark` - minimal benchmarking loop with warmup & statistics
  - `kj::Result<T, E>` - lightweight `expected`-like result type
  - `kj::memory` - aligned `new`/`delete` helpers (cross-platform)

- **I/O**
  - `kj::io::FastInput` / `kj::io::FastOutput` - fast buffered stdin/stdout for CP/ICPC

- **Data Structures**
  - `kj::SkewHeap<T, Comp>` - mergeable heap (min-heap by default)
  - `kj::SkewHeapArena<T, Comp>` - same API, backed by an internal object pool
  - `kj::DSU` - disjoint set union (union-by-size + path compression)
  - `kj::RollbackDSU` - DSU with snapshots and rollback (no path compression)

- **Memory Utilities**
  - `kj::detail::ObjectPool<T>` - reusable object pool with free-list

All components are header-only and follow modern C++17/20/23 idioms (constexpr where possible, `[[nodiscard]]`, `noexcept` where sensible, concepts-friendly).

## Purpose

- Eliminate boilerplate in performance-critical C++ code.
- Provide clean, well-scoped utilities used across competitive programming (ICPC), systems, and HPC contexts.
- Encourage modular design and reuse (small, composable headers).
- Keep builds simple: header-only, zero external deps.

## Quick Start

### As a submodule

```bash
git submodule add https://github.com/kamiljot/kj-utils extern/kj-utils
```

CMake:

```cmake
add_library(kj-utils INTERFACE)
target_include_directories(kj-utils INTERFACE ${CMAKE_SOURCE_DIR}/extern/kj-utils/include)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE kj-utils)
```

Code:

```cpp
#include <kj/skew_heap.hpp>
#include <kj/io/fast_io.hpp>

int main() {
  kj::io::FastInput in;
  kj::io::FastOutput out;

  int n; if (!in.read(n)) return 0;
  kj::SkewHeap<int> h;              // or: kj::SkewHeapArena<int> h; h.reserve_nodes(n);
  for (int i = 0; i < n; ++i) { int x; in.read(x); h.push(x); }
  while (!h.empty()) { out.write_int(h.top()).put_char(' '); h.pop(); }
  out.write_line();
}
```

## Build & Tests

This repository includes a simple CMake setup for examples and tests.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DKJ_UTILS_ENABLE_TESTS=ON -DKJ_UTILS_BUILD_EXAMPLES=ON
cmake --build build -j
ctest --test-dir build -j --output-on-failure
```

> Windows/PowerShell tips:
> - Clean build dir: `Remove-Item -Recurse -Force .\\build\\`
> - If CTest reports JUnit write errors, ensure the reports directory exists:
>   `New-Item -ItemType Directory -Force .\\build\\test\\reports | Out-Null`

### Examples

If `KJ_UTILS_BUILD_EXAMPLES=ON`, an example binary is built:

```bash
./build/example
```

### Tested Toolchains

- MSVC 19.3x (VS 2022), Clang ≥ 15, GCC ≥ 11
- Library headers are C++17-compatible; the top-level CMake uses C++23 by default.  
  If you need C++20, set:
  ```cmake
  set(CMAKE_CXX_STANDARD 20)
  target_compile_features(kj_utils INTERFACE cxx_std_20)
  ```

## Documentation

All headers use Doxygen-style comments. Generate docs locally:

```bash
doxygen Doxyfile
```

This will create `docs/html/index.html` with the full API reference.

## Example Snippets

### Result + ScopeGuard + Timer

```cpp
#include <kj/result.hpp>
#include <kj/scope_guard.hpp>
#include <kj/timer.hpp>

kj::Result<int, const char*> do_work() {
  kj::ScopedTimer t("do_work");
  auto guard = kj::ScopeGuard([] { /* cleanup */ });
  // ...
  return kj::ok<int>(42);
}
```

### DSU

```cpp
#include <kj/dsu.hpp>

kj::DSU dsu(5);
dsu.unite(0,1);
bool same = dsu.same(0,1); // true
int sz = dsu.size(0);      // 2
```

### Skew Heap (pool-backed)

```cpp
#include <kj/skew_heap.hpp>
#include <functional>

kj::SkewHeapArena<int, std::greater<int>> h; // max-heap variant
h.reserve_nodes(100000);                      // optional prealloc
h.push(7); h.push(3); h.push(9);
int top = h.top(); // 9
```

## Integration Notes

- **Header-only**: just add `include/` to your include paths.
- **No exceptions policy**: most utilities avoid throwing; error paths use `kj::Result` or boolean returns where appropriate.
- **Allocator friendliness**: some DS (e.g., `SkewHeapArena`) use an internal pool to reduce allocation overhead in heavy workloads.
- **Modularity**: public headers are thin aliases; implementation lives under `kj/detail/`.

## Projects Using kj-utils

- `gpu-kernel-benchmark` - modular tool for GPU kernel measurement (CUDA)
- `ml-scenario-benchmark` (planned) - benchmarking ML workloads with consistent methodology

kj-utils can also be reused in embedded systems, HPC pipelines, game engines, competitive programming/ICPC, or any modern C++ project.

## Roadmap

- Additional DS/algorithms (Fenwick/Segment Tree, Pairing Heap, Dinic)
- Optional `std::pmr` adapters
- Example ICPC templates

## License

MIT License - free for open source and commercial use.

## Author

Developed by **Kamil Jatkowski** to support clean and modular C++ systems built around performance, simplicity, and reuse.
