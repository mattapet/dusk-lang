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
#include "dusk/AST/Decl.h"
#include "dusk/Parse/Token.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/SMLoc.h"
#include <vector>

namespace llvm {
    class Value;
}

namespace dusk {
    class NumberLiteralExpr;
    class VariableExpr;
    class InfixExpr;
    class FuncCall;
    class CodeBlock;
    class ParamList;
    class ASTWalker;
    
    
    /// Base class for all expression type nodes.
    class Expr: public ASTNode {
    public:
        virtual ~Expr() = default;
    };
    
    
    /// Number literal expression encalsulation.
    class NumberLiteralExpr: public Expr {
        int64_t Value;
        llvm::SMRange ValueLoc;
        
    public:
        NumberLiteralExpr(int64_t V, llvm::SMRange ValL);
        
        int64_t getValue() const { return Value; }
        
        virtual llvm::SMRange getSourceRange() const override;
        virtual bool walk(ASTWalker &Walker) override;
    };

    class VariableExpr: public Expr {
        llvm::StringRef Name;
        llvm::SMRange NameLoc;
    
    public:
        VariableExpr(llvm::StringRef N, llvm::SMRange NL);
        
        llvm::StringRef getName() const { return Name; }
        
        virtual llvm::SMRange getSourceRange() const override;
        virtual bool walk(ASTWalker &Walker) override;
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
        virtual bool walk(ASTWalker &Walker) override;
    };
    
    class AssignExpr: public Expr {
        VariableExpr *LHS;
        Expr *RHS;
        
    public:
        AssignExpr(VariableExpr *L, Expr *R);
        
        Expr *getLHS() const { return LHS; }
        Expr *getRHS() const { return RHS; }
        
        virtual llvm::SMRange getSourceRange() const override;
        virtual bool walk(ASTWalker &Walker) override;
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
        ParamList *getArgs() { return Args; }
        
        virtual llvm::SMRange getSourceRange() const override;
        virtual bool walk(ASTWalker &Walker) override;
    };
    
    
} // namespace dusk

#endif /* DUSK_EXPR_H */
