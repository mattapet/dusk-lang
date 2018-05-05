//===--- GenModule.cpp ----------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "GenModule.h"

#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/ASTWalker.h"

#include "IRGenModule.h"
#include "IRGenFunc.h"
#include "GenFunc.h"

using namespace dusk;
using namespace irgen;

namespace {

/// A simple AST walker, that declares all functions.
class FuncDeclarator: public ASTWalker {
  IRGenModule &IRGM;

public:
  FuncDeclarator(IRGenModule &IRGM) : IRGM(IRGM) {}
  
  // Skip all global declarations except function and module declarations
  virtual bool preWalk(Decl *D) override {
    if (dynamic_cast<FuncDecl *>(D) != nullptr)
      return true;
    if (dynamic_cast<ModuleDecl *>(D) != nullptr)
      return true;
    return false;
  }
  
  // Skip all expressions
  virtual bool preWalk(Expr *E) override { return false; }
  
  virtual bool preWalk(Stmt *S) override {
    // Skip all statements except function declarations
    if (dynamic_cast<FuncStmt *>(S) != nullptr)
      return true;
    if (dynamic_cast<ExternStmt *>(S) != nullptr)
      return true;
    return false;
  }
  
  // Actually declare given function
  virtual bool postWalk(Decl *D) override {
    auto Fn = static_cast<FuncDecl *>(D);
    assert(IRGM.declareFunc(Fn) != nullptr && "Redefinition of a function");
    return true;
  }
};

} // anonymous namespace


static void decalreFunctions(IRGenModule &IRGM) {
  FuncDeclarator FD(IRGM);
  for (auto N : IRGM.Context.getRootModule()->getContents())
    N->walk(FD);
}

void irgen::genModule(IRGenModule &IRGM) {
  decalreFunctions(IRGM);
  
  for (auto N : IRGM.Context.getRootModule()->getContents()) {
    if (auto D = dynamic_cast<ValDecl *>(N)) {
      if (IRGM.declareVal(D) == nullptr)
        llvm_unreachable("Redefinition of a value");
    
    } else if (auto Fn = dynamic_cast<FuncStmt *>(N)) {
      IRGenFunc IRGF(IRGM, IRGM.Builder,
                     IRGM.getFunc(Fn->getPrototype()->getName()), Fn);

      if (!genFunc(IRGF, Fn))
        llvm_unreachable("Error generating a function.");
      
    } else if (auto Fn = dynamic_cast<ExternStmt *>(N)) {
      continue;
      
    } else {
      llvm_unreachable("Unexpected top-level statement");
    }
  }
}
