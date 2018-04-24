//===--- TokenDefinition.h --------------------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//

#ifndef DUSK_TOKEN_DEFINITION_H
#define DUSK_TOKEN_DEFINITION_H

#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

namespace dusk {

enum struct tok {
  // Keywords
  kwVar,
  kwLet,
  kwBreak,
  kwReturn,
  kwIf,
  kwElse,
  kwWhile,
  kwFor,
  kwIn,
  kwFunc,
  kwWriteln,
  kwReadln,
  
  // Types
  kwVoid,
  kwInt,

  // Identifier
  identifier,     // [a-zA-Z_][a-zA-Z0-9_]+

  // Literal
  number_literal, // dec: [0-9]+
                  // hex: 0(x|X)[0-9a-fA-F]+
                  // oct: 0(o|O)[0-7]+

  comment,        // // or /* */

                  // Operators
  assign,         // =
  arrow,          // ->

  elipsis_excl,   // ..
  elipsis_incl,   // ...

  semicolon,      // ;
  colon,          // ,

  l_brace,        // {
  r_brace,        // }

  l_bracket,      // [
  r_bracket,      // ]

  l_paren,        // (
  r_paren,        // )

  // Arithmetic Operators
  plus,           // +
  minus,          // -
  mod,            // %
  divide,         // /
  multipy,        // *

  // Logical Operators
  lnot,           // !
  equals,         // ==
  nequals,        // !=
  less,           // <
  less_eq,        // <=
  greater,        // >
  greater_eq,     // >=

  // End of file
  eof,

  // Unknown token
  unknown
};

} // namespace dusk

namespace llvm {
inline raw_ostream &operator<<(raw_ostream &OS, const dusk::tok &T) {
  switch (T) {
  // Keywords
  case dusk::tok::kwVar:
    return OS << "var";
  case dusk::tok::kwLet:
    return OS << "let";
  case dusk::tok::kwBreak:
    return OS << "break";
  case dusk::tok::kwReturn:
    return OS << "return";
  case dusk::tok::kwIf:
    return OS << "if";
  case dusk::tok::kwElse:
    return OS << "else";
  case dusk::tok::kwWhile:
    return OS << "while";
  case dusk::tok::kwFor:
    return OS << "for";
  case dusk::tok::kwIn:
    return OS << "in";
  case dusk::tok::kwFunc:
    return OS << "func";
  case dusk::tok::kwWriteln:
    return OS << "writeln";
  case dusk::tok::kwReadln:
    return OS << "readln";
      
  // Types
  case dusk::tok::kwVoid:
    return OS << "Void";
  case dusk::tok::kwInt:
    return OS << "Int";
      

  // Operators
  case dusk::tok::assign:
    return OS << "=";
  case dusk::tok::arrow:
    return OS << "->";
  case dusk::tok::elipsis_excl:
    return OS << "..";
  case dusk::tok::elipsis_incl:
    return OS << "...";
  case dusk::tok::semicolon:
    return OS << ";";
  case dusk::tok::colon:
    return OS << ",";
  case dusk::tok::l_brace:
    return OS << "{";
  case dusk::tok::r_brace:
    return OS << "}";
  case dusk::tok::l_bracket:
    return OS << "[";
  case dusk::tok::r_bracket:
    return OS << "]";
  case dusk::tok::l_paren:
    return OS << "(";
  case dusk::tok::r_paren:
    return OS << ")";

  // Arithmetic Operators
  case dusk::tok::plus:
    return OS << "+";
  case dusk::tok::minus:
    return OS << "-";
  case dusk::tok::mod:
    return OS << "%";
  case dusk::tok::divide:
    return OS << "/";
  case dusk::tok::multipy:
    return OS << "*";

  // Logical Operators
  case dusk::tok::lnot:
    return OS << "!";
  case dusk::tok::equals:
    return OS << "==";
  case dusk::tok::nequals:
    return OS << "!=";
  case dusk::tok::less:
    return OS << "<";
  case dusk::tok::less_eq:
    return OS << "<=";
  case dusk::tok::greater:
    return OS << ">";
  case dusk::tok::greater_eq:
    return OS << ">=";
  default:
    llvm_unreachable("Invalid token string conversion.");
  }
  return OS;
}

} // namespace llvm

#endif /* DUSK_TOKEN_DEFINITION_H */
