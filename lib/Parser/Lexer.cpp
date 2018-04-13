//===--- Lexer.cpp --------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a mila-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Parser/Lexer.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/Twine.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/TableGen/Error.h"
#include <cctype>
#include <functional>
#include <iostream>

using namespace dusk;

// MARK: - Validation functions

static bool isValidIdentifierStart(const char *c) {
    return std::isalpha(*c) || *c == '_';
}

static bool isValidIdentifierCont(const char *c) {
    return std::isalnum(*c) || *c == '_';
}

static bool isValidDecDigit(const char *c) {
    return std::isdigit(*c);
}

static bool isValidOctDigit(const char *c) {
    return *c >= '0' && *c <= '7';
}

static bool isValidHexDigit(const char *c) {
    return std::isdigit(*c)
    || (*c >= 'a' && *c <= 'f')
    || (*c >= 'A' && *c <= 'F');
}

// MARK: - Contitional character consumtion functions

/// \brief Consumes a character if provided predicate returns \c true
///
/// \return \c true, if function consumes a character, \c false otherwise.
static bool consumeIf(const char *&ptr,
                      std::function<bool(const char *)> pred)
{
    const char *c = ptr;
    if (pred(c)) {
        ptr++;
        return true;
    }
    return false;
}

static bool consumeIfValidIdentifierStart(const char *&ptr) {
    return consumeIf(ptr, isValidIdentifierStart);
}

static bool consumeIfValidIdentifierCont(const char *&ptr) {
    return consumeIf(ptr, isValidIdentifierCont);
}

static bool consumeIfValidDecNumber(const char *&ptr) {
    return consumeIf(ptr, isValidDecDigit);
}

static bool consumeIfValidOctNumber(const char *&ptr) {
    return consumeIf(ptr, isValidOctDigit);
}

static bool consumeIfValidHexNumber(const char *&ptr) {
    return consumeIf(ptr, isValidHexDigit);
}

// MARK: - Lexer

Lexer::Lexer(const llvm::SourceMgr &SM, unsigned BufferID, bool KeepComments)
: SourceManager(SM), KeepComments(KeepComments) {
    // Extract buffer from source manager
    auto B = SourceManager.getMemoryBuffer(BufferID);

    // Initialize buffer pointers
    BufferStart = B->getBufferStart();
    BufferEnd = B->getBufferEnd();

    CurPtr = BufferStart;
    lexToken();
}

// MARK: - Main lexing loop

void Lexer::lex(Token &Ret) {
    // Assign next token to the reference.
    Ret = NextToken;

    if (Ret.isNot(tok::eof))
        lexToken();
}

void Lexer::lexToken() {
    while (true) {
        const char *TokStart = CurPtr;

        switch (*CurPtr++) {
        case 0:
            // Not ending null character.
            if (CurPtr - 1 != BufferEnd)
                break;
            return formToken(tok::eof, TokStart);

        // Skip whitespace
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            break;

        case '=':
            if (*CurPtr == '=') {
                CurPtr++;
                return formToken(tok::equals, TokStart);
            } else
                return formToken(tok::assign, TokStart);


        case '.':
            if (*CurPtr == '.')
                return lexElipsis();
            else
                return formToken(tok::unknown, TokStart);

        case ',':
            return formToken(tok::colon, TokStart);
        case ';':
            return formToken(tok::semicolon, TokStart);

        case '{':
            return formToken(tok::l_brace, TokStart);
        case '}':
            return formToken(tok::r_brace, TokStart);
        case '[':
            return formToken(tok::l_bracket, TokStart);
        case ']':
            return formToken(tok::r_bracket, TokStart);
        case '(':
            return formToken(tok::l_paren, TokStart);
        case ')':
            return formToken(tok::r_paren, TokStart);

        // Divide or comment start
        case '/':
            // Check if start of a comment
            if (*CurPtr == '/') {        // `//`
                skipLineComment(true);
                if (KeepComments)
                    return formToken(tok::comment, TokStart);
                break; // Ignore comment

            } else if (*CurPtr == '*') { // `/*`
                skipMultilineComment();
                if (KeepComments)
                    return formToken(tok::comment, TokStart);
                break; // Ignore comment
            }

            return formToken(tok::divide, TokStart);


        // Algebraic operands
        case '+':
            return formToken(tok::plus, TokStart);
        case '-':
            return formToken(tok::minus, TokStart);
        case '*':
            return formToken(tok::multipy, TokStart);

        // Logical operands
        case '!':
            if (*CurPtr == '=') {
                CurPtr++;
                return formToken(tok::nequals, TokStart);
            } else
                return formToken(tok::neg, TokStart);

        case '<':
            if (*CurPtr == '=') {
                CurPtr++;
                return formToken(tok::less_eq, TokStart);

            } else
                return formToken(tok::less, TokStart);

        case '>':
            if (*CurPtr == '=') {
                CurPtr++;
                return formToken(tok::greater_eq, TokStart);
            } else
                return formToken(tok::greater, TokStart);

        case ':':
            return formToken(tok::unknown, TokStart);

        // Numbers
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return lexNumber();

        // Identifiers
        case 'a': case 'b': case 'c': case 'd': case 'e':
        case 'f': case 'g': case 'h': case 'i': case 'j':
        case 'k': case 'l': case 'm': case 'n': case 'o':
        case 'p': case 'q': case 'r': case 's': case 't':
        case 'u': case 'v': case 'w': case 'x': case 'y':
        case 'z':
        case 'A': case 'B': case 'C': case 'D': case 'E':
        case 'F': case 'G': case 'H': case 'I': case 'J':
        case 'K': case 'L': case 'M': case 'N': case 'O':
        case 'P': case 'Q': case 'R': case 'S': case 'T':
        case 'U': case 'V': case 'W': case 'X': case 'Y':
        case 'Z': case '_':
            return lexIdentifier();

        default: return formToken(tok::unknown, TokStart);
        }
    }
}

