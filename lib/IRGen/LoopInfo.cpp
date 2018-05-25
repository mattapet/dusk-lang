//===---LoopInfo.cpp ------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "LoopInfo.h"

#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/BasicBlock.h"

using namespace dusk;
using namespace irgen;

LoopInfo::LoopInfo() : Header(nullptr), EndBlock(nullptr) {}

LoopInfo::LoopInfo(llvm::BasicBlock *H, llvm::BasicBlock *E)
    : Header(H), EndBlock(E) {
  assert(isValid() && "Constructing invalid loop info.");
}

void LoopInfoStack::push(llvm::BasicBlock *H, llvm::BasicBlock *E) {
  Active.emplace(H, E);
}

void LoopInfoStack::pop() {
  assert(Active.size() != 0 && "Cannot pop an empty stack.");
  Active.pop();
}

LoopInfoRAII::LoopInfoRAII(LoopInfoStack &S, llvm::BasicBlock *H,
                           llvm::BasicBlock *E)
    : Stack(S) {
  Stack.push(H, E);
}

LoopInfoRAII::~LoopInfoRAII() {
  Stack.pop();
}
