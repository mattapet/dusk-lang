//===--- GenFunc.cpp ------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "GenFunc.h"

#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/Type.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/ASTVisitor.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Type.h"

#include "GenExpr.h"
#include "IRGenFunc.h"

using namespace dusk;
using namespace irgen;

static llvm::Value *emitCond(IRGenFunc &IRGF, Expr *E) {
  auto Cond = codegenExpr(IRGF.IRGM, E);
  auto Ty = llvm::Type::getInt64Ty(IRGF.IRGM.LLVMContext);
  auto Zero = llvm::ConstantInt::get(Ty, 0);
  return IRGF.Builder.CreateICmpNE(Cond, Zero, "ifcond");
}

namespace {

class GenFunc: public ASTVisitor<GenFunc> {
  typedef ASTVisitor super;

  IRGenFunc &IRGF;

public:
  GenFunc(IRGenFunc &IRGF) : IRGF(IRGF) {}

  bool visit(ValDecl *D) {
    auto Addr = IRGF.declare(D);
    if (D->hasValue()) {
      auto Val = codegenExpr(IRGF.IRGM, D->getValue());
      IRGF.Builder.CreateStore(Val, Addr);
    }
    return Addr.isValid();
  }

  bool visit(BlockStmt *S) {
    for (auto N : S->getNodes())
      if (!super::visit(N))
        return false;
    return true;
  }

  bool visit(BreakStmt *S) {
    // Get end block of top level loop
    auto EndBB = IRGF.LoopStack.getInfo().getEndBlock();
    return IRGF.Builder.CreateBr(EndBB) != nullptr;
  }

  bool visit(ReturnStmt *S) {
    if (!IRGF.Fn->getReturnType()->isVoidTy()) {
      auto RetVal = codegenExpr(IRGF.IRGM, S->getValue());
      IRGF.setRetVal(RetVal);
    }
    IRGF.Builder.CreateBr(IRGF.getRetBlock());
    return true;
  }

  bool visit(IfStmt *S) {
    // Create basic blocks
    auto ThenBB =
        llvm::BasicBlock::Create(IRGF.IRGM.LLVMContext, "if.then", IRGF.Fn);
    auto ElseBB = llvm::BasicBlock::Create(IRGF.IRGM.LLVMContext, "if.else");
    auto ContBB = llvm::BasicBlock::Create(IRGF.IRGM.LLVMContext, "if.cont");
    // Add basic blocks to function
    IRGF.Fn->getBasicBlockList().push_back(ElseBB);
    IRGF.Fn->getBasicBlockList().push_back(ContBB);

    // Emit conditional jump
    auto Cond = emitCond(IRGF, S->getCond());
    if (S->hasElseBlock())
      IRGF.Builder.CreateCondBr(Cond, ThenBB, ElseBB);
    else
      IRGF.Builder.CreateCondBr(Cond, ThenBB, ContBB);

    // Emit Then branch
    IRGF.Builder.SetInsertPoint(ThenBB);
    IRGF.IRGM.Lookup.push();
    if (!super::visit(S->getThen()))
      return false;
    IRGF.IRGM.Lookup.pop();
    if (IRGF.Builder.GetInsertBlock()->getTerminator() == nullptr)
      IRGF.Builder.CreateBr(ContBB);

    // Emit else branch
    if (S->hasElseBlock()) {
      IRGF.Builder.SetInsertPoint(ElseBB);
      IRGF.IRGM.Lookup.push();
      if (!super::visit(S->getElse()))
        return false;
      IRGF.IRGM.Lookup.pop();
      if (IRGF.Builder.GetInsertBlock()->getTerminator() == nullptr)
        IRGF.Builder.CreateBr(ContBB);
    } else {
      ElseBB->removeFromParent();
    }

    // Set pointer to the cont block.
    IRGF.Builder.SetInsertPoint(ContBB);
    return true;
  }

  bool visit(WhileStmt *S) {
    // Create loop blocks
    auto HeaderBlock =
        llvm::BasicBlock::Create(IRGF.IRGM.LLVMContext, "loop.header", IRGF.Fn);
    auto BodyBlock =
        llvm::BasicBlock::Create(IRGF.IRGM.LLVMContext, "loop.body");
    auto EndBlock = llvm::BasicBlock::Create(IRGF.IRGM.LLVMContext, "loop.end");
    IRGF.IRGM.Lookup.push();
    IRGF.LoopStack.push(HeaderBlock, EndBlock);

    // Add block to function.
    IRGF.Fn->getBasicBlockList().push_back(BodyBlock);
    IRGF.Fn->getBasicBlockList().push_back(EndBlock);

    IRGF.Builder.CreateBr(HeaderBlock);
    // Emit condition
    IRGF.Builder.SetInsertPoint(HeaderBlock);
    auto Cond = emitCond(IRGF, S->getCond());
    IRGF.Builder.CreateCondBr(Cond, BodyBlock, EndBlock);

    // Emit loop body
    IRGF.Builder.SetInsertPoint(BodyBlock);
    if (!super::visit(S->getBody()))
      return false;
    // Jump back to the condition
    if (IRGF.Builder.GetInsertBlock()->getTerminator() == nullptr)
      IRGF.Builder.CreateBr(HeaderBlock);

    IRGF.Builder.SetInsertPoint(EndBlock);
    IRGF.LoopStack.pop();
    IRGF.IRGM.Lookup.pop();
    return true;
  }

  bool visit(ForStmt *S) { return true; }
  bool visit(FuncStmt *S) { return true; }
  bool visit(RangeStmt *S) { return true; }
  bool visit(SubscriptStmt *S) { return true; }
  bool visit(ExternStmt *S) { return true; }
  bool visit(FuncDecl *S) { return true; }
  bool visit(ModuleDecl *D) { return true; }
  bool visit(ParamDecl *D) { return true; }
  bool visit(NumberLiteralExpr *E) {
    return codegenExpr(IRGF.IRGM, E) != nullptr;
  }
  bool visit(IdentifierExpr *E) {
    return codegenExpr(IRGF.IRGM, E) != nullptr;
  }
  bool visit(ParenExpr *E) {
    return codegenExpr(IRGF.IRGM, E) != nullptr;
  }
  bool visit(AssignExpr *E) {
    return codegenExpr(IRGF.IRGM, E) != nullptr;
  }
  bool visit(InfixExpr *E) {
    return codegenExpr(IRGF.IRGM, E) != nullptr;
  }
  bool visit(PrefixExpr *E) {
    return codegenExpr(IRGF.IRGM, E) != nullptr;
  }
  bool visit(CallExpr *E) {
    return codegenExpr(IRGF.IRGM, E) != nullptr;
  }
  bool visit(SubscriptExpr *E) {
    return codegenExpr(IRGF.IRGM, E) != nullptr;
  }
};

} // anonymous namespace

bool irgen::genFunc(IRGenFunc &IRGF, FuncStmt *F) {
  GenFunc GF(IRGF);
  return GF.ASTVisitor::visit(F->getBody());
}

