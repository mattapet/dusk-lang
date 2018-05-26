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

#include "dusk/AST/ASTContext.h"
#include "dusk/AST/ASTNode.h"
#include "dusk/AST/TypeRepr.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SMLoc.h"
#include <vector>

namespace dusk {
class Decl;
class ValDecl;
class VarDecl;
//class LetDecl;
class ParamDecl;
class FuncDecl;
class Expr;
class Stmt;
class Pattern;
class VarPattern;
class Type;
class FuncRetType;
class ASTWalker;
class ASTContext;

/// Decribes declaration type.
enum struct DeclKind {
#define DECL(CLASS, PARENT) CLASS,
#include "dusk/AST/DeclNodes.def"
};

/// Default declaration node.
class Decl : public ASTNode {
  /// Declaration type
  DeclKind Kind;

  /// Declaration name
  StringRef Name;

  /// Location of declaration
  SMLoc NameLoc;

  /// Type of declaration
  Type *Ty;

  /// Type representation, if present
  TypeRepr *TyRepr;

public:
  Decl(DeclKind K, StringRef N, SMLoc NL);
  Decl(DeclKind K, StringRef N, SMLoc NL, TypeRepr *TyRepr);

  /// Returns declaration kind.
  DeclKind getKind() const { return Kind; }

  /// Returns \c true if delcaration is of given kind, \c false otherwise.
  bool isKind(DeclKind K) const { return Kind == K; }

  /// Returns \c true if declaration has a value.
  bool isValDecl() const;

  /// Returns declaration identifier as string.
  StringRef getName() const { return Name; }

  /// Returns declaration type
  Type *getType() const { return Ty; }

  /// Sets declaration type
  void setType(Type *T) { Ty = T; }

  /// Returns \c true if declaration has an explicit type specification,
  /// \c false otherwise.
  bool hasTypeRepr() const { return TyRepr != nullptr; }

  /// Returns type representation.
  TypeRepr *getTypeRepr() const { return TyRepr; }

  SMRange getSourceRange() const override;

  bool walk(ASTWalker &Walker);

#define DECL(CLASS, PARENT) CLASS##Decl *get##CLASS##Decl();
#include "dusk/AST/DeclNodes.def"
};

/// Declaration of value-holdable node
class ValDecl : public Decl {
public:
  enum class Specifier {
    // Var declarations
    Var,
    Let,
    
    // Param decalrations
    
    Default = Let,
    InOut
  };
  
private:
  /// Value Location
  SMLoc ValLoc;

  /// Initial expression value
  Expr *Value;
  
  /// Declaration specifier
  Specifier Spec;

public:
  ValDecl(DeclKind K, Specifier S, StringRef N, SMLoc NL, Expr *V);
  ValDecl(DeclKind K, Specifier S, StringRef N, SMLoc NL, Expr *V,
          TypeRepr *TR);

  SMLoc getValLoc() const { return ValLoc; }
  bool hasValue() const { return Value != nullptr; }
  Expr *getValue() const { return Value; }
  void setValue(Expr *V) { Value = V; }
  
  Specifier getSpecifier() const { return Spec; }
  void setSpecifier(Specifier S) { Spec = S; }

  bool isLet() const { return getSpecifier() == Specifier::Let; }
  bool isVar() const { return getSpecifier() == Specifier::Var; }
  bool isInOut() const { return getSpecifier() == Specifier::InOut; }
};

/// Declaration of a variable
class VarDecl : public ValDecl {
  /// Location of \c var keyword
  SMLoc VarLoc;

public:
  VarDecl(Specifier S, StringRef N, SMLoc NL, SMLoc VarL, Expr *V);
  VarDecl(Specifier S, StringRef N, SMLoc NL, SMLoc VarL, Expr *V,
          TypeRepr *TR);

  SMLoc getVarLoc() const { return VarLoc; }
  virtual SMRange getSourceRange() const override;
};

/// Declaration of function parameter
class ParamDecl : public ValDecl {
public:
  ParamDecl(Specifier S, StringRef N, SMLoc NL);
  ParamDecl(Specifier S, StringRef N, SMLoc NL, TypeRepr *TR);
};

/// Function declaration
class FuncDecl : public Decl {
  /// Location of \c func keyword
  SMLoc FuncLoc;

  /// Function arguments
  VarPattern *Params;

public:
  FuncDecl(StringRef N, SMLoc NL, SMLoc FuncL, VarPattern *A);
  FuncDecl(StringRef N, SMLoc NL, SMLoc FuncL, VarPattern *A, TypeRepr *TR);

  SMLoc getFuncLoc() const { return FuncLoc; }
  VarPattern *getArgs() const { return Params; }

  virtual SMRange getSourceRange() const override;
};

/// A signle module
///
/// Represents a result of parsing a file.
class ModuleDecl : public Decl {
  std::vector<ASTNode *> Contents;

public:
  ModuleDecl(StringRef N, std::vector<ASTNode *> &&C);

  ArrayRef<ASTNode *> getContents() const { return Contents; }
  MutableArrayRef<ASTNode *> getContents() { return Contents; }
  void setContents(std::vector<ASTNode *> &&C) { Contents = C; }
  virtual SMRange getSourceRange() const override;
};

} // namespace dusk

#endif /* DUSK_DECL_H */
