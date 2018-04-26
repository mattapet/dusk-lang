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

/// IdentType ::=
///     ':' identifier
TypeRepr *Parser::parseIdentType() {
  assert(Tok.is(tok::colon) && "Invalid parse method");
  auto CL = consumeToken();
  auto Ty = Tok;
  if (!consumeIf(tok::kwInt)) {
    diagnose(Tok.getLoc(), diag::expected_identifier);
    return nullptr;
  }
  return makeTypeRepr<IdentTypeRepr>(CL, Ty.getText());
}

