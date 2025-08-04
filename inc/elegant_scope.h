#ifndef ELEGANT_SCOPE_H
#define ELEGANT_SCOPE_H

/*
 * Scope-based resource management
 */

/* Scope cleanup utilities */
#define ELEGANT_DEFER(cleanup_code) \
    __attribute__((cleanup(elegant_defer_cleanup))) \
    struct elegant_defer_t { void (*fn)(void*); void* data; } \
    _elegant_defer = { \
        .fn = (void(*)(void*))({ \
            void _cleanup_fn(void* _unused) { \
                (void)_unused; \
                cleanup_code; \
            } \
            _cleanup_fn; \
        }), \
        .data = NULL \
    }

/* Cleanup function for ELEGANT_DEFER */
static inline void elegant_defer_cleanup(void* defer_ptr) {
    struct elegant_defer_t* defer = (struct elegant_defer_t*)defer_ptr;
    if (defer && defer->fn) {
        defer->fn(defer->data);
    }
}

/* Scoped pointer management */
#define ELEGANT_SCOPED_PTR(type, ptr) \
    __attribute__((cleanup(elegant_scoped_ptr_cleanup_##type))) type* ptr

/* Generate cleanup functions for common types */
#define ELEGANT_DEFINE_SCOPED_CLEANUP(type, cleanup_fn) \
    static inline void elegant_scoped_ptr_cleanup_##type(type** ptr) { \
        if (ptr && *ptr) { \
            cleanup_fn(*ptr); \
            *ptr = NULL; \
        } \
    }

/* Predefined cleanup functions */
ELEGANT_DEFINE_SCOPED_CLEANUP(elegant_array_t, elegant_array_destroy)

/* File handle cleanup */
static inline void elegant_file_cleanup(FILE** fp) {
    if (fp && *fp) {
        fclose(*fp);
        *fp = NULL;
    }
}

#define ELEGANT_SCOPED_FILE(var) \
    __attribute__((cleanup(elegant_file_cleanup))) FILE* var

/* Memory cleanup */
static inline void elegant_ptr_cleanup(void** ptr) {
    if (ptr && *ptr) {
        elegant_free(*ptr);
        *ptr = NULL;
    }
}

#define ELEGANT_SCOPED_MALLOC(type, var, size) \
    __attribute__((cleanup(elegant_ptr_cleanup))) type* var = elegant_malloc(size)

/* Scope guard macro */
#define ELEGANT_SCOPE_GUARD(enter_code, exit_code) \
    do { \
        enter_code; \
        ELEGANT_DEFER(exit_code); \
    } while(0)

#endif /* ELEGANT_SCOPE_H */
