//===--- ASTNode.cpp ------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/ASTNode.h"

#include "dusk/AST/ASTContext.h"
#include "dusk/AST/ASTWalker.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"

using namespace dusk;

void *ASTNode::operator new(size_t Bytes, ASTContext &Context) {
  return Context.Allocate(Bytes);
}

bool ASTNode::walk(ASTWalker &Walker) {
  if (auto D = dynamic_cast<Decl *>(this))
    return D->walk(Walker);
  
  else if (auto E = dynamic_cast<Expr *>(this))
    return E->walk(Walker) != nullptr;
  
  else if (auto S = dynamic_cast<Stmt *>(this))
    return S->walk(Walker);
  
  else
    llvm_unreachable("Unexpected AST node found.");
}


