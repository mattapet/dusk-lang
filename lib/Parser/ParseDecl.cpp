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

Decl *Parser::parseDecl() {
  switch (Tok.getKind()) {
  case tok::kw_let:
    return parseLetDecl();
  case tok::kw_var:
    return parseVarDecl();
  case tok::kw_func:
    return parseFuncDecl();

  default:
    return nullptr;
  }
}

/// Let declaration
///
/// LetDecl ::=
///     'let' identifier ValType '=' Expr ';'
///     'let' identifier '=' Expr ';'
Decl *Parser::parseLetDecl() {
  // Validate correct variable decl
  assert(Tok.is(tok::kw_let) && "Invalid parsing method.");

  auto L = consumeToken();
  auto ID = Tok;
  if (!consumeIf(tok::identifier)) {
    diagnose(Tok.getLoc(), diag::expected_identifier);
    return nullptr;
  }

  TypeRepr *TR = nullptr;
  if (Tok.is(tok::colon))
    if ((TR = parseValDeclType()) == nullptr)
      return nullptr;
  return new (Context) VarDecl(VarDecl::Specifier::Let, ID.getText(),
                               ID.getLoc(), L, parseDeclValue(), TR);
}

/// Var declaration
///
/// VarDecl ::=
///     'var' identifier ValType '=' Expr ';'
///     'var' identifier ValType ';'
///     'var' identifier '=' Expr ';'
Decl *Parser::parseVarDecl() {
  // Validate correct variable decl
  assert(Tok.is(tok::kw_var) && "Invalid parsing method.");

  auto L = consumeToken();
  auto ID = Tok;
  if (!consumeIf(tok::identifier)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_identifier);
    return nullptr;
  }

  TypeRepr *TR = nullptr;
  if (Tok.is(tok::colon))
    if ((TR = parseValDeclType()) == nullptr)
      return nullptr;

  return new (Context) VarDecl(ValDecl::Specifier::Var, ID.getText(),
                               ID.getLoc(), L, parseDeclValue(), TR);
}

/// DeclVal ::=
///     ';'
///     '=' Expr ';'
Expr *Parser::parseDeclValue() {
  /// Empty initialization.
  if (consumeIf(tok::semi))
    return nullptr;

  if (!consumeIf(tok::assign)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_identifier);
    return nullptr;
  }

  auto E = parseExpr();
  if (!consumeIf(tok::semi)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_semicolon)
        .fixItAfter(";", Tok.getLoc());
    return nullptr;
  }
  return E;
}

/// Function declaration
///
/// FuncDecl ::=
///     'func' identifier '(' Args ')' RetType CodeBlock
Decl *Parser::parseFuncDecl() {
  // Ensure `func` keyword
  assert(Tok.is(tok::kw_func) && "Invalid parsing method.");

  auto FL = consumeToken();

  auto ID = Tok;
  if (!consumeIf(tok::identifier)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_identifier);
    return nullptr;
  }

  auto Args = static_cast<VarPattern *>(parseVarPattern());
  auto RetTy = parseFuncDeclType();
  return new (Context) FuncDecl(ID.getText(), ID.getLoc(), FL, Args, RetTy);
}

/// Value decl type
///
/// DeclType ::=
///   epsilon
///   ':' TypeRepr
TypeRepr *Parser::parseValDeclType() {
  assert(Tok.is(tok::colon) && "Invalid parse method");
  consumeToken();
  return parseTypeRepr();
}

/// Function return type
///
/// RetType ::=
///     epsilon
///     '->' 'Int' | 'Void'
TypeRepr *Parser::parseFuncDeclType() {
  // Implicit return type is `Void`
  if (Tok.isAny(tok::l_brace, tok::semi))
    return nullptr;

  if (!consumeIf(tok::arrow)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_return_type)
        .fixItBefore("->", Tok.getLoc());
    return nullptr;
  }

  if (Tok.is(tok::identifier))
    return parseTypeRepr();

  diagnose(Tok.getLoc(), diag::DiagID::expected_type_specifier)
      .fixItBefore("Int", Tok.getLoc())
      .fixItBefore("Void", Tok.getLoc());
  return nullptr;
}

/// Param declaration
Decl *Parser::parseParamDecl() {
  // Validate correct param declaration
  assert(Tok.isAny(tok::identifier, tok::kw_inout) &&
         "Invalid parsing method.");
  
  ValDecl::Specifier Spec = ValDecl::Specifier::Default;

  auto ID = Tok;
  consumeToken();
  if (!consumeIf(tok::colon)) {
    diagnose(Tok.getLoc(), diag::expected_type_annotation);
    return nullptr;
  }
  if (auto TR = parseTypeRepr())
    return new (Context) ParamDecl(Spec, ID.getText(), ID.getLoc(), TR);
  return nullptr;
}
