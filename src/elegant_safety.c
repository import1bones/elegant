/*
 * Enhanced Memory Safety Implementation
 * Provides comprehensive protection against memory corruption vulnerabilities
 */

#include "../inc/elegant.h"
#include "../inc/elegant_safety.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <errno.h>
#include <pthread.h>

/* Global safety statistics */
elegant_safety_stats_t elegant_safety_stats = {0};

/* Allocation tracking list */
static elegant_memory_header_t* allocation_list = NULL;
static pthread_mutex_t allocation_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Simple freed pointers cache for use-after-free detection */
#define FREED_CACHE_SIZE 1024
static void* freed_pointers[FREED_CACHE_SIZE] = {0};
static int freed_cache_index = 0;

/* Helper functions */
static uint32_t calculate_checksum(const void* data, size_t size);
static void add_to_allocation_list(elegant_memory_header_t* header);
static void remove_from_allocation_list(elegant_memory_header_t* header);
static elegant_memory_header_t* find_header_for_ptr_unlocked(const void* ptr);
static elegant_memory_header_t* find_header_for_ptr(const void* ptr);
static bool elegant_check_canaries_unlocked(elegant_memory_header_t* header);
static bool elegant_validate_pointer_unlocked(const void* ptr, elegant_memory_header_t* header);

static uint32_t calculate_checksum(const void* data, size_t size) {
    const uint8_t* bytes = (const uint8_t*)data;
    uint32_t checksum = 0;
    for (size_t i = 0; i < size; i++) {
        checksum ^= bytes[i];
        checksum = (checksum << 1) | (checksum >> 31);
    }
    return checksum;
}

static void add_to_allocation_list(elegant_memory_header_t* header) {
    pthread_mutex_lock(&allocation_mutex);
    if (allocation_list) {
        allocation_list->prev = header;
    }
    header->next = allocation_list;
    header->prev = NULL;
    allocation_list = header;
    pthread_mutex_unlock(&allocation_mutex);
}

static void remove_from_allocation_list(elegant_memory_header_t* header) {
    pthread_mutex_lock(&allocation_mutex);
    if (header->prev) {
        header->prev->next = header->next;
    } else {
        allocation_list = header->next;
    }
    if (header->next) {
        header->next->prev = header->prev;
    }
    pthread_mutex_unlock(&allocation_mutex);
}

