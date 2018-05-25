//===--- IRGenFunc.h - IR generation state for functions --------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "IRGenFunc.h"

#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/Type.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/NameLookup.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Type.h"

#include "GenExpr.h"
#include "GenType.h"
#include <vector>

using namespace dusk;
using namespace irgen;

IRGenFunc::IRGenFunc(IRGenModule &IRGM, llvm::IRBuilder<> &B, llvm::Function *F,
                     FuncStmt *FN)
    : IRGM(IRGM), Builder(B), Fn(F) {

  Proto = static_cast<FuncDecl *>(FN->getPrototype());
  HeaderBlock =
      llvm::BasicBlock::Create(IRGM.LLVMContext, Fn->getName() + ".header", Fn);
  BodyBlock =
      llvm::BasicBlock::Create(IRGM.LLVMContext, Fn->getName() + ".body");
  RetBlock = llvm::BasicBlock::Create(IRGM.LLVMContext, Fn->getName() + ".ret");
  emitHeader();
  Fn->getBasicBlockList().push_back(BodyBlock);
  Builder.SetInsertPoint(BodyBlock);
}

IRGenFunc::~IRGenFunc() { emitRet(); }

void IRGenFunc::emitHeader() {
  IRGM.Lookup.push();
  Builder.SetInsertPoint(HeaderBlock);

  // Create a return value if necessary
  if (!Fn->getReturnType()->isVoidTy())
    RetValue = Builder.CreateAlloca(Fn->getReturnType());

  unsigned idx = 0;
  auto Args = Proto->getArgs()->getVars();
  for (auto &Arg : Fn->args()) {
    IRGM.Lookup.declareLet(Args[idx]);
    auto Ty = codegenType(IRGM, Args[idx]->getType());

    // Reference type
    if (auto Arr = dynamic_cast<ArrayType *>(Args[idx]->getType()))
      Ty = llvm::PointerType::get(Ty, 0);
    auto Addr = IRGM.Builder.CreateAlloca(Ty);
    Builder.CreateStore(&Arg, Addr);
    IRGM.Vals.insert({Args[idx++], Addr});
  }
  Builder.CreateBr(BodyBlock);
}

void IRGenFunc::emitRet() {
  for (auto &BB : Fn->getBasicBlockList()) {
    if (BB.getTerminator() == nullptr) {
      Builder.SetInsertPoint(&BB);
      if (auto Succ = BB.getSingleSuccessor())
        Builder.CreateBr(Succ);
      else
        Builder.CreateBr(RetBlock);
    }
  }

  Fn->getBasicBlockList().push_back(RetBlock);
  Builder.SetInsertPoint(RetBlock);
  if (RetValue.isValid()) {
    auto Val = Builder.CreateLoad(RetValue);
    Builder.CreateRet(Val);
  } else {
    Builder.CreateRetVoid();
  }
  IRGM.Lookup.pop();
}

void IRGenFunc::setRetVal(llvm::Value *V) { Builder.CreateStore(V, RetValue); }

Address IRGenFunc::declare(Decl *N) { return IRGM.declareVal(N); }

Address IRGenFunc::getVal(StringRef N) { return IRGM.getVal(N); }

llvm::Function *IRGenFunc::getFunc(StringRef N) { return IRGM.getFunc(N); }
