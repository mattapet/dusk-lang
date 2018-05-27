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

#include "GenDecl.h"
#include "GenType.h"
#include "GenExpr.h"
#include "IRGenFunc.h"

using namespace dusk;
using namespace irgen;

static llvm::Value *emitCond(IRGenFunc &IRGF, Expr *E) {
  auto Cond = IRGF.IRGM.emitRValue(E);
  auto Ty = llvm::Type::getInt64Ty(IRGF.IRGM.LLVMContext);
  auto Zero = llvm::ConstantInt::get(Ty, 0);
  return IRGF.Builder.CreateICmpNE(Cond, Zero, "ifcond");
}

namespace {

/// Since we don't support structs, we have to simmulate their behavior.
class Iterator {
protected:
  IRGenFunc &IRGF;
  
  llvm::Constant *__step;
  /// Iteration step
  llvm::Value *Step;
  
public:
  Iterator(IRGenFunc &IRGF) : IRGF(IRGF) {
    auto Ty = llvm::Type::getInt64Ty(IRGF.IRGM.LLVMContext);
    __step = llvm::ConstantInt::get(Ty, 1);
  }
  
  /// Real iterator address
  Address Iter;
  
  llvm::Value *getStep() const { return Step; }
  virtual void emitHeader() = 0;
  virtual void emitCond(llvm::BasicBlock *T, llvm::BasicBlock *E) = 0;
  virtual void emitNext() = 0;
};
  
class RangeIterator : Iterator {
  /// Internal iterator
  Address __iter;
  /// Range size
  llvm::Value *__size;
  
  llvm::Value *Offset;
  
public:
  RangeIterator(IRGenFunc &IRGF, Decl *It, RangeStmt *R) : Iterator(IRGF) {
    auto Fn = IRGF.IRGM.getFunc("__iter_range");
    auto Start = IRGF.IRGM.emitRValue(R->getStart());
    auto End = IRGF.IRGM.emitRValue(R->getEnd());
    auto Ty = llvm::Type::getInt64Ty(IRGF.IRGM.LLVMContext);
    Iter = IRGF.IRGM.Builder.CreateAlloca(Ty);
    __iter = IRGF.IRGM.Builder.CreateAlloca(Ty);
    __size = IRGF.IRGM.Builder.CreateCall(Fn, {Start, End});
    Offset = Start;
    
    // If range is not inclusive, subtract one from it's size.
    if (R->isInclusive())
      __size = IRGF.IRGM.Builder.CreateAdd(__size, __step);
    
    // Decalre iterator
    Iter = IRGF.IRGM.declareVal(It);
    
    auto FnStep = IRGF.IRGM.getFunc("__iter_step");
    Step = IRGF.IRGM.Builder.CreateCall(FnStep, {Start, End});
  }
  
  virtual void emitHeader() override {
    auto Ty = llvm::Type::getInt64Ty(IRGF.IRGM.LLVMContext);
    auto Zero = llvm::ConstantInt::get(Ty, 0);
    IRGF.IRGM.Builder.CreateStore(Zero, __iter);
    IRGF.IRGM.Builder.CreateStore(Offset, Iter);
  }
  
  virtual void emitNext() override {
    // Load
    llvm::Value *__iter_val = IRGF.IRGM.Builder.CreateLoad(__iter, "__iter");
    llvm::Value *IterVal = IRGF.IRGM.Builder.CreateLoad(Iter);
    
    // Update
    __iter_val = IRGF.IRGM.Builder.CreateAdd(__iter_val, __step);
    IterVal = IRGF.IRGM.Builder.CreateAdd(IterVal, Step);
    
    // Store
    IRGF.IRGM.Builder.CreateStore(__iter_val, __iter);
    IRGF.IRGM.Builder.CreateStore(IterVal, Iter);
  }
  
