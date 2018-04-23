//===--- IRGenerator.cpp --------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/IRGen/IRGenerator.h"
#include "dusk/IRGen/Context.h"

#include "GenDecl.h"
#include "GenExpr.h"
#include "GenStmt.h"

using namespace dusk;
using namespace irgen;

IRGenerator::IRGenerator(DiagnosticEngine &Diag)
    : Diag(Diag), Builder({Ctx}) {}

IRGenerator::~IRGenerator() {}

bool IRGenerator::gen(ModuleDecl *M) {
  Module = std::make_unique<llvm::Module>(M->getName(), Ctx);
  Context Ctx(this->Ctx, Module.get(), Builder);
  
  for (auto &N : M->getContents()) {
    bool R = false;
    if (auto D = dynamic_cast<Decl *>(N))
      R = GenDecl(D, Ctx).gen();
    else if (auto E = dynamic_cast<Expr *>(N))
      R = GenExpr(E, Ctx).gen() != nullptr;
    else if (auto S = dynamic_cast<Stmt *>(N))
      R = GenStmt(S, Ctx).gen();
    else
      llvm_unreachable("Unexpected node");
    
    if (!R)
      return false;
  }
  Module->print(llvm::errs(), nullptr);
  return true;
}
