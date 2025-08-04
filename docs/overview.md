# Elegant Documentation v0.0.1

Elegant is a functional programming library for C inspired by Haskell, designed to bring Haskell-like functional programming paradigms to the C language. It provides a set of macros and pure functions to enable functional programming features in C, making it easier to write code in a functional, declarative style with automatic memory management and type safety.

## Building and Installation

### Quick Start

```bash
# Configure the build system
./configure --enable-debug --enable-examples

# Build the library and examples
make

# Run the test suite
make check

# Install system-wide (optional)
sudo make install
```

### Build Requirements

- **C Compiler**: GCC 4.9+, Clang 3.5+, or compatible C99 compiler
- **Build Tools**: GNU Autotools (autoconf, automake, libtool), make
- **System Libraries**: Standard C library, libm (math library)

### Configure Options

The autotools build system provides several configuration options:

```bash
# Enable debug mode with memory tracking and assertions
./configure --enable-debug

# Build example programs in examples/ directory
./configure --enable-examples  

# Custom installation prefix
./configure --prefix=/opt/elegant

# Show all configuration options
./configure --help
```

### Development Workflow

```bash
# Full development build
./configure --enable-debug --enable-examples
make clean && make
make check

# Test installation to staging area
make DESTDIR=/tmp/elegant-test install

# Create distribution package
make dist
```

### Using the Library

After installation, include in your C programs:

```c
#include <elegant.h>

// Your functional C code here
```

Compile your programs with:

```bash
# Using pkg-config (recommended)
gcc myprogram.c $(pkg-config --cflags --libs elegant) -o myprogram

# Manual linking
gcc myprogram.c -lelegant -lm -o myprogram
```

## Features

- **Haskell-like Functional Programming Macros**: Inline lambda expressions and functional operations inspired by Haskell's syntax
- **Automatic Memory Management**: Multiple allocation strategies with automatic cleanup - no manual malloc/free required
- **Pure Functions**: A collection of pure functions that avoid side effects and make code more predictable
- **Immutability Support**: Tools and patterns to encourage immutable data handling
- **Higher-Order Functions**: Support for passing functions as arguments and returning functions from other functions
- **Function Composition & Currying**: Utilities for advanced function manipulation with automatic currying
- **Type-Generic Operations**: Map, filter, reduce work with any type including custom structs
- **Custom Type Registration**: Simple macros to register custom structs for automatic memory management
- **Error Handling**: Maybe/Option and Either types for safe error handling
- **Pre-compiler Validation**: Automatic checks for type safety, memory leaks, and pure function verification
- **Declarative Style**: Write expressive, Haskell-like code in C

## Memory Management

Elegant provides automatic memory management with multiple strategies:

### Stack-based Arena (Default)
```c
ELEGANT_SCOPE {
    ```c
    AUTO(arr, ELEGANT_ARRAY(int, 1, 2, 3, 4, 5));
    AUTO(result, MAP(x, arr, x * x));
    // Memory automatically freed at end of scope
```
}
```

### Reference Counting
```c
ELEGANT_SET_MODE(REFERENCE_COUNTING);
AUTO(shared_data, ELEGANT_ARRAY(int, 1, 2, 3));
AUTO(view1, shared_data);  // ref count increases
AUTO(view2, shared_data);  // ref count increases  
// Memory freed when all references are released
```

### Garbage Collection
```c
ELEGANT_SET_MODE(GARBAGE_COLLECTION);
// Periodic cleanup of unused memory
AUTO(data, MAP(x, source, expensive_computation(x)));
// Memory freed during next GC cycle
```

## Error Handling

Elegant provides functional error handling patterns:

### Maybe/Option Types
```c
ELEGANT_MAYBE(int) divide(int a, int b) {
    if (b == 0) return ELEGANT_NONE(int);
    return ELEGANT_SOME(int, a / b);
}

AUTO(result, divide(10, 2));
if (ELEGANT_IS_SOME(result)) {
    printf("Result: %d\n", ELEGANT_UNWRAP(result));
}
```

