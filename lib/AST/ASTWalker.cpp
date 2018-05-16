//===--- ASTWalker.cpp - AST traversal ------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//
//
// This file imeplemnts a recursive traversal of every node in an AST.
//
// The recursive class must be updated whenever an AST node is update, removed,
// and/or added. Number of ASTWalker implementations expect to be called on
// every node of the AST in specific order.
//
// Rules:
//      - Each node of the AST should be walked.
//
//      - Each node should be walked only once per traversal.
//
//      - Nodes should be traversed in in depth-first order.
//
//      - All nodes must be valid and should never have a NULL pointer.
//
//===----------------------------------------------------------------------===//

#include "dusk/AST/ASTWalker.h"
#include "dusk/AST/ASTVisitor.h"

using namespace dusk;

namespace {

/// This class implements recursive AST node traversal. It uses user-provided
/// instance of \c ASTWalker of any class derived from it to actually walk
/// each and every node of the AST.
class Traversal : public ASTVisitor<Traversal> {
  /// Walker to be used to walk the tree.
  ASTWalker &Walker;

  /// Convenience type alias.
  typedef ASTVisitor super;
  
  friend class ASTVisitor<Traversal>;
  
  // MARK: - Declarations
  
  bool visitLetDecl(LetDecl *D) {
    if (D->hasTypeRepr())
      if (!traverse(D->getTypeRepr()))
        return false;
    
    if (D->hasValue()) {
      auto E = traverse(D->getValue());
      if (!E)
        return false;
      D->setValue(E);
    }
    return true;
  }
  
  bool visitVarDecl(VarDecl *D) {
    if (D->hasTypeRepr())
      if (!traverse(D->getTypeRepr()))
        return false;
    
    if (D->hasValue()) {
      auto E = traverse(D->getValue());
      if (!E)
        return false;
      D->setValue(E);
    }
    return true;
  }
  
  bool visitParamDecl(ParamDecl *D) {
    return !D->hasTypeRepr() || traverse(D->getTypeRepr());
  }
  
  bool visitFuncDecl(FuncDecl *D) {
    if (!traverse(D->getArgs()))
      return false;
    return !D->hasTypeRepr() || traverse(D->getTypeRepr());
  }
  
  bool visitModuleDecl(ModuleDecl *D) {
    for (auto &N : D->getContents()) {
      if (auto D = dynamic_cast<Decl *>(N))
        if (!traverse(D))
          return false;
      
      if (auto E = dynamic_cast<Expr *>(N))
        if ((N = traverse(E)) == nullptr)
          return false;
      
      if (auto S = dynamic_cast<Stmt *>(N))
        if (!traverse(S))
          return false;
    }
    return true;
  }
  
  // MARK: - Expressions
  
  Expr *visitNumberLiteralExpr(NumberLiteralExpr *E) {
    return E;
  }
  
  Expr *visitArrayLiteralExpr(ArrayLiteralExpr *E) {
    return E;
  }
  
  Expr *visitIdentifierExpr(IdentifierExpr *E) {
    return E;
  }
  
  Expr *visitParenExpr(ParenExpr *E) {
    if (auto Ex = traverse(E->getExpr()))
      E->setExpr(Ex);
    else
      return nullptr;
    return E;
  }
  
  Expr *visitInfixExpr(InfixExpr *E) {
    auto Ex = traverse(E->getLHS());
    if (!Ex)
      return nullptr;
    E->setLHS(Ex);
    Ex = traverse(E->getRHS());
    if (!Ex)
      return nullptr;
    E->setRHS(Ex);
    return E;
  }
  
  Expr *visitAssignExpr(AssignExpr *E) {
    auto Ex = traverse(E->getDest());
    if (!Ex)
      return nullptr;
    E->setDest(Ex);
    Ex = traverse(E->getSource());
    if (!Ex)
      return nullptr;
    E->setSource(Ex);
    return E;
  }
  
  Expr *visitPrefixExpr(PrefixExpr *E) {
    if (auto Ex = traverse(E->getDest()))
      E->setDest(Ex);
    else
      return nullptr;
    return E;
  }
  
