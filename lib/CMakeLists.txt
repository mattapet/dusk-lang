add_subdirectory(AST)
add_subdirectory(Basic)
add_subdirectory(Frontend)
add_subdirectory(IRGen)
add_subdirectory(Parser)
add_subdirectory(Sema)

set(SOURCE
    ${SOURCE}
    PARENT_SCOPE
)
