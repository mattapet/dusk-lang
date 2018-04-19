add_subdirectory(AST)
add_subdirectory(Basic)
add_subdirectory(Frontend)
add_subdirectory(IRGen)
add_subdirectory(Parse)

set(HEADERS
    ${HEADERS}
    PARENT_SCOPE
)
