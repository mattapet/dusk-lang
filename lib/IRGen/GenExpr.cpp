//===--- GenExpr.cpp ------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "GenExpr.h"

#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/IR/Constant.h"
#include <vector>

#include "IRGenModule.h"
#include "IRGenFunc.h"

using namespace dusk;
using namespace irgen;

static llvm::Value *cast(IRGenModule &IRGM, llvm::Value *V, llvm::Type *Ty) {
  return IRGM.Builder.CreateIntCast(V, Ty, true);
}

llvm::Value *irgen::codegenExpr(IRGenModule &IRGM, NumberLiteralExpr *E) {
  auto Ty = llvm::Type::getInt64Ty(IRGM.LLVMContext);
  return llvm::ConstantInt::get(Ty, E->getValue());
}

llvm::Value *irgen::codegenExpr(IRGenModule &IRGM, IdentifierExpr *E) {
  auto Addr = IRGM.getVal(E->getName());
  return IRGM.Builder.CreateLoad(Addr, E->getName());
}

llvm::Value *irgen::codegenExpr(IRGenModule &IRGM, InfixExpr *E) {
  auto LHS = codegenExpr(IRGM, E->getLHS());
  auto RHS = codegenExpr(IRGM, E->getRHS());
  auto Ty = llvm::Type::getInt64Ty(IRGM.LLVMContext);
  if (!LHS || !RHS)
    return nullptr;

  switch (E->getOp().getKind()) {
  // Arithemtic operations
  case tok::plus:
    return IRGM.Builder.CreateAdd(LHS, RHS, "addtmp");
  case tok::minus:
    return IRGM.Builder.CreateSub(LHS, RHS, "subtmp");
  case tok::multipy:
    return IRGM.Builder.CreateMul(LHS, RHS, "multmp");
  case tok::divide:
    return IRGM.Builder.CreateSDiv(LHS, RHS, "divtmp");
  case tok::mod:
    return IRGM.Builder.CreateSRem(LHS, RHS, "modtmp");

  // Logical operations
  case tok::land:
    return cast(IRGM, IRGM.Builder.CreateAnd(LHS, RHS, "andtmp"), Ty);
  case tok::lor:
    return cast(IRGM, IRGM.Builder.CreateOr(LHS, RHS, "ortmp"), Ty);
  case tok::equals:
    return cast(IRGM, IRGM.Builder.CreateICmpEQ(LHS, RHS, "eqtmp"), Ty);
  case tok::nequals:
    return cast(IRGM, IRGM.Builder.CreateICmpNE(LHS, RHS, "neqtmp"), Ty);
  case tok::greater:
    return cast(IRGM, IRGM.Builder.CreateICmpSGT(LHS, RHS, "gttmp"), Ty);
  case tok::greater_eq:
    return cast(IRGM, IRGM.Builder.CreateICmpSGE(LHS, RHS, "getmp"), Ty);
  case tok::less:
    return cast(IRGM, IRGM.Builder.CreateICmpSLT(LHS, RHS, "lttmp"), Ty);
  case tok::less_eq:
    return cast(IRGM, IRGM.Builder.CreateICmpSLE(LHS, RHS, "letmp"), Ty);

  default:
    llvm_unreachable("Invalid infix operand");
  }
}

llvm::Value *irgen::codegenExpr(IRGenModule &IRGM, PrefixExpr *E) {
  auto Val = codegenExpr(IRGM, E->getDest());
  auto Ty = llvm::Type::getInt64Ty(IRGM.LLVMContext);
  if (!Val)
    return nullptr;

  switch (E->getOp().getKind()) {
  case tok::lnot:
    return cast(IRGM, IRGM.Builder.CreateNot(Val), Ty);

  case tok::minus: {
    auto Ty = llvm::Type::getInt64Ty(IRGM.LLVMContext);
    auto Zero = llvm::ConstantInt::get(Ty, 0);
    return IRGM.Builder.CreateSub(Zero, Val, "unary-subtmp");
  }
  default:
    llvm_unreachable("Invalid prefix operand");
  }
}

llvm::Value *irgen::codegenExpr(IRGenModule &IRGM, AssignExpr *E) {
  // Ensure the LHS is a identifier.
  auto VarExpr = static_cast<IdentifierExpr *>(E->getDest());

  // Get variable address
  auto VarAddr = IRGM.getVal(VarExpr->getName());
  auto Val = codegenExpr(IRGM, E->getSource());
  if (!VarAddr || !Val)
    llvm_unreachable("Invalid val or addr");

  return IRGM.Builder.CreateStore(Val, VarAddr, "assign");
}

llvm::Value *irgen::codegenExpr(IRGenModule &IRGM, CallExpr *E) {
  // Get callee as identifier.
  auto CalleeID = static_cast<IdentifierExpr *>(E->getCalle());

  // Get args
  auto ArgsPttrn = static_cast<ExprPattern *>(E->getArgs());

  // Get declared function
  auto Fn = IRGM.getFunc(CalleeID->getName());

  auto Args = std::vector<llvm::Value *>();
  for (auto Arg : ArgsPttrn->getValues())
    Args.push_back(codegenExpr(IRGM, Arg));
  return IRGM.Builder.CreateCall(Fn, Args);
}


llvm::Value *irgen::codegenExpr(IRGenModule &IRGM, Expr *E) {
    switch (E->getKind()) {
    case ExprKind::NumberLiteral:
      return codegenExpr(IRGM, static_cast<NumberLiteralExpr *>(E));
    case ExprKind::Identifier:
      return codegenExpr(IRGM, static_cast<IdentifierExpr *>(E));
    case ExprKind::Paren:
      return codegenExpr(IRGM, static_cast<ParenExpr *>(E)->getExpr());
    case ExprKind::Assign:
      return codegenExpr(IRGM, static_cast<AssignExpr *>(E));
    case ExprKind::Infix:
      return codegenExpr(IRGM, static_cast<InfixExpr *>(E));
    case ExprKind::Prefix:
      return codegenExpr(IRGM, static_cast<PrefixExpr *>(E));
    case ExprKind::Call:
      return codegenExpr(IRGM, static_cast<CallExpr *>(E));
    case ExprKind::Subscript:
//      return codegenExpr(IRGM, static_cast<SubscriptExpr *>(E));
      llvm_unreachable("Not implemented.");
  }
}

