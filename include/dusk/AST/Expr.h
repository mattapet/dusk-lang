//===--- Expr.h - Dusk expression ASTs --------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_EXPR_H
#define DUSK_EXPR_H

#include "dusk/AST/ASTNode.h"
#include "dusk/AST/Pattern.h"
#include "dusk/Parse/Token.h"
#include "llvm/Support/SMLoc.h"

namespace dusk {
class NumberLiteralExpr;
class ArrayLiteralExpr;
class IdentifierExpr;
class ParenExpr;
class InfixExpr;
class PrefixExpr;
class CallExpr;
class SubscriptExpr;
class BlockStmt;
class ExprPattern;
class Stmt;
class Pattern;
class SubscriptPattern;
class Type;
class TypeRepr;
class ASTWalker;

/// Describes expression type.
enum struct ExprKind {
  NumberLiteral,
  ArrayLiteral,
  Identifier,
  Paren,
  Infix,
  Assign,
  Prefix,
  Call,
  Subscript
};

/// Base class for all expression type nodes.
class Expr : public ASTNode {
  /// Expression type
  ExprKind Kind;
  
  /// Type of declaration
  Type *Ty;
  
public:
  Expr(ExprKind K);
  virtual ~Expr() = default;

  ExprKind getKind() const { return Kind; }
  
  /// Returns declaration type
  Type *getType() const { return Ty; }
  
  /// Sets declaration type
  void setType(Type *T) { Ty = T; }
};

/// Number literal expression encalsulation.
class NumberLiteralExpr : public Expr {
  int64_t Value;
  SMRange ValueLoc;

public:
  NumberLiteralExpr(int64_t V, SMRange ValL);

  SMRange getValLoc() const { return ValueLoc; }
  int getValue() const { return Value; }

  SMRange getSourceRange() const override;
};

class IdentifierExpr : public Expr {
  StringRef Name;
  SMLoc NameLoc;

public:
  IdentifierExpr(StringRef N, SMLoc L);

  StringRef getName() const { return Name; }
  SMLoc getNameLoc() const { return NameLoc; }

  SMRange getSourceRange() const override;
};
  
/// Represents an array literal.
///
/// E.g. '[' Expr ',' Expr ']'
class ArrayLiteralExpr : public Expr {
  /// Initialization list of calues.
  Pattern *Values;
  
public:
  ArrayLiteralExpr(Pattern *V);
  
  SMRange getBrackets() const { return Values->getSourceRange(); }
  Pattern *getValues() const { return Values; }
  
  SMRange getSourceRange() const override;
};

/// Represents a paranthesized expression
///
/// E.g. '(' Expr ')'
class ParenExpr : public Expr {
  /// Paranthesized expression
  Expr *Expression;

  /// Left parethensis
  SMLoc LPar;

  /// Right parethensis
  SMLoc RPar;

public:
  ParenExpr(Expr *E, SMLoc L, SMLoc R);

  Expr *getExpr() const { return Expression; }

  SMRange getSourceRange() const override;
};

/// An infix expression
class InfixExpr : public Expr {
  Expr *LHS;
  Expr *RHS;
  Token Op;

public:
  InfixExpr(Expr *L, Expr *R, Token O);

  Expr *getLHS() const { return LHS; }
  Expr *getRHS() const { return RHS; }
  Token getOp() const { return Op; }

  SMRange getSourceRange() const override;
};

class AssignExpr : public Expr {
  Expr *Dest;
  Expr *Source;

public:
  AssignExpr(Expr *L, Expr *R);

  Expr *getDest() const { return Dest; }
  Expr *getSource() const { return Source; }

  SMRange getSourceRange() const override;
};

class PrefixExpr : public Expr {
  Expr *Dest;
  Token Op;

public:
  PrefixExpr(Expr *D, Token O);

  Expr *getDest() const { return Dest; }
  Token getOp() const { return Op; }

  SMRange getSourceRange() const override;
};

class CallExpr : public Expr {
  /// Function identifier
  Expr *Callee;

  /// Function arguments
  Pattern *Args;

public:
  CallExpr(Expr *C, Pattern *A);

  Expr *getCalle() const { return Callee; }
  Pattern *getArgs() { return Args; }

  SMRange getSourceRange() const override;
};

class SubscriptExpr : public Expr {
  /// Base identifier
  Expr *Base;

  /// Subscription pattern
  Stmt *Subscript;

public:
  SubscriptExpr(Expr *B, Stmt *S);

  Expr *getBase() { return Base; }
  Stmt *getSubscript() { return Subscript; }

  SMRange getSourceRange() const override;
};

} // namespace dusk

#endif /* DUSK_EXPR_H */

