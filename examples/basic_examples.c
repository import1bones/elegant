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
        AUTO(squares, MAP_INT(numbers, x * x));
        printf("Squares: ");
        for (size_t i = 0; i < ELEGANT_LENGTH(squares); i++) {
            printf("%d ", ELEGANT_GET(squares, i, int));
        }
        printf("\n");
        
        // FILTER operation - keep only even numbers (using int-specific filter)
        AUTO(evens, FILTER_INT(numbers, x % 2 == 0));
        printf("Even numbers: ");
        for (size_t i = 0; i < ELEGANT_LENGTH(evens); i++) {
            printf("%d ", ELEGANT_GET(evens, i, int));
        }
        printf("\n");
        
        // REDUCE operation - sum all numbers
        AUTO(sum, REDUCE_INT(numbers, acc + x, 0));
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
        AUTO(step1, MAP_INT(data, x * x));        // Square each number
        AUTO(step2, FILTER_INT(step1, x > 10));   // Keep values > 10
        AUTO(final, REDUCE_INT(step2, acc + x, 0)); // Sum the results
        
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
    
    // For now, advanced maybe types need more implementation
    // Using basic safe array access instead
    ELEGANT_SCOPE {
        AUTO(arr, elegant_create_array_int(10, 20, 30));
        
        // Manual bounds checking for safe access
        size_t valid_index = 1;
        if (valid_index < ELEGANT_LENGTH(arr)) {
            printf("arr[%zu] = %d\n", valid_index, ELEGANT_GET(arr, valid_index, int));
        }
        
        size_t invalid_index = 5;
        if (invalid_index >= ELEGANT_LENGTH(arr)) {
            printf("arr[%zu] = out of bounds (correctly handled)\n", invalid_index);
        }
    }
    
    printf("\n");
}

/* Example 4: Either types for detailed error handling */
void example_either_types(void) {
    printf("=== Either Types Example ===\n");
    
    // For now, Either types need more implementation
    // Using simple return values for error handling
    printf("Either types implementation pending...\n");
    printf("Using simple error handling instead:\n");
    
    // Simple divide-by-zero check
    int a = 10, b = 2;
    if (b != 0) {
        printf("%d / %d = %d\n", a, b, a / b);
    } else {
        printf("Division by zero avoided!\n");
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
        
        AUTO(arr2, MAP_INT(arr1, x * 2));
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
        AUTO(doubled_ints, MAP_INT(ints, x * 2));
        
        // Float arrays  
        AUTO(floats, elegant_create_array_float(1.5f, 2.5f, 3.5f));
        AUTO(squared_floats, MAP_FLOAT(floats, x * x));
        
        // Double arrays
        AUTO(doubles, elegant_create_array_double(1.1, 2.2, 3.3));
        AUTO(sqrt_doubles, MAP_DOUBLE(doubles, sqrt(x)));
        
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
