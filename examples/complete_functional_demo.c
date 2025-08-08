#include <stdio.h>
#include <elegant.h>

// Helper functions for PIPE demonstration
int double_value(int x) { return x * 2; }
int add_ten(int x) { return x + 10; }
int square(int x) { return x * x; }

int main() {
    puts("=== Elegant Functional Programming Features Demo ===\n");
    
    // 1. Basic MAP, FILTER, REDUCE with original type-specific macros
    printf("1. Core Functional Operations:\n");
    AUTO(numbers, elegant_create_array_int(1, 2, 3, 4, 5, 6));
    
    printf("Original: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(numbers); i++) {
        printf("%d ", ELEGANT_GET(numbers, i, int));
    }
    printf("\n");
    
    // MAP - double all values
    AUTO(doubled, MAP_INT(numbers, x * 2));
    printf("Doubled:  ");
    for (size_t i = 0; i < ELEGANT_LENGTH(doubled); i++) {
        printf("%d ", ELEGANT_GET(doubled, i, int));
    }
    printf("\n");
    
    // FILTER - get even numbers only
    AUTO(evens, FILTER_INT(doubled, x % 2 == 0));
    printf("Evens:    ");
    for (size_t i = 0; i < ELEGANT_LENGTH(evens); i++) {
        printf("%d ", ELEGANT_GET(evens, i, int));
    }
    printf("\n");
    
    // REDUCE - sum all values
    int sum = REDUCE_INT(evens, acc + x, 0);
    printf("Sum:      %d\n\n", sum);
    
    // 2. PIPE operator for function composition
    printf("2. PIPE Function Composition:\n");
    int value = 5;
    
    int result1 = PIPE(value, double_value);
    printf("5 |> double = %d\n", result1);
    
    int result2 = PIPE(value, double_value, add_ten);
    printf("5 |> double |> add_ten = %d\n", result2);
    
    int result3 = PIPE(value, double_value, add_ten, square);
    printf("5 |> double |> add_ten |> square = %d\n\n", result3);
    
    // 3. FOLD operations
    printf("3. Fold Operations:\n");
    AUTO(fold_data, elegant_create_array_int(1, 2, 3, 4));
    
    printf("Array: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(fold_data); i++) {
        printf("%d ", ELEGANT_GET(fold_data, i, int));
    }
    printf("\n");
    
    int fold_left = FOLD_LEFT_INT(fold_data, acc + x, 0);
    int fold_right = FOLD_RIGHT_INT(fold_data, acc + x, 0);
    
    printf("Fold Left (+):  %d\n", fold_left);
    printf("Fold Right (+): %d\n\n", fold_right);
    
    // 4. Array utilities - TAKE, DROP
    printf("4. Array Utilities:\n");
    AUTO(demo_array, elegant_create_array_int(10, 20, 30, 40, 50, 60, 70, 80));
    
    printf("Original: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(demo_array); i++) {
        printf("%d ", ELEGANT_GET(demo_array, i, int));
    }
    printf("\n");
    
    // TAKE first 3 elements
    AUTO(first_three, TAKE_INT(demo_array, 3));
    printf("Take 3:   ");
    for (size_t i = 0; i < ELEGANT_LENGTH(first_three); i++) {
        printf("%d ", ELEGANT_GET(first_three, i, int));
    }
    printf("\n");
    
    // DROP first 2 elements
    AUTO(drop_two, DROP_INT(demo_array, 2));
    printf("Drop 2:   ");
    for (size_t i = 0; i < ELEGANT_LENGTH(drop_two); i++) {
        printf("%d ", ELEGANT_GET(drop_two, i, int));
    }
    printf("\n");
    
    // REVERSE array
    AUTO(reversed, elegant_reverse_int(demo_array));
    printf("Reversed: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(reversed); i++) {
        printf("%d ", ELEGANT_GET(reversed, i, int));
    }
    printf("\n\n");
    
    // 5. Currying demonstration
    printf("5. Currying Support:\n");
    AUTO(add_5, CURRY_ADD(5));
    AUTO(multiply_3, CURRY_MULTIPLY(3));
    
    printf("add_5(10) = %d\n", add_5(10));
    printf("multiply_3(7) = %d\n\n", multiply_3(7));
    
    // 6. Safe concatenation
    printf("6. Array Concatenation:\n");
    AUTO(arr1, elegant_create_array_int(1, 2, 3));
    AUTO(arr2, elegant_create_array_int(4, 5, 6));
    AUTO(concatenated, ELEGANT_CONCAT(arr1, arr2));
    
    printf("Array 1: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(arr1); i++) {
        printf("%d ", ELEGANT_GET(arr1, i, int));
    }
    printf("\nArray 2: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(arr2); i++) {
        printf("%d ", ELEGANT_GET(arr2, i, int));
    }
    printf("\nConcat:  ");
    for (size_t i = 0; i < ELEGANT_LENGTH(concatenated); i++) {
        printf("%d ", ELEGANT_GET(concatenated, i, int));
    }
    printf("\n\n");
    
    // 7. Complex functional chain
    printf("7. Functional Processing Chain:\n");
    AUTO(chain_data, elegant_create_array_int(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
    
    // Chain: filter odds -> double them -> take first 3 -> sum
    AUTO(odds, FILTER_INT(chain_data, x % 2 == 1));
    AUTO(doubled_odds, MAP_INT(odds, x * 2));
    AUTO(first_three_doubled, TAKE_INT(doubled_odds, 3));
    int chain_sum = REDUCE_INT(first_three_doubled, acc + x, 0);
    
    printf("Original: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(chain_data); i++) {
        printf("%d ", ELEGANT_GET(chain_data, i, int));
    }
    printf("\nOdds:     ");
    for (size_t i = 0; i < ELEGANT_LENGTH(odds); i++) {
        printf("%d ", ELEGANT_GET(odds, i, int));
    }
    printf("\nDoubled:  ");
    for (size_t i = 0; i < ELEGANT_LENGTH(doubled_odds); i++) {
        printf("%d ", ELEGANT_GET(doubled_odds, i, int));
    }
    printf("\nFirst 3:  ");
    for (size_t i = 0; i < ELEGANT_LENGTH(first_three_doubled); i++) {
        printf("%d ", ELEGANT_GET(first_three_doubled, i, int));
    }
    printf("\nSum:      %d\n\n", chain_sum);
    
    printf("=== All functional programming features working successfully! ===\n");
    return 0;
}
