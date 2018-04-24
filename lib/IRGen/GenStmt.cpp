//===--- GenStmt.cpp ------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "GenStmt.h"
#include "GenDecl.h"
#include "GenExpr.h"
#include <iostream>

using namespace dusk;
using namespace irgen;

// MARK: - Break statement

bool irgen::codegenStmt(Context &Ctx, Scope &Scp, BreakStmt *S) {
  if (!Scp.isBreakScope())
    llvm_unreachable("Unexpected break");
  auto Ret = Scp.isBreakScope() ? Ctx.getRange(&Scp)
                                : Ctx.getRange(Scp.getBreakParent());
  Ctx.Builder.CreateBr(Ret.End);
  return true;
}

// MARK: - Return statement

bool irgen::codegenStmt(Context &Ctx, Scope &Scp, ReturnStmt *S) {
  if (!Scp.isFnScope())
    llvm_unreachable("Unexpected 'return'.");
  if (S->hasValue()) {
    auto Val = codegenExpr(Ctx, S->getValue());
    Ctx.Builder.CreateRet(Val);
  } else {
    Ctx.Builder.CreateRetVoid();
  }
  return true;
}

// MARK: - Block statement

bool irgen::codegenStmt(Context &Ctx, Scope &Scp, BlockStmt *S) {
  for (auto N : S->getNodes()) {
    if (auto D = dynamic_cast<Decl *>(N))
      if (!codegenDecl(Ctx, D))
        return false;
    
    if (auto E = dynamic_cast<Expr *>(N))
      if (codegenExpr(Ctx, E) == nullptr)
        return false;
    
    if (auto S = dynamic_cast<Stmt *>(N))
      if (!codegenStmt(Ctx, Scp, S))
        return false;
  }
  return true;
}

// MARK: - Func statement

void genArgs(Context &Ctx, VarPattern *P) {
  for (auto Arg : P->getVars())
    if (!Ctx.declare(static_cast<ParamDecl *>(Arg)))
      llvm_unreachable("Redefinition of arg");
}

bool irgen::codegenStmt(Context &Ctx, Scope &Scp, FuncStmt *S) {
  auto Scp_ = Scope(&Scp, Scope::FnScope);
  auto F = Ctx.getFunc(S->getPrototype()->getName());
  auto Args = static_cast<FuncDecl *>(S->getPrototype())->getArgs();
  
  // Create entry block for the function
  auto BB = llvm::BasicBlock::Create(Ctx, "entry", F);
  Ctx.Builder.SetInsertPoint(BB);
  Ctx.push();
  genArgs(Ctx, Args);
  codegenStmt(Ctx, Scp_, S->getBody());
  Ctx.pop();
  return true;
}

// MARK: - If statement

llvm::Value *condGen(Context &Ctx, Expr *E) {
  auto CondV = codegenExpr(Ctx, E);
  auto Zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  return Ctx.Builder.CreateICmpNE(CondV, Zero, "cmpZero");
}

bool genBranch(Context &Ctx, Scope &Scp, llvm::BasicBlock *BB, Stmt *S) {
  Ctx.Builder.SetInsertPoint(BB);
  Ctx.push();
   codegenStmt(Ctx, Scp, S);
  Ctx.pop();
  return true;
}

bool irgen::codegenStmt(Context &Ctx, Scope &Scp, IfStmt *S) {
  auto Scp_ = Scope(&Scp, Scope::ControlScope);
  auto Cond = condGen(Ctx, S->getCond());
  auto F = Ctx.Builder.GetInsertBlock()->getParent();
  auto ThenBB = llvm::BasicBlock::Create(Ctx, "then", F);
  auto ElseBB = llvm::BasicBlock::Create(Ctx, "else");
  auto MergeBB = llvm::BasicBlock::Create(Ctx, "endif");
  F->getBasicBlockList().push_back(ElseBB);
  F->getBasicBlockList().push_back(MergeBB);
  
  Ctx.Builder.CreateCondBr(Cond, ThenBB, ElseBB);
  genBranch(Ctx, Scp_, ThenBB, S->getThen());
  if (S->hasElseBlock())
    genBranch(Ctx, Scp_, ElseBB, S->getElse());
  Ctx.Builder.SetInsertPoint(MergeBB);
  return true;
}

// MARK: - While statement

bool irgen::codegenStmt(Context &Ctx, Scope &Scp, WhileStmt *S) {
  return false;
}

// MARK: - For statement

bool irgen::codegenStmt(Context &Ctx, Scope &Scp, ForStmt *S) {
  return false;
}

// MARK: - Default statement

bool irgen::codegenStmt(Context &Ctx, Scope &Scp, Stmt *S) {
  switch (S->getKind()) {
    case StmtKind::Break:
      return codegenStmt(Ctx, Scp, static_cast<BreakStmt *>(S));
    case StmtKind::Return:
      return codegenStmt(Ctx, Scp, static_cast<ReturnStmt *>(S));
    case StmtKind::Range:
      return codegenStmt(Ctx, Scp, static_cast<RangeStmt *>(S));
    case StmtKind::Block:
      return codegenStmt(Ctx, Scp, static_cast<BlockStmt *>(S));
    case StmtKind::Func:
      return codegenStmt(Ctx, Scp, static_cast<FuncStmt *>(S));
    case StmtKind::For:
      return codegenStmt(Ctx, Scp, static_cast<ForStmt *>(S));
    case StmtKind::While:
      return codegenStmt(Ctx, Scp, static_cast<WhileStmt *>(S));
    case StmtKind::If:
      return codegenStmt(Ctx, Scp, static_cast<IfStmt *>(S));
    case StmtKind::Subscript:
      llvm_unreachable("Not implemented yet");
  }
}

