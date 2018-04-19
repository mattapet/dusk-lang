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
#include "llvm/ADT/SmallVector.h"
#include <vector>

using namespace dusk;
using namespace diag;

// MARK: - Parsing error

ParseError::ParseError(ParserError K)
: std::runtime_error("Parsing error"), Kind(K)
{}

// MARK: - Parser

Parser::Parser(llvm::SourceMgr &SM,
               InputFile &SF,
               diag::Diagnostics &Diag,
               unsigned BufferID)
: SourceManager(SM), SourceFile(SF), Diag(Diag),
  L(new Lexer(SM, BufferID, &Diag))
{}

Parser::~Parser() {
    delete L;
}

const Token &Parser::peekToken() const {
    return L->peekNextToken();
}

llvm::SMLoc Parser::consumeToken() {
    PreviousLoc = Tok.getLoc();
    assert(Tok.isNot(tok::eof) && "Lexing past EOF.");

    L->lex(Tok);
    return PreviousLoc;
}

llvm::SMLoc Parser::consumeToken(tok T) {
    assert(Tok.is(T) && "Consumption of invalid token kind.");
    return consumeToken();
}

unsigned Parser::diagnose() {
    // Terminate parsing.
    terminateParsing();
    
    if (Tok.is(tok::unknown))
        // Handeled by lexer.
        return 0;
    return diagnoseUnexpectedToken();
}

// TODO: Somehow remove.
static unsigned getBufferForLoc(const llvm::SourceMgr &SM, llvm::SMLoc Loc) {
    // Validate location
    assert(Loc.isValid());
    
    auto Ptr = Loc.getPointer();
    for (unsigned i = 1; i <= SM.getNumBuffers(); i++) {
        auto Buff = SM.getMemoryBuffer(i);
        if (Ptr >= Buff->getBufferStart() && Ptr <= Buff->getBufferEnd())
            return i;
    }
    llvm_unreachable("Location in non-existing buffer.");
}

unsigned Parser::diagnose(ParserError E) {
    if (Tok.is(tok::unknown))
        // Handeled by lexer.
        return 0;
    
    auto ID = getBufferForLoc(SourceManager, PreviousLoc);
    auto FN = SourceManager.getMemoryBuffer(ID)->getBufferIdentifier();
    auto Loc = Tok.getLoc();
    auto K = llvm::SourceMgr::DiagKind::DK_Error;
    auto Line = Lexer::getLineForLoc(SourceManager, PreviousLoc);
    llvm::StringRef MSG;
    llvm::SmallVector<llvm::SMFixIt, 2> FixIts;
    
    switch (E) {
    case ParserError::unexpected_token:
        MSG = "Unexpected token";
        break;
    case ParserError::missing_semicolon:
        MSG = "Missing semicolon.";
        Loc = Lexer::getLocForEndOfToken(SourceManager, PreviousLoc);
        FixIts.push_back({ Loc, ";" });
        break;
    case ParserError::missing_r_paren:
        MSG = "Missing closing parenthesis ).";
        Loc = Lexer::getLocForEndOfToken(SourceManager, PreviousLoc);
        FixIts.push_back({ Loc, ")" });
        break;
    case ParserError::missing_identfier:
        MSG = "Expected identifier.";
        break;
    case ParserError::missing_assign:
        MSG = "Expected assign operator.";
        Loc = Lexer::getLocForEndOfToken(SourceManager, PreviousLoc);
        FixIts.push_back({ Loc, "=" });
        break;
    case ParserError::missing_in_kw:
        MSG = "Expected `in` keyword.";
        Loc = Lexer::getLocForEndOfToken(SourceManager, PreviousLoc);
        FixIts.push_back({ Loc, "in" });
        break;
    case ParserError::missing_r_brace:
        MSG = "Expected closing brace }.";
        Loc = Lexer::getLocForEndOfToken(SourceManager, PreviousLoc);
        FixIts.push_back({ Loc, "}" });
        break;
    case ParserError::missing_ellipsis_op:
        MSG = "Expected range operator.";
        Loc = Lexer::getLocForEndOfToken(SourceManager, PreviousLoc);
        FixIts.push_back({ Loc, ".." });
        FixIts.push_back({ Loc, "..." });
        break;
    case ParserError::missing_colon:
        MSG = "Expected colon separator.";
        Loc = Lexer::getLocForEndOfToken(SourceManager, PreviousLoc);
        FixIts.push_back({ Loc, "," });
        break;
    case ParserError::missing_r_bracket:
        MSG = "Expected closing bracket ].";
        Loc = Lexer::getLocForEndOfToken(SourceManager, PreviousLoc);
        FixIts.push_back({ Loc, "]" });
        break;
    }
    
    auto [L, C] = SourceManager.getLineAndColumn(Loc);
    auto D = llvm::SMDiagnostic(SourceManager, Loc, FN, L,
                                C, K, MSG, Line, llvm::None, FixIts);
    return Diag.diagnose(std::move(D));
}

unsigned Parser::diagnoseUnexpectedToken() {
    auto ID = getBufferForLoc(SourceManager, Tok.getLoc());
    auto FN = SourceManager.getMemoryBuffer(ID)->getBufferIdentifier();
    auto [L, C] = SourceManager.getLineAndColumn(Tok.getLoc());
    auto K = llvm::SourceMgr::DiagKind::DK_Error;
    auto Line = Tok.getText();
    auto MSG = "Unexpexted token";
    auto D = llvm::SMDiagnostic(SourceManager,
                                Tok.getLoc(),FN, L,
                                C, K, MSG, Line, llvm::None);
    return Diag.diagnose(std::move(D));
}

// MARK: - Main parsing loop

ParserResult &&Parser::parse() {
    std::vector<ASTNode *> Nodes;
    consumeToken();
    try {
        while (Tok.isNot(tok::eof))
            Nodes.push_back(parseGlobal());
    } catch (ParseError &E) {
        diagnose(E.getKind());
    }
    
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
        return parseExpr();

    default:
        throw ParserError();
    }
}

