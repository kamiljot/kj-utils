# Define a test executable that includes all test source files
add_executable(test_kj_utils
    test_main.cpp           # Entry point for Catch2 test runner
    test_buffer.cpp         # Tests for kj::Buffer
    test_scope_exit.cpp     # Tests for kj::ScopeGuard
    test_result.cpp         # Tests for kj::Result
    test_timer.cpp          # Tests for kj::Timer and kj::ScopedTimer
    test_benchmark.cpp      # Tests for kj::Benchmark
)

# Link the test executable against the main kj-utils library and Catch2 framework
target_link_libraries(test_kj_utils
    PRIVATE
        kj::utils           # The kj-utils target defined in the main library
        Catch2::Catch2WithMain # Catch2 entry point and implementation
)

# Add the include path so test files can include headers from 'include/'
target_include_directories(test_kj_utils
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Enable test discovery with CTest (integrates Catch2 tests into 'ctest')
include(CTest)
include(Catch)
catch_discover_tests(test_kj_utils)