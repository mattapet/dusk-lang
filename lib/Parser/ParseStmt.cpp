//===--- ParserStmt.cpp - Dusk language parser statements -----------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Parse/Parser.h"
#include "llvm/ADT/SmallVector.h"
#include <vector>

using namespace dusk;

ASTNode *Parser::parseStatement() { return nullptr; }

/// ExprStmt ::=
///     Expr ';'
Expr *Parser::parseExprStmt() {
  Expr *E;
  switch (Tok.getKind()) {
  case tok::identifier:
  case tok::number_literal:
  case tok::l_paren:
    E = parseExpr();
    break;
  default:
    diagnose(Tok.getLoc());
    return nullptr;
  }
  if (!consumeIf(tok::semicolon)) {
    diagnose(PreviousLoc, diag::DiagID::expected_semicolon)
      .fixItAfter(";", PreviousLoc);
    return nullptr;
  }
  return E;
}

/// BreakStmt ::=
///     break ';'
Stmt *Parser::parseBreakStmt() {
  // Validate `break` keyword
  assert(Tok.is(tok::kwBreak) && "Invalid parse method.");
  auto T = Tok.getText();
  auto S = consumeToken();
  auto E = llvm::SMLoc::getFromPointer(T.data() + T.size());

  if (!consumeIf(tok::semicolon)) {
    diagnose(PreviousLoc, diag::DiagID::expected_semicolon)
      .fixItAfter(";", PreviousLoc);
    return nullptr;
  }
  return make<BreakStmt>(llvm::SMRange{S, E});
}

/// ReturnStmt ::=
///     return Expr ';'
Stmt *Parser::parseReturnStmt() {
  // Validate `return` keyword
  assert(Tok.is(tok::kwReturn) && "Invalid parse method.");
  auto RL = consumeToken();
  Expr *E = nullptr;
  
  switch (Tok.getKind()) {
  case tok::identifier:
  case tok::number_literal:
  case tok::l_paren:
  case tok::minus:
    E = parseExpr();
    break;
  default:
    break;
  }

  if (!consumeIf(tok::semicolon)) {
    diagnose(PreviousLoc, diag::DiagID::expected_semicolon)
    .fixItAfter(";", PreviousLoc);
    return nullptr;
  }
  return make<ReturnStmt>(RL, E);
}

/// SubscriptionPattern ::=
///     [ Expr ]
Stmt *Parser::parseSubscriptStmt() {
  // Validate `[` start.
  assert(Tok.is(tok::l_bracket) && "Invalid parse method.");
  
  auto L = consumeToken();
  auto V = parseExpr();
  if (!consumeIf(tok::r_bracket)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_r_bracket)
    .fixItAfter("]", PreviousLoc);
    return nullptr;
  }
  
  return make<SubscriptStmt>(V, L, PreviousLoc);
}

/// Block ::=
///     '{' BlockBody '}'
Stmt *Parser::parseBlock() {
  // Validate `l_brace` token
  assert(Tok.is(tok::l_brace) && "Invalid parse method.");
  auto L = consumeToken();

  // Gather code block body
  std::vector<ASTNode *> Nodes;
  while (auto Node = parseBlockBody())
    Nodes.push_back(Node);

  if (!consumeIf(tok::r_brace)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_r_brace)
      .fixItBefore("}", Tok.getLoc());
    return nullptr;
  }
  return make<BlockStmt>(L, PreviousLoc, std::move(Nodes));
}

ASTNode *Parser::parseBlockBody() {
  while (true) {

    switch (Tok.getKind()) {
    // End of the code block;
    case tok::r_brace:
      return nullptr;
    // Empty expression
    case tok::semicolon:
      consumeToken();
      break;

    case tok::kwVar:
      return parseVarDecl();
        
    case tok::kwLet:
      return parseConstDecl();

    case tok::kwBreak:
      return parseBreakStmt();
    case tok::kwReturn:
      return parseReturnStmt();

    case tok::identifier:
    case tok::number_literal:
    case tok::l_paren:
      return parseExprStmt();

    case tok::kwFor:
      return parseForStmt();
    case tok::kwIf:
      return parseIfStmt();
    case tok::kwWhile:
      return parseWhileStmt();

    default:
      return nullptr;
    }
  }
}

Stmt *Parser::parseFuncStmt() {
  // Validate `func` keyword
  assert(Tok.is(tok::kwFunc) && "Invalid parse method");
  auto D = parseFuncDecl();
  if (Tok.is(tok::l_brace))
    return make<FuncStmt>(D, parseBlock());
  diagnose(Tok.getLoc(), diag::DiagID::expected_l_brace)
    .fixIt("{", Tok.getLoc());
  
  return nullptr;
}

Stmt *Parser::parseForStmt() {
  // Validate `for` keyword.
  assert(Tok.is(tok::kwFor) && "Invalid parse method");
  auto FLoc = consumeToken();
  if (!Tok.is(tok::identifier)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_identifier);
    return nullptr;
  }

  auto Var = parseIdentifierExpr();
  if (!consumeIf(tok::kwIn)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_in_kw)
      .fixItBefore("in", Tok.getLoc());
    return nullptr;
  }
  
  auto Rng = parseRangeStmt();
  if (!Tok.is(tok::l_brace)) {
    diagnose(Tok.getLoc());
    return nullptr;
  }
  auto Body = parseBlock();
  return make<ForStmt>(FLoc, Var, Rng, Body);
}

Stmt *Parser::parseRangeStmt() {
  auto S = parseExpr();
  auto Op = Tok;
  if (!Tok.isAny(tok::elipsis_incl, tok::elipsis_excl)) {
    diagnose(PreviousLoc, diag::DiagID::expected_ellipsis)
     .fixItAfter("..", PreviousLoc)
     .fixItAfter("...", PreviousLoc);
    return nullptr;
  }
  consumeToken();
  auto E = parseExpr();
  return make<RangeStmt>(S, E, Op);
}

Stmt *Parser::parseWhileStmt() {
  assert(Tok.is(tok::kwWhile) && "Invalid parse method.");
  auto L = consumeToken();

  auto Cond = parseExpr();
  auto Body = parseBlock();
  return make<WhileStmt>(L, Cond, Body);
}

Stmt *Parser::parseIfStmt() {
  assert(Tok.is(tok::kwIf) && "Invalid parse method.");
  auto L = consumeToken();
  auto Cond = parseExpr();
  auto Then = parseBlock();
  auto Else = parseElseStmt();
  return make<IfStmt>(L, Cond, Then, Else);
}

Stmt *Parser::parseElseStmt() {
  if (!consumeIf(tok::kwElse))
    return nullptr;
  return parseBlock();
}
