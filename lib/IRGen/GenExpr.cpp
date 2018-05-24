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
#include "llvm/IR/Instructions.h"
#include <vector>

#include "IRGenModule.h"
#include "IRGenFunc.h"
#include "GenType.h"

using namespace dusk;
using namespace irgen;


LValue irgen::codegenLValue(IRGenModule &IRGM, IdentifierExpr *E) {
  auto Ty = E->getType();
  auto Val = IRGM.getVal(E->getName());
  return LValue::getVal(Ty, Val);
}

LValue irgen::codegenLValue(IRGenModule &IRGM, SubscriptExpr *E) {
  auto Base = codegenLValue(IRGM, E->getBase());
  auto Idx =
      codegenRValue(IRGM, E->getSubscript()->getSubscripStmt()->getValue());
  
  if (Base.isArrayElement()) {
    auto ITy = llvm::Type::getInt64Ty(IRGM.LLVMContext);
    auto Zero = llvm::ConstantInt::get(ITy, 0);
    auto Idx_ = Base.getElementIndex();
    
    auto Addr = IRGM.Builder.CreateGEP(Base.getArrayPtr(), {Zero, Idx_});
    return LValue::getArrayElem(E->getType(), Addr, Idx);
  }
  return LValue::getArrayElem(E->getType(), Base.getAddress(), Idx);
}

LValue irgen::codegenLValue(IRGenModule &IRGM, Expr *E) {
  switch (E->getKind()) {
    case ExprKind::Identifier:
      return codegenLValue(IRGM, static_cast<IdentifierExpr *>(E));
    case ExprKind::Subscript:
      return codegenLValue(IRGM, static_cast<SubscriptExpr *>(E));
      
    default:
      llvm_unreachable("Invalid LValue");
  }
}


RValue irgen::codegenRValue(IRGenModule &IRGM, NumberLiteralExpr *E) { return {}; }
RValue irgen::codegenRValue(IRGenModule &IRGM, ArrayLiteralExpr *E) { return {}; }
RValue irgen::codegenRValue(IRGenModule &IRGM, IdentifierExpr *E) { return {}; }
RValue irgen::codegenRValue(IRGenModule &IRGM, SubscriptExpr *E) { return {}; }
RValue irgen::codegenRValue(IRGenModule &IRGM, InfixExpr *E) { return {}; }
RValue irgen::codegenRValue(IRGenModule &IRGM, PrefixExpr *E) { return {}; }
RValue irgen::codegenRValue(IRGenModule &IRGM, AssignExpr *E) {
  auto Dest = codegenLValue(IRGM, E->getDest());
  auto Src = codegenRValue(IRGM, E->getSource());
    
  return {};
}

RValue irgen::codegenRValue(IRGenModule &IRGM, CallExpr *E) {
  // Get callee as identifier.
  auto CalleeID = static_cast<IdentifierExpr *>(E->getCallee());
  
  // Get args
  auto ArgsPttrn = static_cast<ExprPattern *>(E->getArgs());
  
  // Get declared function
  auto Fn = IRGM.getFunc(CalleeID->getName());
  
  auto Args = std::vector<llvm::Value *>();
  for (auto Arg : ArgsPttrn->getValues())
    Args.push_back(codegenExpr(IRGM, Arg));
  return RValue::get(E->getType(), IRGM.Builder.CreateCall(Fn, Args));
}

RValue irgen::codegenRValue(IRGenModule &IRGM, Expr *E) {
  switch (E->getKind()) {
  case ExprKind::NumberLiteral:
    return codegenRValue(IRGM, static_cast<NumberLiteralExpr *>(E));
  case ExprKind::ArrayLiteral:
    return codegenRValue(IRGM, static_cast<ArrayLiteralExpr *>(E));
  case ExprKind::Identifier:
    return codegenRValue(IRGM, static_cast<IdentifierExpr *>(E));
  case ExprKind::Paren:
    return codegenRValue(IRGM, static_cast<ParenExpr *>(E)->getExpr());
  case ExprKind::Assign:
    return codegenRValue(IRGM, static_cast<AssignExpr *>(E));
  case ExprKind::Infix:
    return codegenRValue(IRGM, static_cast<InfixExpr *>(E));
  case ExprKind::Prefix:
    return codegenRValue(IRGM, static_cast<PrefixExpr *>(E));
  case ExprKind::Call:
    return codegenRValue(IRGM, static_cast<CallExpr *>(E));
  case ExprKind::Subscript:
    return codegenRValue(IRGM, static_cast<SubscriptExpr *>(E));
  }
}




