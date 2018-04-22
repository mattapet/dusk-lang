//===--- GenExpr.cpp ------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "GenExpr.h"

#include "llvm/ADT/APSInt.h"
#include "llvm/IR/Constant.h"
#include <vector>

using namespace dusk;
using namespace irgen;

GenExpr::GenExpr(Expr *R, Context &C) : Root(R), Ctx(C) {}

llvm::Value *GenExpr::gen() { return codegen(Root); }

llvm::Value *GenExpr::codegen(Expr *E) { return nullptr; }

llvm::Value *GenExpr::codegen(NumberLiteralExpr *E) {
  auto Value = llvm::APSInt::get(E->getValue());
  return llvm::ConstantInt::get(Ctx, Value);
}

llvm::Value *GenExpr::codegen(IdentifierExpr *E) {
  auto Addr = Ctx.getVal(E->getName());
  return Ctx.getBuilder().CreateLoad(Addr, E->getName());
}

llvm::Value *GenExpr::codegen(InfixExpr *E) {
  auto LHS = codegen(E->getLHS());
  auto RHS = codegen(E->getRHS());
  if (!LHS || !RHS)
    return nullptr;

  switch (E->getOp().getKind()) {
  // Arithemtic operations
  case tok::plus:
    return Ctx.getBuilder().CreateAdd(LHS, RHS, "addtmp");
  case tok::minus:
    return Ctx.getBuilder().CreateSub(LHS, RHS, "subtmp");
  case tok::multipy:
    return Ctx.getBuilder().CreateMul(LHS, RHS, "multmp");
  case tok::divide:
    return Ctx.getBuilder().CreateSDiv(LHS, RHS, "divtmp");

  // Logical operations
  case tok::equals:
    return Ctx.getBuilder().CreateICmpEQ(LHS, RHS, "eqtmp");
  case tok::nequals:
    return Ctx.getBuilder().CreateICmpNE(LHS, RHS, "neqtmp");
  case tok::greater:
    return Ctx.getBuilder().CreateICmpSGT(LHS, RHS, "gttmp");
  case tok::greater_eq:
    return Ctx.getBuilder().CreateICmpSGE(LHS, RHS, "getmp");
  case tok::less:
    return Ctx.getBuilder().CreateICmpSLT(LHS, RHS, "lttmp");
  case tok::less_eq:
    return Ctx.getBuilder().CreateICmpSLE(LHS, RHS, "letmp");

  default:
    llvm_unreachable("Invalid infix operand");
  }
}

llvm::Value *GenExpr::codegen(PrefixExpr *E) {
  auto Val = codegen(E->getDest());
  if (!Val)
    return nullptr;
  
  switch (E->getOp().getKind()) {
    case tok::neg:
      return Ctx.getBuilder().CreateNot(Val);
      
      // TODO: fix unary minus
    case tok::minus:
    default:
      llvm_unreachable("Invalid prefix operand");
  }
}

llvm::Value *GenExpr::codegen(AssignExpr *E) {
  // Ensure the LHS is a identifier.
  auto VarExpr = dynamic_cast<IdentifierExpr *>(E);
  if (!VarExpr)
    return nullptr;
  
  // Get variable address
  auto VarAddr = Ctx.getVar(VarExpr->getName());
  auto Val = codegen(E->getSource());
  if (!VarAddr || !Val)
    return nullptr;
  
  return Ctx.getBuilder().CreateStore(Val, VarAddr, "assign");
}

llvm::Value *GenExpr::codegen(CallExpr *E) {
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
    Args.push_back(codegen(Arg));
  return Ctx.getBuilder().CreateCall(Fn, Args);
}