### Either Types
```c
ELEGANT_EITHER(const char*, int) parse_int(const char* str) {
    // ... parsing logic
    if (error) return ELEGANT_LEFT(const char*, "Parse error");
    return ELEGANT_RIGHT(int, parsed_value);
}
```

## Custom Type Registration

Register custom structs for automatic memory management and type-generic operations:

```c
typedef struct {
    char name[32];
    int age;
} Person;

ELEGANT_REGISTER_TYPE(Person,
    .copy = person_copy,
    .free = person_free,
    .compare = person_compare
);

// Now Person works with all Elegant operations
AUTO(people, ELEGANT_ARRAY(Person, 
    {"Alice", 25}, {"Bob", 30}, {"Charlie", 35}
));
AUTO(adults, FILTER(p, people, p.age >= 18));
```

## Pre-compiler Validation

Elegant includes compile-time checks for:

- **Type Safety**: Ensures function arguments match expected types
- **Memory Safety**: Detects potential memory leaks
- **Pure Function Verification**: Validates functions have no side effects
- **Performance Optimization**: Suggests more efficient alternatives

Enable different validation modes:
```c
#define ELEGANT_MODE_DEBUG     // Full validation, slower compilation
#define ELEGANT_MODE_SAFE      // Basic safety checks
#define ELEGANT_MODE_FAST      // Minimal checks, fastest compilation
```

## Compiler Requirements & Compatibility

### C99 Compatibility
Elegant works with C99 and later standards. For automatic type deduction, use the `AUTO` macro:
```c
// C99 compatible approach
#define AUTO(var, expr) __typeof__(expr) var = (expr)

// Usage:
AUTO(numbers, ELEGANT_ARRAY(int, 1, 2, 3));
AUTO(result, MAP(x, numbers, x * x));

// Or for GCC/Clang users who want 'auto' keyword:
#ifdef __GNUC__
#define auto __auto_type
#endif
```

### Dependencies
Elegant only requires standard C library headers and avoids GPL dependencies:
```c
#include <stddef.h>    // For size_t
#include <stdlib.h>    // For memory allocation
#include <string.h>    // For memory operations
// Optional: <math.h> for mathematical operations
```

### Compiler Flags
Recommended compilation flags:
```bash
gcc -std=c99 -DELEGANT_MODE_SAFE your_code.c
# Debug mode with macro expansion:
gcc -std=c99 -DELEGANT_MODE_DEBUG -DELEGANT_DEBUG_MACROS your_code.c
```

## Thread Safety

Elegant uses **thread-local memory management** by default. Each thread has its own memory arena, providing:

- **No synchronization overhead** - Maximum performance
- **Simple implementation** - No complex locking mechanisms
- **Thread isolation** - Memory operations don't interfere between threads
- **User responsibility** - Thread safety for shared data is handled by the user

```c
// Each thread gets its own memory space
void* worker_thread(void* arg) {
    // This thread has its own Elegant memory arena
    AUTO(local_data, ELEGANT_ARRAY(int, 1, 2, 3, 4));
    AUTO(result, MAP(x, local_data, x * x));
    
    // Memory automatically cleaned up when thread exits
    return NULL;
}

// Sharing data between threads (user responsibility)
typedef struct {
    ELEGANT_MUTEX mutex;
    int* shared_array;
    size_t length;
} SharedData;

void safe_access(SharedData* data) {
    ELEGANT_LOCK(data->mutex);
    // Access shared_array safely
    ELEGANT_UNLOCK(data->mutex);
}
```

## Getting Started

1. **Installation**: Include the Elegant header in your C project
2. **Include the Header**: `#include "elegant.h"`
3. **Choose Memory Mode**: Set your preferred memory management strategy (optional)
4. **Write Functional Code**: Use inline lambda expressions and functional operations

## Example Usage