void Lexer::diagnose(const Token &T, const char *Message,
                     llvm::SourceMgr::DiagKind Kind) {
    assert(T.is(tok::unknown) && "Next token is not unknown");

    auto Text = T.getText();
    auto Loc = llvm::SMLoc::getFromPointer(Text.data() + Text.size());
    auto [Line, Col] = SourceManager.getLineAndColumn(Loc);
    auto Msg = llvm::StringRef{Message};

    auto Diag = llvm::SMDiagnostic(SourceManager, Loc,
                                   "Unexpected symbol",
                                   Line, Col, Kind,
                                   Message, Text,
                                   llvm::None);
    llvm::raw_os_ostream os(std::cerr);
    Diag.print("milac", os);
}

// MARK: - Static methods

tok Lexer::kindOfIdentifier(llvm::StringRef Str) {
    return llvm::StringSwitch<tok>(Str)
    .Case("var", tok::kwVar)
    .Case("const", tok::kwConst)
    .Case("if", tok::kwIf)
    .Case("else", tok::kwElse)
    .Case("while", tok::kwWhile)
    .Case("for", tok::kwFor)
    .Case("in", tok::kwIn)
    .Case("func", tok::kwFunc)
    .Case("return", tok::kwReturn)
    .Case("writeln", tok::kwWriteln)
    .Case("readln", tok::kwReadln)
    .Default(tok::identifier);
}

// MARK: - Private methods

void Lexer::skipToEndOfLine(bool ConsumeNewline) {
    while (true) {
        switch (*CurPtr++) {
        // Consume next character
        default: break;

        // Found newline character
        case '\r':
        case '\n':
            if (!ConsumeNewline)
                CurPtr--;
            return;

        case 0:
            // Skip random nul character as whitespace
            if (CurPtr - 1 != BufferEnd)
                break;
            // Otherwise there's no newline in rest of the buffer
            CurPtr--;
            return;
        }
    }
    while (*CurPtr != '\n')
        CurPtr++;
}

void Lexer::skipLineComment(bool ConsumeNewLine) {
    // Validate start of line comment
    assert(CurPtr[-1] == '/'
           && CurPtr[0] == '/' && "Not a // comment");
    skipToEndOfLine(ConsumeNewLine);
}

void Lexer::skipMultilineComment() {
    const char *TokStart = CurPtr - 1;
    // Validate start of line comment
    assert(CurPtr[-1] == '/' && CurPtr[0] == '*' && "Not a /* comment");
    CurPtr++;

    while (true) {
        switch (*CurPtr++) {
        // Consume next character
        default: break;

        // Found possible end of multiline comment
        case '*':
            // Return if found end of mutliline comment
            if (*CurPtr++ == '/')
                return;
            // Restore ptr and continue
            CurPtr--;
            break;

        case 0:
            // Skip random nul character as whitespace
            if (CurPtr - 1 != BufferEnd)
                break;
            CurPtr--;
            formToken(tok::unknown, TokStart);
        }
    }
}

