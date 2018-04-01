cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
set(TARGET semstral)
project(${TARGET})

enable_language(C CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
llvm_map_components_to_libnames(llvm_libs support core irreader)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)
add_subdirectory(src)
add_executable(${TARGET} ${SOURCE})

target_link_libraries(${TARGET} ${llvm_libs})