### Basic Functional Operations
```c
#include "elegant.h"

int main() {
    // Create arrays with automatic memory management (C99 compatible)
    AUTO(numbers, ELEGANT_ARRAY(int, 1, 2, 3, 4, 5));
    
    // Map with inline lambda - no function definition needed
    AUTO(squares, MAP(x, numbers, x * x));
    
    // Filter with inline predicate
    AUTO(evens, FILTER(x, numbers, x % 2 == 0));
    
    // Reduce with inline operation
    AUTO(sum, REDUCE(a, b, numbers, a + b, 0));
    
    // Sequential composition (instead of complex COMPOSE)
    AUTO(doubled, MAP(x, numbers, x * 2));
    AUTO(filtered, FILTER(x, doubled, x > 10));
    
    return 0;
}
```

### Understanding Sequential Composition
Instead of complex pipeline syntax, Elegant uses sequential operations that are C99 compatible:
```c
// Sequential functional operations (C99 compatible)
AUTO(numbers, ELEGANT_ARRAY(int, 1, 2, 3, 4, 5));
AUTO(step1, MAP(x, numbers, x * x));        // Square each number
AUTO(step2, FILTER(x, step1, x > 10));      // Keep values > 10
AUTO(final, REDUCE(a, b, step2, a + b, 0)); // Sum the results

// This is equivalent to the mathematical composition:
// final = reduce(filter(map(numbers, square), gt10), add, 0)
```

### Advanced Examples
```c
// Simple function binding (instead of automatic currying)
#define ADD(a) ELEGANT_BIND(add_fn, a)
int add_fn(int a, int b) { return a + b; }

AUTO(add5, ADD(5));  // Creates a function that adds 5
int result = add5(10);  // Result: 15

// Custom struct operations
typedef struct { int x, y; } Point;
ELEGANT_REGISTER_TYPE(Point, 
    .copy = point_copy,
    .free = point_free,
    .compare = point_compare
);

AUTO(points, ELEGANT_ARRAY(Point, (Point){0,0}, (Point){1,1}, (Point){2,2}));
AUTO(distances, MAP(p, points, sqrt(p.x*p.x + p.y*p.y)));

// Error handling with Maybe
ELEGANT_MAYBE(int) safe_divide(int a, int b) {
    if (b == 0) return ELEGANT_NONE(int);
    return ELEGANT_SOME(int, a / b);
}

AUTO(result, safe_divide(10, 2));
if (ELEGANT_IS_SOME(result)) {
    printf("Result: %d\n", ELEGANT_UNWRAP(result));
}
```

## Performance Considerations

### Memory Layout & Array Limits
```c
// ELEGANT_ARRAY automatically checks size limits
AUTO(small_array, ELEGANT_ARRAY(int, 1, 2, 3));  // ✓ OK
// AUTO(big_array, ELEGANT_ARRAY(int, /* > 1000 elements */));  // Compile-time warning

// Manual size checking
ELEGANT_STATIC_ASSERT(ELEGANT_ARRAY_SIZE <= ELEGANT_MAX_STACK_ARRAY);
```

### Performance vs Hand-written C
- **Functional operations**: 5-15% overhead due to function pointers
- **Memory management**: 10-20% overhead for automatic cleanup
- **Compile-time optimizations**: Can eliminate overhead in release builds
- **Cache-friendly**: Elegant uses contiguous memory layouts

```c
// Performance mode - minimal overhead
#define ELEGANT_OPTIMIZE_PERFORMANCE
AUTO(result, MAP(x, array, x * x));  // Can be optimized to simple loop
```

## Debugging Support

### Debug Modes
```c
#define ELEGANT_DEBUG_MACROS    // Expand macros for better debugging
#define ELEGANT_DEBUG_MEMORY    // Track memory allocations
#define ELEGANT_DEBUG_VERBOSE   // Print operation details
```

### Debugging Macro-Heavy Code
```c
// Debug mode expands macros to readable functions
#ifdef ELEGANT_DEBUG_MACROS
#define MAP(x, arr, expr) elegant_debug_map(__FILE__, __LINE__, #expr, ...)
#endif

// Memory debugging
AUTO(array, ELEGANT_ARRAY(int, 1, 2, 3));
// Debug output: "Allocated 12 bytes at main.c:42"
```

