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
class Traversal: public ASTVisitor<Traversal> {
    /// Walker to be used to walk the tree.
    ASTWalker &Walker;
    
    /// Convenience type alias.
    typedef ASTVisitor super;
    
public:
    /// Constructs a basic traversal object.
    Traversal(ASTWalker &W): Walker(W) {}
    
    // MARK: - Declaration nodes
    
    virtual bool visit(ConstDecl *D) {
        // Skip current subtree
        if (!Walker.preWalk(D))
            return true;
        
        if (!super::visit(D->getValue()))
            return false;
        return Walker.postWalk(D);
    }
    
    virtual bool visit(FuncDecl *D) {
        // Skip subtree
        if (!Walker.preWalk(D))
            return true;
        
        if (!super::visit(D->getArgs()))
            return false;
        
        return Walker.postWalk(D);
    }
    
    virtual bool visit(ParamDecl *D) {
        // Skip subtree
        if (!Walker.preWalk(D))
            return true;
        
        return Walker.postWalk(D);
    }
    
    virtual bool visit(VarDecl *D) {
        // Skip subtree
        if (!Walker.preWalk(D))
            return true;
        
        if (!super::visit(D->getValue()))
            return false;
        return Walker.postWalk(D);
    }
    
    
    // MARK: - Expression nodes
    
    virtual bool visit(AssignExpr *E) {
        // Skip subtree
        if (!Walker.preWalk(E))
            return true;
        
        if (!super::visit(E->getLHS()))
            return false;
        if (!super::visit(E->getRHS()))
            return false;
        return Walker.postWalk(E);
    }
    
    virtual bool visit(FuncCall *E) {
        // Skip subtree
        if (!Walker.preWalk(E))
            return true;
        
        if (!super::visit(E->getArgs()))
            return false;
        return Walker.postWalk(E);
    }
    
    virtual bool visit(InfixExpr *E) {
        // Skip subtree
        if (!Walker.postWalk(E))
            return true;
        
        if (!super::visit(E->getLHS()))
            return false;
        if (!super::visit(E->getRHS()))
            return false;
        return Walker.postWalk(E);
    }
    
    virtual bool visit(NumberLiteralExpr *E) {
        // Skip subtree
        if (!Walker.preWalk(E))
            return true;
        return Walker.postWalk(E);
    }
    
    virtual bool visit(VariableExpr *E) {
        // Skip subtree
        if (!Walker.preWalk(E))
            return true;
        
        return Walker.postWalk(E);
    }
    
    
    // MARK: - Statement nodes
    
    virtual bool visit(CodeBlock *S) {
        // Skip subtree
        if (!Walker.preWalk(S))
            return true;
        
        for (auto N : S->getNodes())
            if (!super::visit(N))
                return false;
        return Walker.postWalk(S);
    }
    
    virtual bool visit(ForStmt *S) {
        // Skip subtree
        if (!Walker.preWalk(S))
            return false;
        
        if (!super::visit(S->getRange()))
            return false;
        if (!super::visit(S->getBody()))
            return false;
        return !Walker.postWalk(S);
    }
    
    virtual bool visit(FuncStmt *S) {
        // Skip subtree
        if (!Walker.preWalk(S))
            return true;
        
        if (!super::visit(S->getPrototype()))
            return false;
        if (!super::visit(S->getBody()))
            return false;
        return !Walker.postWalk(S);
    }
    
    virtual bool visit(IfStmt *S) {
        // Skip subtree
        if (!Walker.preWalk(S))
            return true;
        
        if (!super::visit(S->getCond()))
            return false;
        if (!super::visit(S->getThen()))
            return false;
        if (S->hasElseBlock() && !super::visit(S->getElse()))
            return false;
        return !Walker.postWalk(S);
    }
    
    virtual bool visit(ParamList *S) {
        // Skip subtree
        if (!Walker.preWalk(S))
            return true;
        
        for (auto P : S->getParams())
            if (!super::visit(P))
                return false;
        return !Walker.postWalk(S);
    }
    
    virtual bool visit(WhileStmt *S) {
        // Skip subtree
        if (!Walker.preWalk(S))
            return true;
        
        if (!super::visit(S->getCond()))
            return false;
        if (!super::visit(S->getBody()))
            return false;
        return !Walker.postWalk(S);
    }
};
    
} // End of anonymous namespace


// MARK: - Basic ASTNodes implementations

bool ASTNode::walk(ASTWalker &Walker) {
    return Traversal(Walker).ASTVisitor::visit(this);
}

