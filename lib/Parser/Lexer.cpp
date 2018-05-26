//===--- Lexer.cpp - Dusk language lexer implementation -------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#include "dusk/Parse/Lexer.h"
#include "dusk/Basic/SourceManager.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/TableGen/Error.h"
#include <cctype>
#include <functional>

using namespace dusk;

/// Returns start of line containing a given \c currPtr.
static const char *getStartOfLine(const char *buffStart, const char *currPtr) {
  while (buffStart != currPtr) {
    if (*currPtr == '\n' || *currPtr == '\r') {
      currPtr++;
      break;
    }
    currPtr--;
  }
  return currPtr;
}

// MARK: - Validation functions

static bool isValidIdentifierStart(const char *c) {
  return std::isalpha(*c) || *c == '_';
}

static bool isValidIdentifierCont(const char *c) {
  return std::isalnum(*c) || *c == '_';
}

static bool isValidDecDigit(const char *c) { return std::isdigit(*c); }

static bool isValidBinDigit(const char *c) { return *c == '0' || *c == '1'; }

static bool isValidOctDigit(const char *c) { return *c >= '0' && *c <= '7'; }

static bool isValidHexDigit(const char *c) {
  return std::isdigit(*c) ||
         (*c >= 'a' && *c <= 'f') | (*c >= 'A' && *c <= 'F');
}

// MARK: - Contitional character consumtion functions

