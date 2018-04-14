//===--- Decl.h - Dusk declaration ASTs -------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_DECL_H
#define DUSK_DECL_H

#include "dusk/AST/ASTNode.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/SMLoc.h"

namespace dusk {
    class Decl;
    class ValueDecl;
    class VarDecl;
    class ConstDecl;
    class ParamDecl;
    class FuncDecl;
    class Expr;
    class ParamList;
    class ASTWalker;
    
    /// Decribes declaration type.
    enum struct DeclKind {
        Const,
        Var,
        Param,
        Func
    };
    
    
    /// Default declaration node.
    class Decl: public ASTNode {
        /// Declaration type
        DeclKind Kind;
        
        /// Declaration name
        llvm::StringRef Name;
        
        /// Location of declaration
        llvm::SMRange NameLoc;

    public:
        Decl(DeclKind K, llvm::StringRef N, llvm::SMRange NL);
        
        /// Returns declaration kind.
        DeclKind getKind() const { return Kind; }
        
        /// Returns \c true if delcaration is of given kind, \c false otherwise.
        bool isKind(DeclKind K) const { return Kind == K; }
        
        /// Returns \c true if declaration has a value.
        bool isValDecl() const;
        
        /// Returns declaration identifier as string.
        llvm::StringRef getName() const { return Name; }
        
        virtual llvm::SMRange getSourceRange() const override {
            return NameLoc;
        }
    };
    
    
    /// Declaration of value-holdable node
    class ValDecl: public Decl {
        /// Value Location
        llvm::SMLoc ValLoc;
        
        Expr *Value;
        
    public:
        ValDecl(DeclKind K, llvm::StringRef N, llvm::SMRange NL, Expr *V);
        
        llvm::SMLoc getValLoc() const { return ValLoc; }
        const Expr *getValue() const { return Value; }
        
        virtual bool walk(ASTWalker &Walker) override;
    };
    
    
    /// Declaration of a variable
    class VarDecl: public ValDecl {
        /// Location of \c var keyword
        llvm::SMLoc VarLoc;
        
    public:
        VarDecl(llvm::StringRef N, llvm::SMRange NL, llvm::SMLoc VarL, Expr *V);
        virtual llvm::SMRange getSourceRange() const override;
    };
    
    
    /// Declaration of a constant
    class ConstDecl: public ValDecl {
        /// Location of \c const keyword
        llvm::SMLoc ConstLoc;
        
    public:
        ConstDecl(llvm::StringRef N, llvm::SMRange NL, llvm::SMLoc ConstL,
                  Expr *V);
        virtual llvm::SMRange getSourceRange() const override;
    };
    
    
    /// Declaration of function parameter
    class ParamDecl: public Decl {
    public:
        ParamDecl(llvm::StringRef N, llvm::SMRange NL);
        
        virtual bool walk(ASTWalker &Walker) override;
    };
    
    
    /// Function declaration
    class FuncDecl: public Decl {
        /// Location of \c func keyword
        llvm::SMLoc FuncLoc;
        
        /// Function arguments
        ParamList *Args;
        
    public:
        FuncDecl(llvm::StringRef N, llvm::SMRange NR,
                 llvm::SMLoc FuncL, ParamList *A);
        
        virtual llvm::SMRange getSourceRange() const override;
        virtual bool walk(ASTWalker &Walker) override;
    };
    
} // namespace dusk

#endif /* DUSK_DECL_H */