  Expr *visitCallExpr(CallExpr *E) {
    if (auto Ex = traverse(E->getCallee()))
      E->setCallee(Ex);
    else
      return nullptr;
    
    if (!traverse(E->getArgs()))
      return nullptr;
    return E;
  }
  
  Expr *visitSubscriptExpr(SubscriptExpr *E) {
    if (auto Ex = traverse(E->getBase()))
      E->setBase(Ex);
    else
      return nullptr;
    
    if (!traverse(E->getSubscript()))
      return nullptr;
    return E;
  }
  
  // MAKR: - Statements
  
  bool visitBreakStmt(BreakStmt *S) {
    return true;
  }
  
  bool visitReturnStmt(ReturnStmt *S) {
    if (S->hasValue()) {
      if (auto Val = traverse(S->getValue()))
        S->setValue(Val);
      else
        return false;
    }
    return true;
  }
  
  bool visitSubscriptStmt(SubscriptStmt *S) {
    if (auto Val = traverse(S->getValue()))
      S->setValue(Val);
    else
      return false;
    return true;
  }
  
  bool visitRangeStmt(RangeStmt *S) {
    auto Val = traverse(S->getStart());
    if (!Val)
      return false;
    S->setStart(Val);
    
    Val = traverse(S->getEnd());
    if (!Val)
      return false;
    S->setEnd(Val);
    return true;
  }
  
  bool visitBlockStmt(BlockStmt *S) {
    for (auto &N : S->getNodes()) {
      if (auto D = dynamic_cast<Decl *>(N)) {
        if (!traverse(D))
          return false;
      
      } else if (auto E = dynamic_cast<Expr *>(N)) {
        if ((N = traverse(E)) == nullptr)
          return false;
      
      } else if (auto S = dynamic_cast<Stmt *>(N)) {
        if (!traverse(S))
          return false;
        
      } else {
        llvm_unreachable("Unexpected AST node.");
      }
    }
    return true;
  }
  
  bool visitExternStmt(ExternStmt *S) {
    return traverse(S->getPrototype());
  }
  
  bool visitFuncStmt(FuncStmt *S) {
    if (!traverse(S->getPrototype()))
      return false;
    return traverse(S->getBody());
  }
  
  bool visitForStmt(ForStmt *S) {
    if (!traverse(S->getIter()))
      return false;
    if (!traverse(S->getRange()))
      return false;
    return traverse(S->getBody());
  }
  
  bool visitWhileStmt(WhileStmt *S) {
    if (auto C = traverse(S->getCond()))
      S->setCond(C);
    else
      return false;
    return traverse(S->getBody());
  }
  
  bool visitIfStmt(IfStmt *S) {
    if (auto C = traverse(S->getCond()))
      S->setCond(C);
    else
      return false;
    if (!traverse(S->getThen()))
      return false;
    return !S->hasElseBlock() || traverse(S->getElse());
  }
  
  // MARK: - Patterns
  
  bool visitExprPattern(ExprPattern *P) {
    for (auto &E : P->getValues()) {
      if (auto Val = traverse(E))
        E = Val;
      else
        return false;
    }
    return true;
  }
  
  bool visitVarPattern(VarPattern *P) {
    for (auto Var : P->getVars())
      if (!traverse(Var))
        return false;
    return false;
  }
  
  // MARK: - Type representations
  
  bool visitIdentTypeRepr(IdentTypeRepr *TR) {
    return true;
  }
  
  bool visitArrayTypeRepr(ArrayTypeRepr *TR) {
    if (!traverse(TR->getBaseTyRepr()))
      return false;
    return traverse(TR->getSize());
  }
  
public:
  /// Constructs a basic traversal object.
  Traversal(ASTWalker &W) : Walker(W) {}
  
  bool traverse(Decl *D) {
    // Skip current subtree
    if (!Walker.preWalkDecl(D))
      return true;
    if (!super::visit(D))
      return false;
    
    return Walker.postWalkDecl(D);
  }
  
