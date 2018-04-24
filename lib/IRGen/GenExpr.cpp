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
#include "dusk/IRGen/Context.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/IR/Constant.h"
#include <vector>

using namespace dusk;
using namespace irgen;

llvm::Value *cast(llvm::Value *V, Context &Ctx, llvm::Type *Ty) {
  return Ctx.Builder.CreateBitCast(V, Ty);
}

llvm::Value *irgen::codegenExpr(Context &Ctx, NumberLiteralExpr *E) {
  auto Ty = llvm::Type::getInt64Ty(Ctx);
  return llvm::ConstantInt::get(Ty, E->getValue());
}

llvm::Value *irgen::codegenExpr(Context &Ctx, IdentifierExpr *E) {
  auto Addr = Ctx.getVal(E->getName());
  return Ctx.Builder.CreateLoad(Addr, E->getName());
}

llvm::Value *irgen::codegenExpr(Context &Ctx, InfixExpr *E) {
  auto LHS = codegenExpr(Ctx, E->getLHS());
  auto RHS = codegenExpr(Ctx, E->getRHS());
  auto Ty = llvm::Type::getInt64Ty(Ctx);
  if (!LHS || !RHS)
    return nullptr;

  switch (E->getOp().getKind()) {
  // Arithemtic operations
  case tok::plus:
    return Ctx.Builder.CreateAdd(LHS, RHS, "addtmp");
  case tok::minus:
    return Ctx.Builder.CreateSub(LHS, RHS, "subtmp");
  case tok::multipy:
    return Ctx.Builder.CreateMul(LHS, RHS, "multmp");
  case tok::divide:
    return Ctx.Builder.CreateSDiv(LHS, RHS, "divtmp");
  case tok::mod:
    return Ctx.Builder.CreateSRem(LHS, RHS, "modtmp");

  // Logical operations
  case tok::equals:
    return cast(Ctx.Builder.CreateICmpEQ(LHS, RHS, "eqtmp"), Ctx, Ty);
  case tok::nequals:
    return cast(Ctx.Builder.CreateICmpNE(LHS, RHS, "neqtmp"), Ctx, Ty);
  case tok::greater:
    return cast(Ctx.Builder.CreateICmpSGT(LHS, RHS, "gttmp"), Ctx, Ty);
  case tok::greater_eq:
    return cast(Ctx.Builder.CreateICmpSGE(LHS, RHS, "getmp"), Ctx, Ty);
  case tok::less:
    return cast(Ctx.Builder.CreateICmpSLT(LHS, RHS, "lttmp"), Ctx, Ty);
  case tok::less_eq:
    return cast(Ctx.Builder.CreateICmpSLE(LHS, RHS, "letmp"), Ctx, Ty);

  default:
    llvm_unreachable("Invalid infix operand");
  }
}

llvm::Value *irgen::codegenExpr(Context &Ctx, PrefixExpr *E) {
  auto Val = codegenExpr(Ctx, E->getDest());
  auto Ty = llvm::Type::getInt64Ty(Ctx);
  if (!Val)
    return nullptr;

  switch (E->getOp().getKind()) {
  case tok::lnot:
    return cast(Ctx.Builder.CreateNot(Val), Ctx, Ty);

  case tok::minus: {
    auto Ty = llvm::Type::getInt64Ty(Ctx);
    auto Zero = llvm::ConstantInt::get(Ty, 0);
    return Ctx.Builder.CreateSub(Zero, Val, "unary-subtmp");
  }
  default:
    llvm_unreachable("Invalid prefix operand");
  }
}

llvm::Value *irgen::codegenExpr(Context &Ctx, AssignExpr *E) {
  // Ensure the LHS is a identifier.
  auto VarExpr = dynamic_cast<IdentifierExpr *>(E->getDest());
  if (!VarExpr)
    llvm_unreachable("Invalid LHS");

  // Get variable address
  auto VarAddr = Ctx.getVar(VarExpr->getName());
  auto Val = codegenExpr(Ctx, E->getSource());
  if (!VarAddr || !Val)
    llvm_unreachable("Invalid val or addr");

  return Ctx.Builder.CreateStore(Val, VarAddr, "assign");
}

llvm::Value *irgen::codegenExpr(Context &Ctx, CallExpr *E) {
  // Ensure the callee is identifier.
  auto CalleeID = dynamic_cast<IdentifierExpr *>(E->getCalle());

  // Ensure args is expression pattern
  auto ArgsPttrn = dynamic_cast<ExprPattern *>(E->getArgs());
  if (!CalleeID || !ArgsPttrn)
    return nullptr;

  // Ensure calling a declared function
  auto Fn = Ctx.getFunc(CalleeID->getName());
  if (!Fn)
    return nullptr;

  // Invalid number of arguments
  if (Fn->arg_size() != ArgsPttrn->count())
    return nullptr;

  auto Args = std::vector<llvm::Value *>(Fn->arg_size());
  for (auto Arg : ArgsPttrn->getValues())
    Args.push_back(codegenExpr(Ctx, Arg));
  return Ctx.Builder.CreateCall(Fn, Args);
}

llvm::Value *irgen::codegenExpr(Context &Ctx, Expr *E) {
  switch (E->getKind()) {
  case ExprKind::NumberLiteral:
    return codegenExpr(Ctx, static_cast<NumberLiteralExpr *>(E));
  case ExprKind::Identifier:
    return codegenExpr(Ctx, static_cast<IdentifierExpr *>(E));
  case ExprKind::Paren:
    return codegenExpr(Ctx, static_cast<ParenExpr *>(E)->getExpr());
  case ExprKind::Assign:
    return codegenExpr(Ctx, static_cast<AssignExpr *>(E));
  case ExprKind::Infix:
    return codegenExpr(Ctx, static_cast<InfixExpr *>(E));
  case ExprKind::Prefix:
    return codegenExpr(Ctx, static_cast<PrefixExpr *>(E));
  case ExprKind::Call:
    return codegenExpr(Ctx, static_cast<CallExpr *>(E));
  case ExprKind::Subscript:
    return codegenExpr(Ctx, static_cast<SubscriptExpr *>(E));
  }
}