static llvm::Value *cast(IRGenModule &IRGM, llvm::Value *V, llvm::Type *Ty) {
  return IRGM.Builder.CreateIntCast(V, Ty, true);
}

llvm::Value *irgen::codegenExpr(IRGenModule &IRGM, NumberLiteralExpr *E) {
  auto Ty = llvm::Type::getInt64Ty(IRGM.LLVMContext);
  return llvm::ConstantInt::get(Ty, E->getValue());
}

llvm::Value *irgen::codegenExpr(IRGenModule &IRGM, ArrayLiteralExpr *E) {
  auto Vals = static_cast<ExprPattern *>(E->getValues());
  auto ArrTy = static_cast<ArrayType *>(E->getType());
  auto Ty =
      static_cast<llvm::ArrayType *>(codegenType(IRGM, ArrTy));

  std::vector<llvm::Constant *> Values;
  for (auto V : Vals->getValues())
    Values.push_back(static_cast<llvm::Constant *>(codegenExpr(IRGM, V)));
  return llvm::ConstantArray::get(Ty, Values);
}

llvm::Value *irgen::codegenExpr(IRGenModule &IRGM, IdentifierExpr *E) {
  auto Addr = IRGM.getVal(E->getName());
  return IRGM.Builder.CreateLoad(Addr, E->getName());
}



Address codegenExprPtr(IRGenModule &IRGM, Expr *E);

static Address codegenExprPtr(IRGenModule &IRGM, SubscriptExpr *E) {
  auto Ptr = codegenExprPtr(IRGM, E->getBase());
  auto ITy = llvm::Type::getInt64Ty(IRGM.LLVMContext);
  auto Zero = llvm::ConstantInt::get(ITy, 0);
  auto Idx = codegenExpr(
      IRGM, static_cast<SubscriptStmt *>(E->getSubscript())->getValue());

  return IRGM.Builder.CreateGEP(Ptr, {Zero, Idx});
}

static Address codegenExprPtr(IRGenModule &IRGM, IdentifierExpr *E) {
  return IRGM.getVal(E->getName());
}

Address codegenExprPtr(IRGenModule &IRGM, Expr *E) {
  switch (E->getKind()) {
  case ExprKind::Identifier:
    return codegenExprPtr(IRGM, static_cast<IdentifierExpr *>(E));
  case ExprKind::Subscript:
    return codegenExprPtr(IRGM, static_cast<SubscriptExpr *>(E));

  default:
    llvm_unreachable("Not implemented yet.");
  }
}

llvm::Value *irgen::codegenExpr(IRGenModule &IRGM, SubscriptExpr *E) {
  auto Addr = codegenExprPtr(IRGM, E);
  return IRGM.Builder.CreateLoad(Addr, "indexarr");
}

llvm::Value *irgen::codegenExpr(IRGenModule &IRGM, InfixExpr *E) {
  auto LHS = codegenExpr(IRGM, E->getLHS());
  auto RHS = codegenExpr(IRGM, E->getRHS());
  auto Ty = llvm::Type::getInt64Ty(IRGM.LLVMContext);
  auto BitTy = llvm::Type::getInt1Ty(IRGM.LLVMContext);
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
    return cast(IRGM, IRGM.Builder.CreateAnd(cast(IRGM, LHS, BitTy),
                                             cast(IRGM, RHS, BitTy), "andtmp"),
                Ty);
  case tok::lor:
    return cast(IRGM, IRGM.Builder.CreateOr(cast(IRGM, LHS, BitTy),
                                            cast(IRGM, RHS, BitTy), "ortmp"),
                Ty);
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

  IRGM.Builder.CreateStore(Val, VarAddr, "assign");
  return Val;
}

llvm::Value *irgen::codegenExpr(IRGenModule &IRGM, CallExpr *E) {
  // Get callee as identifier.
  auto CalleeID = static_cast<IdentifierExpr *>(E->getCallee());

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
    case ExprKind::ArrayLiteral:
      return codegenExpr(IRGM, static_cast<ArrayLiteralExpr *>(E));
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
      return codegenExpr(IRGM, static_cast<SubscriptExpr *>(E));
  }
}

