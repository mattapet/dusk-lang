//===--- Expr.h - Dusk expression ASTs --------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_EXPR_H
#define DUSK_EXPR_H

#include "dusk/AST/ASTNode.h"
#include "dusk/Parse/Token.h"
#include "llvm/Support/SMLoc.h"

namespace dusk {
class NumberLiteralExpr;
class IdentifierExpr;
class BinrayExpr;
class CallExpr;
class SubscriptExpr;
class BlockStmt;
class ExprPattern;
class SubscriptPattern;
class ASTWalker;

/// Describes expression type.
enum struct ExprKind {
    NumberLiteral,
    Identifier,
    Binary,
    Assign,
    Unary,
    Call,
    Subscript
};
    
/// Base class for all expression type nodes.
class Expr: public ASTNode {
    /// Expression type
    ExprKind Kind;
    
public:
    Expr(ExprKind K): Kind(K) {}
    virtual ~Expr() = default;
    
    ExprKind getKind() const { return Kind; }
};


/// Number literal expression encalsulation.
class NumberLiteralExpr: public Expr {
    int Value;
    llvm::SMRange ValueLoc;
    
public:
    NumberLiteralExpr(int V, llvm::SMRange ValL);
    
    llvm::SMRange getValLoc() const { return ValueLoc; }
    int getValue() const { return Value; }
    
    virtual llvm::SMRange getSourceRange() const override;
};
    
class IdentifierExpr: public Expr {
    llvm::StringRef Name;
    llvm::SMLoc NameLoc;
    
public:
    IdentifierExpr(llvm::StringRef N, llvm::SMLoc L);
    
    llvm::StringRef getName() const { return Name; }
    llvm::SMLoc getNameLoc() const { return NameLoc; }
    
    virtual llvm::SMRange getSourceRange() const override;
};

class BinrayExpr: public Expr {
    Expr *LHS;
    Expr *RHS;
    Token Op;
    
public:
    BinrayExpr(Expr *L, Expr *R, Token O);
    
    Expr *getLHS() const { return LHS; }
    Expr *getRHS() const { return RHS; }
    Token getOp() const { return Op; }
    
    virtual llvm::SMRange getSourceRange() const override;
};

class AssignExpr: public Expr {
    Expr *Dest;
    Expr *Source;
    
public:
    AssignExpr(Expr *L, Expr *R);
    
    Expr *getDest() const { return Dest; }
    Expr *getSource() const { return Source; }
    
    virtual llvm::SMRange getSourceRange() const override;
};
    
class UnaryExpr: public Expr {
    Expr *Dest;
    Token Op;
    
public:
    UnaryExpr(Expr *D, Token O);
    
    Expr *getDest() const { return Dest; }
    Token getOp() const { return Op; }
    
    virtual llvm::SMRange getSourceRange() const override;
};
    
class CallExpr: public Expr {
    /// Function identifier
    IdentifierExpr *Callee;
    
    /// Function arguments
    ExprPattern *Args;
    
public:
    CallExpr(IdentifierExpr *C, ExprPattern *A);
    
    IdentifierExpr *getCalle() const { return Callee; }
    ExprPattern *getArgs() { return Args; }
    
    virtual llvm::SMRange getSourceRange() const override;
};
    
class SubscriptExpr: public Expr {
    /// Base identifier
    IdentifierExpr *Base;
    
    /// Subscription pattern
    SubscriptPattern *Subscript;
    
public:
    SubscriptExpr(IdentifierExpr *B, SubscriptPattern *S);
    
    IdentifierExpr *getBase() { return Base; }
    SubscriptPattern *getSubscript() { return Subscript; }
    
    virtual llvm::SMRange getSourceRange() const override;
};

} // namespace dusk

#endif /* DUSK_EXPR_H */