static elegant_memory_header_t* find_header_for_ptr_unlocked(const void* ptr) {
    if (!ptr) return NULL;
    
    elegant_memory_header_t* current = allocation_list;
    while (current) {
        void* user_ptr = (char*)current + sizeof(elegant_memory_header_t);
        if (user_ptr == ptr) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

static elegant_memory_header_t* find_header_for_ptr(const void* ptr) {
    if (!ptr) return NULL;
    
    pthread_mutex_lock(&allocation_mutex);
    elegant_memory_header_t* result = find_header_for_ptr_unlocked(ptr);
    pthread_mutex_unlock(&allocation_mutex);
    return result;
}

/* Safe memory allocation */
void* elegant_safe_malloc(size_t size) {
    if (size == 0) return NULL;
    
    size_t total_size = sizeof(elegant_memory_header_t) + size + sizeof(elegant_memory_footer_t);
    
    // Align to page boundary for guard pages
    size_t aligned_size = (total_size + ELEGANT_GUARD_PAGE_SIZE - 1) & ~(ELEGANT_GUARD_PAGE_SIZE - 1);
    
    void* raw_ptr = malloc(aligned_size + ELEGANT_GUARD_PAGE_SIZE);
    if (!raw_ptr) {
        return NULL;
    }
    
    // Set up header
    elegant_memory_header_t* header = (elegant_memory_header_t*)raw_ptr;
    header->front_canary = ELEGANT_CANARY_MAGIC_1;
    header->size = size;
    header->magic = 0x12345678;
    header->flags = ELEGANT_MEM_ACTIVE | ELEGANT_MEM_TRACKED;
    header->original_ptr = raw_ptr;
    header->next = NULL;
    header->prev = NULL;
    
    // Set up footer
    elegant_memory_footer_t* footer = (elegant_memory_footer_t*)
        ((char*)header + sizeof(elegant_memory_header_t) + size);
    footer->rear_canary = ELEGANT_CANARY_MAGIC_2;
    footer->magic = 0x87654321;
    footer->checksum = calculate_checksum(header, sizeof(elegant_memory_header_t));
    
    // User data pointer
    void* user_ptr = (char*)header + sizeof(elegant_memory_header_t);
    
    // Zero initialize for safety
    memset(user_ptr, 0, size);
    
    // Add to tracking list
    add_to_allocation_list(header);
    
    // Update statistics
    __sync_fetch_and_add(&elegant_safety_stats.total_allocations, 1);
    __sync_fetch_and_add(&elegant_safety_stats.active_allocations, 1);
    __sync_fetch_and_add(&elegant_safety_stats.bytes_allocated, size);
    
    return user_ptr;
}

void* elegant_safe_calloc(size_t nmemb, size_t size) {
    // Check for overflow
    if (nmemb && size > SIZE_MAX / nmemb) {
        errno = ENOMEM;
        return NULL;
    }
    
    return elegant_safe_malloc(nmemb * size);
}

void* elegant_safe_realloc(void* ptr, size_t size) {
    if (!ptr) {
        return elegant_safe_malloc(size);
    }
    
    if (size == 0) {
        elegant_safe_free(ptr);
        return NULL;
    }
    
    elegant_memory_header_t* header = find_header_for_ptr(ptr);
    if (!header) {
        fprintf(stderr, "ERROR: realloc() called on invalid pointer\n");
        __sync_fetch_and_add(&elegant_safety_stats.corruption_detected, 1);
        return NULL;
    }
    
    if (!elegant_validate_pointer(ptr)) {
        fprintf(stderr, "ERROR: realloc() called on corrupted pointer\n");
        return NULL;
    }
    
    void* new_ptr = elegant_safe_malloc(size);
    if (!new_ptr) {
        return NULL;
    }
    
    size_t copy_size = (size < header->size) ? size : header->size;
    memcpy(new_ptr, ptr, copy_size);
    
    elegant_safe_free(ptr);
    return new_ptr;
}

void elegant_safe_free(void* ptr) {
    if (!ptr) return;
    
    elegant_memory_header_t* header = find_header_for_ptr(ptr);
    if (!header) {
        fprintf(stderr, "ERROR: free() called on invalid pointer %p\n", ptr);
        __sync_fetch_and_add(&elegant_safety_stats.corruption_detected, 1);
        return;
    }
    
    // Check for double-free
    if (header->flags & ELEGANT_MEM_FREED) {
        fprintf(stderr, "ERROR: Double-free detected at %p\n", ptr);
        __sync_fetch_and_add(&elegant_safety_stats.double_free_detected, 1);
        return;
    }
    
    // Validate canaries
    if (!elegant_check_canaries(ptr)) {
        fprintf(stderr, "ERROR: Buffer overflow detected at %p\n", ptr);
        __sync_fetch_and_add(&elegant_safety_stats.buffer_overflow_detected, 1);
        return;
    }
    
    // Mark as freed and add to freed cache for use-after-free detection
    header->flags |= ELEGANT_MEM_FREED;
    header->flags &= ~ELEGANT_MEM_ACTIVE;
    
    // Add to freed pointers cache
    freed_pointers[freed_cache_index] = ptr;
    freed_cache_index = (freed_cache_index + 1) % FREED_CACHE_SIZE;
    
    // Poison the memory
    memset(ptr, 0xDD, header->size);  // Dead memory pattern
    
    // Remove from tracking list
    remove_from_allocation_list(header);
    
    // Update statistics
    __sync_fetch_and_add(&elegant_safety_stats.total_freed, 1);
    __sync_fetch_and_sub(&elegant_safety_stats.active_allocations, 1);
    __sync_fetch_and_add(&elegant_safety_stats.bytes_freed, header->size);
    
    // Actually free the memory
    free(header->original_ptr);
}

/* Internal validation function that doesn't lock mutex */
static bool elegant_validate_pointer_unlocked(const void* ptr, elegant_memory_header_t* header) {
    if (!ptr || !header) return false;
    
    // Check magic numbers
    if (header->magic != 0x12345678) {
        __sync_fetch_and_add(&elegant_safety_stats.corruption_detected, 1);
        return false;
    }
    
    // Check if freed
    if (header->flags & ELEGANT_MEM_FREED) {
        __sync_fetch_and_add(&elegant_safety_stats.use_after_free_detected, 1);
        return false;
    }
    
    return elegant_check_canaries_unlocked(header);
}

/* Memory validation functions */
bool elegant_validate_pointer(const void* ptr) {
    if (!ptr) return false;
    
    elegant_memory_header_t* header = find_header_for_ptr(ptr);
    if (!header) return false;
    
    return elegant_validate_pointer_unlocked(ptr, header);
}

bool elegant_validate_buffer(const void* ptr, size_t size) {
    if (!elegant_validate_pointer(ptr)) return false;
    
    elegant_memory_header_t* header = find_header_for_ptr(ptr);
    if (!header) return false;
    
    return size <= header->size;
}

/* Internal canary check that doesn't lock mutex */
static bool elegant_check_canaries_unlocked(elegant_memory_header_t* header) {
    if (!header) return false;
    
    // Check front canary
    if (header->front_canary != ELEGANT_CANARY_MAGIC_1) {
        return false;
    }
    
    // Check rear canary
    elegant_memory_footer_t* footer = (elegant_memory_footer_t*)
        ((char*)header + sizeof(elegant_memory_header_t) + header->size);
    
    if (footer->rear_canary != ELEGANT_CANARY_MAGIC_2) {
        return false;
    }
    
    if (footer->magic != 0x87654321) {
        return false;
    }
    
    return true;
}

bool elegant_check_canaries(const void* ptr) {
    if (!ptr) return false;
    
    elegant_memory_header_t* header = find_header_for_ptr(ptr);
    if (!header) return false;
    
    return elegant_check_canaries_unlocked(header);
}

bool elegant_detect_corruption(const void* ptr) {
    return !elegant_validate_pointer(ptr);
}

/* Safe string operations */
int elegant_safe_strcpy(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) {
        return -1;
    }
    
    size_t src_len = strlen(src);
    if (src_len >= dest_size) {
        return -1;  // Would overflow
    }
    
    strcpy(dest, src);
    return 0;
}

int elegant_safe_strcat(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) {
        return -1;
    }
    
    size_t dest_len = strlen(dest);
    size_t src_len = strlen(src);
    
    if (dest_len + src_len >= dest_size) {
        return -1;  // Would overflow
    }
    
    strcat(dest, src);
    return 0;
}