### Integration with Existing Code
```c
// Legacy C code integration
void legacy_function(int* data, size_t len) {
    // existing C code
}

// Elegant wrapper
AUTO(elegant_data, ELEGANT_ARRAY(int, 1, 2, 3, 4));
legacy_function(ELEGANT_RAW_PTR(elegant_data), ELEGANT_LENGTH(elegant_data));
// Memory still managed by Elegant
```

## Error Messages & Diagnostics

### Compile-time Error Messages
Elegant provides clear error messages using standard preprocessor directives:

```c
// Type mismatch detection
AUTO(numbers, ELEGANT_ARRAY(int, 1, 2, 3));
AUTO(result, MAP(x, numbers, x + "invalid"));  
// Error: #error "Type mismatch in MAP operation: expected numeric type"

// Size limit warnings
// AUTO(big_array, ELEGANT_ARRAY(int, /* > 1000 elements */));
// Warning: #warning "ELEGANT_ARRAY size exceeds recommended limit (1000 elements)"

// Memory safety checks
AUTO(ptr, ELEGANT_RAW_PTR(freed_array));
// Error: #error "Accessing raw pointer of freed memory"
```

### Common Error Patterns
- **Macro Expansion Errors**: Use `ELEGANT_DEBUG_MACROS` to see expanded code
- **Type Safety Violations**: Clear messages about expected vs actual types  
- **Memory Lifecycle Errors**: Warnings about accessing freed memory
- **Performance Warnings**: Suggestions for optimization opportunities

## Memory Model Details

### Reference Counting Lifecycle
```c
AUTO(data, ELEGANT_ARRAY(int, 1, 2, 3));     // ref_count = 1
AUTO(copy1, data);                           // ref_count = 2  
AUTO(copy2, data);                           // ref_count = 3
// copy1 goes out of scope                   // ref_count = 2
// copy2 goes out of scope                   // ref_count = 1
// data goes out of scope                    // ref_count = 0 -> auto-freed
```

### Memory Alignment
```c
// Default: Aligned to natural boundaries for performance
AUTO(aligned_data, ELEGANT_ARRAY(double, 1.0, 2.0, 3.0));  // 8-byte aligned

// Disable alignment for space optimization
#define ELEGANT_NO_ALIGNMENT
AUTO(packed_data, ELEGANT_ARRAY(char, 'a', 'b', 'c'));     // 1-byte aligned
```

### ELEGANT_SCOPE Implementation
```c
ELEGANT_SCOPE {
    // Creates a new memory arena/stack frame
    AUTO(local_data, ELEGANT_ARRAY(int, 1, 2, 3));
    AUTO(result, MAP(x, local_data, x * x));
    // All allocations in this scope are freed when exiting
}
// Memory automatically reclaimed here
```

## Implementability Analysis

### ✅ **Definitely Implementable in C99**
```c
// Core functional operations using _Generic and __typeof__
AUTO(arr, ELEGANT_ARRAY(int, 1, 2, 3, 4, 5));              // ✅ Macro-based array creation
AUTO(mapped, MAP(x, arr, x * 2));                          // ✅ Statement expressions + loops
AUTO(filtered, FILTER(x, arr, x > 5));                     // ✅ Conditional element selection  
AUTO(sum, REDUCE(a, b, arr, a + b, 0));                    // ✅ Fold operations

// Memory management with arenas
ELEGANT_SCOPE { /* auto cleanup */ }                       // ✅ Scope-based cleanup
ELEGANT_SET_MODE(REFERENCE_COUNTING);                      // ✅ Basic ref counting

// Basic error handling
ELEGANT_MAYBE(int) result = safe_operation();              // ✅ Tagged unions
ELEGANT_EITHER(error_t, int) parse_result;                 // ✅ Sum types

// Simple function binding
#define BIND(fn, arg1) /* creates bound function */         // ✅ Macro-based currying
```

