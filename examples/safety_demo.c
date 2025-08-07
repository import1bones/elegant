#include <stdio.h>
#include <elegant.h>

int main() {
    puts("Elegant Safety Demo - Safe Memory Operations");
    
    // Test 1: Normal operation
    AUTO(arr1, elegant_create_array_int(3, 1, 2, 3));
    AUTO(arr2, elegant_create_array_int(2, 4, 5));
    
    printf("Array 1: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(arr1); i++) {
        printf("%d ", ELEGANT_GET(arr1, i, int));
    }
    printf("\n");
    
    printf("Array 2: ");
    for (size_t i = 0; i < ELEGANT_LENGTH(arr2); i++) {
        printf("%d ", ELEGANT_GET(arr2, i, int));
    }
    printf("\n");
    
    // Test 2: Safe concatenation
    AUTO(concatenated, ELEGANT_CONCAT(arr1, arr2));
    if (concatenated) {
        printf("Concatenated: ");
        for (size_t i = 0; i < ELEGANT_LENGTH(concatenated); i++) {
            printf("%d ", ELEGANT_GET(concatenated, i, int));
        }
        printf("\n");
    }
    
    // Test 3: NULL safety
    AUTO(null_concat, ELEGANT_CONCAT(arr1, NULL, arr2));
    if (null_concat) {
        printf("NULL-safe concat: ");
        for (size_t i = 0; i < ELEGANT_LENGTH(null_concat); i++) {
            printf("%d ", ELEGANT_GET(null_concat, i, int));
        }
        printf("\n");
    }
    
    // Test 4: Empty array handling
    AUTO(empty, elegant_create_array_int(0));
    AUTO(with_empty, ELEGANT_CONCAT(arr1, empty, arr2));
    if (with_empty) {
        printf("With empty array: ");
        for (size_t i = 0; i < ELEGANT_LENGTH(with_empty); i++) {
            printf("%d ", ELEGANT_GET(with_empty, i, int));
        }
        printf("\n");
    }
    
    printf("All safety tests passed!\n");
    return 0;
}
