/*
 * Elegant Library - Simple Examples
 * Version 0.0.1 - Simplified implementation
 */

#include "../inc/elegant.h"

void example_basic_arrays(void) {
    printf("\n=== Basic Array Operations ===\n");
    
    // Create an array of integers
    int int_data[] = {1, 2, 3, 4, 5};
    elegant_array_t* arr = elegant_create_array_impl(sizeof(int), int_data, 5);
    
    printf("Original array length: %zu\n", elegant_array_get_length(arr));
    
    int* data = (int*)elegant_array_get_data(arr);
    printf("Original array: ");
    for (size_t i = 0; i < elegant_array_get_length(arr); i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
    
    // Map operation - double each element
    elegant_array_t* doubled = MAP_INT(arr, x * 2);
    
    int* doubled_data = (int*)elegant_array_get_data(doubled);
    printf("Doubled array: ");
    for (size_t i = 0; i < elegant_array_get_length(doubled); i++) {
        printf("%d ", doubled_data[i]);
    }
    printf("\n");
    
    // Filter operation - keep only even numbers
    elegant_array_t* evens = FILTER_INT(doubled, x % 2 == 0);
    
    int* evens_data = (int*)elegant_array_get_data(evens);
    printf("Even numbers: ");
    for (size_t i = 0; i < elegant_array_get_length(evens); i++) {
        printf("%d ", evens_data[i]);
    }
    printf("\n");
    
    // Reduce operation - sum all elements
    int sum = REDUCE_INT(evens, a + b, 0);
    printf("Sum of even numbers: %d\n", sum);
    
    // Cleanup
    elegant_array_destroy(arr);
    elegant_array_destroy(doubled);
    elegant_array_destroy(evens);
}

void example_maybe_types(void) {
    printf("\n=== Maybe Types ===\n");
    
    // Create Maybe values
    elegant_maybe_int_t maybe_value = ELEGANT_MAYBE_SOME(int, 42);
    elegant_maybe_int_t maybe_empty = ELEGANT_MAYBE_NONE(int);
    
    // Check if values exist
    if (ELEGANT_MAYBE_IS_SOME(maybe_value)) {
        printf("Maybe has value: %d\n", ELEGANT_MAYBE_GET(maybe_value));
    }
    
    if (ELEGANT_MAYBE_IS_NONE(maybe_empty)) {
        printf("Maybe is empty\n");
    }
    
    // Safe extraction with default
    int value_or_default = ELEGANT_MAYBE_GET_OR_DEFAULT(maybe_empty, 99);
    printf("Value or default: %d\n", value_or_default);
}

void example_either_types(void) {
    printf("\n=== Either Types ===\n");
    
    // Create Either values - successful case
    elegant_either_t either_right = ELEGANT_EITHER_RIGHT(int, char*, 123);
    // Create Either values - error case  
    elegant_either_t either_left = ELEGANT_EITHER_LEFT(int, char*, "Error occurred");
    
    // Pattern matching on Either
    if (ELEGANT_EITHER_IS_RIGHT(either_right)) {
        printf("Success value: %d\n", ELEGANT_EITHER_RIGHT_GET(either_right, int));
    }
    
    if (ELEGANT_EITHER_IS_LEFT(either_left)) {
        printf("Error message: %s\n", ELEGANT_EITHER_LEFT_GET(either_left, char*));
    }
}

void example_scope_management(void) {
    printf("\n=== Scope Management ===\n");
    
    elegant_set_memory_mode(ELEGANT_MEMORY_STACK_ARENA);
    
    ELEGANT_SCOPE {
        int data1[] = {10, 20, 30};
        elegant_array_t* arr1 = elegant_create_array_impl(sizeof(int), data1, 3);
        elegant_array_t* arr2 = MAP_INT(arr1, x * 3);
        
        int* data_1 = (int*)elegant_array_get_data(arr1);
        int* data_2 = (int*)elegant_array_get_data(arr2);
        
        printf("Scope-managed arrays:\n");
        printf("Original: %d %d %d\n", data_1[0], data_1[1], data_1[2]);
        printf("Tripled:  %d %d %d\n", data_2[0], data_2[1], data_2[2]);
        
        printf("Arrays will be automatically cleaned up...\n");
    }
    
    printf("Scope exited, memory cleaned up automatically.\n");
}

int main(void) {
    printf("Elegant Library v%s - Simple Examples\n", ELEGANT_VERSION);
    printf("=======================================\n");
    
    example_basic_arrays();
    example_maybe_types();
    example_either_types();
    example_scope_management();
    
    printf("\n=== Memory Statistics ===\n");
    printf("Total allocated bytes: %zu\n", elegant_get_allocated_bytes());
    
    printf("\nAll examples completed successfully!\n");
    return 0;
}