### 🔶 **Implementable but Complex**
```c
// Type registration (requires significant macro work)
ELEGANT_REGISTER_TYPE(CustomStruct, .copy=fn, .free=fn);   // 🔶 Complex but doable

// Basic sorting and grouping
AUTO(sorted, SORT_BY(compare_fn, arr));                    // 🔶 Requires qsort wrapper
AUTO(grouped, GROUP_BY(key_fn, arr));                      // 🔶 Requires hash tables

// String operations with limitations
AUTO(words, SPLIT(" ", "hello world"));                    // 🔶 Limited string handling
AUTO(trimmed, TRIM(string));                               // 🔶 Basic string manipulation

// Array operations requiring dynamic growth
AUTO(appended, ELEGANT_APPEND(arr, element));              // 🔶 Requires realloc logic
AUTO(flattened, FLATTEN(nested_array));                    // 🔶 Nested structure handling
```

### ❌ **Not Realistically Implementable in C99**
```c
// Complex streaming and lazy evaluation
AUTO(stream, STREAM_FROM_SOCKET(fd));                      // ❌ Requires coroutines/state machines
AUTO(lazy_results, STREAM_MAP(fn, infinite_stream));       // ❌ Lazy evaluation is very hard

// Advanced file I/O with automatic parsing
AUTO(lines, READ_LINES("file.txt"));                       // ❌ Too much file handling complexity
WRITE_LINES("output.txt", processed_data);                 // ❌ Complex output formatting

// Deep function composition with placeholders  
AUTO(pipeline, COMPOSE(MAP(x,_,f(x)), FILTER(x,_,g(x))));  // ❌ Placeholder syntax impossible

// Advanced testing framework features
ELEGANT_MEMORY_TEST_BEGIN(); /* framework integration */   // ❌ Requires complex infrastructure
assert(ELEGANT_EQUAL(complex_struct1, complex_struct2));   // ❌ Deep equality for arbitrary types

// Complex thread synchronization
ELEGANT_PARALLEL_MAP(expensive_fn, huge_array);            // ❌ Parallel processing framework
```

## Limitations & Compatibility

### Realistic C99 Constraints
```c
// What we CAN implement well:
AUTO(numbers, ELEGANT_ARRAY(int, 1, 2, 3));               // ✅ Fixed-size arrays
AUTO(doubled, MAP(x, numbers, x * 2));                    // ✅ Simple transformations
AUTO(evens, FILTER(x, numbers, x % 2 == 0));             // ✅ Filtering operations
AUTO(sum, REDUCE(a, b, numbers, a + b, 0));              // ✅ Reduction operations

// What has significant limitations:
// - No true lazy evaluation (everything is eager)
// - No automatic currying (manual macro-based binding only)
// - No complex composition syntax with placeholders
// - Limited string processing capabilities
// - Basic file I/O only (no automatic parsing)
// - Thread-local memory management only (no shared state synchronization)

// Elegant works best for:
// - Data transformation pipelines on known-size arrays
// - Simple functional operations with immediate evaluation  
// - Basic memory management with scope-based cleanup
// - Mathematical computations in functional style
// - Simple error handling with Maybe/Either types

// NOT recommended for:
// - Complex streaming data processing
// - Advanced file format parsing
// - Parallel/concurrent functional programming
// - Large-scale data processing requiring lazy evaluation
// - Integration with complex external libraries
```


## Documentation

- [API Reference](./api.md)
- [Examples](./examples.md)
- [FAQ](./faq.md)

## Functional Operations Roadmap

### Phase 1: Core Operations (Definitely Implementable)
```c
// Basic array operations - these WILL work in C99
AUTO(arr, ELEGANT_ARRAY(int, 1, 2, 3, 4, 5));            // ✅ Array creation
AUTO(mapped, MAP(x, arr, x * 2));                        // ✅ Element transformation
AUTO(filtered, FILTER(x, arr, x > 5));                   // ✅ Element filtering
AUTO(sum, REDUCE(a, b, arr, a + b, 0));                  // ✅ Reduction operations
AUTO(length, ELEGANT_LENGTH(arr));                       // ✅ Array length
AUTO(reversed, REVERSE(arr));                            // ✅ Array reversal
AUTO(taken, TAKE(3, arr));                               // ✅ Take first N elements
AUTO(dropped, DROP(2, arr));                             // ✅ Skip first N elements
```

