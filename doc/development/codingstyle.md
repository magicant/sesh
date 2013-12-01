# Coding style guidelines

## Files

### File names

We use the following file name extensions.

- .hh for C++ normal header files
- .cc for C++ source files
- .tcc for C++ header files that define template class implementation
- .h for C/C++ header files
- .c for C source files

Normally, a .cc/.tcc/.c file has a corresponding header file that declares functions and/or defines classes that are defined in the source file.

### Include guards

A header file must have an include guard of the following form:

``` C++
#ifndef INCLUDED_filepath
#define INCLUDED_filepath

#endif // #ifndef INCLUDED_filepath
```

### Header inclusion

Every header/source file must `#include "buildconfig.h"` at the beginning of the file.
If the source file has a corresponding header, the header must be included immediately after "buildconfig.h".
Including the header first ensures its self-integrity.

And then, after a blank line, system headers (which are not included in the Sesh distribution) are included in alphabetical order in the `#include <name>` syntax.
Inclusion of Sesh header files follows in alphabetical order in the `"#include "name"` syntax.

## Formatting

### Line length

Every line in source files must be at most 79 characters.

Rationale:

- Long lines that don't fit in the editor window are just annoying, so we need a hard limit on line length. The most typical terminal width has been 80 characters for decades, we should not neglect that. (One column is reserved for +/- markers added by the diff tool.)
- A long line is a sign of a too complex statement. Consider splitting the statement into smaller ones or insert a line break. Having a large screen does not mean we can have a larger editor window and write longer lines.

### Line breaking

A declaration, statement, expression, etc. should be split into two or more lines if and only if it does not fit in the line length limit defined above. The second line (and the rest) should be indented two levels deeper than the first.

Split after, not before, a binary operator.
In a nested expression, prefer splitting at a shallower level of nesting.

``` C++
// Good
if (veryLongExpression1 &&
        (very + very + very + long * expression2));

// Bad
if (veryLongExpression1 && (very + very + very
        + long * expression2));
```

If you split a comma-separated list, break after each comma, even if some item is very short, and also break before the first item.

``` C++
fooFunction(
        bar,
        very + very + very + very + very + long +
                argument * expression,
        baz);
```

A block should be split after `{` and before `}`.

``` C++
if (condition) {
    foo();
    bar();
}
```

### Indentation and white spaces

Four spaces should be used for one indentation level.

Don't use tabs for indentation except when syntactically required as in Makefile. (Rationale: Since we enforce a line length limit as above, line length has to be consistent among all editors.)

No unnecessary white spaces should be left at the end of lines.

Contents of a namespace should be indented as deep as the namespace declaration. A `case` label should be aligned with the `switch`. Otherwise, code inside `{ }` should be indented one level deeper.

### If-else statements

If the body of an if statement ends with a jump statement like `break` and `return`, the if statement should have no else clause. An else clause should not end with a jump statement.

``` C++
// Good
if (!condition) {
    foo();
    return;
}
bar();
baz();

// Bad
if (condition) {
    bar();
} else {
    foo();
    return;
}
baz();
```

Exception: If the bodies of an if and else are symmetric and reasonably short, they can end with a jump statement.

``` C++
// Okay
if (x >= y)
    return x - y;
else
    return y - x;
```

### Empty loop body

``` C++
// Prefer
while (foo()) { }

// Avoid
while (foo()) ;
```

Rationale: Using braces is consistent with empty constructor/destructor bodies.

## Types

### Pointers

Raw pointer types should be avoided. To clarify how the pointed-to object is managed, pointers to an object allocated by the new operator should always be wrapped in std::unique_ptr or std::shared_ptr.

The pointed-to object type of a std::shared_ptr should normally be const-qualified. Sharing a modifiable object is typically a bad design.

## Doxygen comments

## Other C++ features

### Forward declaration

Avoid forward declaration of incomplete types except when necessary to define recursively depending types.
When a declaration/definition of a symbol depends on a type declared in another header, just include the header.

Rationale:

- Forward declaration duplicates declaration of the same symbol. It violates the DRY principle.
- Some says forward declaration reduces compilation time, but parsing a few more header files can hardly be a bottleneck in compilation.
- Some says forward declaration improves modularity. It is true that forward declaration decreases header dependency, avoiding unnecessary recompilation after a header change. But it is more important to reduce class dependency than header dependency because class dependency is more essential to better design. Don't obscure class dependency by reducing less essential header dependency.

### Function inlining

Defaulted special member functions should always be defined in the class type definition, rather than defined out-of-line in a single compilation unit.

Rationale:

- An explicitly defaulted definition in the class type definition implicitly infers a suitable exception specification. If defined out-of-line, we would need to give a correct exception specification explicitly, which is more error-prone.
- Compared to other functions, it is more difficult to keep track of whether a special member function should be inlined or not, especially when its base class or the types of the data members are changed later.
- Out-of-line defintions of those functions may make the final binary larger if they are unused.

A disadvantage of the above rule is that it may make compilation time longer.

Other functions should be inlined only if their body is extremely simple.
