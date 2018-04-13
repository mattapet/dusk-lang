//===--- Lexer.h - Dusk language lexer --------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a mila-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//
//
// This file defines lexer interface.
//
//===----------------------------------------------------------------------===//

#include "dusk/Parser/Token.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/SourceMgr.h"

#ifndef Lexer_h
#define Lexer_h

namespace dusk {

/// Mila language lexer.
class Lexer {
    const llvm::SourceMgr &SourceManager;
    
    /// Pointer to the start of the buffer.
    const char *BufferStart;
    
    /// Pointer to one past character of the buffer. This character will always
    /// be a NUL terminator, since the buffer is NUL-terminated.
    const char *BufferEnd;
    
    /// Pointer to the currently consumed character
    const char *CurPtr;
    
    /// Next lexed token
    Token NextToken;
    
    // Option to keep comments as tokens
    bool KeepComments;

public:
    // MARK: - Public interface
    
    /// Create default lexer that scans the whole buffer.
    Lexer(const llvm::SourceMgr &SM, unsigned BufferID,
          bool KeepComments = false);
    
    Lexer(const Lexer &other) = delete;
    void operator=(const Lexer &other) = delete;

    /// Returns a next token needed to parse a LL(1) grammar.
    const Token &peekNextToken() const { return NextToken; }

    /// \brief Lex a token.
    ///
    /// \param Ret - A reference to a token, which should be set to the result
    ///   next lexed token.
    void lex(Token &Ret);
    
    void diagnose(const Token &T, const char *TokStart = "Undefined symbol",
                  llvm::SourceMgr::DiagKind Kind
                  = llvm::SourceMgr::DiagKind::DK_Error);
    
    // MARK: - Static interface
    
    /// \brief Determins if the given string is a valid non-keyword identifier.
    ///  In case if match with a keyword, the appropriate token type
    ///  is returned.
    ///
    /// \return \c tok::identifier, if the string does not match any keyword,
    ///   otherwise approriate \c tok::... token type.
    static tok kindOfIdentifier(llvm::StringRef Str);
    
private: // MARK: - Private interface
    
    void skipToEndOfLine(bool ConsumeNewline);
    void skipLineComment(bool ConsumeNewLine);
    void skipMultilineComment();
    
    /// Updates \c NextToken property with the new token.
    void formToken(tok Kind, const char *TokStart);
    
    /// Main lexing loop method.
    void lexToken();
    
    // MARK: - Helper lexing methods
    
    void lexElipsis();
    void lexIdentifier();
    void lexNumber();
    void lexHexNumber();
    void lexOctNumber();
    void lexDecNumber();
};

} /* namespace mila */

#endif /* Lexer_h */
