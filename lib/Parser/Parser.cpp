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

Parser::Parser(llvm::SourceMgr &SM, InputFile &SF, unsigned BufferID)
: SourceManager(SM), SourceFile(SF), L(new Lexer(SM, BufferID))
{}

const Token &Parser::peekToken() const {
    return L->peekNextToken();
}

llvm::SMLoc Parser::consumeToken() {
    PreviousLoc = Tok.getLoc();
    assert(Tok.isNot(tok::eof) && "Lexing past EOF");
    
    L->lex(Tok);
    return PreviousLoc;
}

// MARK: - Main parsing loop

ASTNode *Parser::parse() {
    std::vector<ASTNode *> Nodes;
    consumeToken();
    while (Tok.isNot(tok::eof))
        Nodes.push_back(parseGlobal());
    if (Nodes.size())
        return new ModuleDecl(SourceFile.file(), std::move(std::move(Nodes)));
    return nullptr;
}

ASTNode *Parser::parseGlobal() {
    switch (Tok.getKind()) {
    case tok::kwVar:
    case tok::kwConst:
        return parseGlobalValDecl();
        
    case tok::kwFunc:
        return parseFuncStmt();
        
    default:
        llvm_unreachable("Unexpected token.");
    }
}