void Lexer::formToken(tok Kind, const char *TokStart) {
    llvm::StringRef TokenText = { TokStart, (size_t)(CurPtr - TokStart) };
    NextToken.setToken(Kind, TokenText);
}

// MARK: - Helper lexing methods

void Lexer::lexElipsis() {
    // Store start of token;
    const char *TokStart = CurPtr - 1;

    // Validate start of elipsis
    assert(CurPtr[-1] == '.' && *CurPtr++ == '.' && "Invalid elipsis token");

    if (CurPtr[1] == '.') {
        // ...
        CurPtr++;
        return formToken(tok::elipsis_incl, TokStart);
    } else
        return formToken(tok::elipsis_excl, TokStart);
}

void Lexer::lexIdentifier() {
    // Store start of text
    const char *TokStart = --CurPtr;

    // Validate start of identifier
    auto didStart = consumeIfValidIdentifierStart(CurPtr);
    assert(didStart && "Unexpected start of identifier");

    // Continue moving until invalid character or buffer end found
    while (consumeIfValidIdentifierCont(CurPtr));

    // Construct token
    auto TokenText = llvm::StringRef{ TokStart, (size_t)(CurPtr - TokStart) };
    tok Kind = kindOfIdentifier(TokenText);
    return formToken(Kind, TokStart);
}

void Lexer::lexNumber() {
    // Validate start of number
    assert(isValidDecDigit(CurPtr - 1) && "Unexpected begining of number");

    // Check if non-decadic type
    if (CurPtr[-1] == '0' && CurPtr[0] == 'x')
        return lexHexNumber();
    else if (CurPtr[-1] == '0' && CurPtr[0] == 'o')
        return lexOctNumber();
    // Lex decadic number
    else
        return lexDecNumber();
}


void Lexer::lexHexNumber() {
    // Store start of token
    const char *TokStart = CurPtr - 1;

    // Validate literal
    assert(*TokStart == '0' && "Not a hex literal");
    assert(*CurPtr == 'x' && "Not a hex literal");

    // Consume [0-9][a-z][A-Z] character to get token string.
    // We'll validate it later.
    while (consumeIfValidIdentifierCont(CurPtr));

    const char *TokEnd = CurPtr;
    CurPtr = TokEnd;

    // Consume only valid [0-9][a-f][A-F] character.
    while (consumeIfValidHexNumber(CurPtr));

    // Validate number of consumed characters.
    if (TokEnd == CurPtr)
        return formToken(tok::number_literal, TokStart);
    else
        return formToken(tok::unknown, TokStart);
}

void Lexer::lexOctNumber() {
    // Store start of token
    const char *TokStart = CurPtr - 1;

    // Validate literal
    assert(*TokStart == '0' && "Not a octal literal");
    assert(*CurPtr == 'o' && "Not a ocatal literal");

    // Consume [0-9][a-z][A-Z] character to get token string.
    // We'll validate it later.
    while (consumeIfValidIdentifierCont(CurPtr));

    const char *TokEnd = CurPtr;
    CurPtr = TokEnd;

    // Consume only valid [0-7] character.
    while (consumeIfValidOctNumber(CurPtr));

    // Validate number of consumed characters.
    if (TokEnd == CurPtr)
        return formToken(tok::number_literal, TokStart);
    else
        return formToken(tok::unknown, TokStart);
}

void Lexer::lexDecNumber() {
    // Store start of token
    const char *TokStart = CurPtr -1;

    // Validate start of number
    assert(isValidDecDigit(TokStart) && "Not a number literal");

    // Consume [0-9][a-z][A-Z] character to get token string.
    // We'll validate it later.
    while (consumeIfValidIdentifierCont(CurPtr));

    const char *TokEnd = CurPtr;
    CurPtr = TokEnd;

    // Consume only valid [0-9] character.
    while (consumeIfValidDecNumber(CurPtr));

    // Validate number of consumed characters.
    if (TokEnd == CurPtr)
        return formToken(tok::number_literal, TokStart);
    else
        return formToken(tok::unknown, TokStart);
}

