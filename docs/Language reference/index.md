# [**Dusk language reference**](#)

**A quick dive into the Dusk programming language**

---

## [**Table of contents**](#)

* [**Dusk language reference**](#dusk-language-reference)
    * [**Table of contents**](#table-of-contents)
    * [**About the language reference**](#about-the-language-reference)
        * [**Grammar notation**](#grammar-notation)
    * [**Lexical Structure**](#lexical-structure)
        * [**Whitespace and Comments**](#whitespace-and-comments)
            * [**Grammar of whitespace**](#grammar-of-whitespace)
        * [**Identifiers**](#identifiers)
            * [**Grammar of identifiers**](#grammar-of-identifiers)
        * [**Keywords and punctuation**](#keywords-and-punctuation)
        * [**Literals**](#literals)
            * [**Integer literals**](#integer-literals)
            * [**Array literals**](#array-literals)
        * [**Operators**](#operators)
            * [**Grammar of operators**](#grammar-of-operators)

---

## [**About the language reference**](#)

This section of documentation describes grammar of Dusk programming language. The grammar described
here is intented to make understanding of the language easier, rather than to describe the actual
implementation of parser or compiler.

The Dusk is very small language with only few basic constructs and types, which we will cover in
the following sections.

### [**Grammar notation**](#)

To describe the grammar, we will use standart [EBNF](https://en.wikipedia.org/wiki/Extended_Backus–Naur_form)
syntax notation.

Before going any further though, we need to get some basic definitions in place first.

```efbn
character = "A" | "B" | "C" | "D" | "E" | "F" | "G"
          | "H" | "I" | "J" | "K" | "L" | "M" | "N"
          | "O" | "P" | "Q" | "R" | "S" | "T" | "U"
          | "V" | "W" | "X" | "Y" | "Z" | "a" | "b"
          | "c" | "d" | "e" | "f" | "g" | "h" | "i"
          | "j" | "k" | "l" | "m" | "n" | "o" | "p"
          | "q" | "r" | "s" | "t" | "u" | "v" | "w"
          | "x" | "y" | "z" ;

symbol = "=" | "-" | ":" | ";" | "," | "." | "&"
       | "(" | ")" | "[" | "]" | "{" | "}" | "+"
       | "-" | "%" | "/" | "*" | "|" | "=" | "!"
       | "<" | ">" ;

digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9";

whitespace-character = " " | "\t" | "\v" | "\r" | "\n" | "\0";

any-symbol = character | symbol | digit | whitespace-character;
```

---

## [**Lexical Structure**](#)

The *lexical structure* describes what strings form valid tokens of the language. These tokens are
the lowest-level building block of the language and are used to describe the rest of the language.
Tokens consist of an identifier, a keyword, punctuation, literal and operator.

All of these are generated from the characters of a Dusk source file by considering longest possible
subsequence of such characters, that form a valid token.

### [**Whitespace and Comments**](#)

Whitespace have only a single use in Dusk, to separate tokens in the source file. We consider
following characters as whitespace: space (` `), horizontal tab (`\t`), vertical tab (`\v`),
carriage return (`\r`), newline (`\n`) and null (`\0`).

Comments are treates as a whitespace by the compiler. Dusk has two types of comments:
- Single line comments starts with a `//` and continue until the newline (`\n`) or carriage return (`\r`)
character is encountered.
- Multiline comments begin with `/*` and end with `*/`. **Note**: Dusk does not allow nesting multiline
comments.

##### [**Grammar of whitespace**](#)

```ebnf
whitespace = { whitespace-item };
whitespace-item = whitespace-character | comment;

comment = singleline-comment | multiline-comment;
singleline-comment = "//" singleline-comment-text "\n";
singleline-comment-text = { any-symbol }

multiline-comment = "/*" multiline-comment-text "*/"
multiline-comment-text = { any-symbol }
```

### [**Identifiers**](#)

*Identifiers* begin with uppercase or lowercase letter from A through Z or an underscore (`_`).
After the first character all digits are also allowed.

##### [**Grammar of identifiers**](#)

```ebnf
identifier = identifier-head identifier-rest;
identifier-head = character | "_";
identifier-body = { character | digit | "_"  };
```

### [**Keywords and punctuation**](#)

Following keywords are resever and cannot be used as identifiers:
- Keywords used in declarations: `func`, `let`, `var` and `inout`.
- Keywords used in statements: `for`, `in`, `while`, `if`, `else`, `break` and `return`.

Following tokens are reserved as punctuation: `(`, `)`, `[`, `]`, `{`, `}`, `..`, `...`, `,`, `:`,
`;`, `=`, `==`, `!=`, `&&`, `||`, `<`, `>`, `<=, `>=`, `+`, `-`, `*`, `/`, `%`, `!`, `&` and `->`.

### [**Literals**](#)

A *literal* is a source code representation of a value of a type. Since the Dusk is such small
language, there are only two possible literals:

```swift
42               // Integer literal
[1, 2, 3, 4, 5]  // Integer array literal
```

Each of the literals has it's own type. This type may be used when for type inference, when declaring
a new variable, e.g. `let x = 42` declares a variable of type `Int` without explicitely telling
the compiler declaration type.

```ebnf
literal = number-literal | array-literal;
```

##### [**Integer literals**](#)

*Integer literals* represents an integer value. By default integers are expressed in decimal, although
you can specify an alternate base using a prefix. Binary integers begins with `0b` or `0B`, octal
with `0o` or `0O` and hexadecimal with `0x` or `0X`.

```ebnf
number-literal = binary-literal | octal-literal | deicmal-literal | hexadecimal-literal;

binary-literal = "0" ( "b" | "B" ) binary-digit { binary-digit };
octal-literal = "0" ( "o" | "O" ) octal-digit { octal-digit };
decimal-literal = decimal-digit { decimal-digit };
hexadecimal-literal = "0" ( "x" | "X" ) hexadecimal-digit { hexadecimal-digit };

binary-digit = "0" | "1";
octal-digit =  "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7";
decimal-digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9";
hexadecimal-digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" |
                  | "A" | "B" | "C" | "D" | "E" | "F" |
                  | "a" | "c" | "b" | "d" | "e" | "f" ;
```

##### [**Array literals**](#)

*Array literals* represents array of values. All values in array must have the same type, however
arrays are not limited to arrays of integers. The underlying value can be yet another array. That can
be used to constructs an *n*-dimensional array.

```ebnf
array-literal = "[" [ literal { "," literal } ] "]";
```

### [**Operators**](#)

In Dusk there are number of operators. If you are familiar with C or C-like languages, you will be
familiar with most of them. Dusk contains prefix and infix operators, which can be used in different
contexts. To learn more about operands, see [Operators]() section.

##### [**Grammar of operators**](#)

```ebnf
operator = ".." | "..." | "=" | "==" | "!=" | "&&" | "||"
          | "<" | ">"   | "<= | ">=" | "+"  | "-"  | "*" | "/" | "%" | "!" | "&"
```

---

[Next 'Types'](/docs/Language%20reference/Types.md)
