//===--- Stmt.h - Dusk statement ASTs ---------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_STMT_H
#define DUSK_STMT_H

#include "dusk/AST/ASTNode.h"
#include "dusk/Parse/Token.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/SMLoc.h"
#include <vector>

namespace dusk {
class Expr;
class BlockStmt;
class ParamDecl;
class FuncDecl;
class IdentifierExpr;
class ASTWalker;

/// Describes statement type.
enum struct StmtKind {
    Break,
    Return,
    Range,
    Block,
    Func,
    For,
    While,
    If
};
    
class Stmt: public ASTNode {
    /// Statement type
    StmtKind Kind;
    
public:
    Stmt(StmtKind K): Kind(K) {}
    virtual ~Stmt() = default;
    
    StmtKind getKind() const { return Kind; }
};

/// Represents a `break` statement in a loop.
class BreakStmt: public Stmt {
    /// Range of \c break keyword
    llvm::SMRange BreakLoc;
    
public:
    BreakStmt(llvm::SMRange BR);
    
    virtual llvm::SMRange getSourceRange() const override;
};
   
/// Represents a `return` statement.
class ReturnStmt: public Stmt {
    /// Location of \c return keyword
    llvm::SMLoc RetLoc;
    
    /// Value that is to be returned.
    Expr *Value;
    
public:
    ReturnStmt(llvm::SMLoc RL, Expr *V);
    
    Expr *getValue() const { return Value; }
    virtual llvm::SMRange getSourceRange() const override;
};
    
/// Represents a range.
class RangeStmt: public Stmt {
    /// Start of the range
    Expr *Start;
    
    /// End of the range
    Expr *End;
    
    /// Range operator
    Token Op;
    
public:
    RangeStmt(Expr *S, Expr *E, Token Op);
    
    Expr *getStart() const { return Start; }
    Expr *getEnd() const { return End; }
    Token getOp() const { return Op; }
    
    /// Return \c true, if range is inclusive, \c false otherwise.
    bool isInclusive() const;
    
    virtual llvm::SMRange getSourceRange() const override;
};


/// Represents an arbitrary block of code.
class BlockStmt: public Stmt {
    /// Location of block's opening \c {
    llvm::SMLoc BlockStart;
    
    /// Location of block's closing \c }
    llvm::SMLoc BlockEnd;
    
    /// Blocks children nodes
    std::vector<ASTNode *> Nodes;
    
public:
    BlockStmt(llvm::SMLoc S, llvm::SMLoc E, std::vector<ASTNode *> &&N);
    
    llvm::ArrayRef<ASTNode *> getNodes() { return Nodes; }
    virtual llvm::SMRange getSourceRange() const override;
};

/// Represents a Function statement a.k.a declaration and definition.
class FuncStmt: public Stmt {
    FuncDecl *Prototype;
    BlockStmt *Body;
    
public:
    FuncStmt(FuncDecl *FP, BlockStmt *B);
    
    FuncDecl *getPrototype() { return Prototype; }
    BlockStmt *getBody() { return Body; }
    
    virtual llvm::SMRange getSourceRange() const override;
};

/// For-in statement representation
class ForStmt: public Stmt {
    /// Location of \c for keyword
    llvm::SMLoc ForLoc;
    
    /// Iterabling variable
    IdentifierExpr *Var;
    
    /// For-in range statement
    RangeStmt *Range;
    
    /// For's block.
    BlockStmt *Body;
    
public:
    ForStmt(llvm::SMLoc FL, IdentifierExpr *V, RangeStmt *R, BlockStmt *C);
    
    IdentifierExpr *getVar() const { return Var; }
    RangeStmt *getRange() const { return Range; }
    BlockStmt *getBody() const { return Body; }
    
    virtual llvm::SMRange getSourceRange() const override;
};

/// While statement encapsulation
class WhileStmt: public Stmt {
    /// Location of \c while keyword
    llvm::SMLoc WhileLoc;
    
    Expr *Cond;
    BlockStmt *Body;
    
public:
    WhileStmt(llvm::SMLoc WL, Expr *C, BlockStmt *B);
    
    Expr *getCond() const { return Cond; }
    BlockStmt *getBody() const { return Body; }
    
    virtual llvm::SMRange getSourceRange() const override;
};

/// If/If-Else statement encapsulation
class IfStmt: public Stmt {
    /// Location of \c if keyword
    llvm::SMLoc IfLoc;
    
    Expr *Cond;
    BlockStmt *Then;
    
    /// An else code block, which may be \c nullptr.
    BlockStmt *Else;
    
public:
    IfStmt(llvm::SMLoc IL, Expr *C, BlockStmt *T, BlockStmt *E = nullptr);
    
    Expr *getCond() const { return Cond; }
    BlockStmt *getThen() const { return Then; }
    BlockStmt *getElse() const { return Else; }
    bool hasElseBlock()  const { return Else != nullptr; }
    
    virtual llvm::SMRange getSourceRange() const override;
};
    
    
} // namesapce dusk

#endif /* DUSK_STMT_H */

