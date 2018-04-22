//===--- Parser.cpp - Dusk language parser implementation -----------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Parse/Parser.h"
#include "dusk/Parse/ParserResult.h"
#include "dusk/Basic/SourceManager.h"
#include "llvm/ADT/SmallVector.h"
#include <vector>

using namespace dusk;

// MARK: - Parser

Parser::Parser(llvm::SourceMgr &SM, InputFile &SF, DiagnosticEngine &Engine,
               unsigned BufferID)
    : SourceManager(SM), SourceFile(SF), Engine(Engine),
      L(new Lexer(SM, BufferID, &Engine)) {}

Parser::~Parser() { delete L; }

const Token &Parser::peekToken() const { return L->peekNextToken(); }

SMLoc Parser::consumeToken() {
  PreviousLoc = Tok.getLoc();
  assert(Tok.isNot(tok::eof) && "Lexing past EOF.");

  L->lex(Tok);
  return PreviousLoc;
}

SMLoc Parser::consumeToken(tok T) {
  assert(Tok.is(T) && "Consumption of invalid token kind.");
  return consumeToken();
}

DiagnosticRef Parser::diagnose(SMLoc Loc, diag::DiagID ID) {
  if (diag::DiagID::unexpected_token == ID && R.isError())
    // No better diagnostics than already given.
    return DiagnosticRef();
  R.setError();
  if (Tok.is(tok::unknown))
    // Handeled by lexer.
    return DiagnosticRef();
  return Engine.diagnose(Loc, ID);
}

// MARK: - Main parsing loop

ParserResult &&Parser::parse() {
  std::vector<ASTNode *> Nodes;
  consumeToken();
  while (Tok.isNot(tok::eof))
    Nodes.push_back(parseGlobal());

  if (Nodes.size() != 0)
    R.setRoot(make<ModuleDecl>(SourceFile.file(), std::move(Nodes)));
  return std::move(R);
}

ASTNode *Parser::parseGlobal() {
  switch (Tok.getKind()) {
  case tok::kwVar:
    return parseVarDecl();
  case tok::kwConst:
    return parseConstDecl();
  case tok::kwFunc:
    return parseFuncStmt();
  case tok::kwFor:
    return parseForStmt();
  case tok::kwWhile:
    return parseWhileStmt();
  case tok::kwIf:
    return parseIfStmt();

  case tok::identifier:
  case tok::number_literal:
  case tok::l_paren:
    return parseExprStmt();

  default:
    diagnose(consumeToken());
    return nullptr;
  }
}
