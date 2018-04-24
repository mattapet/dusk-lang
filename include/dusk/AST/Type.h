//===--- Type.h - Dusk type nodes ------------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_TYPE_H
#define DUSK_TYPE_H

#include "dusk/Basic/LLVM.h"
#include "dusk/Parse/Token.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SMLoc.h"

namespace dusk {
class Type;
enum struct TypeKind;

enum struct TypeKind { FuncRet };

class Type {
  /// Exact kind of the type.
  TypeKind Kind;

public:
  Type(TypeKind K);

  TypeKind getKind() const { return Kind; }

  virtual SMRange getSourceRange() const = 0;

  SMLoc getLocStart() const { return getSourceRange().Start; }
  SMLoc getLocEnd() const { return getSourceRange().End; }
};

class FuncRetType : public Type {
  /// Location of arrow operator
  SMLoc ArrowLoc;

  /// The actual return type
  Token RetType;

      public : FuncRetType(SMLoc AL, Token RTy);

  SMLoc getArrowLoc() const { return ArrowLoc; }
  Token getRetType() const { return RetType; }

  virtual SMRange getSourceRange() const override;
};

} // namespace dusk

#endif /* DUSK_TYPE_H */
