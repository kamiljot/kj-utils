# -----------------------------------------------------------------------------
# CMake configuration for kj-utils tests
# -----------------------------------------------------------------------------

# Enable CTest integration
include(CTest)

# Locate Catch2 v3 (without main; we provide test_main.cpp)
if (NOT TARGET Catch2::Catch2)
  find_package(Catch2 3 REQUIRED)
endif()

# Resolve the kj-utils target name (supports both 'kj::utils' and 'kj-utils')
if (TARGET kj::utils)
  set(KJ_UTILS_TARGET kj::utils)
elseif (TARGET kj-utils)
  set(KJ_UTILS_TARGET kj-utils)
else()
  message(FATAL_ERROR "Could not find 'kj::utils' or 'kj-utils' target. Define the library before tests.")
endif()

# -----------------------------------------------------------------------------
# Single test runner executable that compiles all unit tests
# -----------------------------------------------------------------------------
add_executable(test_kj_utils
    #test_main.cpp           # Catch2 test runner entry (provides main)
    test_buffer.cpp         # Tests for kj::Buffer
    test_scope_exit.cpp     # Tests for kj::ScopeGuard
    test_result.cpp         # Tests for kj::Result
    test_timer.cpp          # Tests for kj::Timer and kj::ScopedTimer
    test_benchmark.cpp      # Tests for kj::Benchmark
    test_skew_heap.cpp      # Tests for kj::SkewHeap
    test_dsu.cpp            # Tests for kj::DSU / RollbackDSU (remove if not present)
)

# Use modern C++ for tests (inherits from top-level if already set)
target_compile_features(test_kj_utils PRIVATE cxx_std_20)

# Link against kj-utils and Catch2 (without main!)
target_link_libraries(test_kj_utils
    PRIVATE
        ${KJ_UTILS_TARGET}
        Catch2::Catch2WithMain
)

# Add include path so test sources can include headers from 'include/'
target_include_directories(test_kj_utils
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# -----------------------------------------------------------------------------
# Register Catch2 tests with CTest
# -----------------------------------------------------------------------------

# Integrate Catch2 tests with CTest
include(Catch)

# Ensure report directory exists
set(CATCH_REPORT_DIR ${CMAKE_CURRENT_BINARY_DIR}/reports)
file(MAKE_DIRECTORY ${CATCH_REPORT_DIR})

catch_discover_tests(test_kj_utils
  TEST_PREFIX "kj::"
  REPORTER junit
  OUTPUT_DIR ${CATCH_REPORT_DIR}
  OUTPUT_PREFIX "report-"
  OUTPUT_SUFFIX .xml
)