### Phase 2: Advanced Operations (Complex but Possible)
```c
// These require significant implementation effort
AUTO(sorted, SORT_BY(compare_fn, arr));                  // 🔶 Wrapper around qsort
AUTO(found, FIND(x, arr, x > 10));                      // 🔶 Linear search with predicate
AUTO(indexed, ENUMERATE(arr));                           // 🔶 Add index to each element
AUTO(zipped, ZIP(arr1, arr2));                          // 🔶 Combine two arrays
AUTO(unique, UNIQUE(arr));                               // 🔶 Remove duplicates (requires compare)
```

### Phase 3: Aspirational Features (Probably Not Feasible)
```c
// These are documented but unlikely to be fully implemented
// AUTO(grouped, GROUP_BY(key_fn, arr));                 // ❌ Requires complex data structures
// AUTO(flattened, FLATTEN(nested_array));               // ❌ Nested type handling is hard
// AUTO(lines, READ_LINES("file.txt"));                  // ❌ File I/O is too complex
// AUTO(stream, STREAM_FROM_SOCKET(fd));                 // ❌ Streaming requires state machines
// AUTO(words, SPLIT(" ", "hello world"));               // ❌ String handling is complex
```

## Implementation Recommendations

### What to Implement First (MVP)
1. **AUTO() macro** - `#define AUTO(var, expr) __typeof__(expr) var = (expr)`
2. **ELEGANT_ARRAY()** - Stack-based array creation with compile-time size
3. **MAP/FILTER/REDUCE** - Core functional operations using statement expressions
4. **ELEGANT_SCOPE** - Basic scope-based memory management
5. **Maybe/Either types** - Simple tagged unions for error handling

### What to Implement Second (Enhanced Features)
1. **ELEGANT_REGISTER_TYPE()** - Basic type registration for custom structs
2. **Reference counting** - Optional memory management mode
3. **TAKE/DROP/REVERSE** - Simple array manipulation operations
4. **Basic SORT_BY** - Wrapper around standard qsort function

### What to Document as Future/Aspirational
1. **Complex streaming** - Mark as "future research" or "not in scope"
2. **Advanced file I/O** - Recommend users use standard C file operations
3. **Parallel processing** - Out of scope for this library
4. **Complex string operations** - Recommend existing string libraries

### Honest Feature Matrix
| Feature | Implementability | Complexity | Recommendation |
|---------|-----------------|------------|----------------|
| MAP/FILTER/REDUCE | ✅ High | Low | Implement in v1.0 |
| AUTO() macro | ✅ High | Low | Implement in v1.0 |
| ELEGANT_ARRAY | ✅ High | Medium | Implement in v1.0 |
| Maybe/Either types | ✅ High | Low | Implement in v1.0 |
| SORT_BY | 🔶 Medium | Medium | Implement in v1.1 |
| GROUP_BY | 🔶 Low | High | Consider for v2.0 |
| READ_LINES/WRITE_LINES | ❌ Very Low | Very High | Not recommended |
| STREAM_* operations | ❌ Very Low | Very High | Not recommended |
| Complex COMPOSE | ❌ Very Low | Very High | Not recommended |

## Testing & Validation

### Unit Testing with Elegant (Basic Implementation)
```c
// Basic testing approach (without complex test framework integration)
#include "elegant.h"
#include <assert.h>

void test_map_operation() {
    AUTO(input, ELEGANT_ARRAY(int, 1, 2, 3));
    AUTO(result, MAP(x, input, x * 2));
    
    // Manual verification (ELEGANT_EQUAL would be complex to implement)
    assert(ELEGANT_LENGTH(result) == 3);
    assert(ELEGANT_RAW_PTR(result)[0] == 2);
    assert(ELEGANT_RAW_PTR(result)[1] == 4);
    assert(ELEGANT_RAW_PTR(result)[2] == 6);
}

// Basic memory tracking (simplified)
void test_memory_safety() {
    size_t initial_memory = elegant_get_allocated_bytes(); // If implemented
    {
        AUTO(data, ELEGANT_ARRAY(int, 1, 2, 3));
        AUTO(processed, MAP(x, data, x * x));
        // Memory should be automatically freed
    }
    size_t final_memory = elegant_get_allocated_bytes();
    assert(final_memory == initial_memory); // Check for leaks
}
```

