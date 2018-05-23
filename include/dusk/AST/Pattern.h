//===--- Pattern.h - Dusk patterns ------------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_PATTERN_H
#define DUSK_PATTERN_H

#include "dusk/AST/ASTNode.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/SMLoc.h"

namespace dusk {
class ASTNode;
class Decl;
class Expr;
class Stmt;
class Type;
class ParamDecl;
class ExprPattern;
class VarPattern;
class ASTWalker;

/// Pattern description.
enum struct PatternKind { Expr, Variable };

class Pattern {
  /// Pattern type.
  PatternKind Kind;

  /// Pattern type
  Type *Ty;

public:
  Pattern(PatternKind K);
  PatternKind getKind() const { return Kind; }

  virtual size_t count() const = 0;
  virtual SMRange getSourceRange() const = 0;

  void setType(Type *T) { Ty = T; }
  Type *getType() const { return Ty; }

  SMLoc getLocStart() { return getSourceRange().Start; }
  SMLoc getLocEnd() { return getSourceRange().End; }

  bool walk(ASTWalker &Walker);
  
  ExprPattern *getExprPattern();
  VarPattern *getVarPattern();

public:
  /// Only allow allocation using \c ASTContext
  void *operator new(size_t Bytes, ASTContext &Context);
  void *operator new(size_t Bytes, void *Mem) throw() { return Mem; }
};

/// Expression pattern
///
/// Used in function calls. Each of the items must be a valid expression.
class ExprPattern : public Pattern {
  SmallVector<Expr *, 128> Values;

  /// Location of left parenthesis
  SMLoc LPar;

  /// Location of right parenthesis
  SMLoc RPar;

public:
  ExprPattern(SmallVector<Expr *, 128> &&V, SMLoc L, SMLoc R);

  ArrayRef<Expr *> getValues() const { return Values; }
  SmallVector<Expr *, 128> &getValues() { return Values; }
  SMLoc getLPar() const { return LPar; }
  SMLoc getRPar() const { return RPar; }

  virtual size_t count() const override;
  virtual SMRange getSourceRange() const override;
};

/// Variable pattern
///
/// Used in function declarations. Each item in the pattern must be a parameter
/// declaration.
class VarPattern : public Pattern {
  /// Variables of the expression
  SmallVector<Decl *, 128> Vars;

  /// Location of left parenthesis
  SMLoc LPar;

  /// Location of right parenthesis
  SMLoc RPar;

public:
  VarPattern(SmallVector<Decl *, 128> &&V, SMLoc L, SMLoc R);

  ArrayRef<Decl *> getVars() const { return Vars; }
  SmallVector<Decl *, 128> &getVars() { return Vars; }
  SMLoc getLPar() const { return LPar; }
  SMLoc getRPar() const { return RPar; }

  virtual size_t count() const override;
  virtual SMRange getSourceRange() const override;
};

} // namespace dusk

#endif /* DUSK_PATTERN_H */
