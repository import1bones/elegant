/*
 * Elegant Library - Basic Examples
 * Demonstrates core functionality of v0.0.1
 */

#include "../inc/elegant.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>

/* Example 1: Basic functional operations */
void example_basic_operations(void) {
    printf("=== Basic Operations Example ===\n");
    
    ELEGANT_SCOPE {
        // Create an array using the ELEGANT_ARRAY macro
        AUTO(numbers, elegant_create_array_int(1, 2, 3, 4, 5));
        printf("Original array length: %zu\n", ELEGANT_LENGTH(numbers));
        
        // MAP operation - square each number
        AUTO(squares, MAP(x, numbers, x * x));
        printf("Squares: ");
        for (size_t i = 0; i < ELEGANT_LENGTH(squares); i++) {
            printf("%d ", ELEGANT_GET(squares, i, int));
        }
        printf("\n");
        
        // FILTER operation - keep only even numbers
        AUTO(evens, FILTER(x, numbers, x % 2 == 0));
        printf("Even numbers: ");
        for (size_t i = 0; i < ELEGANT_LENGTH(evens); i++) {
            printf("%d ", ELEGANT_GET(evens, i, int));
        }
        printf("\n");
        
        // REDUCE operation - sum all numbers
        AUTO(sum, REDUCE(a, b, numbers, a + b, 0));
        printf("Sum: %d\n", sum);
        
        // Array manipulation operations
        AUTO(reversed, REVERSE(numbers));
        printf("Reversed: ");
        for (size_t i = 0; i < ELEGANT_LENGTH(reversed); i++) {
            printf("%d ", ELEGANT_GET(reversed, i, int));
        }
        printf("\n");
        
        AUTO(first_three, TAKE(3, numbers));
        printf("First three: ");
        for (size_t i = 0; i < ELEGANT_LENGTH(first_three); i++) {
            printf("%d ", ELEGANT_GET(first_three, i, int));
        }
        printf("\n");
        
        AUTO(skip_two, DROP(2, numbers));
        printf("Skip first two: ");
        for (size_t i = 0; i < ELEGANT_LENGTH(skip_two); i++) {
            printf("%d ", ELEGANT_GET(skip_two, i, int));
        }
        printf("\n");
    }
    /* All memory automatically cleaned up here */
    
    printf("\n");
}

