//===--- GenDecl.cpp ------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "GenDecl.h"
#include "GenExpr.h"

using namespace dusk;
using namespace irgen;

bool irgen::codegenDecl(Context &Ctx, VarDecl *D) {
  if (!Ctx.declareVal(D))
    llvm_unreachable("Redefinition");
  
  auto Addr = Ctx.getVal(D->getName());
  auto Val = codegenExpr(Ctx, D->getValue());
  Ctx.Builder.CreateStore(Val, Addr);
  return true;
}

bool irgen::codegenDecl(Context &Ctx, LetDecl *D) {
  if (!Ctx.declareVal(D))
    llvm_unreachable("Redefinition");
  
  auto Addr = Ctx.getVal(D->getName());
  auto Val = codegenExpr(Ctx, D->getValue());
  Ctx.Builder.CreateStore(Val, Addr);
  return true;
}

bool irgen::codegenDecl(Context &Ctx, FuncDecl *D) {
  if (!Ctx.declareFunc(D))
    llvm_unreachable("Redefinition of function");
  
  auto P = Ctx.getFuncProto(D->getName());
  auto F = llvm::Function::Create(P, llvm::Function::ExternalLinkage,
                                  D->getName(), Ctx.Module);
  unsigned Idx = 0;
  auto Args = D->getArgs()->getVars();
  for (auto &Arg : F->args())
    Arg.setName(Args[Idx++]->getName());
  return true;
}

bool irgen::codegenDecl(Context &Ctx, ParamDecl *D) {
  if (!Ctx.declareVal(D))
    llvm_unreachable("Redefinition.");
  return true;
}

bool irgen::codegenDecl(Context &Ctx, Decl *D) {
  switch (D->getKind()) {
  case DeclKind::Var:
    return codegenDecl(Ctx, static_cast<VarDecl *>(D));
  case DeclKind::Let:
    return codegenDecl(Ctx, static_cast<LetDecl *>(D));
  case DeclKind::Func:
    return codegenDecl(Ctx, static_cast<FuncDecl *>(D));
  case DeclKind::Param:
    return codegenDecl(Ctx, static_cast<ParamDecl *>(D));
  default:
    llvm_unreachable("Invalid declaration");
  }
}