/// \brief Consumes a character if provided predicate returns \c true
///
/// \return \c true, if function consumes a character, \c false otherwise.
static bool consumeIf(const char *&ptr,
                      std::function<bool(const char *)> pred) {
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

static bool consumeIfValidDecDigit(const char *&ptr) {
  return consumeIf(ptr, isValidDecDigit);
}

static bool consumeIfValidBinDigit(const char *&ptr) {
  return consumeIf(ptr, isValidBinDigit);
}

static bool consumeIfValidOctDigit(const char *&ptr) {
  return consumeIf(ptr, isValidOctDigit);
}

static bool consumeIfValidHexDigit(const char *&ptr) {
  return consumeIf(ptr, isValidHexDigit);
}

// MARK: - Lexer
Lexer::Lexer(const llvm::SourceMgr &SM, unsigned BufferID,
             DiagnosticEngine *Engine, bool KeepComments)
    : SourceManager(SM), Engine(Engine), KeepComments(KeepComments) {
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

  // Never lex pass the eof.
  if (Ret.isNot(tok::eof))
    lexToken();
}

void Lexer::lexToken() {
  while (true) {
    const char *TokStart = CurPtr;

    switch (*CurPtr++) {
    case 0: {
      // Not ending null character.
      if (CurPtr - 1 != BufferEnd)
        break;
      CurPtr--;
      return formToken(tok::eof, TokStart);
    }

    // Skip whitespace
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      break;

    case '=': {
      if (*CurPtr == '=') {
        CurPtr++;
        return formToken(tok::equals, TokStart);
      }
      return formToken(tok::assign, TokStart);
    }

    case '.': {
      if (*CurPtr == '.')
        return lexElipsis();
      formToken(tok::unknown, TokStart);
      return diagnose();
    }

    case ',':
      return formToken(tok::comma, TokStart);
    case ':':
      return formToken(tok::colon, TokStart);
    case ';':
      return formToken(tok::semi, TokStart);

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
    case '/': {
      // Check if start of a comment
      if (*CurPtr == '/') { // `//`
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
    }

    // Minus or arrow operator
    case '-': {
      if (*CurPtr == '>') {
        CurPtr++;
        return formToken(tok::arrow, TokStart);
      }
      return formToken(tok::minus, TokStart);
    }
        
    // Algebraic operands
    case '+':
      return formToken(tok::plus, TokStart);
    case '*':
      return formToken(tok::multipy, TokStart);
    case '%':
      return formToken(tok::mod, TokStart);

    // Logical operands
    case '!': {
      if (*CurPtr == '=') {
        CurPtr++;
        return formToken(tok::nequals, TokStart);
      }
      return formToken(tok::lnot, TokStart);
    }
        
    case '&': {
      if (*CurPtr == '&') {
        CurPtr++;
        return formToken(tok::land, TokStart);
      }
      return formToken(tok::unknown, TokStart);
    }
        
    case '|': {
      if (*CurPtr == '|') {
        CurPtr++;
        return formToken(tok::lor, TokStart);
      }
      return formToken(tok::unknown, TokStart);
    }

    case '<': {
      if (*CurPtr == '=') {
        CurPtr++;
        return formToken(tok::less_eq, TokStart);
      }
      return formToken(tok::less, TokStart);
    }

    case '>': {
      if (*CurPtr == '=') {
        CurPtr++;
        return formToken(tok::greater_eq, TokStart);
      }
      return formToken(tok::greater, TokStart);
    }
      

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

    default:
      formToken(tok::unknown, TokStart);
      return diagnose();
    }
  }
}

void Lexer::diagnose(diag::DiagID ID) { diagnose(NextToken, ID); }

void Lexer::diagnose(Token T, diag::DiagID ID) {
  if (Engine == nullptr)
    return;
  switch (ID) {
  case diag::DiagID::lex_unexpected_symbol:
    Engine->diagnose(T.getLoc(), ID);
    break;
  case diag::DiagID::lex_unterminated_multiline_comment:
    Engine->diagnose(T.getLoc(), ID).fixItAfter("*/", T.getLoc());
    break;

  default:
    llvm_unreachable("Invalid diagnostics");
  }
}

// MARK: - Static methods

tok Lexer::kindOfIdentifier(StringRef Str) {
  return llvm::StringSwitch<tok>(Str)
      .Case("var", tok::kw_var)
      .Case("let", tok::kw_let)
      .Case("break", tok::kw_break)
      .Case("return", tok::kw_return)
      .Case("if", tok::kw_if)
      .Case("else", tok::kw_else)
      .Case("while", tok::kw_while)
      .Case("for", tok::kw_for)
      .Case("in", tok::kw_in)
      .Case("func", tok::kw_func)
      .Case("inout", tok::kw_inout)
      .Case("extern", tok::kw_extern)
      .Default(tok::identifier);
}

Token Lexer::getTokenAtLocation(const llvm::SourceMgr &SM, SMLoc Loc) {
  // Invalid address
  if (!Loc.isValid())
    return Token();

  auto BufferID = getBufferForLoc(SM, Loc);
  // Buffer not found in currently opened buffers
  if (BufferID > SM.getNumBuffers())
    return Token();
  Lexer L(SM, BufferID);
  L.setState(Loc);
  return L.peekNextToken();
}

SMLoc Lexer::getLocForEndOfToken(const llvm::SourceMgr &SM, SMLoc Loc) {
  auto Tok = getTokenAtLocation(SM, Loc);
  return Tok.getRange().End;
}

SMLoc Lexer::getLocForStartOfLine(const llvm::SourceMgr &SM, SMLoc Loc) {
  // Invalid address
  if (!Loc.isValid())
    return Loc;

  auto BufferID = getBufferForLoc(SM, Loc);
  // Buffer not found in currently opened buffers
  if (BufferID > SM.getNumBuffers())
    return SMLoc();

  auto B = SM.getMemoryBuffer(BufferID);
  auto BuffStart = B->getBufferStart();
  auto CurPtr = Loc.getPointer();
  auto StartLoc = getStartOfLine(BuffStart, CurPtr);
  return getSourceLoc(StartLoc);
}

SMLoc Lexer::getLocForEndOfLine(const llvm::SourceMgr &SM, SMLoc Loc) {
  // Invalid address
  if (!Loc.isValid())
    return Loc;

  auto BufferID = getBufferForLoc(SM, Loc);
  // Buffer not found in currently opened buffers
  if (BufferID > SM.getNumBuffers())
    return SMLoc();
  Lexer L(SM, BufferID);
  L.setState(Loc);
  L.skipToEndOfLine(/*ConsumeNewLine: */ true);
  return getSourceLoc(L.CurPtr);
}

StringRef Lexer::getLineForLoc(const llvm::SourceMgr &SM, SMLoc Loc) {
  auto S = getLocForStartOfLine(SM, Loc);
  auto E = getLocForEndOfLine(SM, Loc);
  return {S.getPointer(), (size_t)(E.getPointer() - S.getPointer())};
}

// MARK: - Private methods

void Lexer::skipToEndOfLine(bool ConsumeNewline) {
  while (true) {
    switch (*CurPtr++) {
    // Consume next character
    default:
      break;

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
  assert(CurPtr[-1] == '/' && CurPtr[0] == '/' && "Not a // comment");
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
    default:
      break;

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
      return diagnose(diag::DiagID::lex_unterminated_multiline_comment);
    }
  }
}

void Lexer::formToken(tok Kind, const char *TokStart) {
  StringRef TokenText = {TokStart, (size_t)(CurPtr - TokStart)};
  NextToken.setToken(Kind, TokenText);
}

// MARK: - Helper lexing methods

void Lexer::lexElipsis() {
  // Store start of token;
  const char *TokStart = CurPtr - 1;

  // Validate start of elipsis
  assert(CurPtr[-1] == '.' && *CurPtr++ == '.' && "Invalid elipsis token");

  if (CurPtr[0] == '.') {
    // '...' token
    CurPtr++;
    return formToken(tok::elipsis_incl, TokStart);
  } else {
    return formToken(tok::elipsis_excl, TokStart);
  }
}

void Lexer::lexIdentifier() {
  // Store start of text
  const char *TokStart = --CurPtr;

  // Validate start of identifier
  auto didStart = consumeIfValidIdentifierStart(CurPtr);
  assert(didStart && "Unexpected start of identifier");

  // Continue moving until invalid character or buffer end found
  while (consumeIfValidIdentifierCont(CurPtr))
    ;

  // Construct token
  auto TokenText = StringRef{TokStart, (size_t)(CurPtr - TokStart)};
  tok Kind = kindOfIdentifier(TokenText);
  return formToken(Kind, TokStart);
}

void Lexer::lexNumber() {
  // Validate start of number
  assert(isValidDecDigit(CurPtr - 1) && "Unexpected begining of number");

  // Check if non-decadic type
  if (CurPtr[-1] == '0' && std::tolower(CurPtr[0]) == 'x')
    return lexHexNumber();
  else if (CurPtr[-1] == '0' && std::tolower(CurPtr[0]) == 'b')
    return lexBinNumber();
  else if (CurPtr[-1] == '0' && std::tolower(CurPtr[0]) == 'o')
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
  assert(std::tolower(*CurPtr) == 'x' && "Not a hex literal");

  // Consume [0-9][a-z][A-Z] character to get token string.
  // We'll validate it later.
  while (consumeIfValidIdentifierCont(CurPtr))
    ;

  const char *TokEnd = CurPtr;
  CurPtr = TokStart + 2; // skip `0x` prefix

  // Consume only valid [0-9][a-f][A-F] character.
  while (consumeIfValidHexDigit(CurPtr))
    ;

  // Validate number of consumed characters.
  if (TokEnd == CurPtr)
    return formToken(tok::number_literal, TokStart);

  formToken(tok::unknown, TokStart);
  return diagnose();
}

void Lexer::lexBinNumber() {
  // Store start of token
  const char *TokStart = CurPtr - 1;

  // Validate literal
  assert(*TokStart == '0' && "Not a binary literal");
  assert(std::tolower(*CurPtr == 'b') && "Not a binary literal");

  // Consume [0-9][a-z][A-Z] character to get token string.
  // We'll validate it later.
  while (consumeIfValidIdentifierCont(CurPtr))
    ;

  const char *TokEnd = CurPtr;
  CurPtr = TokStart + 2; // skip `0b` prefix

  // Consume only valid (0|1) character.
  while (consumeIfValidBinDigit(CurPtr))
    ;

  // Validate number of consumed characters.
  if (TokEnd == CurPtr)
    return formToken(tok::number_literal, TokStart);

  formToken(tok::unknown, TokStart);
  return diagnose();
}

void Lexer::lexOctNumber() {
  // Store start of token
  const char *TokStart = CurPtr - 1;

  // Validate literal
  assert(*TokStart == '0' && "Not a octal literal");
  assert(std::tolower(*CurPtr) == 'o' && "Not an octal literal");

  // Consume [0-9][a-z][A-Z] character to get token string.
  // We'll validate it later.
  while (consumeIfValidIdentifierCont(CurPtr))
    ;

  const char *TokEnd = CurPtr;
  CurPtr = TokStart + 2; // skip `0o` prefix

  // Consume only valid [0-7] character.
  while (consumeIfValidOctDigit(CurPtr))
    ;

  // Validate number of consumed characters.
  if (TokEnd == CurPtr)
    return formToken(tok::number_literal, TokStart);

  formToken(tok::unknown, TokStart);
  return diagnose();
}

void Lexer::lexDecNumber() {
  // Store start of token
  const char *TokStart = CurPtr - 1;

  // Validate start of number
  assert(isValidDecDigit(TokStart) && "Not a number literal");

  // Consume [0-9][a-z][A-Z] character to get token string.
  // We'll validate it later.
  while (consumeIfValidIdentifierCont(CurPtr))
    ;

  const char *TokEnd = CurPtr;
  CurPtr = TokEnd;

  // Consume only valid [0-9] character.
  while (consumeIfValidDecDigit(CurPtr))
    ;

  // Validate number of consumed characters.
  if (TokEnd == CurPtr)
    return formToken(tok::number_literal, TokStart);

  formToken(tok::unknown, TokStart);
  return diagnose();
}