/* Example 2: Sequential composition */
void example_sequential_composition(void) {
    printf("=== Sequential Composition Example ===\n");
    
    ELEGANT_SCOPE {
        AUTO(data, elegant_create_array_int(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
        
        // Sequential functional operations (C99 compatible)
        AUTO(step1, MAP(x, data, x * x));        // Square each number
        AUTO(step2, FILTER(x, step1, x > 10));   // Keep values > 10
        AUTO(final, REDUCE(a, b, step2, a + b, 0)); // Sum the results
        
        printf("Original: ");
        for (size_t i = 0; i < ELEGANT_LENGTH(data); i++) {
            printf("%d ", ELEGANT_GET(data, i, int));
        }
        printf("\n");
        
        printf("After squaring: ");
        for (size_t i = 0; i < ELEGANT_LENGTH(step1); i++) {
            printf("%d ", ELEGANT_GET(step1, i, int));
        }
        printf("\n");
        
        printf("After filtering (>10): ");
        for (size_t i = 0; i < ELEGANT_LENGTH(step2); i++) {
            printf("%d ", ELEGANT_GET(step2, i, int));
        }
        printf("\n");
        
        printf("Final sum: %d\n", final);
    }
    
    printf("\n");
}

/* Example 3: Maybe types for error handling */
void example_maybe_types(void) {
    printf("=== Maybe Types Example ===\n");
    
    // Safe division examples
    AUTO(result1, elegant_maybe_divide_int(10, 2));
    if (ELEGANT_IS_SOME(result1)) {
        printf("10 / 2 = %d\n", ELEGANT_UNWRAP(result1));
    }
    
    AUTO(result2, elegant_maybe_divide_int(10, 0));
    if (ELEGANT_IS_NONE(result2)) {
        printf("10 / 0 = undefined (correctly handled)\n");
    }
    
    // Using ELEGANT_UNWRAP_OR for default values
    int safe_result = ELEGANT_UNWRAP_OR(result2, -1);
    printf("10 / 0 with default -1: %d\n", safe_result);
    
    // Maybe array access
    ELEGANT_SCOPE {
        AUTO(arr, elegant_create_array_int(10, 20, 30));
        
        AUTO(element1, ELEGANT_MAYBE_GET(arr, 1, int));
        if (ELEGANT_IS_SOME(element1)) {
            printf("arr[1] = %d\n", ELEGANT_UNWRAP(element1));
        }
        
        AUTO(element5, ELEGANT_MAYBE_GET(arr, 5, int));
        if (ELEGANT_IS_NONE(element5)) {
            printf("arr[5] = out of bounds (correctly handled)\n");
        }
    }
    
    printf("\n");
}

/* Example 4: Either types for detailed error handling */
void example_either_types(void) {
    printf("=== Either Types Example ===\n");
    
    // String to integer parsing
    AUTO(parse1, elegant_parse_int("42"));
    if (ELEGANT_IS_RIGHT(parse1)) {
        printf("Parsed '42' as: %d\n", ELEGANT_UNWRAP_RIGHT(parse1));
    }
    
    AUTO(parse2, elegant_parse_int("not_a_number"));
    if (ELEGANT_IS_LEFT(parse2)) {
        printf("Failed to parse 'not_a_number': %s\n", ELEGANT_UNWRAP_LEFT(parse2));
    }
    
    AUTO(parse3, elegant_parse_int("123abc"));
    if (ELEGANT_IS_LEFT(parse3)) {
        printf("Failed to parse '123abc': %s\n", ELEGANT_UNWRAP_LEFT(parse3));
    }
    
    // Pattern matching example
    const char* test_strings[] = {"100", "abc", "42xyz", NULL};
    for (int i = 0; test_strings[i]; i++) {
        AUTO(result, elegant_parse_int(test_strings[i]));
        
        AUTO(message, ELEGANT_EITHER_MATCH(result,
            error, "Error",
            value, "Success"
        ));
        
        printf("Parsing '%s': %s\n", test_strings[i], message);
    }
    
    printf("\n");
}

/* Example 5: Different memory management modes */
void example_memory_modes(void) {
    printf("=== Memory Management Modes Example ===\n");
    
    printf("Initial memory: %zu bytes\n", elegant_get_allocated_bytes());
    
    // Stack arena mode (default)
    ELEGANT_SET_MODE(STACK_ARENA);
    printf("Using STACK_ARENA mode\n");
    
    ELEGANT_SCOPE {
        AUTO(arr1, elegant_create_array_int(1, 2, 3, 4, 5));
        printf("Created array, memory: %zu bytes\n", elegant_get_allocated_bytes());
        
        AUTO(arr2, MAP(x, arr1, x * 2));
        printf("After MAP, memory: %zu bytes\n", elegant_get_allocated_bytes());
    }
    printf("After scope exit, memory: %zu bytes\n", elegant_get_allocated_bytes());
    
    // Reference counting mode
    ELEGANT_SET_MODE(REFERENCE_COUNTING);
    printf("\nUsing REFERENCE_COUNTING mode\n");
    
    elegant_array_t* shared_arr = elegant_create_array_int(10, 20, 30);
    printf("Created shared array, memory: %zu bytes\n", elegant_get_allocated_bytes());
    
    elegant_array_t* view1 = elegant_array_copy(shared_arr);  // Increases ref count
    elegant_array_t* view2 = elegant_array_copy(shared_arr);  // Increases ref count
    printf("Created 2 views, memory: %zu bytes\n", elegant_get_allocated_bytes());
    
    elegant_array_destroy(view1);  // Decreases ref count
    printf("Destroyed view1, memory: %zu bytes\n", elegant_get_allocated_bytes());
    
    elegant_array_destroy(view2);  // Decreases ref count
    printf("Destroyed view2, memory: %zu bytes\n", elegant_get_allocated_bytes());
    
    elegant_array_destroy(shared_arr);  // Actually frees memory
    printf("Destroyed original, memory: %zu bytes\n", elegant_get_allocated_bytes());
    
    printf("\n");
}

/* Example 6: Working with different data types */
void example_different_types(void) {
    printf("=== Different Data Types Example ===\n");
    
    ELEGANT_SCOPE {
        // Integer arrays
        AUTO(ints, elegant_create_array_int(1, 2, 3));
        AUTO(doubled_ints, MAP(x, ints, x * 2));
        
        // Float arrays  
        AUTO(floats, elegant_create_array_float(1.5f, 2.5f, 3.5f));
        AUTO(squared_floats, MAP(x, floats, x * x));
        
        // Double arrays
        AUTO(doubles, elegant_create_array_double(1.1, 2.2, 3.3));
        AUTO(sqrt_doubles, MAP(x, doubles, sqrt(x)));
        
        printf("Doubled integers: ");
        for (size_t i = 0; i < ELEGANT_LENGTH(doubled_ints); i++) {
            printf("%d ", ELEGANT_GET(doubled_ints, i, int));
        }
        printf("\n");
        
        printf("Squared floats: ");
        for (size_t i = 0; i < ELEGANT_LENGTH(squared_floats); i++) {
            printf("%.2f ", ELEGANT_GET(squared_floats, i, float));
        }
        printf("\n");
        
        printf("Square root of doubles: ");
        for (size_t i = 0; i < ELEGANT_LENGTH(sqrt_doubles); i++) {
            printf("%.2f ", ELEGANT_GET(sqrt_doubles, i, double));
        }
        printf("\n");
    }
    
    printf("\n");
}

int main(void) {
    printf("Elegant Functional Programming Library v0.0.1\n");
    printf("==============================================\n\n");
    
    example_basic_operations();
    example_sequential_composition();
    example_maybe_types();
    example_either_types();
    example_memory_modes();
    example_different_types();
    
    printf("All examples completed successfully!\n");
    printf("Final memory usage: %zu bytes\n", elegant_get_allocated_bytes());
    
    return 0;
}
