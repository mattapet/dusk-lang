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
    return nullptr;
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
    return new BlockStmt(L, R, std::move(Nodes));
}

ASTNode *Parser::parseBlockBody() {
    switch (Tok.getKind()) {
    // End of the code block;
    case tok::r_brace:
        return nullptr;
        
    case tok::kwVar:
        return parseVarDecl();
    
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
        llvm_unreachable("Unexpexted token");
    }
}

Expr *Parser::parseExprStmt() {
    Expr *E;
    switch (Tok.getKind()) {
        case tok::identifier:
        case tok::number_literal:
        case tok::l_paren:
            E = parseExpr();
            break;
            
        default:
            llvm_unreachable("Unexpected token.");
    }
    if (!consumeIf(tok::semicolon))
        assert("Missing semicolon at the end of the line" && false);
    return E;
}

FuncStmt *Parser::parseFuncStmt() {
    // Validate `func` keyword
    assert(Tok.is(tok::kwFunc) && "Invalid parse method");
    
    return new FuncStmt(parseFuncDecl(), parseBlock());
}

ForStmt *Parser::parseForStmt() {
    // Validate `for` keyword.
    assert(Tok.is(tok::kwFor) && "Invalid parse method");
    auto FLoc = consumeToken();
    if (!Tok.is(tok::identifier))
        assert("Expected identifier" && false);
    
    auto Var = parseIdentifierExpr();
    if (!consumeIf(tok::kwIn))
        assert("Expected `in` keyword" && false);
    
    auto Rng = parseRangeStmt();
    auto Body = parseBlock();
    return new ForStmt(FLoc, Var, Rng, Body);
}

RangeStmt *Parser::parseRangeStmt() {
    auto S = parseExpr();
    auto Op = Tok;
    if (!Tok.isAny(tok::elipsis_incl, tok::elipsis_excl))
        assert("Expectec ellipsis operator" && false);
    consumeToken();
    auto E = parseExpr();
    return new RangeStmt(S, E, Op);
}

WhileStmt *Parser::parseWhileStmt() {
    assert(Tok.is(tok::kwWhile) && "Invalid parse method.");
    auto L = consumeToken();
    
    auto Cond = parseExpr();
    auto Body = parseBlock();
    return new WhileStmt(L, Cond, Body);
}

IfStmt *Parser::parseIfStmt() {
    assert(Tok.is(tok::kwIf) && "Invalid parse method.");
    auto L = consumeToken();
    auto Cond = parseExpr();
    auto Then = parseBlock();
    auto Else = parseElseStmt();
    return new IfStmt(L, Cond, Then, Else);
}

BlockStmt *Parser::parseElseStmt() {
    if (!consumeIf(tok::kwElse))
        return nullptr;
    return parseBlock();
}