  virtual void emitCond(llvm::BasicBlock *T, llvm::BasicBlock *E) override {
    llvm::Value *__iter_val = IRGF.IRGM.Builder.CreateLoad(__iter, "__iter");
    auto Cond = IRGF.IRGM.Builder.CreateICmpSLT(__iter_val, __size);
    IRGF.IRGM.Builder.CreateCondBr(Cond, /* then */T, /* else */E);
  }
  
};
  
class GenFunc : public ASTVisitor<GenFunc,
                                  /* Decl */ bool,
                                  /* Expr */ bool,
                                  /* Stmt */ bool,
                                  /* Pattern */ bool,
                                  /* TypeRepr */ bool> {
  typedef ASTVisitor super;

  IRGenFunc &IRGF;

public:
  GenFunc(IRGenFunc &IRGF) : IRGF(IRGF) {}

  Address declareValDecl(ValDecl *D) {
    IRGF.IRGM.Lookup.declareVar(D);
    auto Ty = codegenType(IRGF.IRGM, D->getType());
    auto Addr = IRGF.IRGM.Builder.CreateAlloca(Ty);

    // Array is a reference type.
    if (D->getType()->isRefType()) {
      auto PtrTy = llvm::PointerType::get(Ty, 0);
      auto Ptr = IRGF.IRGM.Builder.CreateAlloca(PtrTy);
      IRGF.IRGM.Builder.CreateStore(Addr, Ptr);
      std::swap(Ptr, Addr);
    }

    if (D->hasValue()) {
      auto Value = IRGF.IRGM.emitRValue(D->getValue());
      IRGF.IRGM.Builder.CreateStore(Value, Addr);
    }

    IRGF.IRGM.Vals.insert({D, Addr});
    return Addr;
  }

  bool visitVarDecl(VarDecl *D) { return declareValDecl(D).isValid(); }

  bool visitBlockStmt(BlockStmt *S) {
    for (auto N : S->getNodes())
      if (auto D = dynamic_cast<Decl *>(N)) {
        if (!super::visit(D))
          return false;
      } else if (auto E = dynamic_cast<Expr *>(N)) {
        if (!super::visit(E))
          return false;
      } else if (auto S = dynamic_cast<Stmt *>(N)) {
        if (!super::visit(S))
          return false;
      } else {
        llvm_unreachable("Unexpected node.");
      }
    return true;
  }

  bool visitBreakStmt(BreakStmt *S) {
    // Get end block of top level loop
    auto EndBB = IRGF.LoopStack.getInfo().getEndBlock();
    return IRGF.Builder.CreateBr(EndBB) != nullptr;
  }

  bool visitReturnStmt(ReturnStmt *S) {
    if (!IRGF.Fn->getReturnType()->isVoidTy()) {
      auto RetVal = IRGF.IRGM.emitRValue(S->getValue());
      IRGF.setRetVal(RetVal);
    }
    IRGF.Builder.CreateBr(IRGF.getRetBlock());
    return true;
  }

  bool visitIfStmt(IfStmt *S) {
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

  bool visitWhileStmt(WhileStmt *S) {
    // Create loop blocks
    auto HeaderBlock =
        llvm::BasicBlock::Create(IRGF.IRGM.LLVMContext, "loop.header", IRGF.Fn);
    auto BodyBlock =
        llvm::BasicBlock::Create(IRGF.IRGM.LLVMContext, "loop.body");
    auto EndBlock = llvm::BasicBlock::Create(IRGF.IRGM.LLVMContext, "loop.end");
    IRGF.IRGM.Lookup.push();
    LoopInfoRAII Push(IRGF.LoopStack, HeaderBlock, EndBlock);

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
    IRGF.IRGM.Lookup.pop();
    return true;
  }
                                  
  bool visitForStmt(ForStmt *S) {
    // Create loop blocks
    auto HeaderBlock =
        llvm::BasicBlock::Create(IRGF.IRGM.LLVMContext, "loop.header", IRGF.Fn);
    auto BodyBlock =
        llvm::BasicBlock::Create(IRGF.IRGM.LLVMContext, "loop.body");
    auto EndBlock = llvm::BasicBlock::Create(IRGF.IRGM.LLVMContext, "loop.end");
    IRGF.IRGM.Lookup.push();
    LoopInfoRAII Push(IRGF.LoopStack, HeaderBlock, EndBlock);

    // Add block to function.
    IRGF.Fn->getBasicBlockList().push_back(BodyBlock);
    IRGF.Fn->getBasicBlockList().push_back(EndBlock);

    RangeIterator Iter(IRGF, S->getIter(), S->getRange()->getRangeStmt());
    Iter.emitHeader();
    
    IRGF.IRGM.Builder.CreateBr(BodyBlock);
    IRGF.IRGM.Builder.SetInsertPoint(BodyBlock);
    if (!super::visit(S->getBody()))
      return false;
    
    Iter.emitNext();
    Iter.emitCond(BodyBlock, EndBlock);

    IRGF.Builder.SetInsertPoint(EndBlock);
    IRGF.IRGM.Lookup.pop();
    return true;
  }

  bool visitFuncStmt(FuncStmt *S) { return true; }
  bool visitRangeStmt(RangeStmt *S) { return true; }
  bool visitSubscriptStmt(SubscriptStmt *S) { return true; }
  bool visitExternStmt(ExternStmt *S) { return true; }
  bool visitFuncDecl(FuncDecl *S) { return true; }
  bool visitModuleDecl(ModuleDecl *D) { return true; }
  bool visitParamDecl(ParamDecl *D) { return true; }

#define EXPR(CLASS, PARENT)                                                    \
  bool visit##CLASS##Expr(CLASS##Expr *E) {                                    \
    IRGF.IRGM.emitRValue(E);                                                   \
    return true;                                                               \
  }
#include "dusk/AST/ExprNodes.def"
};

} // anonymous namespace

bool irgen::genFunc(IRGenFunc &IRGF, FuncStmt *F) {
  return GenFunc(IRGF).visit(F->getBody());
}

