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
Pattern *Parser::parseExprPattern() {
  assert(Tok.is(tok::l_paren) && "Invalid parsing method.");
  // Consume '('
  auto LP = consumeToken();

  // Consume ExprPattern_
  auto C = parseExprPatternBody();

  // Consume ')'
  if (!consumeIf(tok::r_paren)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_r_paren)
    .fixItBefore(")", Tok.getLoc());
    return nullptr;
  }
  return make<ExprPattern>(std::move(C), LP, PreviousLoc);
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
    diagnose(Tok.getLoc());
    return llvm::SmallVector<Expr *, 128>();
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
    diagnose(Tok.getLoc(), diag::DiagID::expected_semicolon)
      .fixItAfter(";", Tok.getLoc());
    return nullptr;
  }
}

/// VarPatter ::=
///     '(' VarPatternBody ')'
Pattern *Parser::parseVarPattern() {
  assert(Tok.is(tok::l_paren) && "Invalid parse method.");
  // Consume '('
  auto LP = consumeToken();

  // Consume VarPatterBody
  auto C = parseVarPatternBody();

  if (!consumeIf(tok::r_paren)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_r_paren)
      .fixItBefore(")", Tok.getLoc());
    return nullptr;
  }
  return make<VarPattern>(std::move(C), LP, PreviousLoc);
}

/// VarPatternBody ::=
///     epsilon
///     identifier VarPatternItem
llvm::SmallVector<Decl *, 128> Parser::parseVarPatternBody() {
  llvm::SmallVector<Decl *, 128> C;
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
    diagnose(Tok.getLoc(), diag::DiagID::expected_colon_separator)
      .fixItAfter(",", PreviousLoc);
    return llvm::SmallVector<Decl *, 128>();
  }
  return C;
}

/// VarPatternItem ::=
///     epsilon
///     ',' identifier VarPatternItem
Decl *Parser::parseVarPatternItem() {
  switch (Tok.getKind()) {
  case tok::r_paren:
    // VarPattern__ -> epsilon
    return nullptr;

  case tok::colon:
    // VarPattern__ -> ',' identifier VarPattern__
    consumeToken();
    return parseParamDecl();
  default:
    diagnose(Tok.getLoc(), diag::DiagID::expected_colon_separator)
      .fixItAfter(",", PreviousLoc);
    return nullptr;
  }
}

