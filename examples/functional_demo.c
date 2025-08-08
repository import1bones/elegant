/*
 * Elegant Library - Basic Functional Operations Demo
 * Version 0.0.1
 */

#include "../inc/elegant.h"

int main(void) {
    printf("Elegant Library v0.0.1 - Functional Operations Demo\n");
    printf("==================================================\n");
    
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
    int sum = REDUCE_INT(evens, acc + x, 0);
    printf("Sum of even numbers: %d\n", sum);
    
    // Test REVERSE operation
    elegant_array_t* reversed = REVERSE(arr);
    int* rev_data = (int*)elegant_array_get_data(reversed);
    printf("Reversed: ");
    for (size_t i = 0; i < elegant_array_get_length(reversed); i++) {
        printf("%d ", rev_data[i]);
    }
    printf("\n");
    
    // Test TAKE and DROP
    elegant_array_t* first_three = TAKE(3, arr);
    printf("First three: ");
    for (size_t i = 0; i < elegant_array_get_length(first_three); i++) {
        printf("%d ", ELEGANT_GET(first_three, i, int));
    }
    printf("\n");
    
    elegant_array_t* last_three = DROP(2, arr);
    printf("Drop first 2: ");
    for (size_t i = 0; i < elegant_array_get_length(last_three); i++) {
        printf("%d ", ELEGANT_GET(last_three, i, int));
    }
    printf("\n");
    
    // Test FIND operation
    int* found = FIND_INT(arr, x > 3);
    if (found) {
        printf("First element > 3: %d\n", *found);
    } else {
        printf("No element > 3 found\n");
    }
    
    printf("\n=== Memory Statistics ===\n");
    printf("Total allocated bytes: %zu\n", elegant_get_allocated_bytes());
    
    // Cleanup
    elegant_array_destroy(arr);
    elegant_array_destroy(doubled);
    elegant_array_destroy(evens);
    elegant_array_destroy(reversed);
    elegant_array_destroy(first_three);
    elegant_array_destroy(last_three);
    
    printf("\nDemo completed successfully!\n");
    return 0;
}
