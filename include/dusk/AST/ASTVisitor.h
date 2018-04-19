//===--- ASTVisitor.h - Dusk language AST visitor ---------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_AST_VISITOR_H
#define DUSK_AST_VISITOR_H

#include "dusk/AST/ASTNode.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"

namespace dusk {

/// \brief This class represents a base visitor object of Dusk AST.
///
/// \description This class provides a default implementation of visit method
/// for \c ASTNode, \c Decl, \c Expr and \c Stmt. Derived classes may add
/// another "base type" methods, but should never override and/or change
/// behavior of these onces.
///
/// Class also provides a default implementation for all concrete Dusk AST
/// nodes. Derived classes should modify them to implement their own traversal
/// behaviour.
///
/// All of the \c visit methods return a \c bool indicating validity of
/// traversal. \c true indicates valid traversal of given node/subtree and
/// visitor may continue traversing. \c false means failure, upon which visitor
/// must immedietly terminate traversal of the AST.
template<typename Derived>
class ASTVisitor {
public:
    /// Returns a reference to the derived class.
    Derived &getDerived() {
        return *static_cast<Derived *>(this);
    }
    
    bool visit(ASTNode *N) {
        if (auto *D = dynamic_cast<Decl *>(N))
            return visit(D);
        if (auto *E = dynamic_cast<Expr *>(N))
            return visit(E);
        if (auto *S = dynamic_cast<Stmt *>(N))
            return visit(S);
        if (auto *P = dynamic_cast<Pattern *>(N))
            return visit(P);
        llvm_unreachable("Unexpected node");
    }
    
    /// Visit a concrete declaration node.
    bool visit(Decl *D) {
        switch (D->getKind()) {
        case DeclKind::Const:
            return getDerived().visit(static_cast<ConstDecl *>(D));
        case DeclKind::Func:
            return getDerived().visit(static_cast<FuncDecl *>(D));
        case DeclKind::Module:
            return getDerived().visit(static_cast<ModuleDecl *>(D));
        case DeclKind::Param:
            return getDerived().visit(static_cast<ParamDecl *>(D));
        case DeclKind::Var:
            return getDerived().visit(static_cast<VarDecl *>(D));
        case DeclKind::Error:
                return getDerived().visit(static_cast<ErrorDecl *>(D));
        }
    }

    /// Visit a conrete expression node.
    bool visit(Expr *E) {
        switch (E->getKind()) {
        case ExprKind::NumberLiteral:
            return getDerived().visit(static_cast<NumberLiteralExpr *>(E));
        case ExprKind::Identifier:
            return getDerived().visit(static_cast<IdentifierExpr *>(E));
        case ExprKind::Paren:
            return getDerived().visit(static_cast<ParenExpr *>(E));
        case ExprKind::Assign:
            return getDerived().visit(static_cast<AssignExpr *>(E));
        case ExprKind::Infix:
            return getDerived().visit(static_cast<InfixExpr *>(E));
        case ExprKind::Prefix:
            return getDerived().visit(static_cast<PrefixExpr *>(E));
        case ExprKind::Call:
            return getDerived().visit(static_cast<CallExpr *>(E));
        case ExprKind::Subscript:
            return getDerived().visit(static_cast<SubscriptExpr *>(E));
        }
    }

    /// Visit a concrete statement node.
    bool visit(Stmt *S) {
        switch (S->getKind()) {
        case StmtKind::Break:
            return getDerived().visit(static_cast<BreakStmt *>(S));
        case StmtKind::Return:
            return getDerived().visit(static_cast<ReturnStmt *>(S));
        case StmtKind::Range:
            return getDerived().visit(static_cast<RangeStmt *>(S));
        case StmtKind::Block:
            return getDerived().visit(static_cast<BlockStmt *>(S));
        case StmtKind::For:
            return getDerived().visit(static_cast<ForStmt *>(S));
        case StmtKind::Func:
            return getDerived().visit(static_cast<FuncStmt *>(S));
        case StmtKind::If:
            return getDerived().visit(static_cast<IfStmt *>(S));
        case StmtKind::While:
            return getDerived().visit(static_cast<WhileStmt *>(S));
        }
    }
    
    /// Visit a concrete pattern node.
    bool visit(Pattern *P) {
        switch (P->getKind()) {
        case PatternKind::Expr:
            return getDerived().visit(static_cast<ExprPattern *>(P));
        case PatternKind::Variable:
            return getDerived().visit(static_cast<VarPattern *>(P));
        case PatternKind::Subscript:
            return getDerived().visit(static_cast<SubscriptPattern *>(P));
        }
    }
};

} // namespace dusk

#endif /* DUSK_AST_VISITOR_H */
