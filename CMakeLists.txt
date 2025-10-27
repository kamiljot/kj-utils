cmake_minimum_required(VERSION 3.20)

# ---------------------------------------------------------------------
# Project
# ---------------------------------------------------------------------
project(kj-utils LANGUAGES CXX)

# Language standard (C++23 here; consumers inherit via compile features)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------
option(KJ_UTILS_BUILD_EXAMPLES "Build kj-utils examples" ON)
option(KJ_UTILS_ENABLE_TESTS  "Enable building kj-utils tests" ON)

# ---------------------------------------------------------------------
# Header-only library
# ---------------------------------------------------------------------
add_library(kj_utils INTERFACE)

# Public include path
target_include_directories(kj_utils
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Consumers require at least C++23 (you can relax to 20/17 if needed)
target_compile_features(kj_utils INTERFACE cxx_std_23)

# Aliases for convenience and compatibility
add_library(kj::utils ALIAS kj_utils)
# Some downstream code may try to link 'kj-utils' - provide an alias too.
add_library(kj-utils ALIAS kj_utils)

# ---------------------------------------------------------------------
# Examples (optional)
# ---------------------------------------------------------------------
if (KJ_UTILS_BUILD_EXAMPLES)
  add_executable(example_basic examples/basic_usage.cpp)
  target_link_libraries(example_basic PRIVATE kj::utils)

  add_executable(example_skew_heap examples/a_skew_heap_template.cpp)
  target_link_libraries(example_skew_heap PRIVATE kj::utils)
  # No need to add include dirs explicitly; they're inherited from the INTERFACE link
endif()

# ---------------------------------------------------------------------
# Tests (optional)
# ---------------------------------------------------------------------
if (KJ_UTILS_ENABLE_TESTS)
  include(CTest)

  # Fetch Catch2 v3 if not already available in the build
  include(FetchContent)
  set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/_deps" CACHE STRING "Base dir for fetched content")

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.11.0
  )
  FetchContent_MakeAvailable(Catch2)

  enable_testing()
  add_subdirectory(test)
endif()

# ---------------------------------------------------------------------
# (Optional) Install rules - keep if you plan 'find_package(kj-utils)'
# ---------------------------------------------------------------------
include(GNUInstallDirs)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS kj_utils EXPORT kj-utilsTargets)
install(EXPORT kj-utilsTargets
  NAMESPACE kj::
  FILE kj-utilsTargets.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kj-utils
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/kj-utilsConfigVersion.cmake"
  VERSION 1.0.0
  COMPATIBILITY SameMajorVersion
)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/kj-utilsConfig.cmake"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/kj-utilsTargets.cmake\")\n")
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/kj-utilsConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/kj-utilsConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kj-utils
)
