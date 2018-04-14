//===--- ASTNode.cpp ------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/ASTNode.h"
#include "dusk/AST/ASTWalker.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"

using namespace dusk;

bool ASTNode::walk(ASTWalker &Walker) {
    if (auto *D = static_cast<Decl *>(this))
        return D->walk(Walker);
    if (auto *E = static_cast<Expr *>(this))
        return E->walk(Walker);
    if (auto *S = static_cast<Stmt *>(this))
        return S->walk(Walker);
    
    llvm_unreachable("Unexpexted AST node");
}
