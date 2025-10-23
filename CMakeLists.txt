cmake_minimum_required(VERSION 3.20)
project(kj-utils LANGUAGES CXX)

add_library(kj_utils INTERFACE)
target_include_directories(kj_utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(kj_utils INTERFACE cxx_std_23)

add_library(kj::utils ALIAS kj_utils)

# Optional: example build
add_executable(example examples/basic_usage.cpp)
target_link_libraries(example PRIVATE kj::utils)