### Testing Frameworks Compatibility
```c
// Works well with popular C testing frameworks:
// - Unity Test Framework
// - Criterion
// - CMocka  
// - Check

// Example with Unity:
#include "unity.h"
#include "elegant.h"

void test_functional_pipeline() {
    AUTO(input, ELEGANT_ARRAY(int, 1, 2, 3, 4, 5));
    
    // Sequential operations instead of COMPOSE (C99 compatible)
    AUTO(squared, MAP(x, input, x * x));
    AUTO(result, FILTER(x, squared, x > 10));
    
    TEST_ASSERT_EQUAL(2, ELEGANT_LENGTH(result));
    TEST_ASSERT_EQUAL(16, result[0]);
    TEST_ASSERT_EQUAL(25, result[1]);
}
```

## Build System Integration

### As a Library
```cmake
# CMake integration
add_library(elegant INTERFACE)
target_include_directories(elegant INTERFACE inc/)
target_compile_features(elegant INTERFACE c_std_99)

# In your project:
target_link_libraries(your_project elegant)
```

```makefile
# Makefile integration  
ELEGANT_DIR = path/to/elegant
CFLAGS += -I$(ELEGANT_DIR)/inc -std=c99
LDFLAGS += -L$(ELEGANT_DIR) -lelegant
```

### Package Distribution
```bash
# As a system library
sudo make install  # Installs to /usr/local/include/elegant/

# As a git submodule
git submodule add https://github.com/user/elegant.git deps/elegant

# As a package manager dependency (future)
# vcpkg install elegant
# conan install elegant/1.0.0@
```

## Quick Reference Cheat Sheet

### Basic Operations
```c
// Arrays
AUTO(arr, ELEGANT_ARRAY(int, 1, 2, 3));
AUTO(len, ELEGANT_LENGTH(arr));
AUTO(ptr, ELEGANT_RAW_PTR(arr));

// Functional operations
AUTO(mapped, MAP(x, arr, x * 2));
AUTO(filtered, FILTER(x, arr, x > 5));
AUTO(sum, REDUCE(a, b, arr, a + b, 0));

// Sequential composition (instead of complex pipelines)
AUTO(step1, MAP(x, arr, x*x));
AUTO(step2, FILTER(x, step1, x>10));

// Memory management
ELEGANT_SET_MODE(REFERENCE_COUNTING);
ELEGANT_SCOPE { /* auto cleanup */ }
```

### Error Handling
```c
// Maybe types
ELEGANT_MAYBE(int) result = function_that_might_fail();
if (ELEGANT_IS_SOME(result)) {
    int value = ELEGANT_UNWRAP(result);
}

// Either types  
ELEGANT_EITHER(error_t, int) result = parse_number(str);
if (ELEGANT_IS_RIGHT(result)) {
    int value = ELEGANT_UNWRAP_RIGHT(result);
}
```

## Migration Guide: Traditional C to Elegant

### Before (Traditional C):
```c
#include <stdlib.h>
#include <stdio.h>

int main() {
    int* numbers = malloc(5 * sizeof(int));
    numbers[0] = 1; numbers[1] = 2; numbers[2] = 3; numbers[3] = 4; numbers[4] = 5;
    
    int* squares = malloc(5 * sizeof(int));
    for (int i = 0; i < 5; i++) {
        squares[i] = numbers[i] * numbers[i];
    }
    
    int* evens = malloc(5 * sizeof(int));
    int even_count = 0;
    for (int i = 0; i < 5; i++) {
        if (numbers[i] % 2 == 0) {
            evens[even_count++] = numbers[i];
        }
    }
    
    int sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += numbers[i];
    }
    
    free(numbers);
    free(squares);
    free(evens);
    return 0;
}
```

