//===--- IRGenerator.cpp --------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/ASTVisitor.h"
#include "dusk/AST/Scope.h"
#include "dusk/IRGen/IRGenerator.h"
#include "dusk/IRGen/Context.h"
#include "llvm/IR/BasicBlock.h"

#include "GenDecl.h"
#include "GenExpr.h"
#include "GenStmt.h"

using namespace dusk;
using namespace irgen;

IRGenerator::IRGenerator(ASTContext &C, DiagnosticEngine &Diag)
    : ASTCtx(C), Diag(Diag), Builder({Ctx}) {}

IRGenerator::~IRGenerator() {}



llvm::Module *IRGenerator::perform() {
  auto M = ASTCtx.getRootModule();
  Module = std::make_unique<llvm::Module>(M->getName(), Ctx);
  Context Ctx(this->Ctx, Module.get(), Builder);
  Scope Scp;
  prepareGlobals(Ctx, M);
  for (auto &N : M->getContents()) {
    bool R = true;
    if (auto D = dynamic_cast<Decl *>(N))
      continue;
    else if (auto E = dynamic_cast<Expr *>(N))
      R = codegenExpr(Ctx, E) != nullptr;
    else if (auto S = dynamic_cast<Stmt *>(N))
      R = codegenStmt(Ctx, Scp, S);
    else
      llvm_unreachable("Unexpected node");
    
    if (!R)
      return nullptr;
  }
  return Module.release();
}

bool IRGenerator::prepareGlobals(Context &Ctx, ModuleDecl *M) {
  for (auto N : M->getContents()) {
    if (auto D = dynamic_cast<Decl *>(N))
      if (!codegenDecl(Ctx, D))
        return false;
    if (auto Fn = dynamic_cast<FuncStmt *>(N))
      if (!codegenDecl(Ctx, Fn->getPrototype()))
        return false;
    if (auto Fn = dynamic_cast<ExternStmt *>(N))
      if (!codegenDecl(Ctx, Fn->getPrototype()))
        return false;
  }
  return true;
}
