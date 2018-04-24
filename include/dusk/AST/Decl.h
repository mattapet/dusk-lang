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
class LetDecl;
class ParamDecl;
class FuncDecl;
class Expr;
class Stmt;
class Pattern;
class VarPattern;
class Type;
class FuncRetType;
class ASTWalker;

/// Decribes declaration type.
enum struct DeclKind { Let, Var, Param, Func, Module };

enum struct RetType { Void, Int };

/// Default declaration node.
class Decl : public ASTNode {
  /// Declaration type
  DeclKind Kind;

  /// Declaration name
  StringRef Name;

  /// Location of declaration
  SMLoc NameLoc;

public:
  Decl(DeclKind K, StringRef N, SMLoc NL);
  virtual ~Decl() = default;

  /// Returns declaration kind.
  DeclKind getKind() const { return Kind; }

  /// Returns \c true if delcaration is of given kind, \c false otherwise.
  bool isKind(DeclKind K) const { return Kind == K; }

  /// Returns \c true if declaration has a value.
  bool isValDecl() const;

  /// Returns declaration identifier as string.
  StringRef getName() const { return Name; }

  virtual SMRange getSourceRange() const override;
};

/// Declaration of value-holdable node
class ValDecl : public Decl {
  /// Value Location
  SMLoc ValLoc;

  Expr *Value;

public:
  ValDecl(DeclKind K, StringRef N, SMLoc NL, Expr *V);

  SMLoc getValLoc() const { return ValLoc; }
  Expr *getValue() const { return Value; }
};

/// Declaration of a variable
class VarDecl : public ValDecl {
  /// Location of \c var keyword
  SMLoc VarLoc;

public:
  VarDecl(StringRef N, SMLoc NL, SMLoc VarL, Expr *V);

  SMLoc getVarLoc() const { return VarLoc; }
  virtual SMRange getSourceRange() const override;
};

/// Declaration of a constant
class LetDecl : public ValDecl {
  /// Location of \c let keyword
  SMLoc LetLoc;

public:
  LetDecl(StringRef N, SMLoc NL, SMLoc LetL, Expr *V);

  SMLoc getConstLoc() const { return LetLoc; }

  virtual SMRange getSourceRange() const override;
};

/// Declaration of function parameter
class ParamDecl : public Decl {
public:
  ParamDecl(StringRef N, SMLoc NL);
};

/// Function declaration
class FuncDecl : public Decl {
  /// Location of \c func keyword
  SMLoc FuncLoc;

  /// Function arguments
  VarPattern *Params;

  /// Function return type
  FuncRetType *RetTy;

public:
  FuncDecl(StringRef N, SMLoc NL, SMLoc FuncL, VarPattern *A,
           FuncRetType *R = nullptr);

  SMLoc getFuncLoc() const { return FuncLoc; }
  VarPattern *getArgs() const { return Params; }

  FuncRetType *getRetType() const { return RetTy; }
  bool hasRetType() const { return RetTy != nullptr; }

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
  virtual SMRange getSourceRange() const override;
};

} // namespace dusk

#endif /* DUSK_DECL_H */
