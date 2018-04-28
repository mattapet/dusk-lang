//===--- Context.cpp - Dusck context implementation -----------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//
//
// This file implements Dusk context implementation.
//
//===----------------------------------------------------------------------===//

#include "dusk/IRGen/Context.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/Type.h"
#include <vector>

#include "GenExpr.h"

using namespace dusk;
using namespace irgen;

// MARK: - Block range

BlockRange::BlockRange() : Start(nullptr), End(nullptr) {}
BlockRange::BlockRange(llvm::BasicBlock *S, llvm::BasicBlock *E)
    : Start(S), End(E) {}

// MARK: - Context values

ContextImpl::ContextImpl(ContextImpl *P) : Parent(P) {}

ContextImpl *ContextImpl::push() { return new ContextImpl(this); }

ContextImpl *ContextImpl::pop() {
  auto Ret = Parent.release();
  delete this;
  return Ret;
}

bool ContextImpl::isDeclared(StringRef Str) const {
  return Values.find(Str) != Values.end();
}

llvm::Value *ContextImpl::get(StringRef Str) const {
  auto Val = Values.find(Str);
  if (Val != Values.end())
    return Val->second;
  if (Parent != nullptr)
    return Parent->get(Str);
  return nullptr;
}

// MARK: - Context

Context::Context(llvm::LLVMContext &C, llvm::Module *M, llvm::IRBuilder<> &B)
    : Ctx(C), Impl(new ContextImpl()), Module(M), Builder(B) {}

Context::~Context() { delete Impl; }

bool Context::declareVal(const Decl *D) {
  // Check if already declared in current scope
  if (Impl->isDeclared(D->getName()) || Funcs[D->getName()] != nullptr)
    return false;

  // Check if global
  if (Depth == 0) {
    auto GV =
        Module->getOrInsertGlobal(D->getName(), llvm::Type::getInt64Ty(Ctx));
    Impl->Values[D->getName()] = GV;
    return true;
  }

  auto Ty = llvm::Type::getInt64Ty(Ctx);
  auto Var = Builder.CreateAlloca(Ty, 0, D->getName());
  Impl->Values[D->getName()] = Var;
  return true;
}

bool Context::declareFunc(const FuncDecl *Fn) {
  // Validate that we're in global scope.
  assert(Depth == 0 && "Function declaration must be declared in global scope");

  // Check if already declared in current scope
  if (Funcs[Fn->getName()] != nullptr)
    return false;

  auto FnType = static_cast<FunctionType *>(Fn->getType());
  auto Ty = llvm::Type::getInt64Ty(Ctx);
  auto Args = std::vector<llvm::Type *>(Fn->getArgs()->count(), Ty);
  llvm::Type *RetTy;

  // Resolve return type
  switch (FnType->getRetType()->getKind()) {
  case TypeKind::Int:
    RetTy = llvm::Type::getInt64Ty(Ctx);
    break;
  case TypeKind::Void:
    RetTy = llvm::Type::getVoidTy(Ctx);
    break;
  default:
    llvm_unreachable("Unsupported return type");
  }
  auto FT = llvm::FunctionType::get(RetTy, Args, false);
  Funcs[Fn->getName()] = FT;
  return true;
}

llvm::Value *Context::getVal(StringRef Str) const { return Impl->get(Str); }

llvm::FunctionType *Context::getFuncProto(StringRef Str) { return Funcs[Str]; }

llvm::Function *Context::getFunc(StringRef Str) {
  if (auto F = Module->getFunction(Str))
    return F;
  return nullptr;
}

bool Context::contains(StringRef Str) const {
  return Funcs.find(Str) != Funcs.end() || Impl->contains(Str);
}

void Context::push() {
  ++Depth;
  // Update the 'virtual' stack.
  Impl = Impl->push();
}

void Context::pop() {
  assert(Depth != 0 && "Cannot pop from global scope");
  --Depth;
  // Update the 'virtual' stack.
  Impl = Impl->pop();
}

void Context::pushScope(Scope *S, BlockRange R) {
  assert(ScopeRanges.find(S) == ScopeRanges.end() && "Duplicate scope range");
  ScopeRanges.try_emplace(S, R);
}

BlockRange &Context::getRange(Scope *S) {
  assert(ScopeRanges.find(S) != ScopeRanges.end() && "Missing scope range");
  return ScopeRanges[S];
}

void Context::popScope(Scope *S) {
  assert(ScopeRanges.find(S) != ScopeRanges.end() && "Missing scope range");
  ScopeRanges.erase(S);
}
