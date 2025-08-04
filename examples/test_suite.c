/*
 * Elegant Library - Simple Test Suite
 * Version 0.0.1
 */

#include "../inc/elegant.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf("FAIL: %s\n", message); \
            return 0; \
        } else { \
            printf("PASS: %s\n", message); \
        } \
    } while(0)

int test_array_creation(void) {
    printf("\n=== Testing Array Creation ===\n");
    
    AUTO(arr, elegant_create_array_int(1, 2, 3, 4, 5));
    TEST_ASSERT(arr != NULL, "Array creation");
    TEST_ASSERT(ELEGANT_LENGTH(arr) == 5, "Array length");
    TEST_ASSERT(ELEGANT_GET(arr, 0, int) == 1, "First element");
    TEST_ASSERT(ELEGANT_GET(arr, 4, int) == 5, "Last element");
    
    elegant_array_destroy(arr);
    return 1;
}

int test_map_operation(void) {
    printf("\n=== Testing MAP Operation ===\n");
    
    ELEGANT_SCOPE {
        AUTO(input, elegant_create_array_int(1, 2, 3));
        AUTO(result, MAP(x, input, x * 2));
        
        TEST_ASSERT(ELEGANT_LENGTH(result) == 3, "MAP result length");
        TEST_ASSERT(ELEGANT_GET(result, 0, int) == 2, "MAP first element");
        TEST_ASSERT(ELEGANT_GET(result, 1, int) == 4, "MAP second element");
        TEST_ASSERT(ELEGANT_GET(result, 2, int) == 6, "MAP third element");
    }
    
    return 1;
}

int test_filter_operation(void) {
    printf("\n=== Testing FILTER Operation ===\n");
    
    ELEGANT_SCOPE {
        AUTO(input, elegant_create_array_int(1, 2, 3, 4, 5, 6));
        AUTO(evens, FILTER(x, input, x % 2 == 0));
        
        TEST_ASSERT(ELEGANT_LENGTH(evens) == 3, "FILTER result length");
        TEST_ASSERT(ELEGANT_GET(evens, 0, int) == 2, "FILTER first element");
        TEST_ASSERT(ELEGANT_GET(evens, 1, int) == 4, "FILTER second element");
        TEST_ASSERT(ELEGANT_GET(evens, 2, int) == 6, "FILTER third element");
    }
    
    return 1;
}

int test_reduce_operation(void) {
    printf("\n=== Testing REDUCE Operation ===\n");
    
    ELEGANT_SCOPE {
        AUTO(input, elegant_create_array_int(1, 2, 3, 4, 5));
        AUTO(sum, REDUCE(a, b, input, a + b, 0));
        AUTO(product, REDUCE(a, b, input, a * b, 1));
        
        TEST_ASSERT(sum == 15, "REDUCE sum");
        TEST_ASSERT(product == 120, "REDUCE product");
    }
    
    return 1;
}

int test_array_operations(void) {
    printf("\n=== Testing Array Operations ===\n");
    
    ELEGANT_SCOPE {
        AUTO(input, elegant_create_array_int(1, 2, 3, 4, 5));
        
        // Test REVERSE
        AUTO(reversed, REVERSE(input));
        TEST_ASSERT(ELEGANT_LENGTH(reversed) == 5, "REVERSE length");
        TEST_ASSERT(ELEGANT_GET(reversed, 0, int) == 5, "REVERSE first element");
        TEST_ASSERT(ELEGANT_GET(reversed, 4, int) == 1, "REVERSE last element");
        
        // Test TAKE
        AUTO(first_three, TAKE(3, input));
        TEST_ASSERT(ELEGANT_LENGTH(first_three) == 3, "TAKE length");
        TEST_ASSERT(ELEGANT_GET(first_three, 0, int) == 1, "TAKE first element");
        TEST_ASSERT(ELEGANT_GET(first_three, 2, int) == 3, "TAKE last element");
        
        // Test DROP
        AUTO(skip_two, DROP(2, input));
        TEST_ASSERT(ELEGANT_LENGTH(skip_two) == 3, "DROP length");
        TEST_ASSERT(ELEGANT_GET(skip_two, 0, int) == 3, "DROP first element");
        TEST_ASSERT(ELEGANT_GET(skip_two, 2, int) == 5, "DROP last element");
    }
    
    return 1;
}

int test_maybe_types(void) {
    printf("\n=== Testing Maybe Types ===\n");
    
    AUTO(some_value, ELEGANT_SOME(int, 42));
    AUTO(no_value, ELEGANT_NONE(int));
    
    TEST_ASSERT(ELEGANT_IS_SOME(some_value), "SOME detection");
    TEST_ASSERT(ELEGANT_IS_NONE(no_value), "NONE detection");
    TEST_ASSERT(ELEGANT_UNWRAP(some_value) == 42, "SOME unwrap");
    TEST_ASSERT(ELEGANT_UNWRAP_OR(no_value, -1) == -1, "NONE unwrap with default");
    
    // Test safe division
    AUTO(valid_division, elegant_maybe_divide_int(10, 2));
    AUTO(invalid_division, elegant_maybe_divide_int(10, 0));
    
    TEST_ASSERT(ELEGANT_IS_SOME(valid_division), "Valid division");
    TEST_ASSERT(ELEGANT_UNWRAP(valid_division) == 5, "Division result");
    TEST_ASSERT(ELEGANT_IS_NONE(invalid_division), "Invalid division");
    
    return 1;
}

