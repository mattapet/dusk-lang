cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
set(LIB_TARGET dusk)
set(PROJECT_VERSION 1.0.0)
project(
    "dusk-lang"
    DESCRIPTION "Dusk programming language"
    VERSION ${PROJECT_VERSION}
)

enable_language(C CXX)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 17)

set(BUILD_SHARED_LIBS ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# link LLVM
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
llvm_map_components_to_libnames(llvm_libs all)

# setup dusk-llvm
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_library(${LIB_TARGET} SHARED ${SOURCE})
set_target_properties(${LIB_TARGET} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)
target_include_directories(${LIB_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${LIB_TARGET} ${llvm_libs})

# add tools executables and stdlib
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stdlib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tools)
