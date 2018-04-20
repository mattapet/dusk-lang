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
    throw ParseError(diag::ParserError::unexpected_token);
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
    throw ParseError(diag::ParserError::unexpected_token);
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
    throw ParseError(diag::ParserError::unexpected_token);
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
    throw ParseError(diag::ParserError::unexpected_token);
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
    throw ParseError(diag::ParserError::unexpected_token);
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
    throw ParseError(diag::ParserError::unexpected_token);
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
    throw ParseError(diag::ParserError::unexpected_token);
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
    throw ParseError(diag::ParserError::unexpected_token);
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
    throw ParseError(diag::ParserError::unexpected_token);
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
    throw ParseError(diag::ParserError::unexpected_token);
  }
}

Expr *Parser::parsePrimaryExprRHS(IdentifierExpr *Dest) {
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
    throw ParseError(diag::ParserError::unexpected_token);
  }
}

IdentifierExpr *Parser::parseIdentifierExpr() {
  // Validate that we really have an identifier to parse
  assert(Tok.is(tok::identifier) && "Invalid parsing method.");

  auto Name = Tok.getText();
  auto Loc = consumeToken();
  return make<IdentifierExpr>(Name, Loc);
}

/// CallExpr ::= idenifier '(' Args ')'
CallExpr *Parser::parseCallExpr(IdentifierExpr *Dest) {
  // Validate `(`
  assert(Tok.is(tok::l_paren) && "Invalid parse method.");
  return make<CallExpr>(Dest, parseExprPattern());
}

/// SubscriptExpr ::= idenifier '[' Args ']'
SubscriptExpr *Parser::parseSubscriptExpr(IdentifierExpr *Dest) {
  // Validate `[`
  assert(Tok.is(tok::l_bracket) && "Invalid parse method.");
  return make<SubscriptExpr>(Dest, parseSubscriptPattern());
}

/// PrimaryExpr ::= '(' Expr ')'
Expr *Parser::parseParenExpr() {
  // Validate `(`
  assert(Tok.is(tok::l_paren) && "Invalid parse method.");
  auto L = consumeToken();
  auto E = parseExpr();
  if (!consumeIf(tok::r_paren))
    throw ParseError(diag::ParserError::missing_r_paren);
  return make<ParenExpr>(E, L, PreviousLoc);
}

PrefixExpr *Parser::parseUnaryExpr() {
  // Validate that we have a unary operand.
  assert(Tok.isAny(tok::neg, tok::minus) && "Invalid parse method.");

  auto Op = Tok;
  consumeToken();
  return make<PrefixExpr>(parsePrimaryExpr(), Op);
}

/// Properly parse number literal
NumberLiteralExpr *Parser::parseNumberLiteralExpr() {
  // Validate that we have a number literal
  assert(Tok.is(tok::number_literal) && "Invalid parsing method.");

  auto Str = Tok.getText();
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
  auto E = llvm::SMLoc::getFromPointer(Str.data() + Str.size());
  return make<NumberLiteralExpr>(Value, llvm::SMRange{PreviousLoc, E});
}
