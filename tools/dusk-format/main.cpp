#include "dusk/Basic/LLVM.h"
#include "dusk/Frontend/CompilerInvocation.h"
#include "dusk/Frontend/CompilerInstance.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include <string>

#include "dusk/AST/ASTContext.h"
#include "dusk/AST/Type.h"

using namespace dusk;
using namespace llvm;

cl::opt<std::string> InFile(cl::Positional, cl::Required,
                            cl::desc("<input file>"), cl::init("-"));

cl::opt<std::string> OutFile("o", cl::desc("Specify output filename"),
                             cl::value_desc("<filename>"));
cl::opt<bool> IsQuiet("quiet", cl::desc("Suppress diagnostics"));
cl::alias IsQuiet2("q", cl::desc("Suppress diagnostics"),
                   cl::aliasopt(IsQuiet));

void initCompilerInstance(CompilerInstance &C) {
  CompilerInvocation Inv;
  Inv.setArgs(C.getSourceManager(), C.getDiags(), InFile, OutFile, IsQuiet);
  C.reset(std::move(Inv));
}

int main(int argc, const char *argv[]) {
  cl::ParseCommandLineOptions(argc, argv);
  CompilerInstance Compiler;
  initCompilerInstance(Compiler);

  Compiler.performCompilation();
  
  return 0;
}
