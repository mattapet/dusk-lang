add_subdirectory(AST)
add_subdirectory(Basic)
add_subdirectory(Frontend)
add_subdirectory(IRGen)
add_subdirectory(Parse)
add_subdirectory(Runtime)
add_subdirectory(Sema)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Strings.h
    ${HEADERS}
    PARENT_SCOPE
)
