//===--- GenRValue.cpp - R value emission -----------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/Expr.h"
#include "dusk/AST/ASTVisitor.h"
#include "llvm/ADT/APSInt.h"

#include "IRGenModule.h"
#include "IRGenValue.h"
#include "GenType.h"

#include <vector>

using namespace dusk;
using namespace irgen;

namespace {

/// Performs an rvalue emission.
class RValueEmitter : public ExprVisitor<RValueEmitter, RValue> {
  IRGenModule &IRGM;

  typedef ASTVisitor super;

  friend super;

public:
  RValueEmitter(IRGenModule &IRGM) : IRGM(IRGM) {}

private:
  llvm::Type *getIntTy() const {
    return llvm::Type::getInt64Ty(IRGM.LLVMContext);
  }

  llvm::Type *getBoolTy() const {
    return llvm::Type::getInt1Ty(IRGM.LLVMContext);
  }

  RValue cast(const RValue &Value, llvm::Type *Ty) {
    auto Res = IRGM.Builder.CreateIntCast(Value, Ty, true);
    return RValue::get(Value.getType(), Res);
  }

  // MARK: - Type rvalue emission

  RValue emitRValue(IntType *Ty) {
    return RValue::get(Ty, llvm::ConstantInt::get(getIntTy(), 0));
  }

  RValue emitRValue(ArrayType *Ty) {
    auto TTy = codegenType(IRGM, Ty);
    auto Value = llvm::ConstantAggregateZero::get(TTy);
    auto GV = new llvm::GlobalVariable(*IRGM.Module, TTy, false,
                                       llvm::GlobalVariable::InternalLinkage,
                                       nullptr, Value->getName());
    GV->setInitializer(Value);
    return RValue::get(Ty, GV);
  }

  // MARK: - Visit methods

  RValue visitNumberLiteralExpr(NumberLiteralExpr *E) {
    auto Value = llvm::ConstantInt::get(getIntTy(), E->getValue());
    return RValue::get(E->getType(), Value);
  }

  RValue visitArrayLiteralExpr(ArrayLiteralExpr *E) {
    // Extract values
    auto Vals = E->getValues()->getExprPattern();
    auto ArrTy = E->getType()->getArrayType();
    auto Ty = static_cast<llvm::ArrayType *>(codegenType(IRGM, ArrTy));
    
    // Emit array elements
    std::vector<llvm::Constant *> Values;
    for (auto V : Vals->getValues()) {
      auto Value = (llvm::Value *)emitRValue(V);
      Values.push_back(static_cast<llvm::Constant *>(Value));
    }

    // Create array
    auto Value = llvm::ConstantArray::get(Ty, Values);
    auto GV = new llvm::GlobalVariable(*IRGM.Module, Ty, false,
                                       llvm::GlobalVariable::InternalLinkage,
                                       nullptr, Value->getName());
    GV->setInitializer(Value);
    return RValue::get(E->getType(), GV);
  }

  RValue visitIdentifierExpr(IdentifierExpr *E) {
    auto Addr = IRGM.getVal(E->getName());
    auto Value = IRGM.Builder.CreateLoad(Addr, E->getName() + ".load");
    return RValue::get(E->getType(), Value);
  }

  RValue visitAssignExpr(AssignExpr *E) {
    auto Dest = IRGM.emitLValue(E->getDest());
    auto Src = IRGM.emitRValue(E->getSource());

    if (Dest.isSimple()) {
      IRGM.Builder.CreateStore(Src, Dest.getPointer());
    } else {
      auto Zero = llvm::ConstantInt::get(getIntTy(), 0);
      auto Ptr = Dest.getArrayPtr();
      auto Idx = Dest.getElementIndex();
      auto Addr = IRGM.Builder.CreateGEP(Ptr, {Zero, Idx});
      IRGM.Builder.CreateStore(Src, Addr);
    }
    // Return LHS as the result of assignement
    return Src;
  }

