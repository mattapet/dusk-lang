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
    for (auto V : Vals->getValues())
      Values.push_back(
          static_cast<llvm::Constant *>((llvm::Value *)emitRValue(V)));
    
    // Create array
    auto Value = llvm::ConstantArray::get(Ty, Values);
    return RValue::get(E->getType(), Value);
  }

  RValue visitIdentifierExpr(IdentifierExpr *E) {
    auto Addr = IRGM.getVal(E->getName());
    auto Value = IRGM.Builder.CreateLoad(Addr, E->getName()+".load");
    return RValue::get(E->getType(), Value);
  }

  RValue visitAssignExpr(AssignExpr *E) { return {}; }

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
      // TODO: Cast to bits
      auto Value = IRGM.Builder.CreateAnd(LHS, RHS, "and");
      // TODO: Cast back to int64_t
      return RValue::get(E->getType(), Value);
    }
        
    case tok::land: {
      // TODO: Cast to bits
      auto Value = IRGM.Builder.CreateOr(LHS, RHS, "or");
      // TODO: Cast back to int64_t
      return RValue::get(E->getType(), Value);
    }
        
    case tok::equals: {
      auto Value = IRGM.Builder.CreateICmpEQ(LHS, RHS, "eq");
      // TODO: Cast back to int64_t
      return RValue::get(E->getType(), Value);
    }
      
    case tok::nequals: {
      auto Value = IRGM.Builder.CreateICmpNE(LHS, RHS, "neq");
      // TODO: Cast back to int64_t
      return RValue::get(E->getType(), Value);
    }
        
    case tok::greater: {
      auto Value = IRGM.Builder.CreateICmpSGT(LHS, RHS, "gt");
      // TODO: Cast back to int64_t
      return RValue::get(E->getType(), Value);
    }
        
    case tok::greater_eq: {
      auto Value = IRGM.Builder.CreateICmpSGE(LHS, RHS, "ge");
      // TODO: Cast back to int64_t
      return RValue::get(E->getType(), Value);
    }
        
    case tok::less: {
      auto Value = IRGM.Builder.CreateICmpSLT(LHS, RHS, "lt");
      // TODO: Cast back to int64_t
      return RValue::get(E->getType(), Value);
    }
        
    case tok::less_eq: {
      auto Value = IRGM.Builder.CreateICmpSLE(LHS, RHS, "le");
      // TODO: Cast back to int64_t
      return RValue::get(E->getType(), Value);
    }
        
    default:
      llvm_unreachable("Invalid infix operand");
    }
  }

  RValue visitPrefixExpr(PrefixExpr *E) {
    auto Dest = emitRValue(E->getDest());
    
    switch (E->getOp().getKind()) {
    case tok::lnot: {
      // TODO: Cast back to int64
      auto Res = IRGM.Builder.CreateNot(Dest);
      return RValue::get(E->getType(), Res);
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
    auto Idx = emitRValue(E->getSubscript()->getSubscripStmt()->getValue());
    
    auto Zero = llvm::ConstantInt::get(getIntTy(), 0);
    // Emit access instructions
    auto Addr = IRGM.Builder.CreateGEP(Base, {Zero, Idx});
    auto Value = IRGM.Builder.CreateLoad(Addr, "indexarr");
    return RValue::get(E->getType(), Value);
  }

  RValue visitParenExpr(ParenExpr *E);
  
public:
  RValue emitRValue(Expr *E) { return super::visit(E); }
};
  
} // anonymous namespace

RValue IRGenModule::emitRValue(Expr *E) {
  return RValueEmitter(*this).emitRValue(E);
}

// MARK: - Invalid methods implementations

RValue RValueEmitter::visitParenExpr(ParenExpr *E) {
  llvm_unreachable("Parenthesis node hould never exist at this point.");
}
