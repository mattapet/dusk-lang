//===--- Lexer.h - Dusk language lexer interface ---------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//
//
// This file defines lexer interface.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_LEXER_H
#define DUSK_LEXER_H

#include "dusk/AST/Diagnostics.h"
#include "dusk/Parse/Token.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/SourceMgr.h"

namespace dusk {

/// Dusk language lexer.
class Lexer {
    const llvm::SourceMgr &SourceManager;

    diag::Diagnostics *Diag;

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

private:
    Lexer(const Lexer &other) = delete;
    void operator=(const Lexer &other) = delete;

public:
    // MARK: - Public interface

    /// Create default lexer that scans the whole buffer.
    Lexer(const llvm::SourceMgr &SM,
          unsigned BufferID,
          diag::Diagnostics *Diag = nullptr,
          bool KeepComments = false);

    /// Returns a next token needed to parse a LL(1) grammar.
    const Token &peekNextToken() const { return NextToken; }

    /// \brief Lex a token.
    ///
    /// \param Ret - A reference to a token, which should be set to the result
    ///   next lexed token.
    void lex(Token &Ret);

    /// Sets the state (position) of the lexer. The position can be either
    /// a location before or after the current location.
    void setState(llvm::SMLoc Loc) {
        assert(Loc.isValid());
        CurPtr = Loc.getPointer();
        lexToken();
    }

    // MARK: - Diagnostics methods.

    void diagnose(diag::LexerError E = diag::LexerError::unexpected_symbol);
    void diagnose(Token T, diag::LexerError E);

    // MARK: - Static interface

    /// \brief Determins if the given string is a valid non-keyword identifier.
    ///  In case if match with a keyword, the appropriate token type
    ///  is returned.
    ///
    /// \return \c tok::identifier, if the string does not match any keyword,
    ///   otherwise approriate \c tok::... token type.
    static tok kindOfIdentifier(llvm::StringRef Str);

    /// Returns a location for given \c Ptr.
    static llvm::SMLoc getSourceLoc(const char *Ptr) {
        return llvm::SMLoc::getFromPointer(Ptr);
    }

    /// \brief Retrieve a Token, which starts at location \c Loc.
    ///
    /// \param SM A \c SourceMgr instance, which provides the buffer context.
    ///
    /// \param Loc The source location at which the token starts. The location
    ///  must be from provided source manager.
    static Token
    getTokenAtLocation(const llvm::SourceMgr &SM, llvm::SMLoc Loc);

    /// \brief Retrieve a location that points one character pass the end
    ///  of the Token referenced by the \c Loc.
    ///
    /// \param SM A \c SourceMgr instance, which provides the buffer context.
    ///
    /// \param Loc Location of the beginning of the token.
    static llvm::SMLoc
    getLocForEndOfToken(const llvm::SourceMgr &SM, llvm::SMLoc Loc);

    /// Retrieve a location for the start of the line referenced by the \c Loc.
    static llvm::SMLoc
    getLocForStartOfLine(const llvm::SourceMgr &SM, llvm::SMLoc Loc);

    /// Retrieve a location for end of line (start of next line) referenced
    /// by the \c Loc.
    static llvm::SMLoc
    getLocForEndOfLine(const llvm::SourceMgr &SM, llvm::SMLoc Loc);

    /// Retrive a line in the source code referenced by the \c Loc.
    static llvm::StringRef
    getLineForLoc(const llvm::SourceMgr &SM, llvm::SMLoc Loc);


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
    void lexBinNumber();
    void lexOctNumber();
    void lexDecNumber();
};

} // namespace dusk

#endif /* DUSK_LEXER_H */
