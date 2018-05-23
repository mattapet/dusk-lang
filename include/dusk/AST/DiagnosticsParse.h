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

#include "dusk/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"

namespace dusk {

namespace diag {

enum DiagID : unsigned {
  // Lexer diagnostics
  lex_unexpected_symbol,
  lex_unterminated_multiline_comment,

  // Parse diagnostics
  unexpected_token,
  expected_semicolon,

  expected_identifier,
  expected_expression,
  expected_variable_initialization,
  expected_l_paren,
  expected_l_brace,

  expected_in_kw,
  expected_ellipsis,
  expected_comma_separator,
  expected_r_paren,
  expected_r_bracket,
  expected_r_brace,
  expected_return_type,
  expected_type_specifier,
  expected_func_kw,
  
  // Semantic diagnostics
  
  unexpected_expresssion,
  unexpected_break_stmt,
  unexpected_return_stmt,
  expression_not_assignable,
  redefinition_of_identifier,
  return_missing_value,
  array_index_out_of_bounds,
  
  // Types
  expected_type_annotation,
  expected_default_initialization,
  expected_value_type_expression,
  cannot_reassign_let_value,
  func_call_non_func_type,
  subscripted_value_not_array,
  invalid_array_size,
  expected_array_size,
  variable_array_size,
  invalid_operand_type,
  
  ambigous_types,
  type_missmatch,
  undefined_identifier,
  array_element_mismatch,
  arguments_mismatch,
  unknown_type,
};

static StringRef getTextForID(DiagID ID) {
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
  case DiagID::expected_expression:
    return "Expected expression";
  case DiagID::expected_variable_initialization:
    return "Expected default initialization '=' for 'let' declaration.";
  case DiagID::expected_l_paren:
    return "Expected '('";
  case DiagID::expected_l_brace:
    return "Expected '{'";

  case DiagID::expected_in_kw:
    return "Expected 'in' keyword";
  case DiagID::expected_ellipsis:
    return "Expected '..' or '...'";
  case DiagID::expected_comma_separator:
    return "Expected ',' after a list item";
  case DiagID::expected_r_paren:
    return "Expected ')' at the end of param list";
  case DiagID::expected_r_bracket:
    return "Expected ']' at the end of subcript";
  case DiagID::expected_r_brace:
    return "Expected '}' at the end of block.";
  case DiagID::expected_return_type:
      return "Expected '->' return type decalration.";
  case DiagID::expected_type_specifier:
      return "Expected type specifier.";
  case DiagID::expected_func_kw:
    return "Expected 'func' keyword to at start of function delaration.";
      
  case DiagID::unexpected_expresssion:
    return "Unexpected expression outside of a function scope.";
  case DiagID::unexpected_break_stmt:
      return "Unexpected 'break' statement.";
  case DiagID::unexpected_return_stmt:
    return "Unexpected 'return' statement.";
  case DiagID::expression_not_assignable:
    return "Expression is not assignable.";
  case DiagID::return_missing_value:
    return "Non-void function must return a value.";
  case DiagID::array_index_out_of_bounds:
    return "Indexing array out of array bounds.";
      
  case DiagID::expected_type_annotation:
    return "Expected type annocation ': Type'.";
  case DiagID::expected_default_initialization:
    return "Expected default initialization";
  case DiagID::expected_value_type_expression:
    return "Expected value type expression.";
  case DiagID::redefinition_of_identifier:
    return "Redefinition of identifier.";
  case DiagID::cannot_reassign_let_value:
    return "Cannot reassign value declared with 'let' qualifier";
  case DiagID::func_call_non_func_type:
    return "Unexpected call expression '()' on non-function type.";
  case DiagID::subscripted_value_not_array:
    return "Subscripted value is not an array type.";
  case DiagID::invalid_array_size:
    return "Array size must be specified as a number literal.";
  case DiagID::expected_array_size:
    return "Every array type must define size of the array.";
  case DiagID::invalid_operand_type:
    return "Invalid operand type.";
  case DiagID::variable_array_size:
    return "Size of the array must be specified as a constant expression.";
      
  case DiagID::ambigous_types:
    return "Ambigous type resolution.";
  case DiagID::type_missmatch:
    return "Type mismatch.";
  case DiagID::undefined_identifier:
    return "Use of undefined identifier.";
  case DiagID::arguments_mismatch:
    return "invalid arguments provided to function call.";
  case DiagID::array_element_mismatch:
    return "Array elements are not the same type.";
  case DiagID::unknown_type:
    return "Use of unknown type.";
  }
}

} // namespace diag

} // namespace dusk

#endif /* DUSK_DIAGNOSTICS_PARSE_H */