  Expr *traverse(Expr *E) {
    auto Pre = Walker.preWalkExpr(E);
    // Skip current subtree
    if (!Pre.first || !Pre.second)
      return Pre.second;
    
    E = super::visit(E);
    
    if (E)
      E = Walker.postWalkExpr(E);
    return E;
  }
  
  bool traverse(Stmt *S) {
    // Skip current subtree
    if (!Walker.preWalkStmt(S))
      return true;
    if (!super::visit(S))
      return false;
    
    return Walker.postWalkStmt(S);
  }
  
  bool traverse(Pattern *P) {
    // Skip current subtree
    if (!Walker.preWalkPattern(P))
      return true;
    if (!super::visit(P))
      return false;
    return Walker.postWalkPattern(P);
  }
  
  bool traverse(TypeRepr *TR) {
    // Skip current subtree
    if (!Walker.preWalkTypeRepr(TR))
      return true;
    if (!super::visit(TR))
      return false;
    
    return Walker.postWalkTypeRepr(TR);
  }
  
  // MARK: - Declaration nodes

  bool visit(LetDecl *D) {
    // Skip current subtree
    if (!Walker.preWalk(D))
      return true;

    if (!super::visit_(D->getValue()))
      return false;
    return Walker.postWalk(D);
  }

  bool visit(FuncDecl *D) {
    // Skip subtree
    if (!Walker.preWalk(D))
      return true;

    if (!super::visit_(D->getArgs()))
      return false;
    return Walker.postWalk(D);
  }

  bool visit(ModuleDecl *D) {
    if (!Walker.preWalk(D))
      return true;

    for (auto C : D->getContents())
      if (!super::visit_(C))
        return false;
    return Walker.postWalk(D);
  }

  bool visit(ParamDecl *D) {
    // Skip subtree
    if (!Walker.preWalk(D))
      return true;

    return Walker.postWalk(D);
  }

  bool visit(VarDecl *D) {
    // Skip subtree
    if (!Walker.preWalk(D))
      return true;
    
    if (D->hasValue() && !super::visit_(D->getValue()))
      return false;
    return Walker.postWalk(D);
  }

  // MARK: - Expression nodes

  bool visit(NumberLiteralExpr *E) {
    // Skip subtree
    if (!Walker.preWalk(E))
      return true;
    return Walker.postWalk(E);
  }

  bool visit(ArrayLiteralExpr *E) {
    // Skip subtree
    if (!Walker.preWalk(E))
      return true;
    if (!super::visit_(E->getValues()))
      return false;
    return Walker.postWalk(E);
  }
  
  bool visit(IdentifierExpr *E) {
    // Skip subtree
    if (!Walker.preWalk(E))
      return true;

    return Walker.postWalk(E);
  }

  bool visit(ParenExpr *E) {
    // Skip subtree
    if (!Walker.preWalk(E))
      return true;

    if (!super::visit_(E->getExpr()))
      return false;
    return Walker.postWalk(E);
  }

  bool visit(AssignExpr *E) {
    // Skip subtree
    if (!Walker.preWalk(E))
      return true;

    if (!super::visit_(E->getDest()))
      return false;
    if (!super::visit_(E->getSource()))
      return false;
    return Walker.postWalk(E);
  }

  bool visit(InfixExpr *E) {
    // Skip subtree
    if (!Walker.preWalk(E))
      return true;

    if (!super::visit_(E->getLHS()))
      return false;
    if (!super::visit_(E->getRHS()))
      return false;
    return Walker.postWalk(E);
  }

  bool visit(PrefixExpr *E) {
    // Skip subtree
    if (!Walker.preWalk(E))
      return true;

    if (!super::visit_(E->getDest()))
      return false;
    return Walker.postWalk(E);
  }

  bool visit(CallExpr *E) {
    // Skip subtree
    if (!Walker.preWalk(E))
      return true;

    if (!super::visit_(E->getCallee()))
      return false;
    if (!super::visit_(E->getArgs()))
      return false;
    return Walker.postWalk(E);
  }

  bool visit(SubscriptExpr *E) {
    // Skip subtree
    if (!Walker.preWalk(E))
      return true;

    if (!super::visit_(E->getBase()))
      return false;
    if (!super::visit_(E->getSubscript()))
      return false;
    return Walker.postWalk(E);
  }

