//===--- ParseExpr.cpp - Dusk language parser for expressions -------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Parse/Parser.h"

using namespace dusk;

Expr *Parser::parseExpr() {
  switch (Tok.getKind()) {
  case tok::identifier:
  case tok::number_literal:
  case tok::l_paren:
  case tok::minus:
    return parseAssignExpr();

  default:
    diagnose(Tok.getLoc());
    return nullptr;
  }
}

Expr *Parser::parseAssignExpr() {
  switch (Tok.getKind()) {
  case tok::identifier:
  case tok::number_literal:
  case tok::l_paren:
  case tok::minus:
    return parseAssignExprRHS(parseLogicalExpr());

  default:
    diagnose(consumeToken());
    return nullptr;
  }
}

Expr *Parser::parseAssignExprRHS(Expr *LHS) {
  switch (Tok.getKind()) {
  case tok::elipsis_incl:
  case tok::elipsis_excl:
  case tok::r_paren:
  case tok::r_bracket:
  case tok::l_brace:
  case tok::colon:
  case tok::semicolon:
    return LHS;

  case tok::assign:
    consumeToken();
    return make<AssignExpr>((IdentifierExpr *)LHS, parseExpr());

  default:
      diagnose(Tok.getLoc());
      return nullptr;
  }
}

Expr *Parser::parseLogicalExpr() {
  switch (Tok.getKind()) {
  case tok::identifier:
  case tok::number_literal:
  case tok::l_paren:
  case tok::minus:
    return parseLogicalExprRHS(parseArithExpr());

  default:
      diagnose(Tok.getLoc());
      return nullptr;
  }
}

Expr *Parser::parseLogicalExprRHS(Expr *LHS) {
  auto T = Tok;
  switch (Tok.getKind()) {
  case tok::assign:
  case tok::elipsis_incl:
  case tok::elipsis_excl:
  case tok::r_paren:
  case tok::r_bracket:
  case tok::l_brace:
  case tok::colon:
  case tok::semicolon:
    return LHS;

  case tok::equals:
  case tok::nequals:
  case tok::less:
  case tok::less_eq:
  case tok::greater:
  case tok::greater_eq:
    consumeToken();
    return make<InfixExpr>(LHS, parseArithExpr(), T);

  default:
      diagnose(Tok.getLoc());
      return nullptr;
  }
}

Expr *Parser::parseArithExpr() {
  switch (Tok.getKind()) {
  case tok::identifier:
  case tok::number_literal:
  case tok::l_paren:
  case tok::minus:
    return parseArithExprRHS(parseMulExpr());

  default:
      diagnose(Tok.getLoc());
      return nullptr;
  }
}

Expr *Parser::parseArithExprRHS(Expr *LHS) {
  auto T = Tok;
  switch (Tok.getKind()) {
  case tok::assign:
  case tok::equals:
  case tok::nequals:
  case tok::less:
  case tok::less_eq:
  case tok::greater:
  case tok::greater_eq:
  case tok::elipsis_incl:
  case tok::elipsis_excl:
  case tok::r_paren:
  case tok::r_bracket:
  case tok::l_brace:
  case tok::colon:
  case tok::semicolon:
    return LHS;

  case tok::plus:
  case tok::minus:
    consumeToken();
    return make<InfixExpr>(LHS, parseExpr(), T);

  default:
      diagnose(Tok.getLoc());
      return nullptr;
  }
}

Expr *Parser::parseMulExpr() {
  switch (Tok.getKind()) {
  case tok::identifier:
  case tok::number_literal:
  case tok::l_paren:
  case tok::minus:
    return parseMulExprRHS(parsePrimaryExpr());

  default:
      diagnose(Tok.getLoc());
      return nullptr;
  }
}

Expr *Parser::parseMulExprRHS(Expr *LHS) {
  auto T = Tok;
  switch (Tok.getKind()) {
  case tok::plus:
  case tok::minus:
  case tok::equals:
  case tok::nequals:
  case tok::less:
  case tok::less_eq:
  case tok::greater:
  case tok::greater_eq:
  case tok::assign:
  case tok::elipsis_incl:
  case tok::elipsis_excl:
  case tok::r_paren:
  case tok::r_bracket:
  case tok::l_brace:
  case tok::colon:
  case tok::semicolon:
    return LHS;

  case tok::mod:
  case tok::multipy:
  case tok::divide:
    consumeToken();
    return make<InfixExpr>(LHS, parseExpr(), T);

  default:
      diagnose(Tok.getLoc());
      return nullptr;
  }
}