int test_either_types(void) {
    printf("\n=== Testing Either Types ===\n");
    
    AUTO(right_value, ELEGANT_RIGHT(int, 42));
    AUTO(left_value, ELEGANT_LEFT(const char*, "error"));
    
    TEST_ASSERT(ELEGANT_IS_RIGHT(right_value), "RIGHT detection");
    TEST_ASSERT(ELEGANT_IS_LEFT(left_value), "LEFT detection");
    TEST_ASSERT(ELEGANT_UNWRAP_RIGHT(right_value) == 42, "RIGHT unwrap");
    TEST_ASSERT(strcmp(ELEGANT_UNWRAP_LEFT(left_value), "error") == 0, "LEFT unwrap");
    
    // Test parsing
    AUTO(valid_parse, elegant_parse_int("123"));
    AUTO(invalid_parse, elegant_parse_int("abc"));
    
    TEST_ASSERT(ELEGANT_IS_RIGHT(valid_parse), "Valid parse");
    TEST_ASSERT(ELEGANT_UNWRAP_RIGHT(valid_parse) == 123, "Parse result");
    TEST_ASSERT(ELEGANT_IS_LEFT(invalid_parse), "Invalid parse");
    
    return 1;
}

int test_memory_management(void) {
    printf("\n=== Testing Memory Management ===\n");
    
    size_t initial_memory = elegant_get_allocated_bytes();
    
    // Test scope-based cleanup
    ELEGANT_SCOPE {
        AUTO(arr1, elegant_create_array_int(1, 2, 3, 4, 5));
        AUTO(arr2, MAP(x, arr1, x * 2));
        (void)arr2; // Suppress unused variable warning
        
        size_t scope_memory = elegant_get_allocated_bytes();
        TEST_ASSERT(scope_memory > initial_memory, "Memory allocated in scope");
    }
    
    size_t final_memory = elegant_get_allocated_bytes();
    TEST_ASSERT(final_memory <= initial_memory + 100, "Memory cleaned up after scope"); // Small tolerance for implementation overhead
    
    // Test reference counting
    ELEGANT_SET_MODE(REFERENCE_COUNTING);
    
    elegant_array_t* arr = elegant_create_array_int(1, 2, 3);
    elegant_array_t* copy1 = elegant_array_copy(arr);
    elegant_array_t* copy2 = elegant_array_copy(arr);
    
    TEST_ASSERT(arr == copy1 && arr == copy2, "Reference counting shares same pointer");
    
    elegant_array_destroy(copy1);
    elegant_array_destroy(copy2);
    elegant_array_destroy(arr);
    
    // Reset to default mode
    ELEGANT_SET_MODE(STACK_ARENA);
    
    return 1;
}

int test_sequential_composition(void) {
    printf("\n=== Testing Sequential Composition ===\n");
    
    ELEGANT_SCOPE {
        AUTO(numbers, elegant_create_array_int(1, 2, 3, 4, 5));
        AUTO(step1, MAP(x, numbers, x * x));        // Square
        AUTO(step2, FILTER(x, step1, x > 10));      // Filter > 10
        AUTO(final, REDUCE(a, b, step2, a + b, 0)); // Sum
        
        TEST_ASSERT(ELEGANT_LENGTH(step1) == 5, "Step 1 length");
        TEST_ASSERT(ELEGANT_LENGTH(step2) == 2, "Step 2 length"); // 16, 25
        TEST_ASSERT(final == 41, "Sequential composition result"); // 16 + 25
    }
    
    return 1;
}

int main(void) {
    printf("Elegant Library Test Suite v0.0.1\n");
    printf("==================================\n");
    
    int total_tests = 0;
    int passed_tests = 0;
    
    #define RUN_TEST(test_func) \
        do { \
            total_tests++; \
            if (test_func()) { \
                passed_tests++; \
            } \
        } while(0)
    
    RUN_TEST(test_array_creation);
    RUN_TEST(test_map_operation);
    RUN_TEST(test_filter_operation);
    RUN_TEST(test_reduce_operation);
    RUN_TEST(test_array_operations);
    RUN_TEST(test_maybe_types);
    RUN_TEST(test_either_types);
    RUN_TEST(test_memory_management);
    RUN_TEST(test_sequential_composition);
    
    printf("\n=== Test Results ===\n");
    printf("Passed: %d/%d tests\n", passed_tests, total_tests);
    
    if (passed_tests == total_tests) {
        printf("All tests PASSED! ✅\n");
        printf("Elegant v0.0.1 is working correctly.\n");
        return 0;
    } else {
        printf("Some tests FAILED! ❌\n");
        return 1;
    }
}
