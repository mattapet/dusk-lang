set(C_TARGET dusk-format)
set(C_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_executable(${C_TARGET} ${C_SOURCE})
target_link_libraries(${C_TARGET} ${llvm_libs} ${LIB_TARGET})
