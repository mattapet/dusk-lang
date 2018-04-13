cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
set(TARGET dusk-lang)
project(${TARGET})

enable_language(C CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${TARGET} ${SOURCE} ${HEADERS})

llvm_map_components_to_libnames(llvm_libs support core irreader tablegen)
target_link_libraries(${TARGET} ${llvm_libs})
