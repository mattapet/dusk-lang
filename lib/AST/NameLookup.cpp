//===--- NameLookup.cpp ---------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/NameLookup.h"
#include "dusk/AST/Decl.h"

using namespace dusk;

// MARK: - Context values

LookupImpl::LookupImpl() : Parent(nullptr) {}

LookupImpl::LookupImpl(LookupImpl *P) : Parent(P) {}

LookupImpl *LookupImpl::push() { return new LookupImpl(this); }

LookupImpl *LookupImpl::pop() {
  auto Ret = Parent.release();
  delete this;
  return Ret;
}

bool LookupImpl::isDeclared(StringRef Str) const {
  return Vars.find(Str) != Vars.end() || Consts.find(Str) != Consts.end();
}

Decl *LookupImpl::getVar(StringRef Str) const {
  auto Var = Vars.find(Str);
  if (Var != Vars.end())
    return Var->second;
  
  if (Parent != nullptr)
    return Parent->getVar(Str);
  return nullptr;
}

Decl *LookupImpl::get(StringRef Str) const {
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

NameLookup::NameLookup(): Impl(new LookupImpl()) {}

NameLookup::~NameLookup() { delete Impl; }

bool NameLookup::declareVar(Decl *D) {
  // Check if already declared in current scope
  if (Impl->isDeclared(D->getName()) || Funcs[D->getName()] != nullptr)
    return false;
  
  Impl->Vars[D->getName()] = D;
  return true;
}

bool NameLookup::declareLet(Decl *D) {
  // Check if already declared in current scope
  if (Impl->isDeclared(D->getName()) || Funcs[D->getName()] != nullptr)
    return false;
  Impl->Consts[D->getName()] = D;
  return true;
}

bool NameLookup::declareFunc(Decl *Fn) {
  // Validate that we're in global scope.
  assert(Depth == 0 && "Function declaration must be declared in global scope");
  
  // Check if already declared in current scope
  if (Funcs[Fn->getName()] != nullptr)
    return false;
  Funcs[Fn->getName()] = Fn;
  return true;
}

Decl *NameLookup::getVal(StringRef Str) const { return Impl->get(Str); }

Decl *NameLookup::getVar(StringRef Str) const { return Impl->getVar(Str); }

Decl *NameLookup::getFunc(StringRef Str) { return Funcs[Str]; }


bool NameLookup::contains(StringRef Str) const {
  return Funcs.find(Str) != Funcs.end() || Impl->contains(Str);
}

void NameLookup::push() {
  ++Depth;
  // Update the 'virtual' stack.
  Impl = Impl->push();
}

void NameLookup::pop() {
  assert(Depth != 0 && "Cannot pop from global scope");
  --Depth;
  // Update the 'virtual' stack.
  Impl = Impl->pop();
}


