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

GenDecl::GenDecl(Decl *D, Context &C) : Declaration(D), Ctx(C) {}

bool GenDecl::gen() { return codegen(Declaration); }

bool GenDecl::genArgs() {
  assert(Declaration->getKind() == DeclKind::Func &&
         "Invalid declaration type");
  auto FT = static_cast<FuncDecl *>(Declaration);
  
  for (auto &Arg : FT->getArgs()->getVars())
    if (!codegen(Arg))
      llvm_unreachable("Error argument declaration");
  return true;
}

bool GenDecl::codegen(VarDecl *D) {
  if (!Ctx.declare(D))
    llvm_unreachable("Redefinition");
  
  auto Addr = Ctx.getVal(D->getName());
  auto Val = GenExpr(D->getValue(), Ctx).gen();
  Ctx.Builder.CreateStore(Val, Addr);
  return true;
}

bool GenDecl::codegen(ConstDecl *D) {
  if (!Ctx.declare(D))
    llvm_unreachable("Redefinition");

  auto Addr = Ctx.getVal(D->getName());
  auto Val = GenExpr(D->getValue(), Ctx).gen();
  Ctx.Builder.CreateStore(Val, Addr);
  return true;
}

bool GenDecl::codegen(FuncDecl *D) {
  if (!Ctx.declare(D))
    llvm_unreachable("Redefinition of function");

  auto P = Ctx.getFuncProto(D->getName());
  if (!P)
    llvm_unreachable("Function not found");

  auto F = llvm::Function::Create(P, llvm::Function::ExternalLinkage,
                                  D->getName(), Ctx.Module);
  unsigned Idx = 0;
  auto Args = D->getArgs()->getVars();
  for (auto &Arg : F->args())
    Arg.setName(Args[Idx++]->getName());
  return true;
}

bool GenDecl::codegen(ParamDecl *D) {
  return Ctx.declare(D);
}

bool GenDecl::codegen(Decl *D) {
  switch (D->getKind()) {
  case DeclKind::Var:
    return codegen(static_cast<VarDecl *>(D));
  case DeclKind::Const:
    return codegen(static_cast<ConstDecl *>(D));
  case DeclKind::Func:
    return codegen(static_cast<FuncDecl *>(D));
  case DeclKind::Param:
    return codegen(static_cast<ParamDecl *>(D));
  default:
    llvm_unreachable("Invalid declaration");
  }
}

