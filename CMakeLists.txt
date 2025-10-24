cmake_minimum_required(VERSION 3.20)

# Project declaration
project(kj-utils LANGUAGES CXX)

# Require C++23 standard with no compiler extensions
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define a header-only interface library for kj-utils
add_library(kj_utils INTERFACE)

# Make the 'include' directory visible to consumers
target_include_directories(kj_utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Require C++23 features for all consumers of kj_utils
target_compile_features(kj_utils INTERFACE cxx_std_23)

# Define an alias target for clarity
add_library(kj::utils ALIAS kj_utils)

# Build the example program demonstrating usage of kj-utils
add_executable(example examples/basic_usage.cpp)
target_link_libraries(example PRIVATE kj::utils)

# Optionally enable building and running tests (enabled by default)
option(KJ_UTILS_ENABLE_TESTS "Enable building kj-utils tests" ON)

if (KJ_UTILS_ENABLE_TESTS)
  # Use FetchContent to download Catch2 test framework
  include(FetchContent)
  set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps CACHE STRING "Base directory for fetched content")

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.11.0
  )
  FetchContent_MakeAvailable(Catch2)

  enable_testing()

  # Add test sources from the test directory
  add_subdirectory(test)
endif()
