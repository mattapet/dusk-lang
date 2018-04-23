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

using namespace dusk;
using namespace irgen;

GenStmt::GenStmt(Stmt *S, Context &C)
: Statement(S), Ctx(C)
{}

bool GenStmt::gen() {
  return codegen(Statement);
}

bool GenStmt::codegen(BreakStmt *S) {
  return false;
}

bool GenStmt::codegen(ReturnStmt *S) {
  return false;
}

bool GenStmt::codegen(RangeStmt *S) {
  return false;
}

bool GenStmt::codegen(SubscriptStmt *S) {
  return false;
}

bool GenStmt::codegen(BlockStmt *S) {
  for (auto &N : S->getNodes()) {
    bool R = false;

    if (auto D = dynamic_cast<Decl *>(N))
      R = GenDecl(D, Ctx).gen();
    else if (auto E = dynamic_cast<Expr *>(N))
      R = GenExpr(E, Ctx).gen() != nullptr;
    else if (auto S = dynamic_cast<Stmt *>(N))
      R = GenStmt(S, Ctx).gen();
    else
      llvm_unreachable("Unexpected node");

    if (!R)
      return false;
  }
  return true;
}

bool GenStmt::codegen(ForStmt *S) {
  return false;
}

bool GenStmt::codegen(WhileStmt *S) {
  return false;
}

bool GenStmt::codegen(IfStmt *S) {
  auto CondV = GenExpr(S->getCond(), Ctx).gen();
  if (!CondV)
    llvm_unreachable("Error generating condition.");
  
  auto Zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(Ctx), 0);
  // Convert to bool by comparing with 0
  CondV = Ctx.Builder.CreateICmpNE(CondV, Zero, "ifcond");
  auto F = Ctx.Builder.GetInsertBlock()->getParent();
  
  auto *ThenBB = llvm::BasicBlock::Create(Ctx, "then", F);
  auto *ElseBB = llvm::BasicBlock::Create(Ctx, "else");
  auto *MergeBB = llvm::BasicBlock::Create(Ctx, "endif");
  
  Ctx.Builder.CreateCondBr(CondV, ThenBB, ElseBB);
  
  // Generate then block
  Ctx.Builder.SetInsertPoint(ThenBB);
  Ctx.push();
  if (!GenStmt(S->getThen(), Ctx).gen())
    llvm_unreachable("Error generating Then");
  Ctx.pop();
  // pop back to point after the condition.
  Ctx.Builder.CreateBr(MergeBB);
  
  
  if (!S->hasElseBlock()) {
    // Generate merge block
    F->getBasicBlockList().push_back(MergeBB);
    Ctx.Builder.SetInsertPoint(MergeBB);
    return true;
  }
    
  // Generate else block
  F->getBasicBlockList().push_back(ElseBB);
  Ctx.push();
  Ctx.Builder.SetInsertPoint(ElseBB);
  if (!GenStmt(S->getElse(), Ctx).gen())
    llvm_unreachable("Error generating else");
  Ctx.pop();
  // pop back to point after the condition.
  Ctx.Builder.CreateBr(MergeBB);
  
  // Generate merge block
  F->getBasicBlockList().push_back(MergeBB);
  Ctx.Builder.SetInsertPoint(MergeBB);
  return true;
}

bool GenStmt::codegen(FuncStmt *S) {
  GenDecl FD(S->getPrototype(), Ctx);
  // Declare
  if (!FD.gen())
    return false;

  // Get function
  auto F = Ctx.getFunc(S->getPrototype()->getName());
  if (!F)
    return false;

  // Create entry block of the function
  auto BB = llvm::BasicBlock::Create(Ctx, "entry", F);
  Ctx.Builder.SetInsertPoint(BB);

  // Push new context scope
  Ctx.push();
  if (!FD.genArgs()) {
    Ctx.pop();
    return false;
  }

  if (!codegen(S->getBody())) {
    Ctx.pop();
    return false;
  }

  Ctx.pop();
  return true;
}

bool GenStmt::codegen(Stmt *S) {
  switch (S->getKind()) {
    case StmtKind::Break:
      return codegen(static_cast<BreakStmt *>(S));
    case StmtKind::Return:
      return codegen(static_cast<ReturnStmt *>(S));
    case StmtKind::Range:
      return codegen(static_cast<RangeStmt *>(S));
    case StmtKind::Block:
      return codegen(static_cast<BlockStmt *>(S));
    case StmtKind::Func:
      return codegen(static_cast<FuncStmt *>(S));
    case StmtKind::For:
      return codegen(static_cast<ForStmt *>(S));
    case StmtKind::While:
      return codegen(static_cast<WhileStmt *>(S));
    case StmtKind::If:
      return codegen(static_cast<IfStmt *>(S));
    case StmtKind::Subscript:
      llvm_unreachable("Not implemented yet");
  }
}

