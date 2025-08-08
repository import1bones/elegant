#include <stdio.h>
#include <elegant.h>

int main() {
    puts("=== Simple Generic Test ===");
    
    // Test basic array creation
    AUTO(numbers, elegant_create_array_int(3, 1, 2, 3));
    printf("Created array: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(numbers); i++) {
        printf("%d ", ELEGANT_GET(numbers, i, int));
    }
    printf("\n");
    
    // Test legacy MAP_INT
    AUTO(doubled_old, MAP_INT(numbers, x * 2));
    printf("Legacy MAP_INT: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(doubled_old); i++) {
        printf("%d ", ELEGANT_GET(doubled_old, i, int));
    }
    printf("\n");
    
    // Test updated quicksort
    AUTO(test_array, elegant_create_array_int(5, 5, 2, 8, 1, 9));
    printf("Before sort: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(test_array); i++) {
        printf("%d ", ELEGANT_GET(test_array, i, int));
    }
    printf("\n");
    
    // Test FILTER with new syntax
    AUTO(small_numbers, FILTER(test_array, x < 5, int));
    printf("Numbers < 5: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(small_numbers); i++) {
        printf("%d ", ELEGANT_GET(small_numbers, i, int));
    }
    printf("\n");
    
    // Test ELEGANT_CONCAT
    AUTO(concatenated, ELEGANT_CONCAT(numbers, small_numbers));
    printf("Concatenated: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(concatenated); i++) {
        printf("%d ", ELEGANT_GET(concatenated, i, int));
    }
    printf("\n");
    
    puts("=== Test completed successfully! ===");
    return 0;
}
