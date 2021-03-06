//===--- IRGenModule.cpp --------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "IRGenModule.h"

#include "dusk/AST/Decl.h"
#include "dusk/AST/Type.h"
#include "dusk/AST/ASTContext.h"
#include "dusk/Runtime/RuntimeFuncWrapper.h"
#include "dusk/Runtime/RuntimeFuncs.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/DerivedTypes.h"
#include <vector>

#include "GenType.h"
#include "GenDecl.h"

using namespace dusk;
using namespace irgen;

IRGenModule::IRGenModule(ASTContext &Ctx, llvm::LLVMContext &LLVMCtx,
                         llvm::Module *M, llvm::IRBuilder<> &B)
    : Context(Ctx), LLVMContext(LLVMCtx), Module(M), Builder(B) {}

Address IRGenModule::declareVal(Decl *D) { return codegenDecl(*this, D); }

Address IRGenModule::declareFunc(FuncDecl *D) {
  if (Lookup.contains(D->getName()))
    llvm_unreachable("Redefinition of a function");

  auto FnTy = static_cast<FunctionType *>(D->getType());

  std::vector<llvm::Type *> Args;
  for (auto Arg : D->getArgs()->getVars()) {
    auto Ty = codegenType(*this, Arg->getType());
    if (Arg->getType()->isRefType())
      Ty = llvm::PointerType::get(Ty, 0);
    Args.push_back(Ty);
  }

  llvm::Type *RetTy = codegenType(*this, FnTy->getRetType());
  // If reference type
  if (FnTy->getRetType()->isRefType())
    RetTy = llvm::PointerType::get(RetTy, 0);
  auto Proto = llvm::FunctionType::get(RetTy, Args, false);
  auto Fn = llvm::Function::Create(Proto, llvm::Function::ExternalLinkage,
                                   D->getName(), Module);
  Lookup.declareFunc(D);
  return Fn;
}

Address IRGenModule::getVal(StringRef N) {
  if (auto Val = Vals[Lookup.getVal(N)])
    return Val;
  return Module->getGlobalVariable(N, true);
}

llvm::Function *IRGenModule::getFunc(StringRef N) {
  return Module->getFunction(N);
}

llvm::Value *dusk::getRuntimeFunc(llvm::Module *M, StringRef N,
                                  ArrayRef<llvm::Type *> ArgsT,
                                  llvm::Type *RetT) {
  auto Proto = llvm::FunctionType::get(RetT, ArgsT, false);
  return llvm::Function::Create(Proto, llvm::GlobalValue::ExternalLinkage);
}
