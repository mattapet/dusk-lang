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
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/SMLoc.h"
#include <vector>

namespace dusk {
    class Expr;
    class CodeBlock;
    class ParamDecl;
    class FuncDecl;
    class ASTWalker;
    
    class Stmt: public ASTNode {
    public:
        virtual ~Stmt() = default;
    };
    
    class ParamList: public Stmt {
        /// Location of opening \c (
        llvm::SMLoc ListStart;
        
        /// Location of closing \c )
        llvm::SMLoc ListEnd;
        
        llvm::SmallVector<ParamDecl *, 128> Params;
        
    public:
        ParamList(llvm::SMLoc LS, llvm::SMLoc LE,
                  llvm::SmallVector<ParamDecl *, 128> &&P);
        
        llvm::SmallVector<ParamDecl *, 128> getParams() { return Params; }
        
        virtual llvm::SMRange getSourceRange() const override;
        virtual bool walk(ASTWalker &Walker) override;
    };
    
    /// Represents an arbitrary block of code.
    class CodeBlock: public Stmt {
        /// Location of block's opening \c {
        llvm::SMLoc BlockStart;
        
        /// Location of block's closing \c }
        llvm::SMLoc BlockEnd;
        
        /// Blocks children nodes
        std::vector<ASTNode *> Nodes;
        
    public:
        CodeBlock(llvm::SMLoc S, llvm::SMLoc E, std::vector<ASTNode *> &&N);
        
        std::vector<ASTNode *> &getNodes() { return Nodes; }
        const std::vector<ASTNode *> &getNodes() const { return Nodes; }
        
        virtual llvm::SMRange getSourceRange() const override;
        virtual bool walk(ASTWalker &Walker) override;
    };
    
    /// Represents a Function statement a.k.a declaration and definition.
    class FuncStmt: public Stmt {
        FuncDecl *Prototype;
        CodeBlock *Body;
        
    public:
        FuncStmt(FuncDecl *FP, CodeBlock *B);
        
        FuncDecl *getPrototype() { return Prototype; }
        CodeBlock *getBody() { return Body; }
        
        virtual llvm::SMRange getSourceRange() const override;
        virtual bool walk(ASTWalker &Walker) override;
    };
    
//    class ForStmt: public Stmt {
//
//    };
    
    /// While statement encapsulation
    class WhileStmt: public Stmt {
        /// Location of \c while keyword
        llvm::SMLoc WhileLoc;
        
        Expr *Cond;
        CodeBlock *Body;
        
    public:
        WhileStmt(llvm::SMLoc WL, Expr *C, CodeBlock *B);
        
        Expr *getCond() const { return Cond; }
        CodeBlock *getBody() const { return Body; }
        
        virtual llvm::SMRange getSourceRange() const override;
        virtual bool walk(ASTWalker &Walker) override;
    };
    
    /// If/If-Else statement encapsulation
    class IfStmt: public Stmt {
        /// Location of \c if keyword
        llvm::SMLoc IfLoc;
        
        Expr *Cond;
        CodeBlock *Then;
        
        /// An else code block, which may be \c nullptr.
        CodeBlock *Else;
        
    public:
        IfStmt(llvm::SMLoc IL, Expr *C, CodeBlock *T, CodeBlock *E = nullptr);
        
        Expr *getCond() const { return Cond; }
        CodeBlock *getThen() const { return Then; }
        CodeBlock *getElse() const { return Else; }
        bool hasElseBlock()  const { return Else != nullptr; }
        
        virtual llvm::SMRange getSourceRange() const override;
        virtual bool walk(ASTWalker &Walker) override;
    };
    
} // namesapce dusk

#endif /* DUSK_STMT_H */