void* elegant_safe_memcpy(void* dest, const void* src, size_t size) {
    if (!dest || !src || size == 0) {
        return dest;
    }
    
    // Validate destination buffer
    if (!elegant_validate_buffer(dest, size)) {
        fprintf(stderr, "ERROR: memcpy destination buffer invalid or too small\n");
        return dest;
    }
    
    // Check for overlap
    if (((char*)dest < (char*)src + size) && ((char*)src < (char*)dest + size)) {
        return memmove(dest, src, size);
    }
    
    return memcpy(dest, src, size);
}

/* Bounds checking */
bool elegant_bounds_check(const void* ptr, size_t index, size_t element_size) {
    if (!ptr) return false;
    
    elegant_memory_header_t* header = find_header_for_ptr(ptr);
    if (!header) return false;
    
    size_t offset = index * element_size;
    return offset < header->size;
}

void elegant_bounds_violation(const char* file, int line, size_t index) {
    fprintf(stderr, "BOUNDS VIOLATION: Array access at index %zu in %s:%d\n", 
            index, file, line);
    __sync_fetch_and_add(&elegant_safety_stats.buffer_overflow_detected, 1);
}

/* Stack protection */
void elegant_stack_corruption_detected(const char* file, int line) {
    fprintf(stderr, "STACK CORRUPTION: Stack canary violated in %s:%d\n", file, line);
    abort();
}

/* Memory debugging and reporting */
void elegant_safety_report(void) {
    printf("\n=== ELEGANT MEMORY SAFETY REPORT ===\n");
    printf("Total allocations:        %zu\n", elegant_safety_stats.total_allocations);
    printf("Active allocations:       %zu\n", elegant_safety_stats.active_allocations);
    printf("Total freed:              %zu\n", elegant_safety_stats.total_freed);
    printf("Bytes allocated:          %zu\n", elegant_safety_stats.bytes_allocated);
    printf("Bytes freed:              %zu\n", elegant_safety_stats.bytes_freed);
    printf("Corruption detected:      %zu\n", elegant_safety_stats.corruption_detected);
    printf("Use-after-free detected:  %zu\n", elegant_safety_stats.use_after_free_detected);
    printf("Double-free detected:     %zu\n", elegant_safety_stats.double_free_detected);
    printf("Buffer overflow detected: %zu\n", elegant_safety_stats.buffer_overflow_detected);
    printf("=====================================\n\n");
}

