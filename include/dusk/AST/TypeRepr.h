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
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/SMLoc.h"

namespace dusk {
  
enum struct TypeReprKind {
  Ident,
  Function
};
  
class TypeRepr {
  TypeReprKind Kind;
  
public:
  TypeRepr(TypeReprKind K);
  
  SMLoc getLocStart() const { return getSourceRange().Start; }
  SMLoc getLocEnd() const { return getSourceRange().End; }
  virtual SMRange getSourceRange() const = 0;
};
  
class IdentTypeRepr : public TypeRepr {
  SMLoc IdentLoc;
};

//class ParamListTypeRepr : public TypeRepr {
//
//};
//
//class FunctionTypeRepr : public TypeRepr {
//  TypeRepr *ArgsTy;
//  TypeRepr *RetTy;
//  SMLoc ArrowLoc;
//};
  
} // namespace dusk

#endif /* DUSK_TYPE_REPR_H */
