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
#include "dusk/AST/Type.h"
#include "dusk/AST/ASTWalker.h"

#include "IRGenModule.h"
#include "IRGenFunc.h"
#include "GenFunc.h"
#include "GenExpr.h"
#include "GenType.h"

using namespace dusk;
using namespace irgen;

namespace {

/// A simple AST walker, that declares all functions.
class FuncDeclarator : public ASTWalker {
  IRGenModule &IRGM;

public:
  FuncDeclarator(IRGenModule &IRGM) : IRGM(IRGM) {}

  // Skip all global declarations except function and module declarations
  virtual bool preWalkDecl(Decl *D) override {
    if (dynamic_cast<FuncDecl *>(D) != nullptr)
      return true;
    else if (dynamic_cast<ModuleDecl *>(D) != nullptr)
      return true;
    else
      return false;
  }

  // Skip all expressions
  std::pair<bool, Expr *> preWalkExpr(Expr *E) override { return {false, E}; }
  // Skip all patterns
  bool preWalkPattern(Pattern *P) override { return false; }
  // Skip all types
  bool preWalkTypeRepr(TypeRepr *TR) override { return false; }

  bool preWalkStmt(Stmt *S) override {
    // Skip all statements except function declarations
    if (dynamic_cast<FuncStmt *>(S) != nullptr)
      return true;
    else if (dynamic_cast<ExternStmt *>(S) != nullptr)
      return true;
    else
      return false;
  }

  // Actually declare given function
  bool postWalkDecl(Decl *D) override {
    if (auto M = dynamic_cast<ModuleDecl *>(D))
      return true;
    auto Fn = static_cast<FuncDecl *>(D);
    assert(IRGM.declareFunc(Fn) != nullptr && "Redefinition of a function");
    return true;
  }
};
} // anonymous namespace

static void codegenValDecl(IRGenModule &IRGM, ValDecl *D) {
  // Decalare variable
  IRGM.Lookup.declareVar(D);
  // Get LLVM type and create a global variable object
  auto Ty = codegenType(IRGM, D->getType());
  if (D->getType()->isRefType())
    Ty = llvm::PointerType::get(Ty, 0);
  auto GV = new llvm::GlobalVariable(*IRGM.Module, Ty, false,
                                     llvm::GlobalValue::InternalLinkage,
                                     nullptr, D->getName());
  // Get initial value
  llvm::Value *Val;
  if (D->hasValue())
    Val = IRGM.emitRValue(D->getValue());
  else
    Val = IRGM.emitRValue(D->getType());
  // Set initial value and insert address to values map.
  GV->setInitializer(static_cast<llvm::Constant *>(Val));
  IRGM.Vals.insert({D, GV});
}

static void codegenFuncStmt(IRGenModule &IRGM, FuncStmt *S) {
  auto FnName = S->getPrototype()->getName();
  IRGenFunc IRGF(IRGM, IRGM.Builder, IRGM.getFunc(FnName), S);
  genFunc(IRGF, S);
}

static void codegenModule(IRGenModule &IRGM, ModuleDecl *D) {
  for (auto N : D->getContents()) {
    if (auto D = dynamic_cast<ValDecl *>(N))
      codegenValDecl(IRGM, D);
    else if (auto S = dynamic_cast<FuncStmt *>(N))
      codegenFuncStmt(IRGM, S);
    else if (auto S = dynamic_cast<ExternStmt *>(N))
      continue;
    else
      llvm_unreachable("Unexpected node in module scope");
  }
}

void irgen::genModule(IRGenModule &IRGM) {
  auto Module = IRGM.Context.getRootModule();
  FuncDeclarator FnDecl(IRGM);
  Module->walk(FnDecl);
  codegenModule(IRGM, Module);
}
