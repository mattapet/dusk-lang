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
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SMLoc.h"
#include <vector>

namespace dusk {
class Decl;
class ValDecl;
class VarDecl;
class ConstDecl;
class ParamDecl;
class FuncDecl;
class Expr;
class VarPattern;
class ASTWalker;

/// Decribes declaration type.
enum struct DeclKind {
    Const,
    Var,
    Param,
    Func,
    Module
};


/// Default declaration node.
class Decl: public ASTNode {
    /// Declaration type
    DeclKind Kind;
    
    /// Declaration name
    llvm::StringRef Name;
    
    /// Location of declaration
    llvm::SMLoc NameLoc;

public:
    Decl(DeclKind K, llvm::StringRef N, llvm::SMLoc NL);
    virtual ~Decl() = default;
    
    /// Returns declaration kind.
    DeclKind getKind() const { return Kind; }
    
    /// Returns \c true if delcaration is of given kind, \c false otherwise.
    bool isKind(DeclKind K) const { return Kind == K; }
    
    /// Returns \c true if declaration has a value.
    bool isValDecl() const;
    
    /// Returns declaration identifier as string.
    llvm::StringRef getName() const { return Name; }
    
    virtual llvm::SMRange getSourceRange() const override;
};


/// Declaration of value-holdable node
class ValDecl: public Decl {
    /// Value Location
    llvm::SMLoc ValLoc;
    
    Expr *Value;
    
public:
    ValDecl(DeclKind K, llvm::StringRef N, llvm::SMLoc NL, Expr *V);
    
    llvm::SMLoc getValLoc() const { return ValLoc; }
    Expr *getValue() const { return Value; }
};


/// Declaration of a variable
class VarDecl: public ValDecl {
    /// Location of \c var keyword
    llvm::SMLoc VarLoc;
    
public:
    VarDecl(llvm::StringRef N, llvm::SMLoc NL, llvm::SMLoc VarL, Expr *V);
    
    llvm::SMLoc getVarLoc() const { return VarLoc; }
    virtual llvm::SMRange getSourceRange() const override;
};


/// Declaration of a constant
class ConstDecl: public ValDecl {
    /// Location of \c const keyword
    llvm::SMLoc ConstLoc;
    
public:
    ConstDecl(llvm::StringRef N, llvm::SMLoc NL,
              llvm::SMLoc ConstL, Expr *V);
    
    llvm::SMLoc getConstLoc() const { return ConstLoc; }
    
    virtual llvm::SMRange getSourceRange() const override;
};


/// Declaration of function parameter
class ParamDecl: public Decl {
public:
    ParamDecl(llvm::StringRef N, llvm::SMLoc NL);
};


/// Function declaration
class FuncDecl: public Decl {
    /// Location of \c func keyword
    llvm::SMLoc FuncLoc;
    
    /// Function arguments
    VarPattern *Params;
    
public:
    FuncDecl(llvm::StringRef N, llvm::SMLoc NR,
             llvm::SMLoc FuncL, VarPattern *A);
    
    llvm::SMLoc getFuncLoc() const { return FuncLoc; }
    VarPattern *getArgs() const { return Params; }
    
    virtual llvm::SMRange getSourceRange() const override;
};

/// A signle module
///
/// Represents a result of parsing a file.
class ModuleDecl: public Decl {
    std::vector<ASTNode *> Contents;
    
public:
    ModuleDecl(llvm::StringRef N, std::vector<ASTNode *> &&C);
    
    llvm::ArrayRef<ASTNode *> getContents() const { return Contents; }
    virtual llvm::SMRange getSourceRange() const override;
};

} // namespace dusk

#endif /* DUSK_DECL_H */
