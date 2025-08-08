#ifndef ELEGANT_SAFETY_H
#define ELEGANT_SAFETY_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Enhanced Memory Safety Features for Elegant C Library
 * Provides protection against common memory corruption vulnerabilities
 */

/* Memory safety configuration */
#ifndef ELEGANT_SAFETY_ENABLED
#define ELEGANT_SAFETY_ENABLED 1
#endif

#ifndef ELEGANT_GUARD_PAGE_SIZE
#define ELEGANT_GUARD_PAGE_SIZE 4096
#endif

#ifndef ELEGANT_CANARY_SIZE
#define ELEGANT_CANARY_SIZE 8
#endif

/* Magic canary values for detection */
#define ELEGANT_CANARY_MAGIC_1 0xDEADBEEFCAFEBABEULL
#define ELEGANT_CANARY_MAGIC_2 0xBADC0FFEE0DDF00DULL
#define ELEGANT_FREED_MAGIC    0xFEEDFACEDEADC0DEULL

/* Memory block header for tracking */
typedef struct elegant_memory_header {
    uint64_t front_canary;
    size_t size;
    uint32_t magic;
    uint32_t flags;
    void* original_ptr;  /* For alignment tracking */
    struct elegant_memory_header* next;
    struct elegant_memory_header* prev;
} elegant_memory_header_t;

/* Memory block footer for overflow detection */
typedef struct elegant_memory_footer {
    uint64_t rear_canary;
    uint32_t magic;
    uint32_t checksum;
} elegant_memory_footer_t;

/* Memory safety flags */
typedef enum {
    ELEGANT_MEM_ACTIVE      = 0x01,
    ELEGANT_MEM_FREED       = 0x02,
    ELEGANT_MEM_GUARDED     = 0x04,
    ELEGANT_MEM_ZEROED      = 0x08,
    ELEGANT_MEM_ALIGNED     = 0x10,
    ELEGANT_MEM_TRACKED     = 0x20
} elegant_memory_flags_t;

/* Memory safety statistics */
typedef struct {
    size_t total_allocations;
    size_t active_allocations;
    size_t total_freed;
    size_t bytes_allocated;
    size_t bytes_freed;
    size_t corruption_detected;
    size_t use_after_free_detected;
    size_t double_free_detected;
    size_t buffer_overflow_detected;
} elegant_safety_stats_t;

/* Global safety statistics */
extern elegant_safety_stats_t elegant_safety_stats;

/* Core safety functions */
void* elegant_safe_malloc(size_t size);
void* elegant_safe_calloc(size_t nmemb, size_t size);
void* elegant_safe_realloc(void* ptr, size_t size);
void elegant_safe_free(void* ptr);

/* Memory validation functions */
bool elegant_validate_pointer(const void* ptr);
bool elegant_validate_buffer(const void* ptr, size_t size);
bool elegant_check_canaries(const void* ptr);
bool elegant_detect_corruption(const void* ptr);

/* Buffer safety macros */
#define ELEGANT_SAFE_STRCPY(dest, src, size) \
    elegant_safe_strcpy((dest), (src), (size))

#define ELEGANT_SAFE_STRCAT(dest, src, size) \
    elegant_safe_strcat((dest), (src), (size))

#define ELEGANT_SAFE_MEMCPY(dest, src, size) \
    elegant_safe_memcpy((dest), (src), (size))

#define ELEGANT_BOUNDS_CHECK(ptr, index, size) \
    elegant_bounds_check((ptr), (index), (size))

/* Stack protection */
#define ELEGANT_STACK_GUARD() \
    uint64_t __stack_canary = ELEGANT_CANARY_MAGIC_1; \
    (void)__stack_canary

#define ELEGANT_STACK_CHECK() \
    do { \
        if (__stack_canary != ELEGANT_CANARY_MAGIC_1) { \
            elegant_stack_corruption_detected(__FILE__, __LINE__); \
        } \
    } while(0)

/* Array bounds checking wrapper */
#define ELEGANT_ARRAY_ACCESS(arr, index, type) \
    (elegant_bounds_check((arr), (index), sizeof(type)) ? \
     ((type*)(arr))[index] : \
     (elegant_bounds_violation(__FILE__, __LINE__, (index)), \
      ((type*)(arr))[0]))

/* Safe string operations */
int elegant_safe_strcpy(char* dest, const char* src, size_t dest_size);
int elegant_safe_strcat(char* dest, const char* src, size_t dest_size);
void* elegant_safe_memcpy(void* dest, const void* src, size_t size);

/* Bounds checking */
bool elegant_bounds_check(const void* ptr, size_t index, size_t element_size);
void elegant_bounds_violation(const char* file, int line, size_t index);

/* Stack protection */
void elegant_stack_corruption_detected(const char* file, int line);

/* Memory debugging and reporting */
void elegant_safety_report(void);
void elegant_dump_active_allocations(void);
void elegant_check_all_allocations(void);

/* Memory pool safety */
typedef struct elegant_safe_pool {
    void* base;
    size_t size;
    size_t used;
    bool* allocation_map;
    uint64_t canary;
} elegant_safe_pool_t;

elegant_safe_pool_t* elegant_create_safe_pool(size_t size);
void* elegant_pool_alloc(elegant_safe_pool_t* pool, size_t size);
void elegant_pool_free(elegant_safe_pool_t* pool, void* ptr);
void elegant_destroy_safe_pool(elegant_safe_pool_t* pool);

/* Use-after-free detection */
void elegant_mark_freed(void* ptr);
bool elegant_is_freed_pointer(const void* ptr);

/* Configuration macros */
#if ELEGANT_SAFETY_ENABLED
    #define ELEGANT_MALLOC(size)           elegant_safe_malloc(size)
    #define ELEGANT_CALLOC(nmemb, size)    elegant_safe_calloc(nmemb, size)
    #define ELEGANT_REALLOC(ptr, size)     elegant_safe_realloc(ptr, size)
    #define ELEGANT_FREE(ptr)              elegant_safe_free(ptr)
    #define ELEGANT_VALIDATE(ptr)          elegant_validate_pointer(ptr)
#else
    #define ELEGANT_MALLOC(size)           malloc(size)
    #define ELEGANT_CALLOC(nmemb, size)    calloc(nmemb, size)
    #define ELEGANT_REALLOC(ptr, size)     realloc(ptr, size)
    #define ELEGANT_FREE(ptr)              free(ptr)
    #define ELEGANT_VALIDATE(ptr)          true
#endif

#endif /* ELEGANT_SAFETY_H */
