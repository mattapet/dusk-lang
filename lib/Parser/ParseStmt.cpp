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

ASTNode *Parser::parseStatement() {
  while (true) {
    switch (Tok.getKind()) {
    // End of the code block;
    case tok::r_brace:
      return nullptr;

    // Empty statement
    case tok::semi:
      consumeToken();
      break;

    case tok::identifier:
    case tok::number_literal:
    case tok::l_bracket:
    case tok::l_paren:
      return parseExprStmt();

    case tok::kw_break:
      return parseBreakStmt();

    case tok::kw_return:
      return parseReturnStmt();

    case tok::kw_func:
      return parseFuncStmt();

    case tok::kw_for:
      return parseForStmt();

    case tok::kw_if:
      return parseIfStmt();

    case tok::kw_while:
      return parseWhileStmt();

    default:
      return nullptr;
    }
  }
}

/// ExprStmt ::=
///     Expr ';'
Expr *Parser::parseExprStmt() {
  Expr *E;
  switch (Tok.getKind()) {
  case tok::identifier:
  case tok::number_literal:
  case tok::l_paren:
  case tok::l_bracket:
    E = parseExpr();
    break;
  default:
    diagnose(Tok.getLoc());
    return nullptr;
  }
  if (!consumeIf(tok::semi)) {
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
  assert(Tok.is(tok::kw_break) && "Invalid parse method.");
  auto T = Tok.getText();
  auto S = consumeToken();
  auto E = llvm::SMLoc::getFromPointer(T.data() + T.size());

  if (!consumeIf(tok::semi)) {
    diagnose(PreviousLoc, diag::DiagID::expected_semicolon)
        .fixItAfter(";", PreviousLoc);
    return nullptr;
  }
  return new (Context) BreakStmt(llvm::SMRange{S, E});
}

/// ReturnStmt ::=
///     return Expr ';'
Stmt *Parser::parseReturnStmt() {
  // Validate `return` keyword
  assert(Tok.is(tok::kw_return) && "Invalid parse method.");
  auto RL = consumeToken();
  Expr *E = nullptr;

  switch (Tok.getKind()) {
  case tok::identifier:
  case tok::number_literal:
  case tok::l_paren:
  case tok::minus:
  case tok::lnot:
  case tok::l_bracket:
    E = parseExpr();
    break;

  case tok::semi:
    break;

  default:
    if (Tok.isNot(tok::eof))
      consumeToken();
    diagnose(PreviousLoc);
    break;
  }

  if (!consumeIf(tok::semi)) {
    diagnose(PreviousLoc, diag::expected_semicolon)
        .fixItAfter(";", PreviousLoc);
    return nullptr;
  }
  return new (Context) ReturnStmt(RL, E);
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

  return new (Context) SubscriptStmt(V, L, PreviousLoc);
}

/// Block ::=
///     '{' BlockBody '}'
Stmt *Parser::parseBlock() {
  // Validate `l_brace` token
  assert(Tok.is(tok::l_brace) && "Invalid parse method.");
  auto L = consumeToken();

  // Gather code block body
  std::vector<ASTNode *> Nodes;
  while (auto Node = parse())
    Nodes.push_back(Node);

  if (!consumeIf(tok::r_brace)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_r_brace)
        .fixItBefore("}", Tok.getLoc());
    return nullptr;
  }
  return new (Context) BlockStmt(L, PreviousLoc, std::move(Nodes));
}

ASTNode *Parser::parseBlockBody() { return nullptr; }

/// ExternStmt ::=
///     'extern' 'func' indentifier
Stmt *Parser::parseExterStmt() {
  // Validate `exter` keyword
  assert(Tok.is(tok::kw_extern) && "Invalid parse method");
  auto EL = consumeToken();
  if (Tok.isNot(tok::kw_func)) {
    diagnose(Tok.getLoc(), diag::DiagID::expected_func_kw)
        .fixItBefore("func", Tok.getLoc());
    return nullptr;
  }

  auto D = new (Context) ExternStmt(EL, parseFuncDecl());
  if (consumeIf(tok::semi))
    return D;

  diagnose(Tok.getLoc(), diag::DiagID::expected_semicolon)
      .fixItBefore(";", Tok.getLoc());
  return nullptr;
}

/// FuncStmt ::=
///     'func' identifier '(' Args ')' RetType Block
Stmt *Parser::parseFuncStmt() {
  // Validate `func` keyword
  assert(Tok.is(tok::kw_func) && "Invalid parse method");
  auto D = parseFuncDecl();
  if (Tok.is(tok::l_brace))
    return new (Context) FuncStmt(D, parseBlock());

  diagnose(Tok.getLoc(), diag::DiagID::expected_l_brace)
      .fixIt("{", Tok.getLoc());
  return nullptr;
}

/// ForStmt ::=
///     'for' identifier 'in' Expr ('..' | '...') Expr Block
Stmt *Parser::parseForStmt() {
  // Validate `for` keyword.
  assert(Tok.is(tok::kw_for) && "Invalid parse method");
  auto FLoc = consumeToken();
  auto Ident = Tok;
  if (!consumeIf(tok::identifier)) {
    diagnose(Tok.getLoc(), diag::expected_identifier);
    return nullptr;
  }

  auto Spec = ValDecl::Specifier::Let;
  auto Var = new (Context) ParamDecl(Spec, Ident.getText(), Ident.getLoc());
  if (!consumeIf(tok::kw_in)) {
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
  return new (Context) ForStmt(FLoc, Var, Rng, Body);
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
  return new (Context) RangeStmt(S, E, Op);
}

Stmt *Parser::parseWhileStmt() {
  assert(Tok.is(tok::kw_while) && "Invalid parse method.");
  auto L = consumeToken();

  auto Cond = parseExpr();
  auto Body = parseBlock();
  return new (Context) WhileStmt(L, Cond, Body);
}

Stmt *Parser::parseIfStmt() {
  assert(Tok.is(tok::kw_if) && "Invalid parse method.");
  auto L = consumeToken();
  auto Cond = parseExpr();
  auto Then = parseBlock();
  auto Else = parseElseStmt();
  return new (Context) IfStmt(L, Cond, Then, Else);
}

Stmt *Parser::parseElseStmt() {
  if (!consumeIf(tok::kw_else))
    return nullptr;
  return parseBlock();
}
