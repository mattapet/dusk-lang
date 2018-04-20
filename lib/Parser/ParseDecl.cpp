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
ConstDecl *Parser::parseConstDecl() {
  // Validate correct variable decl
  assert(Tok.is(tok::kwConst) && "Invalid parsing method.");

  auto L = consumeToken();
  auto ID = Tok;
  if (!consumeIf(tok::identifier))
    throw ParseError(diag::ParserError::missing_identfier);

  return make<ConstDecl>(ID.getText(), ID.getLoc(), L, parseDeclValue());
}

/// Var declaration
///
/// VarDecl ::=
///     'var' identifier '=' Expr ';'
VarDecl *Parser::parseVarDecl() {
  // Validate correct variable decl
  assert(Tok.is(tok::kwVar) && "Invalid parsing method.");

  auto L = consumeToken();
  auto ID = Tok;
  if (!consumeIf(tok::identifier))
    throw ParseError(diag::ParserError::missing_identfier);

  return make<VarDecl>(ID.getText(), ID.getLoc(), L, parseDeclValue());
}

/// DeclVal ::=
///     '=' Expr ';'
Expr *Parser::parseDeclValue() {
  if (!consumeIf(tok::assign))
    throw ParseError(diag::ParserError::missing_assign);

  auto E = parseExpr();
  if (!consumeIf(tok::semicolon))
    throw ParseError(diag::ParserError::missing_semicolon);
  return E;
}

/// Function declaration
///
/// FuncDecl ::=
///     'func' identifier '(' Args ')' CodeBlock
FuncDecl *Parser::parseFuncDecl() {
  // Ensure `func` keyword
  assert(Tok.is(tok::kwFunc) && "Invalid parsing method.");

  auto FL = consumeToken();

  auto ID = Tok;
  if (!consumeIf(tok::identifier))
    throw ParseError(diag::ParserError::missing_identfier);

  return make<FuncDecl>(ID.getText(), ID.getLoc(), FL, parseVarPattern());
}

/// Param declaration
ParamDecl *Parser::parseParamDecl() {
  // Validate correct param declaration
  assert(Tok.is(tok::identifier) && "Invalid parsing method.");

  auto ID = Tok;
  consumeToken();
  return make<ParamDecl>(ID.getText(), ID.getLoc());
}


