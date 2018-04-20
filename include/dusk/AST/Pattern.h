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
class ParamDecl;

/// Pattern description.
enum struct PatternKind { Expr, Variable, Subscript };

class Pattern : public ASTNode {
  /// Pattern type.
  PatternKind Kind;

public:
  Pattern(PatternKind K);
  PatternKind getKind() const { return Kind; }

  llvm::SMLoc getLocStart() const { return getSourceRange().Start; }
  llvm::SMLoc getLocEnd() const { return getSourceRange().End; }
};

/// Expression pattern
///
/// Used in function calls. Each of the items must be a valid expression.
class ExprPattern : public Pattern {
  llvm::SmallVector<Expr *, 128> Values;

  /// Location of left parenthesis
  llvm::SMLoc LPar;

  /// Location of right parenthesis
  llvm::SMLoc RPar;

public:
  ExprPattern(llvm::SmallVector<Expr *, 128> &&V, llvm::SMLoc L, llvm::SMLoc R);

  llvm::ArrayRef<Expr *> getValues() const { return Values; }
  llvm::SMLoc getLPar() const { return LPar; }
  llvm::SMLoc getRPar() const { return RPar; }

  virtual llvm::SMRange getSourceRange() const override;
};

/// Variable pattern
///
/// Used in function declarations. Each item in the pattern must be a parameter
/// declaration.
class VarPattern : public Pattern {
  /// Variables of the expression
  llvm::SmallVector<ParamDecl *, 128> Vars;

  /// Location of left parenthesis
  llvm::SMLoc LPar;

  /// Location of right parenthesis
  llvm::SMLoc RPar;

public:
  VarPattern(llvm::SmallVector<ParamDecl *, 128> &&V, llvm::SMLoc L,
             llvm::SMLoc R);

  llvm::ArrayRef<ParamDecl *> getVars() const { return Vars; }
  llvm::SMLoc getLPar() const { return LPar; }
  llvm::SMLoc getRPar() const { return RPar; }

  virtual llvm::SMRange getSourceRange() const override;
};

/// Subscript pattern
///
/// Pattern used in array declaration and in accessing array elements.
class SubscriptPattern : public Pattern {
  /// Subscript value
  Expr *Value;

  /// Location of left bracket
  llvm::SMLoc LBracet;

  /// Location of right bracket
  llvm::SMLoc RBracet;

public:
  SubscriptPattern(Expr *V, llvm::SMLoc L, llvm::SMLoc R);

  Expr *getValue() const { return Value; }
  llvm::SMLoc getLBracket() const { return LBracet; }
  llvm::SMLoc getRBracket() const { return RBracet; }

  virtual llvm::SMRange getSourceRange() const override;
};

} // namespace dusk

#endif /* DUSK_PATTERN_H */
