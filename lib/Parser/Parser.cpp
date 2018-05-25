
//===--- Parser.cpp - Dusk language parser implementation -----------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Parse/Parser.h"
#include "dusk/Basic/SourceManager.h"
#include "llvm/ADT/SmallVector.h"
#include <vector>

using namespace dusk;

// MARK: - Parser

Parser::Parser(ASTContext &C, llvm::SourceMgr &SM, SourceFile &SF,
               DiagnosticEngine &Diag, unsigned BufferID)
    : Context(C), SourceManager(SM), SF(SF), Diag(Diag),
      L(new Lexer(SM, BufferID, &Diag)) {}

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
  if (diag::DiagID::unexpected_token == ID && Context.isError())
    // No better diagnostics than already given.
    return DiagnosticRef();
  Context.setError();
  if (Tok.is(tok::unknown))
    // Handeled by lexer.
    return DiagnosticRef();
  return Diag.diagnose(Loc, ID);
}

// MARK: - Main parsing loop

ModuleDecl *Parser::parseModule() {
  std::vector<ASTNode *> Nodes;
  consumeToken();
  while (Tok.isNot(tok::eof) && !Context.isError())
    Nodes.push_back(parse());

  return new (Context) ModuleDecl(SF.file(), std::move(Nodes));
}

ASTNode *Parser::parse() {
  switch (Tok.getKind()) {
#define DECL_KEYWORD(KW) case tok::kw_##KW:
#include "dusk/Basic/TokenDefinitions.def"
    return parseDecl();

#define STMT_KEYWORD(KW) case tok::kw_##KW:
#include "dusk/Basic/TokenDefinitions.def"
  case tok::identifier:
  case tok::number_literal:
  case tok::l_paren:
  case tok::semi:
    return parseStatement();

  case tok::eof:
  case tok::r_brace:
    return nullptr;

  default:
    diagnose(consumeToken());
    return nullptr;
  }
}
