# Elegant C Library - API Reference

## Table of Contents

1. [Core Types and Macros](#core-types-and-macros)
2. [Memory Management](#memory-management)
3. [Memory Safety](#memory-safety)
4. [Collection Operations](#collection-operations)
5. [Functional Programming](#functional-programming)
6. [Maybe/Option Types](#maybeOption-types)
7. [Either Types](#either-types)
8. [Scope Management](#scope-management)
9. [Configuration and Modes](#configuration-and-modes)

---

## Core Types and Macros

### Basic Types

```c
typedef struct elegant_array elegant_array_t;
```
Core array type used throughout the library.

### Essential Macros

```c
#define AUTO(var, expr) __typeof__(expr) var = (expr)
```
**Description**: Automatic type deduction for variables.  
**Parameters**: 
- `var` - Variable name to declare
- `expr` - Expression whose type to infer

**Example**:
```c
AUTO(numbers, CREATE_INT_ARRAY(1, 2, 3, 4, 5));
```

```c
#define LET(var, expr) AUTO(var, expr)
```
**Description**: Alias for `AUTO` with more functional programming style.

```c
#define ELEGANT_MAX_STACK_ALLOC (64 * 1024)
```
**Description**: Maximum size for stack-based allocations (64KB).

---

## Memory Management

### Memory Modes

```c
typedef enum {
    ELEGANT_MEMORY_STACK_ARENA = 0,
    ELEGANT_MEMORY_REFERENCE_COUNTING = 1,
    ELEGANT_MEMORY_GARBAGE_COLLECTION = 2
} elegant_memory_mode_t;
```

### Memory Mode Functions

```c
void elegant_set_memory_mode(elegant_memory_mode_t mode);
```
**Description**: Set the memory management strategy.  
**Parameters**: `mode` - Memory management mode  
**Thread Safety**: Thread-local setting

```c
elegant_memory_mode_t elegant_get_memory_mode(void);
```
**Description**: Get current memory management mode.  
**Returns**: Current memory mode  
**Thread Safety**: Thread-safe

```c
#define ELEGANT_SET_MODE(mode) elegant_set_memory_mode(ELEGANT_MEMORY_##mode)
```
**Description**: Convenient macro to set memory mode.  
**Example**: `ELEGANT_SET_MODE(STACK_ARENA)`

### Core Array Functions

```c
elegant_array_t* elegant_array_create_int(size_t capacity);
elegant_array_t* elegant_array_create_float(size_t capacity);
```
**Description**: Create typed arrays with specified capacity.  
**Parameters**: `capacity` - Initial capacity  
**Returns**: New array or NULL on failure

```c
void elegant_array_destroy(elegant_array_t* arr);
```
**Description**: Destroy array and free memory.  
**Parameters**: `arr` - Array to destroy  
**Note**: Respects current memory mode

```c
void* elegant_array_get_data(elegant_array_t* arr);
size_t elegant_array_get_length(elegant_array_t* arr);
```
**Description**: Access array data and length.  
**Parameters**: `arr` - Array to query  
**Returns**: Data pointer or length

---

## Memory Safety

### Safe Memory Allocation

```c
void* elegant_safe_malloc(size_t size);
void* elegant_safe_calloc(size_t nmemb, size_t size);
void* elegant_safe_realloc(void* ptr, size_t size);
void elegant_safe_free(void* ptr);
```
**Description**: Safe memory allocation functions with canary protection.  
**Features**: 
- Buffer overflow detection with canaries
- Use-after-free detection
- Double-free protection
- Memory corruption detection

### Safety Macros

```c
#define ELEGANT_MALLOC(size)           elegant_safe_malloc(size)
#define ELEGANT_CALLOC(nmemb, size)    elegant_safe_calloc(nmemb, size)
#define ELEGANT_REALLOC(ptr, size)     elegant_safe_realloc(ptr, size)
#define ELEGANT_FREE(ptr)              elegant_safe_free(ptr)
#define ELEGANT_VALIDATE(ptr)          elegant_validate_pointer(ptr)
```
**Description**: Safe allocation macros (when `ELEGANT_SAFETY_ENABLED`).

### Validation Functions

```c
bool elegant_validate_pointer(const void* ptr);
```
**Description**: Validate pointer integrity.  
**Parameters**: `ptr` - Pointer to validate  
**Returns**: `true` if valid, `false` if corrupted  
**Checks**: Magic numbers, canaries, freed status

```c
bool elegant_validate_buffer(const void* ptr, size_t size);
```
**Description**: Validate buffer size bounds.  
**Parameters**: 
- `ptr` - Buffer pointer
- `size` - Required size
**Returns**: `true` if buffer is large enough

```c
bool elegant_check_canaries(const void* ptr);
```
**Description**: Check canary integrity.  
**Parameters**: `ptr` - Pointer to check  
**Returns**: `true` if canaries intact

```c
bool elegant_detect_corruption(const void* ptr);
```
**Description**: General corruption detection.  
**Parameters**: `ptr` - Pointer to check  
**Returns**: `true` if corruption detected

```c
bool elegant_is_freed_pointer(const void* ptr);
```
**Description**: Check if pointer was freed.  
**Parameters**: `ptr` - Pointer to check  
**Returns**: `true` if pointer was freed

### Bounds Checking

```c
bool elegant_bounds_check(const void* ptr, size_t index, size_t element_size);
```
**Description**: Check array bounds access.  
**Parameters**: 
- `ptr` - Array pointer
- `index` - Access index
- `element_size` - Size of each element
**Returns**: `true` if access is within bounds

```c
#define ELEGANT_ARRAY_ACCESS(arr, index, type) \
    (elegant_bounds_check((arr), (index), sizeof(type)) ? \
     ((type*)(arr))[index] : \
     (elegant_bounds_violation(__FILE__, __LINE__, (index)), \
      ((type*)(arr))[0]))
```
**Description**: Safe array access with bounds checking.

```c
void elegant_bounds_violation(const char* file, int line, size_t index);
```
**Description**: Report bounds violation.  
**Parameters**: 
- `file` - Source file name
- `line` - Line number
- `index` - Violating index

### Safe String Operations

```c
int elegant_safe_strcpy(char* dest, const char* src, size_t dest_size);
```
**Description**: Safe string copy with overflow protection.  
**Parameters**: 
- `dest` - Destination buffer
- `src` - Source string
- `dest_size` - Destination buffer size
**Returns**: 0 on success, -1 on overflow

```c
int elegant_safe_strcat(char* dest, const char* src, size_t dest_size);
```
**Description**: Safe string concatenation.  
**Parameters**: 
- `dest` - Destination buffer
- `src` - Source string
- `dest_size` - Destination buffer size
**Returns**: 0 on success, -1 on overflow

```c
void* elegant_safe_memcpy(void* dest, const void* src, size_t size);
```
**Description**: Safe memory copy with validation.  
**Parameters**: 
- `dest` - Destination buffer
- `src` - Source buffer
- `size` - Bytes to copy
**Features**: Buffer validation, overlap detection

### Stack Protection

```c
#define ELEGANT_STACK_GUARD() \
    uint64_t __stack_canary = ELEGANT_CANARY_MAGIC_1; \
    (void)__stack_canary
```
**Description**: Place stack canary guard.  
**Usage**: Call at function start

```c
#define ELEGANT_STACK_CHECK() \
    do { \
        if (__stack_canary != ELEGANT_CANARY_MAGIC_1) { \
            elegant_stack_corruption_detected(__FILE__, __LINE__); \
        } \
    } while(0)
```
**Description**: Check stack canary integrity.  
**Usage**: Call before function exit

```c
void elegant_stack_corruption_detected(const char* file, int line);
```
**Description**: Handle stack corruption detection.  
**Parameters**: 
- `file` - Source file
- `line` - Line number
**Behavior**: Aborts program with error message

### Memory Pool Safety

```c
typedef struct elegant_safe_pool elegant_safe_pool_t;

elegant_safe_pool_t* elegant_create_safe_pool(size_t size);
```
**Description**: Create safe memory pool.  
**Parameters**: `size` - Pool size in bytes  
**Returns**: Pool handle or NULL

```c
void* elegant_pool_alloc(elegant_safe_pool_t* pool, size_t size);
```
**Description**: Allocate from pool.  
**Parameters**: 
- `pool` - Pool handle
- `size` - Allocation size
**Returns**: Allocated memory or NULL

```c
void elegant_pool_free(elegant_safe_pool_t* pool, void* ptr);
```
**Description**: Free pool allocation (currently no-op).

```c
void elegant_destroy_safe_pool(elegant_safe_pool_t* pool);
```
**Description**: Destroy memory pool.  
**Parameters**: `pool` - Pool to destroy

### Memory Debugging

```c
void elegant_safety_report(void);
```
**Description**: Print comprehensive safety statistics.  
**Output**: 
- Total/active allocations
- Bytes allocated/freed
- Error detection counts

```c
void elegant_dump_active_allocations(void);
```
**Description**: List all active allocations.  
**Output**: Allocation addresses and sizes

```c
void elegant_check_all_allocations(void);
```
**Description**: Validate all tracked allocations.  
**Output**: Corruption reports

---

## Collection Operations

### Array Creation Macros

```c
#define CREATE_INT_ARRAY(...) elegant_create_int_array_from_values(__VA_ARGS__)
#define CREATE_FLOAT_ARRAY(...) elegant_create_float_array_from_values(__VA_ARGS__)
```
**Description**: Create arrays from value lists.  
**Example**: `CREATE_INT_ARRAY(1, 2, 3, 4, 5)`

### Type-Specific Operations

```c
#define MAP_INT(arr, expr) elegant_map_int(arr, expr)
#define FILTER_INT(arr, expr) elegant_filter_int(arr, expr)  
#define REDUCE_INT(arr, expr, init) elegant_reduce_int(arr, expr, init)
```
**Description**: Type-specific collection operations for integers.  
**Parameters**: 
- `arr` - Array to operate on
- `expr` - Expression/predicate
- `init` - Initial value (for reduce)

```c
#define MAP_FLOAT(arr, expr) elegant_map_float(arr, expr)
#define FILTER_FLOAT(arr, expr) elegant_filter_float(arr, expr)
#define REDUCE_FLOAT(arr, expr, init) elegant_reduce_float(arr, expr, init)
```
**Description**: Type-specific collection operations for floats.

### Generic Operations

```c
#define MAP(arr, expr, type) elegant_map_generic(arr, expr, sizeof(type))
#define FILTER(arr, expr, type) elegant_filter_generic(arr, expr, sizeof(type))
#define REDUCE(arr, expr, init, type) elegant_reduce_generic(arr, expr, init, sizeof(type))
```
**Description**: Generic collection operations for any type.  
**Parameters**: 
- `arr` - Array to operate on
- `expr` - Expression/predicate  
- `type` - Element type
- `init` - Initial value (for reduce)

**Example**:
```c
AUTO(doubled, MAP(numbers, x * 2, int));
AUTO(evens, FILTER(numbers, x % 2 == 0, int));
AUTO(sum, REDUCE(numbers, acc + x, 0, int));
```

---

## Functional Programming

### Pipeline Operations

```c
#define PIPE(data, func) func(data)
#define PIPE2(data, func1, func2) func2(func1(data))
#define PIPE3(data, func1, func2, func3) func3(func2(func1(data)))
#define PIPE4(data, func1, func2, func3, func4) func4(func3(func2(func1(data))))
#define PIPE5(data, func1, func2, func3, func4, func5) func5(func4(func3(func2(func1(data)))))
```
**Description**: Function composition pipeline macros.  
**Parameters**: 
- `data` - Initial data
- `func1`, `func2`, etc. - Functions to apply in sequence

**Example**:
```c
AUTO(result, PIPE3(numbers, 
    (lambda(elegant_array_t* arr) { return MAP_INT(arr, x * 2); }),
    (lambda(elegant_array_t* arr) { return FILTER_INT(arr, x > 10); }),
    (lambda(elegant_array_t* arr) { return REDUCE_INT(arr, acc + x, 0); })
));
```

### Fold Operations

```c
#define FOLD_LEFT(arr, func, init, type) REDUCE(arr, func, init, type)
#define FOLD_RIGHT(arr, func, init, type) elegant_fold_right_generic(arr, func, init, sizeof(type))
```
**Description**: Left and right fold operations.  
**Parameters**: 
- `arr` - Array to fold
- `func` - Folding function
- `init` - Initial accumulator value
- `type` - Element type

### Array Utilities

```c
#define TAKE_INT(arr, n) elegant_take_int(arr, n)
#define DROP_INT(arr, n) elegant_drop_int(arr, n)
```
**Description**: Array slicing operations.  
**Parameters**: 
- `arr` - Source array
- `n` - Number of elements

```c
elegant_array_t* elegant_reverse_int(elegant_array_t* arr);
```
**Description**: Reverse integer array.  
**Parameters**: `arr` - Array to reverse  
**Returns**: New reversed array

### Currying Support

```c
typedef struct {
    int first_arg;
} curry_add_t;

typedef struct {
    int first_arg;  
} curry_multiply_t;

#define CURRY_ADD(x) elegant_curry_add(x)
#define CURRY_MULTIPLY(x) elegant_curry_multiply(x)
```
**Description**: Currying support for basic operations.  
**Example**:
```c
AUTO(add5, CURRY_ADD(5));
AUTO(result, MAP_INT(numbers, add5.first_arg + x));
```

### String Operations

```c
elegant_array_t* elegant_concat_strings(elegant_array_t* arr1, elegant_array_t* arr2);
```
**Description**: Safe string concatenation.  
**Parameters**: 
- `arr1`, `arr2` - String arrays to concatenate
**Returns**: New concatenated array

```c
#define ELEGANT_CONCAT(arr1, arr2) elegant_concat_strings(arr1, arr2)
```
**Description**: Macro for string concatenation.

---

## Maybe/Option Types

### Maybe Type Definition

```c
typedef struct {
    bool has_value;
    union {
        int int_value;
        float float_value;
        void* ptr_value;
    };
} elegant_maybe_t;
```

### Maybe Constructors

```c
#define SOME_INT(value) elegant_some_int(value)
#define SOME_FLOAT(value) elegant_some_float(value)  
#define SOME_PTR(value) elegant_some_ptr(value)
#define NONE() elegant_none()
```
**Description**: Create Maybe values.  
**Returns**: Maybe type with or without value

### Maybe Operations

```c
#define IS_SOME(maybe) elegant_is_some(maybe)
#define IS_NONE(maybe) elegant_is_none(maybe)
```
**Description**: Check Maybe state.  
**Parameters**: `maybe` - Maybe value to check  
**Returns**: Boolean result

```c
#define UNWRAP_OR(maybe, default_val) elegant_unwrap_or(maybe, default_val)
```
**Description**: Safely unwrap Maybe with default.  
**Parameters**: 
- `maybe` - Maybe value
- `default_val` - Default if None
**Returns**: Value or default

```c
#define MAYBE_MAP(maybe, func) elegant_maybe_map(maybe, func)
```
**Description**: Apply function to Maybe value.  
**Parameters**: 
- `maybe` - Maybe value
- `func` - Function to apply
**Returns**: Maybe result

### Safe Operations

```c
#define SAFE_DIVIDE(a, b) elegant_safe_divide(a, b)
```
**Description**: Safe division returning Maybe.  
**Parameters**: `a`, `b` - Division operands  
**Returns**: `SOME(result)` or `NONE()` if division by zero

---

## Either Types

### Either Type Definition

```c
typedef struct {
    bool is_right;
    union {
        char* error_msg;    // Left (error)
        union {
            int int_value;
            float float_value;
            void* ptr_value;
        } value;           // Right (success)
    };
} elegant_either_t;
```

### Either Constructors

```c
#define LEFT(error) elegant_left(error)
#define RIGHT_INT(value) elegant_right_int(value)
#define RIGHT_FLOAT(value) elegant_right_float(value)
#define RIGHT_PTR(value) elegant_right_ptr(value)
```
**Description**: Create Either values for success/error cases.

### Either Operations

```c
#define IS_LEFT(either) elegant_is_left(either)
#define IS_RIGHT(either) elegant_is_right(either)
```
**Description**: Check Either state.

```c
#define EITHER_MAP(either, func) elegant_either_map(either, func)
```
**Description**: Apply function to Right value.

```c
#define PARSE_INT(str) elegant_parse_int(str)
```
**Description**: Safe string to integer parsing.  
**Parameters**: `str` - String to parse  
**Returns**: `RIGHT_INT(value)` or `LEFT(error_msg)`

---

## Scope Management

### Scope Operations

```c
void elegant_scope_enter(void);
void elegant_scope_exit(void);
void elegant_scope_register(elegant_array_t* array);
```
**Description**: Manual scope management.  
**Usage**: Enter scope, register allocations, exit to cleanup

```c
#define ELEGANT_SCOPE \
    for (int _scope_init = (elegant_scope_enter(), 1); \
         _scope_init; \
         _scope_init = 0, elegant_scope_exit())
```
**Description**: Automatic scope-based memory management.  
**Example**:
```c
ELEGANT_SCOPE {
    AUTO(numbers, CREATE_INT_ARRAY(1, 2, 3, 4, 5));
    // numbers automatically freed at scope exit
}
```

### Reference Counting

```c
elegant_array_t* elegant_array_retain(elegant_array_t* arr);
void elegant_array_release(elegant_array_t* arr);
```
**Description**: Reference counting operations.  
**Parameters**: `arr` - Array to retain/release

---

## Configuration and Modes

### Memory Safety Configuration

```c
#define ELEGANT_SAFETY_ENABLED 1        // Enable/disable safety features
#define ELEGANT_GUARD_PAGE_SIZE 4096     // Guard page size  
#define ELEGANT_CANARY_SIZE 8           // Canary size in bytes
```

### Debug Configuration

```c
#ifdef ELEGANT_DEBUG_MEMORY
void elegant_memory_debug_dump(void);
size_t elegant_memory_debug_get_allocations(void);
#define ELEGANT_MEMORY_DEBUG_DUMP() elegant_memory_debug_dump()
#else
#define ELEGANT_MEMORY_DEBUG_DUMP() do {} while(0)
#endif
```

### Safety Statistics

```c
typedef struct {
    size_t total_allocations;
    size_t active_allocations;
    size_t total_freed;
    size_t bytes_allocated;
    size_t bytes_freed;
    size_t corruption_detected;
    size_t use_after_free_detected;
    size_t double_free_detected;
    size_t buffer_overflow_detected;
} elegant_safety_stats_t;

extern elegant_safety_stats_t elegant_safety_stats;
```
**Description**: Global safety statistics structure.

---

## Usage Patterns

### Basic Pattern

```c
#include "elegant.h"

int main() {
    // Set memory mode
    ELEGANT_SET_MODE(STACK_ARENA);
    
    // Create and process data
    AUTO(numbers, CREATE_INT_ARRAY(1, 2, 3, 4, 5));
    AUTO(doubled, MAP_INT(numbers, x * 2));
    AUTO(sum, REDUCE_INT(doubled, acc + x, 0));
    
    printf("Sum: %d\n", sum);
    return 0;
}
```

### Safe Memory Pattern

```c
#include "elegant.h"

int main() {
    ELEGANT_STACK_GUARD();
    
    // Safe allocation
    int* buffer = (int*)ELEGANT_MALLOC(sizeof(int) * 100);
    
    // Safe access
    for (int i = 0; i < 100; i++) {
        if (elegant_bounds_check(buffer, i, sizeof(int))) {
            buffer[i] = i * i;
        }
    }
    
    // Validate and cleanup
    if (ELEGANT_VALIDATE(buffer)) {
        ELEGANT_FREE(buffer);
    }
    
    ELEGANT_STACK_CHECK();
    elegant_safety_report();
    return 0;
}
```

### Functional Pipeline Pattern

```c
AUTO(result, PIPE4(data,
    lambda(auto x) { return MAP(x, elem * 2, int); },
    lambda(auto x) { return FILTER(x, elem > 10, int); },
    lambda(auto x) { return TAKE_INT(x, 5); },
    lambda(auto x) { return REDUCE(x, acc + elem, 0, int); }
));
```

---

## Error Handling

### Return Values

- Most functions return `NULL` or `false` on failure
- Memory functions may set `errno`
- Safety functions print error messages to `stderr`

### Safety Violations

- Buffer overflows: Detected via canary corruption
- Use-after-free: Detected via freed pointer tracking  
- Double-free: Detected via allocation state flags
- Stack corruption: Causes immediate program abort

### Best Practices

1. Always check return values
2. Use `ELEGANT_VALIDATE()` before accessing pointers
3. Prefer scoped allocation with `ELEGANT_SCOPE`
4. Call `elegant_safety_report()` for debugging
5. Use bounds checking for array access
6. Initialize memory safety early in program

This API reference provides comprehensive coverage of all Elegant C library features, from basic collection operations to advanced memory safety mechanisms.
