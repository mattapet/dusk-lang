#include "dusk/Frontend/InputFile.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SourceMgr.h"
#include <vector>
#include <memory>

#ifndef Compiler_h
#define Compiler_h

namespace dusk {

class Compiler {
    llvm::SourceMgr SourceManager;
    std::vector<std::unique_ptr<InputFile>> InputFiles;

public:
    Compiler(std::vector<llvm::StringRef> Filenames);

    void Compile();
    void Lex();
};

} /* namesapce mila */

#endif /* Compiler_h */