  RValue visitInfixExpr(InfixExpr *E) {
    // Emit values for both sides of expression
    auto LHS = emitRValue(E->getLHS());
    auto RHS = emitRValue(E->getRHS());

    switch (E->getOp().getKind()) {
    // Arithmetic operators
    case tok::plus: {
      auto Value = IRGM.Builder.CreateAdd(LHS, RHS, "addtmp");
      return RValue::get(E->getType(), Value);
    }

    case tok::minus: {
      auto Value = IRGM.Builder.CreateSub(LHS, RHS, "subtmp");
      return RValue::get(E->getType(), Value);
    }

    case tok::multipy: {
      auto Value = IRGM.Builder.CreateMul(LHS, RHS, "multmp");
      return RValue::get(E->getType(), Value);
    }

    case tok::divide: {
      auto Value = IRGM.Builder.CreateSDiv(LHS, RHS, "divtmp");
      return RValue::get(E->getType(), Value);
    }

    case tok::mod: {
      auto Value = IRGM.Builder.CreateSRem(LHS, RHS, "modtmp");
      return RValue::get(E->getType(), Value);
    }

    // Logical operators
    case tok::lnot: {
      auto Value = IRGM.Builder.CreateAnd(cast(LHS, getBoolTy()),
                                          cast(RHS, getBoolTy()), "and");
      return cast(RValue::get(E->getType(), Value), getIntTy());
    }

    case tok::land: {
      auto Zero = llvm::ConstantInt::get(getIntTy(), 0);
      auto L = IRGM.Builder.CreateICmpNE(LHS, Zero);
      auto R = IRGM.Builder.CreateICmpNE(RHS, Zero);
      auto Value = IRGM.Builder.CreateAnd(L, R, "and");
      return cast(RValue::get(E->getType(), Value), getIntTy());
    }
        
    case tok::lor: {
      auto Zero = llvm::ConstantInt::get(getIntTy(), 0);
      auto L = IRGM.Builder.CreateICmpNE(LHS, Zero);
      auto R = IRGM.Builder.CreateICmpNE(RHS, Zero);
      auto Value = IRGM.Builder.CreateOr(L, R, "or");
      return cast(RValue::get(E->getType(), Value), getIntTy());
    }

    case tok::equals: {
      auto Value = IRGM.Builder.CreateICmpEQ(LHS, RHS, "eq");
      return cast(RValue::get(E->getType(), Value), getIntTy());
    }

    case tok::nequals: {
      auto Value = IRGM.Builder.CreateICmpNE(LHS, RHS, "neq");
      return cast(RValue::get(E->getType(), Value), getIntTy());
    }

    case tok::greater: {
      auto Value = IRGM.Builder.CreateICmpSGT(LHS, RHS, "gt");
      return cast(RValue::get(E->getType(), Value), getIntTy());
    }

    case tok::greater_eq: {
      auto Value = IRGM.Builder.CreateICmpSGE(LHS, RHS, "ge");
      return cast(RValue::get(E->getType(), Value), getIntTy());
    }

    case tok::less: {
      auto Value = IRGM.Builder.CreateICmpSLT(LHS, RHS, "lt");
      return cast(RValue::get(E->getType(), Value), getIntTy());
    }

    case tok::less_eq: {
      auto Value = IRGM.Builder.CreateICmpSLE(LHS, RHS, "le");
      return cast(RValue::get(E->getType(), Value), getIntTy());
    }

    default:
      llvm_unreachable("Invalid infix operand");
    }
  }

  RValue visitPrefixExpr(PrefixExpr *E) {
    auto Dest = emitRValue(E->getDest());

    switch (E->getOp().getKind()) {
    case tok::lnot: {
      auto Res = IRGM.Builder.CreateNot(Dest);
      return cast(RValue::get(E->getType(), Res), getIntTy());
    }

    case tok::minus: {
      auto Zero = llvm::ConstantInt::get(getIntTy(), 0);
      auto Res = IRGM.Builder.CreateSub(Zero, Dest);
      return RValue::get(E->getType(), Res);
    }

    default:
      llvm_unreachable("Invalid prefix operand");
    }
  }

  RValue visitCallExpr(CallExpr *E) {
    // Extract callee and arguments
    auto Callee = IRGM.getFunc(E->getCallee()->getIdentifierExpr()->getName());
    auto ArgsPttrn = E->getArgs()->getExprPattern();

    // Emit values for arguments
    std::vector<llvm::Value *> Args;
    for (auto Arg : ArgsPttrn->getValues())
      Args.push_back(emitRValue(Arg));

    // Emit call instruction
    auto Value = IRGM.Builder.CreateCall(Callee, Args);
    return RValue::get(E->getType(), Value);
  }

  RValue visitSubscriptExpr(SubscriptExpr *E) {
    // Emit base access
    auto Base = emitRValue(E->getBase());
    auto Idx = emitRValue(E->getSubscript()->getSubscriptStmt()->getValue());

    auto Zero = llvm::ConstantInt::get(getIntTy(), 0);

    llvm::Value *Value;
    // Emit access instructions
    auto Addr = IRGM.Builder.CreateGEP(Base, {Zero, Idx});
    if (auto Ty = dynamic_cast<ArrayType *>(E->getType()))
      Value = Addr;
    else
      Value = IRGM.Builder.CreateLoad(Addr, "index");
    return RValue::get(E->getType(), Value);
  }

  RValue visitParenExpr(ParenExpr *E);

public:
  RValue emitRValue(Expr *E) { return super::visit(E); }
  RValue emitRValue(Type *Ty) {
    switch (Ty->getKind()) {
    default:
      llvm_unreachable("Not a value type.");
#define TYPE(CLASS, PARENT)
#define VALUE_TYPE(CLASS, PARENT)                                              \
  case TypeKind::CLASS:                                                        \
    return emitRValue(static_cast<CLASS##Type *>(Ty));
#include "dusk/AST/TypeNodes.def"
    }
  }
};

} // anonymous namespace

RValue IRGenModule::emitRValue(Expr *E) {
  return RValueEmitter(*this).emitRValue(E);
}

RValue IRGenModule::emitRValue(Type *Ty) {
  return RValueEmitter(*this).emitRValue(Ty);
}

// MARK: - Invalid methods implementations

RValue RValueEmitter::visitParenExpr(ParenExpr *E) {
  llvm_unreachable("Parenthesis node hould never exist at this point.");
}
