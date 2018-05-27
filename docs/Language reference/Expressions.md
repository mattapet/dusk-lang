# [**Expresssions**](#)

In Dusk, there are found kinds of expressions: prefix expressions, binary expressions, primary
expressions and postfix expressions. Evaluating expression returns a value, causes a side effect
or both.

Prefix expressions and binary expression let you apply operands to smaller expressions. Primary
expressions are effectively the smallest kind of expressions and they provide a way to access values.


## [**Table of Contents**](#)
- [**Expresssions**](#expresssions)
    - [**Table of Contents**](#table-of-contents)
        - [**Grammar of expressions**](#grammar-of-expressions)
    - [**Prefix Expressions**](#prefix-expressions)
        - [**Grammar of prefix expressions**](#grammar-of-prefix-expressions)
    - [**Binary Expressions**](#binary-expressions)
        - [**Grammar of binary expressions**](#grammar-of-binary-expressions)
        - [**Assignment Expression**](#assignment-expression)
            - [**Grammar of assignment expression**](#grammar-of-assignment-expression)
        - [**Primary Expressions**](#primary-expressions)
            - [**Grammar of primary expresssions**](#grammar-of-primary-expresssions)
        - [**Postfix Expressions**](#postfix-expressions)
            - [**Grammar of postfix expressions**](#grammar-of-postfix-expressions)

---

##### [**Grammar of expressions**](#)

```ebnf
expression = assignment-expression | binary-expression
            | prefix-expression    | primary-expression
            | postfix-expression   ;
```

## [**Prefix Expressions**](#)

*Prefix expressions* combine optional prefix operator with a primary expression. Dusk has three prefix
operands: `-`, `!` and `&`. To learn more about them, see [Operands](#operands).

##### [**Grammar of prefix expressions**](#)

```ebnf
prefix-expression = value-prefix-expression | inout-prefix-expression;
value-prefix-expression = ( "-" | "!" ) primary-expression;
inout-prefix-expression = "&" identifier;
```

## [**Binary Expressions**](#)

*Binary expression* combine an infix operator with expression on it's left hand side and right hand
side in the following form  `left-hand argument` `operator` `right-hand argument`.

All of the infix operator follow expected precedense rules you are used to from otehr languages.
The weakes is compare operators `==` and `!=`, logical operators `&&` and `||`, terminal arithemtic
operators `+` and `-` and the hight precedence have factor operators `*`, `/` and `%`. You can of
course set the precedence of yourself by using paranthesis like  `(2 + 3) * 8`.


##### [**Grammar of binary expressions**](#)

```ebnf
binary-expression = primary-expression binary-operator primary-expression;
binary-operator = "==" | "!=" | "&&" | "||" | "+" | "-" | "*" | "/" | "%";
```

### [**Assignment Expression**](#)

*Assignment expression* set a new value for a give expression. It has a following form:
`<#expression#>` `=` `<#value#>`.

The value of *expression* is set to the value obtained by evaluating the *value*. Assignment expression
returns the value which was set to the *expression*.

##### [**Grammar of assignment expression**](#)

```ebnf
assignment-expresssion = expression assignment-operator expression;
assignment-operator = "=";
```

## [**Primary Expressions**](#)

*Primary expressions* are the most basic types of expressions. They can be used on their own and can
be combined with other tokens to produce prefix expresssions, binary expressions and/or postfix
expressions as well.

##### [**Grammar of primary expresssions**](#)

```ebnf
primary-expression = identifier | literal-expression | parenthesized-expression;
literal-expression = literal;
parenthesized-expresssion = "(" expression ")";
```

## [**Postfix Expressions**](#)

*Postfix expressions* are formed by applying postfix operator on a primary expression or other
postfix expression. In Dusk there are only two postfix operands: subcript operator `[]` which you can
use on any array-like type and a call operator `()`, which you can use to call functions.

##### [**Grammar of postfix expressions**](#)

```ebnf
postfix-expression = ( primary-expression | postfix-expression ) postfix-operand;
postfix-operand = subscript-operand | call-operand;

subcript-operand = "[" expresssion "]";
call-operand = "(" [ expresssion [ { "," expression } ] ] ")";
```

---

[Previous 'Types'](/docs/Language%20reference/Types.md) | [Next 'Statements'](/docs/Language%20reference/Statements.md)
