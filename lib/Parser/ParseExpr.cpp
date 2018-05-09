//===--- ParseExpr.cpp - Dusk language parser for expressions -------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Parse/Parser.h"
#include <cctype>

using namespace dusk;

Expr *Parser::parseExpr() {
  auto Primary = parsePrimaryExpr();
  if (!Primary)
    return nullptr;

  return parseBinExprRHS(Primary, 0);
}

Expr *Parser::parseBinExprRHS(Expr *LHS, unsigned P) {
  while (true) {
    auto Prec = Tok.getPrecedence();

    // If the precedence of current operator is lower or equal to previous one
    // (encounting invalid 0), return already parsed part of the expression.
    if (Prec <= P)
      return LHS;
    auto Op = Tok;
    consumeToken();
    
    // Return nullptr on error
    auto RHS = parsePrimaryExpr();
    if (!RHS)
      return nullptr;

    // If precedence of next operand is greater than the current one, parse
    // expression in favor of the next operand.
    if (Prec < Tok.getPrecedence()) {
      RHS = parseBinExprRHS(RHS, Prec);
      if (!RHS)
        return nullptr;
    }

    // Update the current expression.
    if (Op.is(tok::assign))
      LHS = makeNode<AssignExpr>(LHS, RHS);
    else
      LHS = makeNode<InfixExpr>(LHS, RHS, Op);
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
      
  case tok::l_bracket:
      return parseArrayLiteralExpr();

  case tok::minus:
  case tok::lnot:
    return parseUnaryExpr();

  default:
    diagnose(Tok.getLoc(), diag::expected_expression);
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
  case tok::comma:
  case tok::semicolon:
    return Dest;

  /// Enable nested postfix operator expression e.g. <code>a[1][2]</code>.
  case tok::l_paren:
    return parsePrimaryExprRHS(parseCallExpr(Dest));
  case tok::l_bracket:
    return parsePrimaryExprRHS(parseSubscriptExpr(Dest));

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
  return makeNode<IdentifierExpr>(Name, Loc);
}

/// CallExpr ::= idenifier '(' Args ')'
Expr *Parser::parseCallExpr(Expr *Dest) {
  // Validate `(`
  assert(Tok.is(tok::l_paren) && "Invalid parse method.");
  return makeNode<CallExpr>(Dest, parseExprPattern());
}

/// SubscriptExpr ::= idenifier '[' Args ']'
Expr *Parser::parseSubscriptExpr(Expr *Dest) {
  // Validate `[`
  assert(Tok.is(tok::l_bracket) && "Invalid parse method.");
  return makeNode<SubscriptExpr>(Dest, parseSubscriptStmt());
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
  return makeNode<ParenExpr>(E, L, PreviousLoc);
}

Expr *Parser::parseUnaryExpr() {
  // Validate that we have a unary operand.
  assert(Tok.isAny(tok::lnot, tok::minus) && "Invalid parse method.");

  auto Op = Tok;
  consumeToken();
  return makeNode<PrefixExpr>(parsePrimaryExpr(), Op);
}

/// Array literal
Expr *Parser::parseArrayLiteralExpr() {
  assert(Tok.is(tok::l_bracket) && "Invalid parsing method.");
  auto LB = consumeToken();
  auto Values = parseExprPatternBody();
  if (!consumeIf(tok::r_bracket)) {
    diagnose(Tok.getLoc(), diag::expected_r_bracket)
      .fixIt("]", Tok.getLoc());
    return nullptr;
  }
  auto Pttrn = makePattern<ExprPattern>(std::move(Values), LB, PreviousLoc);
  return makeNode<ArrayLiteralExpr>(Pttrn);
}

/// Properly parse number literal
Expr *Parser::parseNumberLiteralExpr() {
  // Validate that we have a number literal
  assert(Tok.is(tok::number_literal) && "Invalid parsing method.");

  auto Str = Tok.getText();
  auto R = Tok.getRange();
  int64_t Value;
  if (Str.size() > 1) {
    llvm::StringRef B = Str.slice(2, Str.size());

    // Parse hexadecimal literal
    if (Str[0] == '0' && std::tolower(Str[1]) == 'x')
      B.getAsInteger(16, Value);

    // Parse octal litera
    else if (Str[0] == '0' && std::tolower(Str[1]) == 'o')
      B.getAsInteger(8, Value);

    // Parse binary literal
    else if (Str[0] == '0' && std::tolower(Str[1]) == 'b')
      B.getAsInteger(2, Value);
    else
      // Parse decimal literal
      Str.getAsInteger(10, Value);
  } else {
    // Parse decimal literal
    Str.getAsInteger(10, Value);
  }

  consumeToken();
  auto NL = makeNode<NumberLiteralExpr>(Value, R);
  NL->setType(makeType<IntType>());
  return NL;
}
