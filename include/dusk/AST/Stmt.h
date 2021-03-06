//===--- Stmt.h - Dusk statement ASTs ---------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_STMT_H
#define DUSK_STMT_H

#include "dusk/AST/ASTNode.h"
#include "dusk/Parse/Token.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/SMLoc.h"
#include <vector>

namespace dusk {
class Decl;
class Expr;
class Stmt;
class BreakStmt;
class ReturnStmt;
class SubscriptStmt;
class RangeStmt;
class ExternStmt;
class BlockStmt;
class FuncStmt;
class ForStmt;
class WhileStmt;
class IfStmt;
class IdentifierExpr;
class ASTWalker;
class ASTContext;

/// Describes statement type.
enum struct StmtKind {
#define STMT(CLASS, PARENT) CLASS,
#include "dusk/AST/StmtNodes.def"
};

class Stmt : public ASTNode {
  /// Statement type
  StmtKind Kind;

public:
  Stmt(StmtKind K);

  StmtKind getKind() const { return Kind; }

  bool walk(ASTWalker &Walker);

#define STMT(CLASS, PARENT) CLASS##Stmt *get##CLASS##Stmt();
#include "dusk/AST/StmtNodes.def"
};

/// Represents a \c break statement in a loop.
class BreakStmt : public Stmt {
  /// Range of \c break keyword
  SMRange BreakLoc;

public:
  BreakStmt(SMRange BR);

  SMRange getSourceRange() const override;
};

/// Represents a \c return statement.
class ReturnStmt : public Stmt {
  /// Location of \c return keyword
  SMLoc RetLoc;

  /// Value that is to be returned.
  Expr *Value;

public:
  ReturnStmt(SMLoc RL, Expr *V);

  Expr *getValue() const { return Value; }
  void setValue(Expr *V) { Value = V; }
  bool hasValue() const { return Value != nullptr; }
  virtual SMRange getSourceRange() const override;
};

/// Subscript statement.
class SubscriptStmt : public Stmt {
  /// Subcript value
  Expr *Value;

  /// Location of left bracket
  SMLoc LBracket;

  /// Location of right bracket
  SMLoc RBracket;

public:
  SubscriptStmt(Expr *V, SMLoc L, SMLoc R);

  Expr *getValue() const { return Value; }
  void setValue(Expr *V) { Value = V; }
  SMLoc getLBracket() const { return LBracket; }
  SMLoc getRBracket() const { return RBracket; }

  virtual SMRange getSourceRange() const override;
};

/// Represents a range.
class RangeStmt : public Stmt {
  /// Start of the range
  Expr *Start;

  /// End of the range
  Expr *End;

  /// Range operator
  Token Op;

public:
  RangeStmt(Expr *S, Expr *E, Token Op);

  Expr *getStart() const { return Start; }
  Expr *getEnd() const { return End; }
  void setStart(Expr *S) { Start = S; }
  void setEnd(Expr *E) { End = E; }
  Token getOp() const { return Op; }

  /// Return \c true, if range is inclusive, \c false otherwise.
  bool isInclusive() const;

  virtual SMRange getSourceRange() const override;
};

/// Represents an arbitrary block of code.
class BlockStmt : public Stmt {
  /// Location of block's opening \c {
  SMLoc BlockStart;

  /// Location of block's closing \c }
  SMLoc BlockEnd;

  /// Blocks children nodes
  std::vector<ASTNode *> Nodes;

public:
  BlockStmt(SMLoc S, SMLoc E, std::vector<ASTNode *> &&N);

  ArrayRef<ASTNode *> getNodes() const { return Nodes; }
  MutableArrayRef<ASTNode *> getNodes() { return Nodes; }
  virtual SMRange getSourceRange() const override;
};

class ExternStmt : public Stmt {
  /// Location of extern keyword
  SMLoc ExternLoc;

  /// Function prototype
  Decl *Prototype;

public:
  ExternStmt(SMLoc EL, Decl *P);

  SMLoc getExternLoc() const { return ExternLoc; }
  Decl *getPrototype() const { return Prototype; }

  virtual SMRange getSourceRange() const override;
};

/// Represents a Function statement a.k.a declaration and definition.
class FuncStmt : public Stmt {
  Decl *Prototype;
  Stmt *Body;

public:
  FuncStmt(Decl *FP, Stmt *B);

  Decl *getPrototype() { return Prototype; }
  Stmt *getBody() { return Body; }

  virtual SMRange getSourceRange() const override;
};

/// For-in statement representation
class ForStmt : public Stmt {
  /// Location of \c for keyword
  SMLoc ForLoc;

  /// Iterabling variable
  Decl *Iter;

  /// For-in range statement
  Stmt *Range;

  /// For's block.
  Stmt *Body;

public:
  ForStmt(SMLoc FL, Decl *V, Stmt *R, Stmt *C);

  Decl *getIter() const { return Iter; }
  Stmt *getRange() const { return Range; }
  Stmt *getBody() const { return Body; }

  virtual SMRange getSourceRange() const override;
};

/// While statement encapsulation
class WhileStmt : public Stmt {
  /// Location of \c while keyword
  SMLoc WhileLoc;

  Expr *Cond;
  Stmt *Body;

public:
  WhileStmt(SMLoc WL, Expr *C, Stmt *B);

  Expr *getCond() const { return Cond; }
  void setCond(Expr *C) { Cond = C; }
  Stmt *getBody() const { return Body; }

  virtual SMRange getSourceRange() const override;
};

/// If/If-Else statement encapsulation
class IfStmt : public Stmt {
  /// Location of \c if keyword
  SMLoc IfLoc;

  Expr *Cond;
  Stmt *Then;

  /// An else code block, which may be \c nullptr.
  Stmt *Else;

public:
  IfStmt(SMLoc IL, Expr *C, Stmt *T, Stmt *E = nullptr);

  Expr *getCond() const { return Cond; }
  void setCond(Expr *C) { Cond = C; }
  Stmt *getThen() const { return Then; }
  Stmt *getElse() const { return Else; }
  bool hasElseBlock() const { return Else != nullptr; }

  virtual SMRange getSourceRange() const override;
};

} // namesapce dusk

#endif /* DUSK_STMT_H */

