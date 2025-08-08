#include <stdio.h>
#include <elegant.h>

// Helper functions for demonstration
int add(int a, int b) { return a + b; }
int multiply_by_2(int x) { return x * 2; }
int add_10(int x) { return x + 10; }
int square(int x) { return x * x; }

int main() {
    puts("=== Elegant v0.0.1+ Advanced Functional Programming Demo ===\n");
    
    // 1. Generic MAP, FILTER, REDUCE
    printf("1. Generic Operations:\n");
    AUTO(numbers, elegant_create_array_int(5, 1, 2, 3, 4, 5));
    
    // Generic MAP
    AUTO(doubled, MAP(numbers, x * 2, int));
    printf("Original: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(numbers); i++) {
        printf("%d ", ELEGANT_GET(numbers, i, int));
    }
    printf("\nDoubled:  ");
    for (size_t i = 0; i < ELEGANT_LENGTH(doubled); i++) {
        printf("%d ", ELEGANT_GET(doubled, i, int));
    }
    printf("\n");
    
    // Generic FILTER
    AUTO(evens, FILTER(numbers, x % 2 == 0, int));
    printf("Evens:    ");
    for (size_t i = 0; i < ELEGANT_LENGTH(evens); i++) {
        printf("%d ", ELEGANT_GET(evens, i, int));
    }
    printf("\n");
    
    // Generic REDUCE
    int sum = REDUCE(numbers, acc + x, 0, int);
    printf("Sum:      %d\n\n", sum);
    
    // 2. FOLD operations
    printf("2. Fold Operations:\n");
    AUTO(fold_numbers, elegant_create_array_int(4, 1, 2, 3, 4));
    
    int fold_left_result = FOLD_LEFT(fold_numbers, acc + x, 0, int);
    int fold_right_result = FOLD_RIGHT(fold_numbers, x + acc, 0, int);
    
    printf("Array: 1 2 3 4\n");
    printf("Fold Left (+):  %d\n", fold_left_result);
    printf("Fold Right (+): %d\n\n", fold_right_result);
    
    // 3. PIPE for function composition
    printf("3. Pipeline Operations:\n");
    int value = 5;
    
    // Single function
    int result1 = PIPE(value, multiply_by_2);
    printf("5 |> (*2) = %d\n", result1);
    
    // Multiple functions
    int result2 = PIPE(value, multiply_by_2, add_10);
    printf("5 |> (*2) |> (+10) = %d\n", result2);
    
    // Complex pipeline
    int result3 = PIPE(value, multiply_by_2, add_10, square);
    printf("5 |> (*2) |> (+10) |> (^2) = %d\n\n", result3);
    
    // 4. Array utilities
    printf("4. Array Utilities:\n");
    AUTO(demo_array, elegant_create_array_int(6, 10, 20, 30, 40, 50, 60));
    
    printf("Original: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(demo_array); i++) {
        printf("%d ", ELEGANT_GET(demo_array, i, int));
    }
    printf("\n");
    
    // REVERSE
    AUTO(reversed, elegant_reverse(demo_array));
    printf("Reversed: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(reversed); i++) {
        printf("%d ", ELEGANT_GET(reversed, i, int));
    }
    printf("\n");
    
    // TAKE
    AUTO(first_three, elegant_take(demo_array, 3));
    printf("Take 3:   ");
    for (size_t i = 0; i < ELEGANT_LENGTH(first_three); i++) {
        printf("%d ", ELEGANT_GET(first_three, i, int));
    }
    printf("\n");
    
    // DROP
    AUTO(drop_two, elegant_drop(demo_array, 2));
    printf("Drop 2:   ");
    for (size_t i = 0; i < ELEGANT_LENGTH(drop_two); i++) {
        printf("%d ", ELEGANT_GET(drop_two, i, int));
    }
    printf("\n\n");
    
    // 5. Function chaining with arrays
    printf("5. Functional Array Processing:\n");
    AUTO(data, elegant_create_array_int(8, 1, 2, 3, 4, 5, 6, 7, 8));
    
    // Chain: filter evens, double them, take first 3
    AUTO(step1, FILTER(data, x % 2 == 0, int));
    AUTO(step2, MAP(step1, x * 2, int));
    AUTO(final, elegant_take(step2, 3));
    
    printf("Original: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(data); i++) {
        printf("%d ", ELEGANT_GET(data, i, int));
    }
    printf("\nEvens:    ");
    for (size_t i = 0; i < ELEGANT_LENGTH(step1); i++) {
        printf("%d ", ELEGANT_GET(step1, i, int));
    }
    printf("\nDoubled:  ");
    for (size_t i = 0; i < ELEGANT_LENGTH(step2); i++) {
        printf("%d ", ELEGANT_GET(step2, i, int));
    }
    printf("\nFirst 3:  ");
    for (size_t i = 0; i < ELEGANT_LENGTH(final); i++) {
        printf("%d ", ELEGANT_GET(final, i, int));
    }
    printf("\n\n");
    
    // 6. Type flexibility
    printf("6. Type Flexibility:\n");
    AUTO(floats, elegant_create_array_float(4, 1.5f, 2.5f, 3.5f, 4.5f));
    AUTO(float_doubled, MAP(floats, x * 2.0f, float));
    
    printf("Float array doubled: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(float_doubled); i++) {
        printf("%.1f ", ELEGANT_GET(float_doubled, i, float));
    }
    printf("\n");
    
    float float_sum = REDUCE(floats, acc + x, 0.0f, float);
    printf("Float sum: %.1f\n\n", float_sum);
    
    printf("=== All advanced functional programming features working! ===\n");
    return 0;
}
