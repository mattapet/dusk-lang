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
  
enum struct TypeReprKind {
  Ident,
  FuncRet
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
  
class IdentTypeRepr : public TypeRepr {
  /// Position of type specification colon.
  SMLoc ColonLoc;
  
  /// Type identifier
  StringRef Ident;
  
public:
  IdentTypeRepr(SMLoc CL, StringRef ID);
  
  SMLoc getColonLoc() const { return ColonLoc; }
  StringRef getIdent() const { return Ident; }
  
  virtual SMRange getSourceRange() const override;
};

class FuncRetTypeRepr : public TypeRepr {
  /// Position of arrow location.
  SMLoc ArrowLoc;
  
  /// Type identifier.
  StringRef Ident;
  
public:
  FuncRetTypeRepr(SMLoc AL, StringRef ID);
  
  SMLoc getArrowLoc() const { return ArrowLoc; }
  StringRef getIdent() const { return Ident; }
  
  virtual SMRange getSourceRange() const override;
};
  
  
} // namespace dusk

#endif /* DUSK_TYPE_REPR_H */
