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
#include <vector>

using namespace dusk;
using namespace irgen;

IRGenModule::IRGenModule(ASTContext &Ctx, llvm::LLVMContext &LLVMCtx,
                         llvm::Module *M, llvm::IRBuilder<> &B)
    : Context(Ctx), LLVMContext(LLVMCtx), Module(M), Builder(B) {}

Address IRGenModule::declareVal(Decl *D) {
  auto Ty = llvm::Type::getInt64Ty(LLVMContext);
  
  if (Lookup.getDepth() == 0) {
    if (!Lookup.declareVar(D))
      llvm_unreachable("Redefinition of a variable");
    
    auto Ty = llvm::Type::getInt64Ty(LLVMContext);

    auto GV = new llvm::GlobalVariable(*Module, Ty, false,
                                         llvm::GlobalValue::InternalLinkage,
                                         nullptr, D->getName());
    GV->setInitializer(llvm::ConstantInt::get(Ty, 0));
    return GV;
  } else {
    if (!Lookup.declareVar(D))
      llvm_unreachable("Redefinition of a variable");
    auto Addr = Builder.CreateAlloca(Ty);
    Vals[D] = Addr;
    return Addr;
  }
}

Address IRGenModule::declareFunc(FuncDecl *D) {
  if (Lookup.contains(D->getName()))
    llvm_unreachable("Redefinition of a function");

  auto FnTy = static_cast<FunctionType *>(D->getType());
  auto Ty = llvm::Type::getInt64Ty(LLVMContext);
  auto Args = std::vector<llvm::Type *>(D->getArgs()->count(), Ty);

  // Resolve return type
  llvm::Type *RetTy;
  switch (FnTy->getRetType()->getKind()) {
  case TypeKind::Int:
    RetTy = llvm::Type::getInt64Ty(LLVMContext);
    break;
  case TypeKind::Void:
    RetTy = llvm::Type::getVoidTy(LLVMContext);
    break;
  default:
    llvm_unreachable("Unsupported return type");
  }
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
  auto Fn = llvm::Function::Create(Proto, llvm::GlobalValue::ExternalLinkage);
  return Fn;
}

