//===--- Token.h - Token interface ------------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_TOKEN_H
#define DUSK_TOKEN_H

#include "dusk/Basic/LLVM.h"
#include "dusk/Basic/TokenDefinition.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SMLoc.h"

namespace dusk {

/// \brief Encapsulation of lexed token.
class Token {
  /// Actual token type.
  tok Kind = tok::unknown;

  /// The actual text representation in source buffer.
  StringRef Text;

public:
  Token() = default;
  Token(tok Kind, StringRef Text) : Kind(Kind), Text(Text) {}

  // MARK: - Setting token properties

  void setToken(tok K, StringRef T) {
    Kind = K;
    Text = T;
  }

  void setKind(tok K) { Kind = K; }

  // MARK: - Getting token properties
  tok getKind() const { return Kind; }

  StringRef getText() const { return Text; }

  unsigned getLength() const { return Text.size(); }

  SMLoc getLoc() const { return SMLoc::getFromPointer(Text.begin()); }

  SMRange getRange() const {
    auto E = getLoc().getPointer() + getLength();
    return {getLoc(), SMLoc::getFromPointer(E)};
  }

  // MARK: - Token predicates

  /// \brief Predicate to indicate if token is of provided kind.
  ///
  /// \param K Token kind to compare current token with.
  ///
  /// \return \c true, if token is of provided kind, \c false otherwise.
  bool is(tok K) const { return Kind == K; }
  bool isNot(tok K) const { return Kind != K; }

  bool isAny(tok K) const { return is(K); }
  
  /// \brief Predicate to indicate if token is any of provided kinds.
  ///
  /// \return \c true, if token is any of provided kinds, \c false otherwise.
  template <typename... T> bool isAny(tok K1, tok K2, T... K) {
    if (is(K1))
      return true;
    return isAny(K2, K...);
  }

  template <typename... T> bool isNot(tok K1, T... K) const {
    return !isAny(K1, K...);
  }

  bool isOperator() const {
    return is(tok::lnot) || is(tok::arrow) || isBinaryOperator();
  }

  bool isBinaryOperator() const {
    switch (Kind) {
    case tok::assign:
    case tok::elipsis_excl:
    case tok::elipsis_incl:
    case tok::plus:
    case tok::minus:
    case tok::mod:
    case tok::divide:
    case tok::multipy:
    case tok::equals:
    case tok::nequals:
    case tok::less:
    case tok::less_eq:
    case tok::greater:
    case tok::greater_eq:
      return true;
    default:
      return false;
    }
  }

  bool isElipsis() const {
    return is(tok::elipsis_excl) || is(tok::elipsis_incl);
  }

  bool isIdentifier() const { return is(tok::identifier); }

  /// Returns \c true, if token is a keyword, \c false otherwise.
  bool isKeyword() const {
    switch (Kind) {
    case tok::kwVar:
    case tok::kwLet:
    case tok::kwBreak:
    case tok::kwReturn:
    case tok::kwIf:
    case tok::kwElse:
    case tok::kwWhile:
    case tok::kwFor:
    case tok::kwIn:
    case tok::kwFunc:
    case tok::kwWriteln:
    case tok::kwReadln:
    case tok::kwVoid:
    case tok::kwInt:
      return true;
    default:
      return false;
    }
  }

  /// \brief Determins, if current token is a number literal.
  ///
  /// \note In \c dusk language, the only valid literals are integers
  ///   and arrays.
  ///
  /// \return \c true, if token is a number literal, \c false otherwise.
  bool isLiteral() const { return is(tok::number_literal); }
};

} // namespace dusk

#endif /* DUSK_TOKEN_H */
