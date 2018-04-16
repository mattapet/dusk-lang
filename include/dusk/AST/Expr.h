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
#include <vector>

namespace dusk {
class NumberLiteralExpr;
class VariableExpr;
class InfixExpr;
class FuncCall;
class CodeBlock;
class ParamList;
class ASTWalker;

/// Describes expression type.
enum struct ExprKind {
    NumberLiteral,
    Variable,
    Infix,
    Assign,
    FuncCall
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

class VariableExpr: public Expr {
    llvm::StringRef Name;
    llvm::SMLoc NameLoc;

public:
    VariableExpr(llvm::StringRef N, llvm::SMLoc NL);
    
    llvm::StringRef getName() const { return Name; }
    llvm::SMLoc getNameLoc() const { return NameLoc; }
    
    virtual llvm::SMRange getSourceRange() const override;
};

class InfixExpr: public Expr {
    Expr *LHS;
    Expr *RHS;
    Token Op;
    
public:
    InfixExpr(Expr *L, Expr *R, Token O);
    
    Expr *getLHS() const { return LHS; }
    Expr *getRHS() const { return RHS; }
    Token getOp() const { return Op; }
    
    virtual llvm::SMRange getSourceRange() const override;
};

class AssignExpr: public Expr {
    VariableExpr *LHS;
    Expr *RHS;
    
public:
    AssignExpr(VariableExpr *L, Expr *R);
    
    Expr *getLHS() const { return LHS; }
    Expr *getRHS() const { return RHS; }
    
    virtual llvm::SMRange getSourceRange() const override;
};

class FuncCall: public Expr {
    /// Function identifier
    llvm::StringRef Callee;
    
    /// Location of function identifier
    llvm::SMLoc FnLoc;
    
    ParamList *Args;
    
public:
    FuncCall(llvm::StringRef FN, llvm::SMLoc FL, ParamList *A);
    
    llvm::StringRef getCalle() const { return Callee; }
    llvm::SMLoc getFnLoc() const { return FnLoc; }
    ParamList *getArgs() { return Args; }
    
    virtual llvm::SMRange getSourceRange() const override;
};


} // namespace dusk

#endif /* DUSK_EXPR_H */
