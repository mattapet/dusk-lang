//===--- GenDecl.cpp ------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "GenDecl.h"

#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/Type.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/DerivedTypes.h"

#include "IRGenModule.h"
#include "GenExpr.h"
#include "GenType.h"

using namespace dusk;
using namespace irgen;

static llvm::Constant *initGlobal(IRGenModule &IRGM, ValDecl *D, IntType *Ty) {
  auto VTy = codegenType(IRGM, Ty);
  if (auto Val = dynamic_cast<NumberLiteralExpr *>(D->getValue()))
    return llvm::ConstantInt::get(VTy, Val->getValue());
  else
    return llvm::ConstantInt::get(VTy, 0);
}

static llvm::Constant *initGlobal(IRGenModule &IRGM, ValDecl *D,
                                  ArrayType *Ty) {
  auto VTy = static_cast<llvm::ArrayType *>(codegenType(IRGM, Ty));
  std::vector<llvm::Constant *> Values;
  if (auto Val = dynamic_cast<ArrayLiteralExpr *>(D->getValue()))
    return static_cast<llvm::Constant *>(codegenExpr(IRGM, Val));
  return llvm::ConstantAggregateZero::get(VTy);
}

llvm::Constant *initGlobal(IRGenModule &IRGM, Decl *D, Type *Ty) {
  auto DD = static_cast<ValDecl *>(D);
  switch (Ty->getKind()) {
  case TypeKind::Array:
    return initGlobal(IRGM, DD, static_cast<ArrayType *>(Ty));
  case TypeKind::Int:
    return initGlobal(IRGM, DD, static_cast<IntType *>(Ty));
  default:
    llvm_unreachable("Unexpected type.");
  }
}

static void initLocal(IRGenModule &IRGM, Decl *DD, IntType *Ty) {
  auto D = dynamic_cast<ValDecl *>(DD);
  if (!D || !D->hasValue())
    return;
  auto Addr = IRGM.getVal(D->getName());
  if (D->hasValue())
    IRGM.Builder.CreateStore(codegenExpr(IRGM, D->getValue()), Addr);
  else
    IRGM.Builder.CreateStore(codegenInit(IRGM, Ty), Addr);
}

static void initLocal(IRGenModule &IRGM, Decl *DD, ArrayType *Ty) {
  auto D = dynamic_cast<ValDecl *>(DD);
  if (!D || !D->hasValue())
    return;
  auto Addr = IRGM.getVal(D->getName());
  auto Val = IRGM.emitRValue(D->getValue());
  IRGM.Builder.CreateStore(Val, Addr);
}

void initLocal(IRGenModule &IRGM, Decl *D, Type *Ty) {
  switch (Ty->getKind()) {
  case TypeKind::Array:
    return initLocal(IRGM, D, static_cast<ArrayType *>(Ty));
  case TypeKind::Int:
    return initLocal(IRGM, D, static_cast<IntType *>(Ty));
  default:
    llvm_unreachable("Unexpected type.");
  }
}

Address irgen::codegenDeclGlobal(IRGenModule &IRGM, Decl *D) {
  auto Ty = codegenType(IRGM, D->getType());
  auto GV = new llvm::GlobalVariable(*IRGM.Module, Ty, false,
                                     llvm::GlobalValue::InternalLinkage,
                                     nullptr, D->getName());
  IRGM.Vals[D] = GV;
  GV->setInitializer(initGlobal(IRGM, D, D->getType()));
  return GV;
}

Address irgen::codegenDeclLocal(IRGenModule &IRGM, Decl *D) {
  auto Addr = codegenAlloca(IRGM, D->getType());
  IRGM.Vals[D] = Addr;
  initLocal(IRGM, D, D->getType());
  return Addr;
}

Address irgen::codegenDecl(IRGenModule &IRGM, Decl *D) {
  assert(IRGM.Lookup.declareVar(D) && "Redeclaration of value");

  if (IRGM.Lookup.getDepth() == 0) {
    return codegenDeclGlobal(IRGM, D);
  } else {
    return codegenDeclLocal(IRGM, D);
  }
}
