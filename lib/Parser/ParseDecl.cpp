//===--- ParseDecl.cpp - Dusk language parser for declarations ------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Parse/Parser.h"

using namespace dusk;

/// Const declaration
///
/// ConstDecl ::=
///     'const' identifier '=' Expr ';'
Decl *Parser::parseConstDecl() {
  // Validate correct variable decl
  assert(Tok.is(tok::kwConst) && "Invalid parsing method.");

  auto L = consumeToken();
  auto ID = Tok;
  if (!consumeIf(tok::identifier)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_identifier);
    return nullptr;
  }

  return make<ConstDecl>(ID.getText(), ID.getLoc(), L, parseDeclValue());
}

/// Var declaration
///
/// VarDecl ::=
///     'var' identifier '=' Expr ';'
Decl *Parser::parseVarDecl() {
  // Validate correct variable decl
  assert(Tok.is(tok::kwVar) && "Invalid parsing method.");

  auto L = consumeToken();
  auto ID = Tok;
  if (!consumeIf(tok::identifier)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_identifier);
    return nullptr;
  }

  return make<VarDecl>(ID.getText(), ID.getLoc(), L, parseDeclValue());
}

/// DeclVal ::=
///     '=' Expr ';'
Expr *Parser::parseDeclValue() {
  if (!consumeIf(tok::assign)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_identifier);
    return nullptr;
  }

  auto E = parseExpr();
  if (!consumeIf(tok::semicolon)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_semicolon)
      .fixItAfter(";", Tok.getLoc());
    return nullptr;
  }
  return E;
}

/// Function declaration
///
/// FuncDecl ::=
///     'func' identifier '(' Args ')' CodeBlock
Decl *Parser::parseFuncDecl() {
  // Ensure `func` keyword
  assert(Tok.is(tok::kwFunc) && "Invalid parsing method.");

  auto FL = consumeToken();

  auto ID = Tok;
  if (!consumeIf(tok::identifier)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_identifier);
    return nullptr;
  }

  return make<FuncDecl>(ID.getText(), ID.getLoc(), FL, parseVarPattern());
}

/// Param declaration
Decl *Parser::parseParamDecl() {
  // Validate correct param declaration
  assert(Tok.is(tok::identifier) && "Invalid parsing method.");

  auto ID = Tok;
  consumeToken();
  return make<ParamDecl>(ID.getText(), ID.getLoc());
}


