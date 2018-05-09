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

enum struct TypeReprKind {
  Ident,
  Array
};
  
class TypeRepr {
  TypeReprKind Kind;
  
public:
  TypeRepr(TypeReprKind K);
  
  TypeReprKind getKind() const { return Kind; }
  SMLoc getLocStart() const { return getSourceRange().Start; }
  SMLoc getLocEnd() const { return getSourceRange().End; }
  virtual SMRange getSourceRange() const = 0;
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
