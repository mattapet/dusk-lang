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

    bool visit(ConstDecl *D) {
        // Skip current subtree
        if (!Walker.preWalk(D))
            return true;

        if (!super::visit(D->getValue()))
            return false;
        return Walker.postWalk(D);
    }

    bool visit(FuncDecl *D) {
        // Skip subtree
        if (!Walker.preWalk(D))
            return true;

        for (auto V : D->getArgs()->getVars())
            if (!super::visit(V))
                return false;
        return Walker.postWalk(D);
    }

    bool visit(ModuleDecl *D) {
        if (!Walker.preWalk(D))
            return true;

        for (auto C : D->getContents())
            if (!super::visit(C))
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

        if (!super::visit(D->getValue()))
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
        
        if (!super::visit(E->getExpr()))
            return false;
        return false;
    }
    
    bool visit(AssignExpr *E) {
        // Skip subtree
        if (!Walker.preWalk(E))
            return true;

        if (!super::visit(E->getDest()))
            return false;
        if (!super::visit(E->getSource()))
            return false;
        return Walker.postWalk(E);
    }

    bool visit(InfixExpr *E) {
        // Skip subtree
        if (!Walker.preWalk(E))
            return true;

        if (!super::visit(E->getLHS()))
            return false;
        if (!super::visit(E->getRHS()))
            return false;
        return Walker.postWalk(E);
    }

    bool visit(PrefixExpr *E) {
        // Skip subtree
        if (!Walker.preWalk(E))
            return true;

        if (!super::visit(E->getDest()))
            return false;
        return Walker.postWalk(E);
    }
    
    bool visit(CallExpr *E) {
        // Skip subtree
        if (!Walker.preWalk(E))
            return true;
        
        for (auto V : E->getArgs()->getValues())
            if (!super::visit(V))
                return false;
        return Walker.postWalk(E);
    }

    bool visit(SubscriptExpr *E) {
        // Skip subtree
        if (!Walker.preWalk(E))
            return true;

        if (!super::visit(E->getBase()))
            return false;
        if (!super::visit(E->getSubscript()->getValue()))
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
        
        if (!super::visit(S->getValue()))
            return false;
        return Walker.postWalk(S);
    }
    
    bool visit(RangeStmt *S) {
        // Skip subtree
        if (!Walker.preWalk(S))
            return false;

        if (!super::visit(S->getStart()))
            return false;
        if (!super::visit(S->getEnd()))
            return false;
        return Walker.postWalk(S);
    }

    bool visit(BlockStmt *S) {
        // Skip subtree
        if (!Walker.preWalk(S))
            return true;

        for (auto N : S->getNodes())
            if (!super::visit(N))
                return false;
        return Walker.postWalk(S);
    }

    bool visit(ForStmt *S) {
        // Skip subtree
        if (!Walker.preWalk(S))
            return false;

        if (!super::visit(S->getRange()))
            return false;
        if (!super::visit(S->getBody()))
            return false;
        return !Walker.postWalk(S);
    }

    bool visit(FuncStmt *S) {
        // Skip subtree
        if (!Walker.preWalk(S))
            return true;

        if (!super::visit(S->getPrototype()))
            return false;
        if (!super::visit(S->getBody()))
            return false;
        return !Walker.postWalk(S);
    }

    bool visit(IfStmt *S) {
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

    bool visit(WhileStmt *S) {
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

