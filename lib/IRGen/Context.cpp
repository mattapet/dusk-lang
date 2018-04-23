

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
#include <vector>

#include "GenExpr.h"

using namespace dusk;
using namespace irgen;

// MARK: - Context values

ContextVals::ContextVals(ContextVals *P) : Parent(P) {}

ContextVals *ContextVals::push() { return new ContextVals(this); }

ContextVals *ContextVals::pop() {
  auto Ret = Parent.release();
  delete this;
  return Ret;
}

bool ContextVals::isDeclared(StringRef Str) const {
  return Vars.find(Str) != Vars.end() || Consts.find(Str) != Consts.end();
}

llvm::Value *ContextVals::getVar(StringRef Str) const {
  auto Var = Vars.find(Str);
  if (Var != Vars.end())
    return Var->second;

  if (Parent != nullptr)
    return Parent->getVar(Str);
  return nullptr;
}

llvm::Value *ContextVals::getVar(StringRef Str) {
  auto Var = Vars.find(Str);
  if (Var != Vars.end())
    return Var->second;

  if (Parent != nullptr)
    return Parent->getVar(Str);
  return nullptr;
}

llvm::Value *ContextVals::get(StringRef Str) const {
  if (auto Var = getVar(Str))
    return Var;

  auto Const = Consts.find(Str);
  if (Const != Consts.end())
    return Const->second;

  if (Parent != nullptr)
    return Parent->get(Str);
  return nullptr;
}

// MARK: - Context

Context::Context(llvm::LLVMContext &C, llvm::Module *M, llvm::IRBuilder<> &B)
    : Ctx(C), Vals(new ContextVals()), Module(M), Builder(B) {}

Context::~Context() { delete Vals; }

bool Context::declare(const VarDecl *D) {
  // Check if already declared in current scope
  if (Vals->isDeclared(D->getName()) || Funcs[D->getName()] != nullptr)
    return false;

  // Check if global
  if (Depth == 0) {
    auto GV =
        Module->getOrInsertGlobal(D->getName(), llvm::Type::getInt64Ty(Ctx));
    Vals->Vars[D->getName()] = GV;
    return true;
  }

  auto Ty = llvm::Type::getInt64Ty(Ctx);
  auto Var = Builder.CreateAlloca(Ty, 0, D->getName());
  Vals->Vars[D->getName()] = Var;
  return true;
}

bool Context::declare(const ParamDecl *D) {
  assert(Depth != 0 && "Param declaration happen only in functions");
  // Check if already declared in current scope
  if (Vals->isDeclared(D->getName()) || Funcs[D->getName()] != nullptr)
    return false;

  auto Ty = llvm::Type::getInt64Ty(Ctx);
  auto Par = Builder.CreateAlloca(Ty, 0, D->getName());
  Vals->Vars[D->getName()] = Par;
  return true;
}

bool Context::declare(const ConstDecl *D) {
  // Check if already declared in current scope
  if (Vals->isDeclared(D->getName()) || Funcs[D->getName()] != nullptr)
    return false;

  // Check if global
  if (Depth == 0) {
    auto GV =
        Module->getOrInsertGlobal(D->getName(), llvm::Type::getInt64Ty(Ctx));
    Vals->Consts[D->getName()] = GV;
    return true;
  }

  auto Ty = llvm::Type::getInt64Ty(Ctx);
  auto Const = Builder.CreateAlloca(Ty, 0, D->getName());
  Vals->Consts[D->getName()] = Const;
  return true;
}

bool Context::declare(const FuncDecl *Fn) {
  // Validate that we're in global scope.
  assert(Depth == 0 && "Function declaration must be declared in global scope");

  // Check if already declared in current scope
  if (Funcs[Fn->getName()] != nullptr)
    return false;

  auto Ty = llvm::Type::getInt64Ty(Ctx);
  auto Args = std::vector<llvm::Type *>(Fn->getArgs()->count(), Ty);
  auto FT = llvm::FunctionType::get(llvm::Type::getVoidTy(Ctx), Args, false);
  Funcs[Fn->getName()] = FT;
  return true;
}

llvm::Value *Context::getVal(StringRef Str) const { return Vals->get(Str); }

llvm::Value *Context::getVar(StringRef Str) { return Vals->getVar(Str); }

llvm::FunctionType *Context::getFuncProto(StringRef Str) { return Funcs[Str]; }

llvm::Function *Context::getFunc(StringRef Str) {
  if (auto F = Module->getFunction(Str))
    return F;
  return nullptr;
}

bool Context::contains(StringRef Str) const {
  return Funcs.find(Str) != Funcs.end() || Vals->contains(Str);
}

void Context::push() {
  ++Depth;
  // Update the 'virtual' stack.
  Vals = Vals->push();
}

void Context::pop() {
  assert(Depth != 0 && "Cannot pop from global scope");
  --Depth;
  // Update the 'virtual' stack.
  Vals = Vals->pop();
}

