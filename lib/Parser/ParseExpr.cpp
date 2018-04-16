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
    return nullptr;
}

ASTNode *Parser::parseExpr_() {
    return nullptr;
}

ASTNode *Parser::parseFuncCall() {
    return nullptr;
}

VariableExpr *Parser::parseExprIdentifier() {
    // Validate `identifier` token
    assert(Tok.is(tok::identifier) && "Invalid token.");
    auto N   = Tok.getText();
    auto Loc = consumeToken();
    
    return new VariableExpr(N, Loc);
}

NumberLiteralExpr *Parser::parseExprNumberLiteral() {
    // Validate `numeber_literal` token
    assert(Tok.is(tok::number_literal) && "Invalid token.");
    
    auto Txt = Tok.getText();
    auto S = consumeToken();
    auto E = llvm::SMLoc::getFromPointer(Txt.data() + Txt.size());
    
    int R;
    Txt.getAsInteger(0, R);
    return new NumberLiteralExpr(R, { S, E });
}
