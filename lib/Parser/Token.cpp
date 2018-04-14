//===--- Token.cpp --------------------------------------------------------===//
//
//                                 dusk-lang
// This source file is part of a dusk-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//
#include "dusk/Parse/Token.h"

using namespace dusk;

std::string Token::ToString() const {
    switch (Kind) {
    case tok::kwVar: return "tok::kwVar";
    case tok::kwConst: return "tok::kwConst";
    case tok::kwIf: return "tok::kwIf";
    case tok::kwElse: return "tok::kwElse";
    case tok::kwWhile: return "tok::kwWhile";
    case tok::kwWriteln: return "tok::kwWrite";
    case tok::kwFor: return "tok::kwFor";
    case tok::kwIn: return "tok::kwIn";
    case tok::kwFunc: return "tok::kwFunc";
    case tok::kwReturn: return "tok::kwReturn";
    case tok::kwReadln: return "tok::kwRead";
    case tok::identifier: return "tok::identifier";
    case tok::number_literal: return "tok::number_literal";
    case tok::assign: return "tok::assign";
    case tok::semicolon: return "tok::semicolon";
    case tok::colon: return "tok::colon";
    case tok::elipsis_excl: return "tok::elipsis_excl";
    case tok::elipsis_incl: return "tok::elipsis_incl";
    case tok::l_brace: return "tok::l_brace";
    case tok::r_brace: return "tok::r_brace";
    case tok::l_bracket: return "tok::l_bracket";
    case tok::r_bracket: return "tok::r_bracket";
    case tok::l_paren: return "tok::l_paren";
    case tok::r_paren: return "tok::r_paren";
    case tok::plus: return "tok::plus";
    case tok::minus: return "tok::minus";
    case tok::divide: return "tok::divide";
    case tok::multipy: return "tok::multipy";
    case tok::equals: return "tok::equals";
    case tok::neg: return "tok::neg";
    case tok::nequals: return "tok::nequals";
    case tok::less: return "tok::less";
    case tok::less_eq: return "tok::less_eq";
    case tok::greater: return "tok::greater";
    case tok::greater_eq: return "tok::greater_eq";
    case tok::comment: return "tok::comment";
    case tok::eof: return "tok::eof";
    case tok::unknown: return "tok::unknown";
    }
}

