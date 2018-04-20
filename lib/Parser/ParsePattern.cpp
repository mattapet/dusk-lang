//===--- ParserStmt.cpp - Dusk language pattern parsing -------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Parse/Parser.h"
#include "llvm/ADT/SmallVector.h"

using namespace dusk;

/// ExprPattern ::=
///     '(' ExprPattern_ ')'
ExprPattern *Parser::parseExprPattern() {
  assert(Tok.is(tok::l_paren) && "Invalid parsing method.");
  // Consume '('
  auto LP = consumeToken();

  // Consume ExprPattern_
  auto C = parseExprPatternBody();

  // Consume ')'
  auto RP = consumeToken();
  return make<ExprPattern>(std::move(C), LP, RP);
}

/// ExprPatternBody ::=
///     epsilon
///     Expr ExprPatternBody
llvm::SmallVector<Expr *, 128> Parser::parseExprPatternBody() {
  llvm::SmallVector<Expr *, 128> C;
  switch (Tok.getKind()) {
  case tok::r_paren:
    // ExprPatternBody -> epsilon
    break;

  case tok::number_literal:
  case tok::identifier:
  case tok::l_paren:
    // ExprPatternBody -> Expr ExprPatternBody
    C.push_back(parseExpr());

    // Simulation of right recursion in
    // `ExprPatternItem -> ',' Expr ExprPatternItem` rule
    while (auto E = parseExprPatternItem())
      C.push_back(E);
    break;

  default:
    throw ParseError(diag::ParserError::unexpected_token);
  }
  return C;
}

/// ExprPatternItem ::=
///     epsilon
///     ',' Expr ExprPatternItem
Expr *Parser::parseExprPatternItem() {
  switch (Tok.getKind()) {
  case tok::r_paren:
    // ExprPatternItem -> epsilon
    return nullptr;

  case tok::colon:
    // ExprPatternItem -> ',' Expr ExprPatternItem
    consumeToken();
    return parseExpr();

  default:
    throw ParseError(diag::ParserError::missing_colon);
  }
}

/// VarPatter ::=
///     '(' VarPatternBody ')'
VarPattern *Parser::parseVarPattern() {
  assert(Tok.is(tok::l_paren) && "Invalid parse method.");
  // Consume '('
  auto LP = consumeToken();

  // Consume VarPatterBody
  auto C = parseVarPatternBody();

  // Consume ')'
  auto RP = consumeToken();
  return make<VarPattern>(std::move(C), LP, RP);
}

/// VarPatternBody ::=
///     epsilon
///     identifier VarPatternItem
llvm::SmallVector<ParamDecl *, 128> Parser::parseVarPatternBody() {
  llvm::SmallVector<ParamDecl *, 128> C;
  switch (Tok.getKind()) {
  case tok::r_paren:
    // VarPattern -> epsilon
    break;

  case tok::identifier:
    // VarPatternBody -> identifier VarPatternItem
    C.push_back(parseParamDecl());

    // Simulation of right recursion in
    // `VarPatternItem -> ',' identifier VarPatternItem` rule
    while (auto V = parseVarPatternItem())
      C.push_back(V);

    break;

  default:
    throw ParseError(diag::ParserError::missing_colon);
  }
  return C;
}

/// VarPatternItem ::=
///     epsilon
///     ',' identifier VarPatternItem
ParamDecl *Parser::parseVarPatternItem() {
  switch (Tok.getKind()) {
  case tok::r_paren:
    // VarPattern__ -> epsilon
    return nullptr;

  case tok::colon:
    // VarPattern__ -> ',' identifier VarPattern__
    consumeToken();
    return parseParamDecl();
  default:
    throw ParseError(diag::ParserError::missing_colon);
  }
}

/// SubscriptionPattern ::=
///     [ Expr ]
SubscriptPattern *Parser::parseSubscriptPattern() {
  // Validate `[` start.
  assert(Tok.is(tok::l_bracket) && "Invalid parse method.");

  auto L = consumeToken();
  auto V = parseExpr();
  if (!consumeIf(tok::r_bracket))
    throw ParseError(diag::ParserError::missing_r_bracket);

  return make<SubscriptPattern>(V, L, PreviousLoc);
}

