//===--- Context.cpp ------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Sema/Context.h"
#include "dusk/AST/Decl.h"

using namespace dusk;
using namespace sema;

// MARK: - Context values

ContextImpl::ContextImpl(ContextImpl *P) : Parent(P) {}

ContextImpl *ContextImpl::push() { return new ContextImpl(this); }

ContextImpl *ContextImpl::pop() {
  auto Ret = Parent.release();
  delete this;
  return Ret;
}

bool ContextImpl::isDeclared(StringRef Str) const {
  return Vars.find(Str) != Vars.end() || Consts.find(Str) != Consts.end();
}

Decl *ContextImpl::getVar(StringRef Str) const {
  auto Var = Vars.find(Str);
  if (Var != Vars.end())
    return Var->second;
  
  if (Parent != nullptr)
    return Parent->getVar(Str);
  return nullptr;
}

Decl *ContextImpl::getVar(StringRef Str) {
  auto Var = Vars.find(Str);
  if (Var != Vars.end())
    return Var->second;
  
  if (Parent != nullptr)
    return Parent->getVar(Str);
  return nullptr;
}

Decl *ContextImpl::get(StringRef Str) const {
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

Context::Context(): Impl(new ContextImpl()) {}

Context::~Context() { delete Impl; }

bool Context::declareVar(Decl *D) {
  // Check if already declared in current scope
  if (Impl->isDeclared(D->getName()) || Funcs[D->getName()] != nullptr)
    return false;
  
  Impl->Vars[D->getName()] = D;
  return true;
}

bool Context::declareLet(Decl *D) {
  // Check if already declared in current scope
  if (Impl->isDeclared(D->getName()) || Funcs[D->getName()] != nullptr)
    return false;
  Impl->Consts[D->getName()] = D;
  return true;
}

bool Context::declareFunc(Decl *Fn) {
  // Validate that we're in global scope.
  assert(Depth == 0 && "Function declaration must be declared in global scope");
  
  // Check if already declared in current scope
  if (Funcs[Fn->getName()] != nullptr)
    return false;
  Funcs[Fn->getName()] = Fn;
  return true;
}

Decl *Context::getVal(StringRef Str) const { return Impl->get(Str); }

Decl *Context::getVar(StringRef Str) const { return Impl->getVar(Str); }

Decl *Context::getFunc(StringRef Str) { return Funcs[Str]; }


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


