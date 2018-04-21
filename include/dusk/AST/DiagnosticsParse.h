//===--- DiagnosticsParse.h - Parser and Lexer diagnostics ------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_DIAGNOSTICS_PARSE_H
#define DUSK_DIAGNOSTICS_PARSE_H

#include "llvm/ADT/StringRef.h"

namespace dusk {

namespace diag {

enum struct DiagID : unsigned {
  // Lexer diagnostics
  lex_unexpected_symbol,
  lex_unterminated_multiline_comment,

  // Parse diagnostics
  unexpected_token,
  expected_semicolon,

  expected_identifier,
  expected_variable_initialization,
  expected_l_paren,
  expected_l_brace,

  expected_in_kw,
  expected_ellipsis,
  expected_colon_separator,
  expected_r_paren,
  expected_r_bracket,
  expected_r_brace
};

static llvm::StringRef getTextForID(DiagID ID) {
  switch (ID) {
  // Lexer diagnostics
  case DiagID::lex_unexpected_symbol:
    return "Unexpected symbol";
  case DiagID::lex_unterminated_multiline_comment:
    return "Unterminated '/*' comment";

  // Parse diagnostics
  case DiagID::unexpected_token:
    return "Unexpected token";
  case DiagID::expected_semicolon:
    return "Expected ';'";

  case DiagID::expected_identifier:
    return "Expected identifier";
  case DiagID::expected_variable_initialization:
    return "Expected '=' initialization.";
  case DiagID::expected_l_paren:
    return "Expected '('";
  case DiagID::expected_l_brace:
    return "Expected '{'";

  case DiagID::expected_in_kw:
    return "Expected 'in' keyword";
  case DiagID::expected_ellipsis:
    return "Expected '..' or '...'";
  case DiagID::expected_colon_separator:
    return "Expected ',' after a list item";
  case DiagID::expected_r_paren:
    return "Expected ')' at the end of param list";
  case DiagID::expected_r_bracket:
    return "Expected ']' at the end of subcript";
  case DiagID::expected_r_brace:
    return "Expected '}' at the end of block";
  }
}

} // namespace diag

} // namespace dusk

#endif /* DUSK_DIAGNOSTICS_PARSE_H */