### After (Elegant C):
```c
#include "elegant.h"

int main() {
    AUTO(numbers, ELEGANT_ARRAY(int, 1, 2, 3, 4, 5));
    AUTO(squares, MAP(x, numbers, x * x));
    AUTO(evens, FILTER(x, numbers, x % 2 == 0));
    AUTO(sum, REDUCE(a, b, numbers, a + b, 0));
    
    // Memory automatically managed
    return 0;
}
```

### Migration Strategy:
1. **Start Small**: Convert utility functions first
2. **Gradual Adoption**: Use `ELEGANT_RAW_PTR()` for legacy interface compatibility
3. **Memory Boundaries**: Use `ELEGANT_SCOPE` to control automatic cleanup
4. **Performance Critical**: Keep traditional C for performance-critical inner loops

## Versioning & Compatibility

### Version 1.0.0 Compatibility Promise
- **API Stability**: All public APIs remain backward compatible within 1.x.x versions
- **ABI Compatibility**: Binary compatibility maintained for patch versions (1.0.x)  
- **Memory Layout**: Internal memory representation may change, but public interface remains stable
- **Compilation**: Code compiled with 1.0.0 will compile with any 1.x.x version

### Migration Between Versions
```c
// Version detection
#if ELEGANT_VERSION >= ELEGANT_VERSION_1_1_0
    // Use new features available in 1.1.0+
    AUTO(result, NEW_FEATURE(data));
#else
    // Fallback for older versions
    AUTO(result, OLD_APPROACH(data));
#endif

// Deprecation warnings (for major version changes)
#ifdef ELEGANT_DEPRECATED_WARNINGS
#warning "Function XYZ is deprecated, use ABC instead"
#endif
```

## Real-world Complete Examples

### Data Processing Pipeline (Simplified)
```c
#include "elegant.h"
#include <stdio.h>

// Process data using basic file operations and elegant functional style
int main() {
    // Manual file reading (since READ_LINES is too complex for C99)
    FILE* file = fopen("sales_data.csv", "r");
    if (!file) return 1;
    
    // Process data line by line with manual parsing
    char line[256];
    AUTO(valid_records, ELEGANT_ARRAY(SalesRecord, /* empty */));
    
    while (fgets(line, sizeof(line), file)) {
        SalesRecord record = parse_csv_line(line);
        if (record.amount > 0 && record.date_valid) {
            // Use ELEGANT_APPEND for dynamic growth (if implemented)
            valid_records = ELEGANT_APPEND(valid_records, record);
        }
    }
    fclose(file);
    
    // Apply functional operations to processed data
    AUTO(sorted_records, SORT_BY(date_compare, valid_records));
    AUTO(monthly_sums, MAP(r, sorted_records, r.amount));
    AUTO(total, REDUCE(a, b, monthly_sums, a + b, 0.0));
    
    printf("Processed %zu records, total: %.2f\n", 
           ELEGANT_LENGTH(sorted_records), total);
    
    return 0;
}
```

### Network Data Processing (Conceptual - Not Fully Implementable)
```c
#include "elegant.h"
#include <sys/socket.h>

// Basic packet processing (without complex streaming)
void process_network_packets(int socket_fd) {
    // Manual packet reading (STREAM_FROM_SOCKET too complex for C99)
    char buffer[1024];
    AUTO(valid_packets, ELEGANT_ARRAY(Packet, /* empty */));
    
    // Read packets one by one
    while (1) {
        ssize_t bytes = recv(socket_fd, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break;
        
        Packet packet = parse_packet(buffer, bytes);
        if (is_valid_packet(packet)) {
            valid_packets = ELEGANT_APPEND(valid_packets, packet);
        }
        
        // Process batch when we have enough packets
        if (ELEGANT_LENGTH(valid_packets) >= 100) {
            AUTO(parsed_data, MAP(p, valid_packets, extract_stats(p)));
            AUTO(total_stats, REDUCE(a, b, parsed_data, aggregate_stats(a, b), empty_stats()));
            log_statistics(total_stats);
            
            // Reset for next batch
            valid_packets = ELEGANT_ARRAY(Packet, /* empty */);
        }
    }
}
```

## License

This project is licensed under the MIT License. See the [LICENSE](../LICENSE) file for details.
