//===--- TokenDefinition.h --------------------------------------*- C++ -*-===//
//
//                                 dusk-lang
// This source file is part of a mila-lang project, which is a semestral
// assignement for BI-PJP course at Czech Technical University in Prague.
// The software is provided "AS IS", WITHOUT WARRANTY OF ANY KIND.
//
//===----------------------------------------------------------------------===//
#ifndef TokenDefinition_h
#define TokenDefinition_h

namespace dusk {

enum struct tok {
// Keywords
    kwVar,
    kwConst,
    kwIf,
    kwElse,
    kwWhile,
    kwFor,
    kwIn,
    kwFunc,
    kwReturn,
    kwWriteln,
    kwReadln,

// Identifier
    identifier,     // [a-zA-Z_][a-zA-Z0-9_]+
    
// Literal
    number_literal, // dec: [0-9]+
                    // hex: 0(x|X)[0-9a-fA-F]+
                    // oct: 0(o|O)[0-7]+

    comment,        // // or /* */
    
// Operators
    assign,         // =
    
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
    divide,         // /
    multipy,        // *

// Logical Operators
    neg,            // !
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

} /* namespace mila */

#endif /* TokenDefinition_h */

