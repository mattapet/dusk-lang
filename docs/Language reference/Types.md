# [**Types**](#)

In Dusk there are two kinds of types: named types and compound types. *Named types* are the ones, that
consists of a single *identifier* token, such as `Int`.

*Compount types* are the ones, that consits
of a type without a name, defined in Dusk language itself. An example of a compount type is an array
type such as `Int[5]`.

## [**Table of Contents**](#)
- [**Types**](#types)
    - [**Table of Contents**](#table-of-contents)
         - [**Grammar of types**](#grammar-of-types)
    - [**Type Annotation**](#type-annotation)
        - [**Grammar of type annotations**](#grammar-of-type-annotations)
    - [**Type Identifier**](#type-identifier)
        - [**Grammar of type identifiers**](#grammar-of-type-identifiers)
    - [**Array types**](#array-types)
        - [**Grammar of arrays**](#grammar-of-arrays)
    - [**Type inference**](#type-inference)

---

##### [**Grammar of types**](#)

```ebnf
type = type-identifier | array-type;
```

## [**Type Annotation**](#)

A *type annotation* explicitely specifies the type of a variable declaration. Type annotation begins
with colon (`:`) followed by the type itself, as showed below:

```swift
let someArray: Int[3] = [1, 2, 3];
func someFunction(a: Int) { /* ... */ }
```

In the first example, the expression `someArray` is specified to have type `Int[3]`. In the second
example, the parameter `a` of function `someFunction` is specified to have type `Int`.

##### [**Grammar of type annotations**](#)

```ebnf
type-annotation = ":"  [ "inout" ] type;
```

## [**Type Identifier**](#)

*Type identifier* refers to a named type.

In Dusk there are only two named types: `Int` for integer values and `Void` type.

##### [**Grammar of type identifiers**](#)

```ebnf
type-identifier = "Int" | "Void";
```

## [**Array types**](#)

The Dusk language provides following syntax for specifying array types:

```swift
let someArray: Int[3] = [1, 2, 3];
let some2DArray: Int[3][2] = [[1, 2, 3], [1, 2, 3]];
```

In the first example, `someArray` is specified to have type array of integers of size three. In the second
example `some2DArray` is specified to have type 2D array consisting of two arrays of integers of size
three.

##### [**Grammar of arrays**](#)

```ebnf
array-type = type "[" expression "]";
```

## [**Type inference**](#)

Dusk uses type inferece whenever possible, saving you specifying the type of variables of expressions
explicitly. For example, instead of writing `let x: Int = 0;` you can write `let x = 0;` and let
the compiler to figure out the type for you.

---

[Previous 'About the Language Reference'](/docs/Language%20reference/index.md) | [Next 'Expressions'](/docs/Language%20reference/Expressions.md)
