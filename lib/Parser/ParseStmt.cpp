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
    throw ParseError(diag::ParserError::unexpected_token);
  }
  if (!consumeIf(tok::semicolon))
    throw ParseError(diag::ParserError::missing_semicolon);
  return E;
}

/// BreakStmt ::=
///     break ';'
BreakStmt *Parser::parseBreakStmt() {
  // Validate `break` keyword
  assert(Tok.is(tok::kwBreak) && "Invalid parse method.");
  auto T = Tok.getText();
  auto S = consumeToken();
  auto E = llvm::SMLoc::getFromPointer(T.data() + T.size());

  if (!consumeIf(tok::semicolon))
    throw ParseError(diag::ParserError::missing_semicolon);
  return make<BreakStmt>(llvm::SMRange{S, E});
}

/// ReturnStmt ::=
///     return Expr ';'
ReturnStmt *Parser::parseReturnStmt() {
  // Validate `return` keyword
  assert(Tok.is(tok::kwReturn) && "Invalid parse method.");
  auto RL = consumeToken();
  auto E = parseExpr();

  if (!consumeIf(tok::semicolon))
    throw ParseError(diag::ParserError::missing_semicolon);
  return make<ReturnStmt>(RL, E);
}

/// Block ::=
///     '{' BlockBody '}'
BlockStmt *Parser::parseBlock() {
  // Validate `l_brace` token
  assert(Tok.is(tok::l_brace) && "Invalid parse method.");
  auto L = consumeToken();

  // Gather code block body
  std::vector<ASTNode *> Nodes;
  while (auto Node = parseBlockBody())
    Nodes.push_back(Node);

  auto R = consumeToken();
  return make<BlockStmt>(L, R, std::move(Nodes));
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
      throw ParseError(diag::ParserError::missing_r_brace);
    }
  }
}

FuncStmt *Parser::parseFuncStmt() {
  // Validate `func` keyword
  assert(Tok.is(tok::kwFunc) && "Invalid parse method");

  return make<FuncStmt>(parseFuncDecl(), parseBlock());
}

ForStmt *Parser::parseForStmt() {
  // Validate `for` keyword.
  assert(Tok.is(tok::kwFor) && "Invalid parse method");
  auto FLoc = consumeToken();
  if (!Tok.is(tok::identifier))
    throw ParseError(diag::ParserError::missing_identfier);

  auto Var = parseIdentifierExpr();
  if (!consumeIf(tok::kwIn))
    throw ParseError(diag::ParserError::missing_in_kw);

  auto Rng = parseRangeStmt();
  auto Body = parseBlock();
  return make<ForStmt>(FLoc, Var, Rng, Body);
}

RangeStmt *Parser::parseRangeStmt() {
  auto S = parseExpr();
  auto Op = Tok;
  if (!Tok.isAny(tok::elipsis_incl, tok::elipsis_excl))
    throw ParseError(diag::ParserError::missing_ellipsis_op);
  consumeToken();
  auto E = parseExpr();
  return make<RangeStmt>(S, E, Op);
}

WhileStmt *Parser::parseWhileStmt() {
  assert(Tok.is(tok::kwWhile) && "Invalid parse method.");
  auto L = consumeToken();

  auto Cond = parseExpr();
  auto Body = parseBlock();
  return make<WhileStmt>(L, Cond, Body);
}

IfStmt *Parser::parseIfStmt() {
  assert(Tok.is(tok::kwIf) && "Invalid parse method.");
  auto L = consumeToken();
  auto Cond = parseExpr();
  auto Then = parseBlock();
  auto Else = parseElseStmt();
  return make<IfStmt>(L, Cond, Then, Else);
}

BlockStmt *Parser::parseElseStmt() {
  if (!consumeIf(tok::kwElse))
    return nullptr;
  return parseBlock();
}
