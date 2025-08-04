# elegant

Elegant is a C library that brings Haskell-like functional programming features to the C language.

Elegant provides a set of macros and pure functions to make C support many of the features found in functional programming languages, especially those inspired by Haskell. It enables you to write C code in a functional, declarative style with automatic memory management and type safety.

## Features

- **Haskell-like functional programming macros and idioms**
- **Pure functions and immutability support**
- **Higher-order functions** (functions as arguments and return values)
- **Function composition, currying, and partial application**
- **Map, filter, reduce, and other collection operations**
- **Automatic memory management** with configurable algorithms
- **Custom struct registration** for type-generic operations
- **Inline lambda expressions** for concise functional code
- **Error handling** with Maybe/Option and Either types
- **Pre-compiler checks** for type safety and memory leak detection
- **Declarative and expressive code style**

## Key Design Principles

- **Zero manual memory management** - Elegant handles allocation and deallocation automatically
- **Type-generic operations** - Works with built-in types and custom structs
- **Inline functional syntax** - Write lambda-like expressions directly in code
- **Automatic currying** - Functions are curried by default for partial application
- **Safe by default** - Pre-compiler checks prevent common errors
- **C99+ compatible** - Works with modern C compilers

## Quick Example

```c
#include "elegant.h"

int main() {
    // Automatic allocation, inline lambda, no manual memory management
    AUTO(numbers, ELEGANT_ARRAY(int, 1, 2, 3, 4, 5));
    
    // Map with inline expression - no function definition needed
    AUTO(squares, MAP(x, numbers, x * x));
    
    // Filter with inline predicate  
    AUTO(evens, FILTER(x, numbers, x % 2 == 0));
    
    // Sequential composition (C99 compatible)
    AUTO(step1, MAP(x, numbers, x * 2));
    AUTO(result, FILTER(x, step1, x > 5));
    
    // Memory automatically freed when variables go out of scope
    return 0;
}
```

See the documentation in `docs/overview.md` for more details and examples.