//===--- Parser.h - Dusk language parser interface --------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_PARSER_H
#define DUSK_PARSER_H

#include "dusk/AST/ASTNode.h"
#include "dusk/AST/Decl.h"
#include "dusk/AST/Expr.h"
#include "dusk/AST/Pattern.h"
#include "dusk/AST/Stmt.h"
#include "dusk/AST/Diagnostics.h"
#include "dusk/Parse/Token.h"
#include "dusk/Parse/Lexer.h"
#include "dusk/Parse/ParserResult.h"
#include "dusk/Frontend/InputFile.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/SourceMgr.h"
#include <exception>

namespace dusk {

/// Represents a single parse exception
///
/// Each exception holds it's diagnostic type.
class ParseError : public std::runtime_error {
  diag::ParserError Kind = diag::ParserError::unexpected_token;

public:
  ParseError() = default;
  ParseError(diag::ParserError K);

  diag::ParserError getKind() const { return Kind; }
};

/// The main class used for parsing a dusk-lang (.dusk) source file.
class Parser {
  llvm::SourceMgr &SourceManager;
  InputFile &SourceFile;
  diag::Diagnostics &Diag;
  Lexer *L;

  /// Parsing result.
  ParserResult R;

  /// Token currently evaluated by the parser.
  Token Tok;

  /// Location of previous token
  llvm::SMLoc PreviousLoc;

public:
  Parser(llvm::SourceMgr &SM, InputFile &SF, diag::Diagnostics &Diag,
         unsigned BufferID);

  ~Parser();

  /// Returns next token to be parsed.
  const Token &peekToken() const;

  /// Consumes current token and returns it's location.
  llvm::SMLoc consumeToken();

  /// Consumes token if and only if the token kind is the same as specified.
  llvm::SMLoc consumeToken(tok T);

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

  unsigned diagnose();
  unsigned diagnose(diag::ParserError E);
  unsigned diagnoseUnexpectedToken();

  /// Main parsing method.
  ///
  /// \return A pointer to the root of the AST representing parsed
  ///  source file.
  ParserResult &&parse();

private:
  //===------------------------------------------------------------------------===
  //
  //                     Recursive descent parsing methods
  //
  //===------------------------------------------------------------------------===

  ASTNode *parseGlobal();

  // MARK: - Declarations

  VarDecl *parseVarDecl();

  ConstDecl *parseConstDecl();

  Expr *parseDeclValue();

  FuncDecl *parseFuncDecl();

  BlockStmt *parseBlock();

  ASTNode *parseBlockBody();

  ParamDecl *parseParamDecl();

  // MARK: - Expressions

  Expr *parseExpr();
  Expr *parseAssignExpr();
  Expr *parseAssignExprRHS(Expr *LHS);

  Expr *parseLogicalExpr();
  Expr *parseLogicalExprRHS(Expr *LHS);

  Expr *parseArithExpr();
  Expr *parseArithExprRHS(Expr *LHS);

  Expr *parseMulExpr();
  Expr *parseMulExprRHS(Expr *LHS);

  Expr *parsePrimaryExpr();
  Expr *parsePrimaryExprRHS(IdentifierExpr *Dest);

  IdentifierExpr *parseIdentifierExpr();
  CallExpr *parseCallExpr(IdentifierExpr *Dest);
  SubscriptExpr *parseSubscriptExpr(IdentifierExpr *Dest);

  Expr *parseParenExpr();
  NumberLiteralExpr *parseNumberLiteralExpr();
  PrefixExpr *parseUnaryExpr();

  // MARK: - Statements

  ASTNode *parseStatement();

  Expr *parseExprStmt();

  BreakStmt *parseBreakStmt();
  ReturnStmt *parseReturnStmt();

  FuncStmt *parseFuncStmt();

  ForStmt *parseForStmt();
  RangeStmt *parseRangeStmt();

  WhileStmt *parseWhileStmt();

  IfStmt *parseIfStmt();
  BlockStmt *parseElseStmt();

  // MARK: - Patterns

  ExprPattern *parseExprPattern();
  llvm::SmallVector<Expr *, 128> parseExprPatternBody();
  Expr *parseExprPatternItem();

  VarPattern *parseVarPattern();
  llvm::SmallVector<ParamDecl *, 128> parseVarPatternBody();
  ParamDecl *parseVarPatternItem();

  SubscriptPattern *parseSubscriptPattern();

  /// Creates and adds a new instance of \c ASTNode to the parser result
  /// and returns a pointer to it.
  template <typename Node, typename... Args> Node *make(Args &&... args) {
    auto N = std::unique_ptr<Node>(new Node(std::forward<Args>(args)...));
    return R.push(std::move(N));
  }
};

} // namespace dusk

#endif /* DUSK_PARSER_H */