Expr *Parser::parsePrimaryExpr() {
  switch (Tok.getKind()) {
  case tok::l_paren:
    return parseParenExpr();

  case tok::identifier:
    return parsePrimaryExprRHS(parseIdentifierExpr());

  case tok::number_literal:
    return parseNumberLiteralExpr();

  case tok::minus:
  case tok::neg:
    return parseUnaryExpr();

  default:
      diagnose(Tok.getLoc());
      return nullptr;
  }
}

Expr *Parser::parsePrimaryExprRHS(Expr *Dest) {
  if (Tok.isOperator())
    return Dest;

  switch (Tok.getKind()) {
  case tok::elipsis_incl:
  case tok::elipsis_excl:
  case tok::r_paren:
  case tok::r_bracket:
  case tok::l_brace:
  case tok::colon:
  case tok::semicolon:
    return Dest;

  case tok::l_paren:
    return parseCallExpr(Dest);

  case tok::l_bracket:
    return parseSubscriptExpr(Dest);

  default:
      diagnose(Tok.getLoc());
      return nullptr;
  }
}

Expr *Parser::parseIdentifierExpr() {
  // Validate that we really have an identifier to parse
  assert(Tok.is(tok::identifier) && "Invalid parsing method.");

  auto Name = Tok.getText();
  auto Loc = consumeToken();
  return make<IdentifierExpr>(Name, Loc);
}

/// CallExpr ::= idenifier '(' Args ')'
Expr *Parser::parseCallExpr(Expr *Dest) {
  // Validate `(`
  assert(Tok.is(tok::l_paren) && "Invalid parse method.");
  return make<CallExpr>(Dest, parseExprPattern());
}

/// SubscriptExpr ::= idenifier '[' Args ']'
Expr *Parser::parseSubscriptExpr(Expr *Dest) {
  // Validate `[`
  assert(Tok.is(tok::l_bracket) && "Invalid parse method.");
  return make<SubscriptExpr>(Dest, parseSubscriptStmt());
}

/// PrimaryExpr ::= '(' Expr ')'
Expr *Parser::parseParenExpr() {
  // Validate `(`
  assert(Tok.is(tok::l_paren) && "Invalid parse method.");
  auto L = consumeToken();
  auto E = parseExpr();
  if (!consumeIf(tok::r_paren)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_r_paren)
      .fixItAfter(")", Tok.getLoc());
    return nullptr;
  }
  return make<ParenExpr>(E, L, PreviousLoc);
}

Expr *Parser::parseUnaryExpr() {
  // Validate that we have a unary operand.
  assert(Tok.isAny(tok::neg, tok::minus) && "Invalid parse method.");

  auto Op = Tok;
  consumeToken();
  return make<PrefixExpr>(parsePrimaryExpr(), Op);
}

/// Properly parse number literal
Expr *Parser::parseNumberLiteralExpr() {
  // Validate that we have a number literal
  assert(Tok.is(tok::number_literal) && "Invalid parsing method.");

  auto Str = Tok.getText();
  auto R = Tok.getRange();
  int Value;
  if (Str.size() > 1) {
    llvm::StringRef B = Str.slice(2, Str.size() - 1);

    // Parse hexadecimal literal
    if (Str[0] == '0' && Str[1] == 'x')
      B.getAsInteger(16, Value);

    // Parse octal litera
    else if (Str[0] == '0' && Str[1] == 'o')
      B.getAsInteger(8, Value);

    // Parse binary literal
    else if (Str[0] == '0' && Str[1] == 'b')
      B.getAsInteger(2, Value);
    else
      // Parse decimal literal
      Str.getAsInteger(10, Value);
  } else {
    // Parse decimal literal
    Str.getAsInteger(10, Value);
  }

  consumeToken();
  return make<NumberLiteralExpr>(Value, R);
}