  // MARK: - Statement nodes

  bool visit(BreakStmt *S) {
    // Skip subtree
    if (!Walker.preWalk(S))
      return true;
    return Walker.postWalk(S);
  }

  bool visit(ReturnStmt *S) {
    // Skip subtree
    if (!Walker.preWalk(S))
      return true;

    if (S->hasValue() && !super::visit_(S->getValue()))
      return false;
    return Walker.postWalk(S);
  }

  bool visit(RangeStmt *S) {
    // Skip subtree
    if (!Walker.preWalk(S))
      return false;

    if (!super::visit_(S->getStart()))
      return false;
    if (!super::visit_(S->getEnd()))
      return false;
    return Walker.postWalk(S);
  }
  
  bool visit(SubscriptStmt *S) {
    // Skip subtree
    if (!Walker.preWalk(S))
      return true;
    
    if (!super::visit_(S->getValue()))
      return false;
    return Walker.postWalk(S);
  }

  bool visit(BlockStmt *S) {
    // Skip subtree
    if (!Walker.preWalk(S))
      return true;

    for (auto N : S->getNodes())
      if (!super::visit_(N))
        return false;
    return Walker.postWalk(S);
  }

  bool visit(ExternStmt *S) {
    // Skip subtree
    if (!Walker.preWalk(S))
      return true;
    
    if (!super::visit_(S->getPrototype()))
      return false;
    return Walker.postWalk(S);
  }
  
  bool visit(FuncStmt *S) {
    // Skip subtree
    if (!Walker.preWalk(S))
      return true;
    
    if (!super::visit_(S->getPrototype()))
      return false;
    if (!super::visit_(S->getBody()))
      return false;
    return Walker.postWalk(S);
  }
  
  bool visit(ForStmt *S) {
    // Skip subtree
    if (!Walker.preWalk(S))
      return false;

    if (!super::visit_(S->getRange()))
      return false;
    if (!super::visit_(S->getBody()))
      return false;
    return Walker.postWalk(S);
  }

  bool visit(IfStmt *S) {
    // Skip subtree
    if (!Walker.preWalk(S))
      return true;

    if (!super::visit_(S->getCond()))
      return false;
    if (!super::visit_(S->getThen()))
      return false;
    if (S->hasElseBlock() && !super::visit_(S->getElse()))
      return false;
    return Walker.postWalk(S);
  }

  bool visit(WhileStmt *S) {
    // Skip subtree
    if (!Walker.preWalk(S))
      return true;

    if (!super::visit_(S->getCond()))
      return false;
    if (!super::visit_(S->getBody()))
      return false;
    return Walker.postWalk(S);
  }

  // MARK: - Patterns

  bool visit(ExprPattern *P) {
    // Skip subtree
    if (!Walker.preWalk(P))
      return true;

    for (auto V : P->getValues())
      if (!super::visit_(V))
        return false;
    return Walker.postWalk(P);
  }

  bool visit(VarPattern *P) {
    // Skip subtree
    if (!Walker.preWalk(P))
      return true;

    for (auto V : P->getVars())
      if (!super::visit_(V))
        return false;
    return Walker.postWalk(P);
  }
};

} // End of anonymous namespace

// MARK: - Basic ASTNodes implementations

bool Decl::walk(ASTWalker &Walker) {
//  return Traversal(Walker).ASTVisitor::visit_(this);
  return Traversal(Walker).traverse(this);
}

bool Expr::walk(ASTWalker &Walker) {
  //  return Traversal(Walker).ASTVisitor::visit_(this);
    return Traversal(Walker).traverse(this) != nullptr;
}

bool Stmt::walk(ASTWalker &Walker) {
  //  return Traversal(Walker).ASTVisitor::visit_(this);
    return Traversal(Walker).traverse(this);
}

bool Pattern::walk(ASTWalker &Walker) {
  return Traversal(Walker).traverse(this);
}

bool TypeRepr::walk(ASTWalker &Walker) {
  return Traversal(Walker).traverse(this);
}
