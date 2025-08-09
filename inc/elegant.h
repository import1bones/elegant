#ifndef ELEGANT_H
#define ELEGANT_H

/*
 * Elegant - Functional Programming Library for C99
 * Version 0.0.1
 * 
 * A Haskell-inspired functional programming library for C with automatic
 * memory management, type-generic operations, and inline lambda expressions.
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Version information */
#define ELEGANT_VERSION_MAJOR 0
#define ELEGANT_VERSION_MINOR 0
#define ELEGANT_VERSION_PATCH 1
#define ELEGANT_VERSION ((ELEGANT_VERSION_MAJOR << 16) | (ELEGANT_VERSION_MINOR << 8) | ELEGANT_VERSION_PATCH)

/* Compiler requirements */
#if !defined(__GNUC__) && !defined(__clang__)
#warning "Elegant works best with GCC or Clang for type-generic features"
#endif

/* IMPORTANT: This library requires GCC for full functionality */
#if defined(__clang__) || defined(_MSC_VER) || defined(__INTEL_COMPILER)
#warning "NOTICE: Elegant Library requires GCC for nested functions support. Some features may not compile with Clang, MSVC, or other compilers."
#endif

/* AUTO macro for type deduction (C99 compatible) */
#define AUTO(var, expr) __typeof__(expr) var = (expr)

/* Optional auto keyword for GCC/Clang users */
#ifdef __GNUC__
#ifndef auto
#define auto __auto_type
#endif
#endif

/* Configuration macros */
#ifndef ELEGANT_MAX_ARRAY_SIZE
#define ELEGANT_MAX_ARRAY_SIZE 1000
#endif

#ifndef ELEGANT_MAX_STACK_ALLOC
#define ELEGANT_MAX_STACK_ALLOC (64 * 1024)  /* 64KB stack limit */
#endif

/* Memory management modes */
typedef enum {
    ELEGANT_MEMORY_STACK_ARENA = 0,
    ELEGANT_MEMORY_REFERENCE_COUNTING = 1,
    ELEGANT_MEMORY_GARBAGE_COLLECTION = 2
} elegant_memory_mode_t;

/* Global memory mode (thread-local) */
extern __thread elegant_memory_mode_t elegant_current_memory_mode;

/* Memory management functions */
void elegant_set_memory_mode(elegant_memory_mode_t mode);
elegant_memory_mode_t elegant_get_memory_mode(void);
size_t elegant_get_allocated_bytes(void);

/* Array structure - internal representation */
typedef struct {
    void* data;
    size_t length;
    size_t element_size;
    size_t capacity;
    int ref_count;
    void (*destructor)(void*);
} elegant_array_t;

/* Core array operations */
elegant_array_t* elegant_array_create(size_t element_size, size_t length);
void elegant_array_destroy(elegant_array_t* arr);
elegant_array_t* elegant_array_copy(elegant_array_t* arr);
void* elegant_array_get_data(elegant_array_t* arr);
size_t elegant_array_get_length(elegant_array_t* arr);

/* Include sub-headers */
#include "elegant_core.h"
#include "elegant_collection.h"
#include "elegant_advanced.h"
#include "elegant_memory.h"
#include "elegant_maybe.h"
#include "elegant_either.h"
#include "elegant_scope.h"
#include "elegant_safety.h"

#ifdef __cplusplus
}
#endif

#endif /* ELEGANT_H */
