//===--- ParseDecl.cpp - Dusk language parser for declarations ------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Parse/Parser.h"

using namespace dusk;

/// Parsing top-level symbols
///
/// GlobalScope ::=
///     'var'   identifier '=' ( identifier | number_literal ) ';'
///     'const' identifier '=' ( identifier | number_literal ) ';'
///     'func'  identifier '(' Args ')' CodeBlock
ValDecl *Parser::parseGlobalValDecl() {
    switch (Tok.getKind()) {
    case tok::kwVar:
        return parseGlobalVarDecl();
    case tok::kwConst:
        return parseGlobalConstDecl();
    default:
        llvm_unreachable("Unexpected token.");
    }
    return nullptr;
}

/// Variable declaration
///
/// VarDecl ::=
///     'var' identifier '=' ( identifier | number_literal ) ';'
VarDecl *Parser::parseGlobalVarDecl() {
    // Ensure `var`keyword.
    assert(Tok.is(tok::kwVar) && "Invalid parsing method.");
    
    // Capture `var` keyword location.
    auto VL = consumeToken();
    auto ID = Tok;
    
    if (!consumeIf(tok::identifier))
        assert("Expected identifier" && false);
    
    if (!consumeIf(tok::assign))
        assert("Expected assign" && false);
    
    auto Value = parseGlobalDeclValue();
    
    if (!consumeIf(tok::semicolon))
        assert("Extected semicolon" && false);
    
    return new VarDecl(ID.getText(), ID.getLoc(), VL, Value);
}

/// Constant declaration
///
/// ConstDecl ::=
///     'const' identifier '=' ( identifier | number_literal ) ';'
ConstDecl *Parser::parseGlobalConstDecl() {
    // Ensure `const` keyword
    assert(Tok.is(tok::kwConst) && "Invalid parsing method..");
    
    // Capture `const` keyword location.
    auto CL = consumeToken();
    auto ID = Tok;
    
    if (!consumeIf(tok::identifier))
        assert("Expected identifier" && false);
    
    if (!consumeIf(tok::assign) && false)
        assert("Expected assign" && false);
    
    auto Value = parseGlobalDeclValue();
    
    if (!consumeIf(tok::semicolon))
        assert("Extected semicolon" && false);
    
    return new ConstDecl(ID.getText(), ID.getLoc(), CL, Value);
}

/// Global value
///
/// GlobValue ::=
///     identifier | number_literal
Expr *Parser::parseGlobalDeclValue() {
    switch (Tok.getKind()) {
    case tok::identifier:
        return parseExprIdentifier();
    case tok::number_literal:
        return parseExprNumberLiteral();
    default:
        llvm_unreachable("Unexpected token.");
    }
}


/// Function declaration
///
/// FuncDecl ::=
///     'func' identifier '(' Args ')' CodeBlock
FuncDecl *Parser::parseFuncDecl() {
    // Ensure `func` keyword
    assert(Tok.is(tok::kwFunc) && "Invalid parsing method.");
    
    auto FL = consumeToken();
    
    auto ID = Tok;
    if (!consumeIf(tok::identifier))
        assert("Expected identifier" && false);
    
    return new FuncDecl(ID.getText(), ID.getLoc(), FL, parseParamList());
}

/// Var declaration
///
/// VarDecl ::=
///     'var' identifier '=' Expr ';'
VarDecl *Parser::parseVarDecl() {
    // Validate correct variable decl
    assert(Tok.is(tok::kwVar) && "Invalid parsing method.");
    
    auto L = consumeToken();
    auto ID = Tok;
    if (!consumeIf(tok::identifier))
        assert("Expected identifier" && false);
    if (!consumeIf(tok::assign))
        assert("Expected `=`" && false);
    
    auto V = parseGlobalDeclValue();
//    auto V = parseExpr();
    if (!consumeIf(tok::semicolon))
        assert("Expected `;`.");
    
    return new VarDecl(ID.getText(), ID.getLoc(), L, V);
}