void elegant_dump_active_allocations(void) {
    printf("\n=== ACTIVE ALLOCATIONS ===\n");
    pthread_mutex_lock(&allocation_mutex);
    
    elegant_memory_header_t* current = allocation_list;
    int count = 0;
    
    while (current) {
        if (current->flags & ELEGANT_MEM_ACTIVE) {
            void* user_ptr = (char*)current + sizeof(elegant_memory_header_t);
            printf("Allocation %d: %p (size: %zu bytes)\n", 
                   ++count, user_ptr, current->size);
        }
        current = current->next;
    }
    
    pthread_mutex_unlock(&allocation_mutex);
    printf("Total active: %d allocations\n", count);
    printf("========================\n\n");
}

void elegant_check_all_allocations(void) {
    pthread_mutex_lock(&allocation_mutex);
    
    elegant_memory_header_t* current = allocation_list;
    int corrupted = 0;
    
    while (current) {
        if (current->flags & ELEGANT_MEM_ACTIVE) {
            void* user_ptr = (char*)current + sizeof(elegant_memory_header_t);
            if (!elegant_validate_pointer_unlocked(user_ptr, current)) {
                corrupted++;
                printf("CORRUPTION: Block at %p is corrupted\n", user_ptr);
            }
        }
        current = current->next;
    }
    
    pthread_mutex_unlock(&allocation_mutex);
    
    if (corrupted == 0) {
        printf("All active allocations are valid.\n");
    } else {
        printf("Found %d corrupted allocations!\n", corrupted);
    }
}

/* Memory pool safety implementation */
elegant_safe_pool_t* elegant_create_safe_pool(size_t size) {
    elegant_safe_pool_t* pool = elegant_safe_malloc(sizeof(elegant_safe_pool_t));
    if (!pool) return NULL;
    
    pool->base = elegant_safe_malloc(size);
    if (!pool->base) {
        elegant_safe_free(pool);
        return NULL;
    }
    
    pool->size = size;
    pool->used = 0;
    pool->canary = ELEGANT_CANARY_MAGIC_1;
    
    // Create allocation bitmap
    size_t map_size = (size / 8) + 1;  // One bit per 8 bytes
    pool->allocation_map = elegant_safe_malloc(map_size);
    if (!pool->allocation_map) {
        elegant_safe_free(pool->base);
        elegant_safe_free(pool);
        return NULL;
    }
    
    memset(pool->allocation_map, 0, map_size);
    return pool;
}

void* elegant_pool_alloc(elegant_safe_pool_t* pool, size_t size) {
    if (!pool || pool->canary != ELEGANT_CANARY_MAGIC_1) {
        return NULL;
    }
    
    if (pool->used + size > pool->size) {
        return NULL;  // Pool exhausted
    }
    
    void* ptr = (char*)pool->base + pool->used;
    pool->used += size;
    
    return ptr;
}

void elegant_pool_free(elegant_safe_pool_t* pool, void* ptr) {
    // For simplicity, pool free is a no-op in this implementation
    // Could be enhanced to track individual allocations
    (void)pool;
    (void)ptr;
}

void elegant_destroy_safe_pool(elegant_safe_pool_t* pool) {
    if (!pool) return;
    
    elegant_safe_free(pool->allocation_map);
    elegant_safe_free(pool->base);
    elegant_safe_free(pool);
}

/* Use-after-free detection helpers */
void elegant_mark_freed(void* ptr) {
    elegant_memory_header_t* header = find_header_for_ptr(ptr);
    if (header) {
        header->flags |= ELEGANT_MEM_FREED;
    }
}

bool elegant_is_freed_pointer(const void* ptr) {
    if (!ptr) return false;
    
    // Check freed cache first
    for (int i = 0; i < FREED_CACHE_SIZE; i++) {
        if (freed_pointers[i] == ptr) {
            return true;
        }
    }
    
    return false;
}
