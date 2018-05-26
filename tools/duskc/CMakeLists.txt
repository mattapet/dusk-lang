set(FORMAT_TARGET duskc)
set(FORMAT_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_executable(${FORMAT_TARGET} ${FORMAT_SOURCE})
target_link_libraries(${FORMAT_TARGET} ${llvm_libs} ${LIB_TARGET})
