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

/// TopExpr ::=
///     identifier TopExpr_ ';'
///     number_literal Expr_ ';'
Expr *Parser::parseTopExpr() {
    Expr *R;
    switch (Tok.getKind()) {
    case tok::identifier:
        R = parseTopExpr_();
        break;
    
    case tok::number_literal:
        R = parseExpr();
        break;
        
    default:
        llvm_unreachable("Unexpected symbol.");
    }
    
    // End of top expression
    if (!consumeIf(tok::semicolon))
        assert("Missing `;`" && false);
    return R;
}

/// TopExpr_ ::=
///     '=' Expr
///     '(' Args ')'
///      epsilon
Expr *Parser::parseTopExpr_() {
    assert(Tok.is(tok::identifier) && "Invalid parse method.");
    auto V = Tok;
    consumeToken();
    
    switch (Tok.getKind()) {
    case tok::semicolon:
        return nullptr;
            
    case tok::assign:
        consumeToken();
        return new AssignExpr(new VariableExpr(V.getText(), V.getLoc()), parseExpr());
            
    case tok::l_paren:
        return new FuncCall(V.getText(), V.getLoc(), parseParamList());
        
    default:
        llvm_unreachable("Unexpected token.");
    }
}

Expr *Parser::parseExpr() {
    Expr *LHS;
    switch (Tok.getKind()) {
    case tok::identifier:
        LHS = parseIDExpr();
        return parseExpr_(LHS);
        
    case tok::number_literal:
        LHS = parseExprNumberLiteral();
        return parseExpr_(LHS);
    case tok::l_paren:
        consumeToken();
        LHS = parseExpr();
        if (!consumeIf(tok::r_paren))
            assert("Missing `)`" && false);
        return LHS;
        
    default:
        llvm_unreachable("Unexpected token.");
    }
}

Expr *Parser::parseExpr_(Expr *LHS) {
    if (Tok.isOperator()) {
        auto Op = Tok;
        consumeToken();
        return new InfixExpr(LHS, parseExpr(), Op);
    }
    
    switch (Tok.getKind()) {
    case tok::r_paren:
    case tok::l_brace:
    case tok::semicolon:
        return LHS;
        
    default:
        llvm_unreachable("Unexpected token.");
    }
}

Expr *Parser::parseIDExpr() {
    assert(Tok.is(tok::identifier) && "Invalid parse method.");
    auto ID = Tok;
    consumeToken();
    if (Tok.isOperator())
        return parseExprIdentifier();
    
    switch (Tok.getKind()) {
    case tok::r_paren:
    case tok::l_brace:
    case tok::semicolon:
        return parseExprIdentifier();
    
    case tok::l_paren:
        return new FuncCall(ID.getText(), ID.getLoc(), parseParamList());
        
    default:
        llvm_unreachable("Unexpected token.");
    }
}

ASTNode *Parser::parseFuncCall() {
    // Validate `identifier` token
    assert(Tok.is(tok::identifier) && "Invalid parse methods.");
    
    auto FN = Tok;
    consumeToken();
    
    return new FuncCall(FN.getText(), FN.getLoc(), parseParamList());
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
