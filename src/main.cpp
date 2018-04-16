#include "dusk/Frontend/Compiler.h"
#include "dusk/AST/ASTPrinter.h"
#include "dusk/AST/ASTNode.h"
#include "dusk/AST/Decl.h"
#include "llvm/ADT/StringRef.h"
#include <iostream>
#include <vector>

using namespace dusk;

int main(int argc, const char *argv[]) {
    std::vector<llvm::StringRef> Filenames;
    for (size_t i = 1; i < argc; i++) {
        Filenames.push_back(argv[i]);
    }
    Compiler C(Filenames);
    C.Compile();

    return 0;
}
