# [**Statements**](#)

In Dusk, there are two kinds of statements: simple statements and control flow statements. Simple
statements are the most common and consist of either an expression of a declaration.

Control flow statements are to control flow of the programs execution. There are several types of
control flow statements like loops, conditional statements, branching statements and more.

## [**Table of Contents**](#)
- [**Statements**](#statements)
    - [**Table of Contents**](#table-of-contents)
                - [**Grammar of Statements**](#grammar-of-statements)
    - [**Loop statements**](#loop-statements)
                    - [**Grammar of a loop statement**](#grammar-of-a-loop-statement)
        - [**For-in Statement**](#for-in-statement)
                - [**Grammar of a For-in Statement**](#grammar-of-a-for-in-statement)
        - [**While Statement**](#while-statement)
                - [**Grammar of While Statements**](#grammar-of-while-statements)
    - [**Branch Statements**](#branch-statements)
        - [**If Statement**](#if-statement)
                - [**Grammar of If Statement**](#grammar-of-if-statement)
        - [**Break Statement**](#break-statement)
                - [**Grammar of Break Statement**](#grammar-of-break-statement)
    - [**Return Statement**](#return-statement)
                - [**Grammar of Return Statement**](#grammar-of-return-statement)
    - [**Range Statement**](#range-statement)
                - [**Grammar of Range Statements**](#grammar-of-range-statements)

##### [**Grammar of Statements**](#)

```ebnf
statements = [ { statement } ];
statement = expression ";" | declaration ";" | loop-statement | branch-statement;
```

---

## [**Loop statements**](#)

Loop statements, as expected, allow a block of code to be executed repeatedly, depending on the
condition. Dusk has two kind of loop statements: a `for-in` statement and a `while` statement.

Control flow in the loop statements can be changed by a `break` and/or `continue` statements, and is
discussed in [Break Statement](#break-statement) and [Control Statement](#control-statement) sections
below.

###### [**Grammar of a loop statement**](#)

```ebnf
loop-statement = for-in-statement | while-statement;
```

### [**For-in Statement**](#)

A `for-in` statement allows a code block to be executed for each item in the [Range Statement](#range-statement).

A `for-in` statement has the following form:

```swift
for <#iterator#> in <#range-statement#> {
    <#statements#>
}
```

##### [**Grammar of a For-in Statement**](#)

```ebnf
for-in-statement = "for" identifier "in" range-statement "{" statements "}";
```

### [**While Statement**](#)

A `while` loop allows a block of code to be executed repeatedly, as long as the condition remains
true.

A `while` loop has the following form:

```swift
while <#condition#> {
    <#statements#>
}
```

A condition of a `while` statement is evaluated before execution of it's block, therefore the block
of the while loop statement can be executed zero or more times.

##### [**Grammar of While Statements**](#)

```ebnf
while-statement = "while" expression "{" statements "}";
```


## [**Branch Statements**](#)

*Branch statements* allows to change the flow of the programs execution depending one zero of more
values.


### [**If Statement**](#)

An `if` statement is used for executing code based on the evaluation of a single condition.

There are two basic form of an `if` statement. In each, opening and closing braces are **required**.

The first form allows a code to be executed if condition is evaluated to be true and has following
form:

```swift
if <#condition#> {
    <#statements#>
}
```

The second form, also known as `if-else` statement has also a second path the condition evaluation
can cause to execute. Beware, the Dusk **does not** support multiple `if-else` statements.
Therefor the only valid `if-else` statement is with a single `else` clause which has a following form:

```swift
if <#condition#> {
    <#statements to execute when condition is true#>
} else {
    <#statements to execute when condition is false#>
}
```

**Note**: Since Dusk does not have booleans, it supports truthly and falsly values.


##### [**Grammar of If Statement**](#)

```ebnf
if-statement = "if" expression "{" statements "}" [ "else" "{" statements "}" ];
```

### [**Break Statement**](#)

A `break` statement end an execution of a loop statement. A `break` statement must consist of only
a `break` keyword and has a following form form:

```swift
break
```

When a `break` statements is encountered, the execution jumps out of the current statement block to
the first statement after the parent statement block.


##### [**Grammar of Break Statement**](#)

```ebnf
break-statement = "break";
```


## [**Return Statement**](#)

A `return` statement appears in the body of a function definition and causes program execution to
return from to the calling function. Program execution continues at the point immediately following
the function call.

A `return` statement can consist of only the `return` keyword, or it can consist of the `return`
keyword followed by an expression, as shown below:

```swift
return

return <#expression#>
```

When a `return` statement is followed by an expresssion, the value of the expression is returned
to the calling function. **Note**: type of the expresssion must match the return type specified in
the function declaration.

##### [**Grammar of Return Statement**](#)

```ebnf
return-statement = "return" [ expression ];
```


## [**Range Statement**](#)

*Range statement* is used to express a range of values. Range statements are used purely in `for-in`
statements and have following form:

```swift
<#expression#> range-operator <#expression#>
```

Range statement express a range of values. The range can be ascending or descending depending on
values provided to the range statement and is resolved at runtime. There are two kinds of ranged:
*inclusive* and *exclusive*. *Inclusive* range statements use `...` operator and represents
an inclusive range of values such as `[Start, end]`.

*Exclusive* range staements use `..` and represent an exlusive range of values, such as `[Start, End)`.

##### [**Grammar of Range Statements**](#)

```ebnf
range-statement = inclusive-range-statement | exlusive-range-statement;
inclusive-range-statement = expression "..." expression;
exclusive-range-statement = expression ".." expression;
```

---

[Previous 'Expresssions'](/docs/Language%20reference/Expressions.md) | [Next 'Declarations'](/docs/Language%20reference/Declarations.md)
