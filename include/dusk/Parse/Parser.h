//===--- Parser.h - Dusk language parser interface --------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//
//
// This file defines a parser interface.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_PARSER_H
#define DUSK_PARSER_H

#include "dusk/AST/ASTNode.h"
#include "dusk/AST/ASTContext.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/Type.h"
#include "dusk/AST/Diagnostics.h"
#include "dusk/Parse/Token.h"
#include "dusk/Parse/Lexer.h"
#include "dusk/Parse/ParserResult.h"
#include "dusk/Frontend/SourceFile.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/SourceMgr.h"

namespace dusk {

/// The main class used for parsing a dusk-lang (.dusk) source file.
class Parser {
  ASTContext &Context;
  
  llvm::SourceMgr &SourceManager;
  
  SourceFile &SF;
  
  DiagnosticEngine &Diag;
  
  Lexer *L;

  /// Parsing result.
  ParserResult R;

  /// Token currently evaluated by the parser.
  Token Tok;

  /// Location of previous token
  SMLoc PreviousLoc;

public:
  Parser(ASTContext &C,
         SourceMgr &SM,
         SourceFile &SF,
         DiagnosticEngine &Diag,
         unsigned BufferID);

  ~Parser();

  /// Returns next token to be parsed.
  const Token &peekToken() const;

  /// Consumes current token and returns it's location.
  SMLoc consumeToken();

  /// Consumes token if and only if the token kind is the same as specified.
  SMLoc consumeToken(tok T);

  /// Consumes token, if it's of expected type
  ///
  /// \retrun \c true if a token was consumed, \c false otherwise.
  bool consumeIf(tok T) {
    if (Tok.is(T)) {
      PreviousLoc = consumeToken();
      return true;
    }
    return false;
  }

  /// Force immediate termination of parsing.
  void terminateParsing() { Tok.setKind(tok::eof); }

  DiagnosticRef diagnose(SMLoc Loc,
                         diag::DiagID ID = diag::DiagID::unexpected_token);

  /// Main parsing method.
  ModuleDecl *parseModule();

private:
//===------------------------------------------------------------------------===
//
//                     Recursive descent parsing methods
//
//===------------------------------------------------------------------------===

  ASTNode *parseGlobal();

  // MARK: - Declarations

  Decl *parseVarDecl();

  Decl *parseConstDecl();

  Expr *parseDeclValue();

  Decl *parseFuncDecl();
  FuncRetType *parseFuncDeclType();

  Stmt *parseBlock();

  ASTNode *parseBlockBody();

  Decl *parseParamDecl();

  // MARK: - Expressions

  Expr *parseExpr();
  Expr *parseBinExprRHS(Expr *LHS, unsigned P);
  
  Expr *parsePrimaryExpr();
  Expr *parsePrimaryExprRHS(Expr *Dest);

  Expr *parseIdentifierExpr();
  Expr *parseCallExpr(Expr *Dest);
  Expr *parseSubscriptExpr(Expr *Dest);

  Expr *parseParenExpr();
  Expr *parseNumberLiteralExpr();
  Expr *parseUnaryExpr();

  // MARK: - Statements

  ASTNode *parseStatement();

  Expr *parseExprStmt();

  Stmt *parseBreakStmt();
  Stmt *parseReturnStmt();
  
  Stmt *parseSubscriptStmt();

  Stmt *parseExterStmt();
  Stmt *parseFuncStmt();

  Stmt *parseForStmt();
  Stmt *parseRangeStmt();

  Stmt *parseWhileStmt();

  Stmt *parseIfStmt();
  Stmt *parseElseStmt();

  // MARK: - Patterns

  Pattern *parseExprPattern();
  SmallVector<Expr *, 128> parseExprPatternBody();
  Expr *parseExprPatternItem();

  Pattern *parseVarPattern();
  SmallVector<Decl *, 128> parseVarPatternBody();
  Decl *parseVarPatternItem();

  /// Creates and adds a new instance of \c ASTNode to the parser result
  /// and returns a pointer to it.
  template <typename Node, typename... Args> Node *makeNode(Args &&... args) {
    auto N = std::unique_ptr<Node>(new Node(std::forward<Args>(args)...));
    return Context.pushNode(std::move(N));
  }

  /// Creates and adds a new instance of \c ASTNode to the parser result
  /// and returns a pointer to it.
  template <typename Pattern, typename... Args>
  Pattern *makePattern(Args &&... args) {
    auto P = std::unique_ptr<Pattern>(new Pattern(std::forward<Args>(args)...));
    return Context.pushPattern(std::move(P));
  }

  /// Creates and adds a new instance of \c ASTNode to the parser result
  /// and returns a pointer to it.
  template <typename Type, typename... Args> Type *makeType(Args &&... args) {
    auto T = std::unique_ptr<Type>(new Type(std::forward<Args>(args)...));
    return Context.pushType(std::move(T));
  }
};

} // namespace dusk

#endif /* DUSK_PARSER_H */

