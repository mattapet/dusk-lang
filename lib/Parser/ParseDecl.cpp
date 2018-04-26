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

/// Let declaration
///
/// LetDecl ::=
///     'let' identifier ':' identifier '=' Expr ';'
///     'let' identifier '=' Expr ';'
Decl *Parser::parseLetDecl() {
  // Validate correct variable decl
  assert(Tok.is(tok::kwLet) && "Invalid parsing method.");

  auto L = consumeToken();
  auto ID = Tok;
  if (!consumeIf(tok::identifier)) {
    diagnose(Tok.getLoc(), diag::expected_identifier);
    return nullptr;
  }
  
  TypeRepr *TR = nullptr;
  if (Tok.is(tok::colon))
    if ((TR = parseIdentType()) == nullptr)
      return nullptr;
  
  return makeNode<LetDecl>(ID.getText(), ID.getLoc(), L, parseDeclValue(), TR);
}

/// Var declaration
///
/// VarDecl ::=
///     'var' identifier ':' identifier '=' Expr ';'
///     'var' identifier ':' identifier ';'
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
  
  TypeRepr *TR;
  if (Tok.is(tok::colon))
    if ((TR = parseIdentType()) == nullptr)
      return nullptr;

  return makeNode<VarDecl>(ID.getText(), ID.getLoc(), L, parseDeclValue(), TR);
}

/// DeclVal ::=
///     ';'
///     '=' Expr ';'
Expr *Parser::parseDeclValue() {
  /// Empty initialization.
  if (consumeIf(tok::semicolon))
    return nullptr;
  
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
///     'func' identifier '(' Args ')' RetType CodeBlock
Decl *Parser::parseFuncDecl() {
  // Ensure `func` keyword
  assert(Tok.is(tok::kwFunc) && "Invalid parsing method.");

  auto FL = consumeToken();

  auto ID = Tok;
  if (!consumeIf(tok::identifier)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_identifier);
    return nullptr;
  }

  auto Args = static_cast<VarPattern *>(parseVarPattern());
  auto RetTy = parseFuncDeclType();
  return makeNode<FuncDecl>(ID.getText(), ID.getLoc(), FL, Args, RetTy);
}

/// Function return type
///
/// RetType ::=
///     epsilon
///     '->' 'Int' | 'Void'
FuncRetType *Parser::parseFuncDeclType() {
  // Implicit return type is `Void`
  if (Tok.isAny(tok::l_brace, tok::semicolon))
    return nullptr;
  
  if (!consumeIf(tok::arrow)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_return_type)
        .fixItBefore("->", Tok.getLoc());
    return nullptr;
  }
  auto ArrowLoc = PreviousLoc;
  auto Ty = Tok;
  
  if (consumeIf(tok::kwVoid))
    return new FuncRetType(ArrowLoc, Ty);
  if (consumeIf(tok::kwInt))
    return new FuncRetType(ArrowLoc, Ty);

  diagnose(Tok.getLoc(), diag::DiagID::expected_type_specifier)
      .fixItBefore("Int", Tok.getLoc())
      .fixItBefore("Void", Tok.getLoc());
  return nullptr;
}

/// Param declaration
Decl *Parser::parseParamDecl() {
  // Validate correct param declaration
  assert(Tok.is(tok::identifier) && "Invalid parsing method.");

  auto ID = Tok;
  consumeToken();
  if (Tok.isNot(tok::colon)) {
    diagnose(Tok.getLoc(), diag::expected_type_annotation);
    return nullptr;
  }
  if (auto TR = parseIdentType())
    return makeNode<ParamDecl>(ID.getText(), ID.getLoc(), TR);
  return nullptr;
}

