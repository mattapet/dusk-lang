//===--- ParseDecl.cpp - Dusk language parser for types -------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Parse/Parser.h"
#include "dusk/AST/TypeRepr.h"

using namespace dusk;

/// TypeRepr ::=
///     IdentTypeRepr
///     ArrayTypeRepr
TypeRepr *Parser::parseTypeRepr() {
  if (Tok.is(tok::identifier))
    return parseIdentType();
  // Unexpected token
  diagnose(Tok.getLoc());
  return nullptr;
}

/// IdentType ::=
///     ':' identifier
TypeRepr *Parser::parseIdentType() {
  assert(Tok.is(tok::identifier) && "Invalid parse method");
  auto Ty = Tok;
  consumeToken();
  return parseArrayType(new (Context) IdentTypeRepr(Ty.getText()));
}

/// ArrayType ::=
///    epsilon
///    '[' Expr ']' ArrayType
TypeRepr *Parser::parseArrayType(TypeRepr *Base) {
  switch (Tok.getKind()) {
  case tok::comma:
  case tok::assign:
  case tok::r_paren:
  case tok::semi:
    return Base;

  case tok::l_bracket: {
    auto L = consumeToken();
    Expr *E = parseExpr();

    if (!consumeIf(tok::r_bracket)) {
      diagnose(Tok.getLoc(), diag::expected_r_bracket).fixIt("]", Tok.getLoc());
      return nullptr;
    }
    auto S = new (Context) SubscriptStmt(E, L, PreviousLoc);
    return parseArrayType(new (Context) ArrayTypeRepr(Base, S));
  }

  default:
    llvm_unreachable("Unexpected token.");
  }
}

