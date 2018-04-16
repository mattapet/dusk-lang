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

CodeBlock *Parser::parseCodeBlock() {
    // Validate `l_brace` token
    assert(Tok.is(tok::l_brace) && "Invalid parse method.");
    auto S = consumeToken();
    
    // Gather code block body
    std::vector<ASTNode *> Nodes;
    while (auto Node = parseCodeBlock_())
        Nodes.push_back(Node);
    
    auto E = Tok.getLoc();
    
    if (!consumeIf(tok::r_brace))
        assert("Expected closing brace" && false);
    return new CodeBlock(S, E, std::move(Nodes));
}

FuncStmt *Parser::parseFuncStmt() {
    // Validate `func` keyword
    assert(Tok.is(tok::kwFunc) && "Invalid parse method");
    
    return new FuncStmt(parseFuncDecl(), parseCodeBlock());
}

ASTNode *Parser::parseCodeBlock_() {
    switch (Tok.getKind()) {
    // End of the code block;
    case tok::r_brace:
        return nullptr;
        
    case tok::kwVar:
        return parseVarDecl();
    
    case tok::identifier:
    case tok::number_literal:
    case tok::l_paren:
        return parseTopExpr();
 
    case tok::kwFor:
    case tok::kwIf:
    case tok::kwWhile:
        return parseStatement();
        
    default:
        llvm_unreachable("Unexpexted token");
    }
}

ASTNode *Parser::parseForStmt() {
    return nullptr;
}

ASTNode *Parser::parseWhileStmt() {
    return nullptr;
}

ASTNode *Parser::parseIfStmt() {
    return nullptr;
}

ASTNode *Parser::parseElseStmt() {
    return nullptr;
}

ParamList *Parser::parseParamList() {
    // Validate `l_paren` token.
    assert(Tok.is(tok::l_paren) && "Invalid parse method.");
    
    auto S = consumeToken();
    llvm::SmallVector<ParamDecl *, 128> Params;
    
    // Consume parameter list
    while (auto P = parseParamList_()) {
        Params.push_back(P);
        consumeIf(tok::colon);
    }
    
    auto E = Tok.getLoc();
    if (!consumeIf(tok::r_paren))
        assert("Expected ) at the end of the parameter list." && false);
    
    return new ParamList(S, E, std::move(Params));
}

ParamDecl *Parser::parseParamList_() {
    llvm::StringRef N;
    llvm::SMLoc L;
    
    switch (Tok.getKind()) {
    case tok::r_paren:
        /// End of the list
        return nullptr;
        
    case tok::identifier:
        N = Tok.getText();
        L = consumeToken();
        return new ParamDecl(N, L);
        
    default:
        llvm_unreachable("Unexpected token.");
    }
}
