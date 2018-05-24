//===--- TypeRepr.h - Dusk type representations -----------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_TYPE_REPR_H
#define DUSK_TYPE_REPR_H

#include "dusk/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/SMLoc.h"

namespace dusk {
class Expr;
class Stmt;
class Type;
class IdentTypeRepr;
class ArrayTypeRepr;
class ASTWalker;
class ASTContext;

enum struct TypeReprKind {
#define TYPE_REPR(CLASS, PARENT) CLASS,
#include "dusk/AST/TypeReprNodes.def"
};

class TypeRepr {
  TypeReprKind Kind;

  Type *Ty;

public:
  TypeRepr(TypeReprKind K);

  TypeReprKind getKind() const { return Kind; }
  SMLoc getLocStart() const { return getSourceRange().Start; }
  SMLoc getLocEnd() const { return getSourceRange().End; }
  virtual SMRange getSourceRange() const = 0;

  Type *getType() const { return Ty; }
  void setType(Type *T) { Ty = T; }

  bool walk(ASTWalker &Walker);

#define TYPE_REPR(CLASS, PARENT) \
  CLASS##TypeRepr *get##CLASS##TypeRepr();
#include "dusk/AST/TypeReprNodes.def"

public:
  /// Only allow allocation using \c ASTContext
  void *operator new(size_t Bytes, ASTContext &Context);
  void *operator new(size_t Bytes, void *Mem) throw() { return Mem; }
};

/// Simple single identifier type e.g. ': Int'
class IdentTypeRepr : public TypeRepr {
  /// Type identifier
  StringRef Ident;

public:
  IdentTypeRepr(StringRef ID);

  StringRef getIdent() const { return Ident; }

  SMRange getSourceRange() const override;
};

/// Array type e.g. 'Int[5]';
class ArrayTypeRepr : public TypeRepr {
  /// Base type of array.
  TypeRepr *BaseTyRepr;

  /// Array size specifier.
  Stmt *Size;

public:
  ArrayTypeRepr(TypeRepr *B, Stmt *S);

  TypeRepr *getBaseTyRepr() const { return BaseTyRepr; }
  Stmt *getSize() const { return Size; }

  SMRange getSourceRange() const override;
};

} // namespace dusk

#endif /* DUSK_TYPE_REPR_H */
