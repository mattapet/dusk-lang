//===--- Parser.cpp - Dusk language parser implementation -----------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Parse/Parser.h"
#include <vector>

using namespace dusk;
using namespace diag;

Parser::Parser(llvm::SourceMgr &SM,
               DiagnosticEngine &DE,
               InputFile &SF,
               unsigned BufferID)
: SourceManager(SM), DiagEngine(DE), SourceFile(SF),
  L(new Lexer(SM, BufferID)), R(new ParserResult())
{}

Parser::~Parser() {
    delete L;
    delete R;
}

const Token &Parser::peekToken() const {
    return L->peekNextToken();
}

llvm::SMLoc Parser::consumeToken() {
    PreviousLoc = Tok.getLoc();
    assert(Tok.isNot(tok::eof) && "Lexing past EOF");

    L->lex(Tok);
    return PreviousLoc;
}

// MARK: - Diagnostics

Diagnostic Parser::diagnose() {
    return Diagnostic();
}

// MARK: - Main parsing loop

ASTNode *Parser::parse() {
    std::vector<ASTNode *> Nodes;
    consumeToken();
    while (Tok.isNot(tok::eof))
        Nodes.push_back(parseGlobal());
    if (Nodes.size())
        return make<ModuleDecl>(SourceFile.file(), std::move(Nodes));
    return nullptr;
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
        return parseExpr();

    default:
        llvm_unreachable("Unexpected token.");
    }
}

