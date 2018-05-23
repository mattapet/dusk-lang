//===--- ASTContext.cpp ---------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/ASTContext.h"
#include "dusk/AST/Type.h"

using namespace dusk;

ASTContext::ASTContext()
    : TheIntType(new (*this) IntType()), TheVoidType(new (*this) VoidType()) {}

ASTContext::~ASTContext() {
  for (auto Cleanup : Cleanups) {
    Cleanup();
  }
}

void *ASTContext::Allocate(size_t Bytes) {
  if (Bytes == 0)
    return nullptr;

  auto Res = new uint8_t[Bytes];
  Cleanups.push_back([Res] { delete[] Res; });

  return static_cast<void *>(Res);
}

VoidType *ASTContext::getVoidType() const { return TheVoidType; }

IntType *ASTContext::getIntType() const { return TheIntType; }
