# [**Declarations**](#)

A *declaration* introduces a new name to the program. Sincle Dusk is very simple language, you can
only delcare *variables* and *functions*. In Dusk, most declarations are also definitions since they
also provide initialization or deifnition of the declaration. The only exception is an `extern`
declaration, which only declares an external function, but does not require it's definition.


## [**Table of Contents**](#)
- [**Declarations**](#declarations)
    - [**Table of Contents**](#table-of-contents)
                - [**Grammar of Declarations**](#grammar-of-declarations)
    - [**Top-Level Code**](#top-level-code)
        - [**Constant Declaration**](#constant-declaration)
                - [**Grammar of Constant Declaration**](#grammar-of-constant-declaration)
    - [**Variable declararion**](#variable-declararion)
                - [**Grammar of Variable Declarations**](#grammar-of-variable-declarations)
    - [**Function declaration**](#function-declaration)
                - [**Grammar of Function Declaration**](#grammar-of-function-declaration)
        - [**Parameter Declaration**](#parameter-declaration)
                - [**Gramamr of Parameter Declarations**](#gramamr-of-parameter-declarations)
        - [**Extern Declaration**](#extern-declaration)
                - [**Grammar of Extern Declaration**](#grammar-of-extern-declaration)


##### [**Grammar of Declarations**](#)

```ebnf
delcaration = variable-declaration | constatn-declaration
            | function-declaration | extern-declaration;
```

## [**Top-Level Code**](#)

Top level code in Dusk consists of zero or more declarations. Top level code cannot contain any
expressions and/or control statements.

All of the top-level declarations in Dusk are considered *global* and since there are no expressions
permitted, all initializations of there declarations must be known at compile time.


### [**Constant Declaration**](#)

A *constant declaration* introduce a constant name value into the program. Constant declarartions are
using a `let` keyword and have following form:

```swift
let <#constant name#>: <#type#> = <#expression#>
```

Aconstant declaration defines an immutable binding between the constant name and the value of
the initializer expression; after the value of a constant is set, it cannot be changed. That said
if a constant is initialized with a reference type such as array, the referenced value itself cannot
change.

##### [**Grammar of Constant Declaration**](#)

```ebnf
constant-declaration = "let" identifier [ type-annotation ] "=" expression;
```

## [**Variable declararion**](#)

A *variable declaration* a variable named value in into the program. The variables are decalred with
a `var` keyword and have following form:

```swift
var <#variable name#>: <#type#> = <#expresssion#>
```

Compared to constants, variables does not have to be initialized at declaration. If no value is
assigned, an undefined value is going to be assigned to the variable. **Note**: type of variable
needs to be known at declaration, therefor ommiting type annotation when declaring a variable
without an initial value is not permitted.


##### [**Grammar of Variable Declarations**](#)

```ebnf
variable-declarations = "var" identifier [ type-annotation ] [ = expression ];
```

## [**Function declaration**](#)

A *function declaration* introduces a function to into the program. Function declaration are
declared using a `func` keyword and have following form:

```swift
func <#function name#>(<#parameters#>) -> <#return type#> {
    <#statements#>
}
```

If function returns `Void`, the return type specifier can be ommited as follows:

```
func <#function name#>(<#parameters#>) {
    <#statements#>
}
```

The type of each parameter **must** be included - it cannot be infered. The parameters are by default
constants. If a parameter is a reference type, the parameter can be specified to be **In-Out**,
which means it can be modify referenced value inside function's scope.

##### [**Grammar of Function Declaration**](#)

```ebnf
function-declaration = "func" identifier "(" parameters ")" [ "->" type ] "{" statements "}";
```


### [**Parameter Declaration**](#)

Each parameter declaration consists of parameter name and it's type such as:

```swift
<#parameter name#>: <#parameter type#>
```

The parameter name is used to access it's value inside the function's scope such as:

```swift
func add(x: Int, y: Int) -> Int { return x + y;  }
add(1, 2);
```

##### [**Gramamr of Parameter Declarations**](#)

```ebnf
parameters = parameter [ { "," parameter } ];
parameter = identifier type-annotation;
```

### [**Extern Declaration**](#)

An *Extern declaration* is used to declare a function in your programme without actually definint it.
*Extern declaration* has the following form:

```swift
extern func <#function name#>(<#parameter list#>)
```

##### [**Grammar of Extern Declaration**](#)

```ebnf
extern-declaration = "extern" "func" identifier "(" parameters ")";
```

---

[Previous 'Statements'](/docs/Language%20reference/Statements.md)
