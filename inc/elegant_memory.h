#ifndef ELEGANT_MEMORY_H
#define ELEGANT_MEMORY_H

/*
 * Memory management system with multiple strategies
 */

/* Memory management macros */
#define ELEGANT_SET_MODE(mode) elegant_set_memory_mode(ELEGANT_MEMORY_##mode)

/* Scope-based memory management */
typedef struct elegant_scope_frame {
    elegant_array_t** allocations;
    size_t allocation_count;
    size_t allocation_capacity;
    struct elegant_scope_frame* parent;
} elegant_scope_frame_t;

/* Thread-local scope stack */
extern __thread elegant_scope_frame_t* elegant_current_scope;

/* Scope management functions */
void elegant_scope_enter(void);
void elegant_scope_exit(void);
void elegant_scope_register(elegant_array_t* array);

/* ELEGANT_SCOPE macro for automatic cleanup */
#define ELEGANT_SCOPE \
    for (int _scope_init = (elegant_scope_enter(), 1); \
         _scope_init; \
         _scope_init = 0, elegant_scope_exit())

/* Reference counting support */
elegant_array_t* elegant_array_retain(elegant_array_t* arr);
void elegant_array_release(elegant_array_t* arr);

/* Memory debugging support */
#ifdef ELEGANT_DEBUG_MEMORY
void elegant_memory_debug_dump(void);
size_t elegant_memory_debug_get_allocations(void);
#define ELEGANT_MEMORY_DEBUG_DUMP() elegant_memory_debug_dump()
#else
#define ELEGANT_MEMORY_DEBUG_DUMP() do {} while(0)
#endif

/* Memory allocation wrappers */
void* elegant_malloc(size_t size);
void* elegant_calloc(size_t nmemb, size_t size);
void* elegant_realloc(void* ptr, size_t size);
void elegant_free(void* ptr);

#endif /* ELEGANT_MEMORY_H */
