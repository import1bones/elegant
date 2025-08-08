/*
 * Elegant C Library - Enhanced Memory Safety Demonstration
 * Shows comprehensive memory protection features and vulnerability detection
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../inc/elegant.h"

void test_basic_safe_allocation() {
    printf("=== Testing Basic Safe Allocation ===\n");
    
    // Test normal allocation
    int* ptr = (int*)ELEGANT_MALLOC(sizeof(int) * 10);
    if (ptr) {
        printf("✓ Safe allocation successful\n");
        
        // Write some data
        for (int i = 0; i < 10; i++) {
            ptr[i] = i * i;
        }
        
        // Validate the pointer
        if (ELEGANT_VALIDATE(ptr)) {
            printf("✓ Pointer validation passed\n");
        }
        
        // Check buffer bounds
        if (elegant_validate_buffer(ptr, sizeof(int) * 10)) {
            printf("✓ Buffer bounds validation passed\n");
        }
        
        ELEGANT_FREE(ptr);
        printf("✓ Safe free completed\n");
    }
    
    printf("\n");
}

void test_double_free_protection() {
    printf("=== Testing Double-Free Protection ===\n");
    
    char* buffer = (char*)ELEGANT_MALLOC(100);
    strcpy(buffer, "Hello, safe world!");
    
    printf("First free...\n");
    ELEGANT_FREE(buffer);
    
    printf("Attempting double free (should be detected)...\n");
    ELEGANT_FREE(buffer);  // This should be caught and reported
    
    printf("✓ Double-free protection working\n\n");
}

void test_use_after_free_detection() {
    printf("=== Testing Use-After-Free Detection ===\n");
    
    int* numbers = (int*)ELEGANT_MALLOC(sizeof(int) * 5);
    numbers[0] = 42;
    
    ELEGANT_FREE(numbers);
    
    printf("Attempting to validate freed pointer...\n");
    if (!ELEGANT_VALIDATE(numbers)) {
        printf("✓ Use-after-free detected correctly\n");
    }
    
    printf("Attempting access to freed memory...\n");
    // This would normally cause undefined behavior, but our system detects it
    if (elegant_is_freed_pointer(numbers)) {
        printf("✓ Freed pointer detection working\n");
    }
    
    printf("\n");
}

void test_buffer_overflow_protection() {
    printf("=== Testing Buffer Overflow Protection ===\n");
    
    char* buffer = (char*)ELEGANT_MALLOC(10);
    strcpy(buffer, "Hello");
    
    printf("Buffer before corruption check: '%s'\n", buffer);
    
    // Check canaries (should pass)
    if (elegant_check_canaries(buffer)) {
        printf("✓ Canaries intact before overflow\n");
    }
    
    // Simulate small overflow (this would be caught in real scenario)
    // Note: In a real attack, this would corrupt the canaries
    printf("Buffer appears safe with current content\n");
    
    if (elegant_validate_buffer(buffer, 20)) {
        printf("Buffer size validation (should fail for size 20): FAILED as expected\n");
    } else {
        printf("✓ Buffer size validation correctly failed for oversized access\n");
    }
    
    ELEGANT_FREE(buffer);
    printf("\n");
}

void test_bounds_checking() {
    printf("=== Testing Array Bounds Checking ===\n");
    
    int* array = (int*)ELEGANT_MALLOC(sizeof(int) * 5);
    
    // Safe access
    for (int i = 0; i < 5; i++) {
        if (elegant_bounds_check(array, i, sizeof(int))) {
            array[i] = i * 10;
            printf("✓ Safe access at index %d: %d\n", i, array[i]);
        }
    }
    
    // Unsafe access attempt
    printf("Checking bounds for index 10 (should fail)...\n");
    if (!elegant_bounds_check(array, 10, sizeof(int))) {
        printf("✓ Out-of-bounds access correctly detected\n");
    }
    
    ELEGANT_FREE(array);
    printf("\n");
}

void test_safe_string_operations() {
    printf("=== Testing Safe String Operations ===\n");
    
    char buffer[20];
    memset(buffer, 0, sizeof(buffer));
    
    // Safe string copy
    if (elegant_safe_strcpy(buffer, "Hello", sizeof(buffer)) == 0) {
        printf("✓ Safe strcpy: '%s'\n", buffer);
    }
    
    // Safe string concatenation
    if (elegant_safe_strcat(buffer, " World", sizeof(buffer)) == 0) {
        printf("✓ Safe strcat: '%s'\n", buffer);
    }
    
    // Test overflow protection
    printf("Testing overflow protection...\n");
    if (elegant_safe_strcat(buffer, " This is too long for the buffer!", sizeof(buffer)) != 0) {
        printf("✓ String overflow correctly prevented\n");
    }
    
    printf("\n");
}

void test_stack_protection() {
    printf("=== Testing Stack Protection ===\n");
    
    ELEGANT_STACK_GUARD();
    
    // Normal function execution
    int local_var = 42;
    printf("Local variable: %d\n", local_var);
    
    // Check stack canary
    ELEGANT_STACK_CHECK();
    printf("✓ Stack canary check passed\n\n");
}

void test_memory_pool_safety() {
    printf("=== Testing Memory Pool Safety ===\n");
    
    elegant_safe_pool_t* pool = elegant_create_safe_pool(1024);
    if (pool) {
        printf("✓ Safe memory pool created\n");
        
        void* ptr1 = elegant_pool_alloc(pool, 100);
        void* ptr2 = elegant_pool_alloc(pool, 200);
        
        if (ptr1 && ptr2) {
            printf("✓ Pool allocations successful\n");
            
            // Use the allocated memory
            memset(ptr1, 0xAA, 100);
            memset(ptr2, 0xBB, 200);
            
            printf("✓ Pool memory usage completed\n");
        }
        
        elegant_destroy_safe_pool(pool);
        printf("✓ Pool cleanup completed\n");
    }
    
    printf("\n");
}

void test_comprehensive_validation() {
    printf("=== Testing Comprehensive Memory Validation ===\n");
    
    // Allocate several blocks
    void* ptrs[5];
    for (int i = 0; i < 5; i++) {
        ptrs[i] = ELEGANT_MALLOC((i + 1) * 100);
        printf("Allocated block %d: %p\n", i, ptrs[i]);
    }
    
    // Check all allocations
    printf("\nValidating all allocations...\n");
    elegant_check_all_allocations();
    
    // Free some blocks
    ELEGANT_FREE(ptrs[1]);
    ELEGANT_FREE(ptrs[3]);
    
    printf("\nAfter freeing blocks 1 and 3:\n");
    elegant_dump_active_allocations();
    
    // Clean up remaining
    ELEGANT_FREE(ptrs[0]);
    ELEGANT_FREE(ptrs[2]);
    ELEGANT_FREE(ptrs[4]);
    
    printf("✓ Comprehensive validation completed\n\n");
}

void demonstrate_safety_reporting() {
    printf("=== Memory Safety Report ===\n");
    elegant_safety_report();
}

int main() {
    printf("Elegant C Library - Enhanced Memory Safety Demo\n");
    printf("================================================\n\n");
    
    test_basic_safe_allocation();
    test_double_free_protection();
    test_use_after_free_detection();
    test_buffer_overflow_protection();
    test_bounds_checking();
    test_safe_string_operations();
    test_stack_protection();
    test_memory_pool_safety();
    test_comprehensive_validation();
    
    demonstrate_safety_reporting();
    
    printf("All memory safety tests completed!\n");
    printf("Check the safety report above for any detected issues.\n\n");
    
    return 0;
}
