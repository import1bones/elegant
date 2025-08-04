/*
 * Elegant - Core Implementation
 * Version 0.0.1
 */

#include "elegant.h"
#include <stdio.h>
#include <assert.h>
#include <limits.h>

/* Thread-local memory mode */
__thread elegant_memory_mode_t elegant_current_memory_mode = ELEGANT_MEMORY_STACK_ARENA;

/* Thread-local scope stack */
__thread elegant_scope_frame_t* elegant_current_scope = NULL;

/* Thread-local memory statistics */
__thread size_t elegant_allocated_bytes = 0;
__thread size_t elegant_allocation_count = 0;

/* Memory management functions */
void elegant_set_memory_mode(elegant_memory_mode_t mode) {
    elegant_current_memory_mode = mode;
}

elegant_memory_mode_t elegant_get_memory_mode(void) {
    return elegant_current_memory_mode;
}

size_t elegant_get_allocated_bytes(void) {
    return elegant_allocated_bytes;
}

/* Memory allocation wrappers */
void* elegant_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr) {
        elegant_allocated_bytes += size;
        elegant_allocation_count++;
    }
    return ptr;
}

void* elegant_calloc(size_t nmemb, size_t size) {
    void* ptr = calloc(nmemb, size);
    if (ptr) {
        elegant_allocated_bytes += nmemb * size;
        elegant_allocation_count++;
    }
    return ptr;
}

void* elegant_realloc(void* old_ptr, size_t size) {
    void* ptr = realloc(old_ptr, size);
    if (ptr && ptr != old_ptr) {
        elegant_allocated_bytes += size;
        elegant_allocation_count++;
    }
    return ptr;
}

void elegant_free(void* ptr) {
    if (ptr) {
        free(ptr);
        elegant_allocation_count--;
        /* Note: We can't track exact freed bytes without additional metadata */
    }
}

/* Array implementation */
elegant_array_t* elegant_array_create(size_t element_size, size_t length) {
    if (length > ELEGANT_MAX_ARRAY_SIZE) {
        fprintf(stderr, "Elegant: Array size %zu exceeds maximum %d\n", 
                length, ELEGANT_MAX_ARRAY_SIZE);
        return NULL;
    }
    
    elegant_array_t* arr = elegant_malloc(sizeof(elegant_array_t));
    if (!arr) return NULL;
    
    arr->element_size = element_size;
    arr->length = length;
    arr->capacity = length;
    arr->ref_count = 1;
    arr->destructor = NULL;
    
    if (length > 0) {
        arr->data = elegant_calloc(length, element_size);
        if (!arr->data) {
            elegant_free(arr);
            return NULL;
        }
    } else {
        arr->data = NULL;
    }
    
    /* Register with current scope if in stack arena mode */
    if (elegant_current_memory_mode == ELEGANT_MEMORY_STACK_ARENA && elegant_current_scope) {
        elegant_scope_register(arr);
    }
    
    return arr;
}

void elegant_array_destroy(elegant_array_t* arr) {
    if (!arr) return;
    
    if (elegant_current_memory_mode == ELEGANT_MEMORY_REFERENCE_COUNTING) {
        arr->ref_count--;
        if (arr->ref_count > 0) {
            return;
        }
    }
    
    if (arr->destructor && arr->data) {
        arr->destructor(arr->data);
    }
    
    elegant_free(arr->data);
    elegant_free(arr);
}

elegant_array_t* elegant_array_copy(elegant_array_t* arr) {
    if (!arr) return NULL;
    
    if (elegant_current_memory_mode == ELEGANT_MEMORY_REFERENCE_COUNTING) {
        arr->ref_count++;
        return arr;
    }
    
    elegant_array_t* new_arr = elegant_array_create(arr->element_size, arr->length);
    if (!new_arr) return NULL;
    
    if (arr->data && new_arr->data) {
        memcpy(new_arr->data, arr->data, arr->length * arr->element_size);
    }
    
    new_arr->destructor = arr->destructor;
    return new_arr;
}

void* elegant_array_get_data(elegant_array_t* arr) {
    return arr ? arr->data : NULL;
}

size_t elegant_array_get_length(elegant_array_t* arr) {
    return arr ? arr->length : 0;
}

elegant_array_t* elegant_array_retain(elegant_array_t* arr) {
    if (arr) {
        arr->ref_count++;
    }
    return arr;
}

void elegant_array_release(elegant_array_t* arr) {
    if (arr) {
        arr->ref_count--;
        if (arr->ref_count <= 0) {
            elegant_array_destroy(arr);
        }
    }
}

/* Generic array creation implementation */
elegant_array_t* elegant_create_array_impl(size_t element_size, void* data, size_t length) {
    elegant_array_t* arr = elegant_array_create(element_size, length);
    if (!arr) return NULL;
    
    if (data && arr->data && length > 0) {
        memcpy(arr->data, data, length * element_size);
    }
    
    return arr;
}

/* Scope management implementation */
void elegant_scope_enter(void) {
    elegant_scope_frame_t* frame = elegant_malloc(sizeof(elegant_scope_frame_t));
    if (!frame) {
        fprintf(stderr, "Elegant: Failed to allocate scope frame\n");
        return;
    }
    
    frame->allocations = NULL;
    frame->allocation_count = 0;
    frame->allocation_capacity = 0;
    frame->parent = elegant_current_scope;
    
    elegant_current_scope = frame;
}

void elegant_scope_exit(void) {
    if (!elegant_current_scope) return;
    
    elegant_scope_frame_t* frame = elegant_current_scope;
    
    /* Clean up all allocations in this scope */
    for (size_t i = 0; i < frame->allocation_count; i++) {
        if (frame->allocations[i]) {
            elegant_array_destroy(frame->allocations[i]);
        }
    }
    
    elegant_free(frame->allocations);
    elegant_current_scope = frame->parent;
    elegant_free(frame);
}

void elegant_scope_register(elegant_array_t* array) {
    if (!elegant_current_scope || !array) return;
    
    elegant_scope_frame_t* frame = elegant_current_scope;
    
    /* Resize allocation array if needed */
    if (frame->allocation_count >= frame->allocation_capacity) {
        size_t new_capacity = frame->allocation_capacity ? frame->allocation_capacity * 2 : 8;
        elegant_array_t** new_allocations = elegant_realloc(
            frame->allocations, 
            new_capacity * sizeof(elegant_array_t*)
        );
        
        if (!new_allocations) {
            fprintf(stderr, "Elegant: Failed to resize scope allocation array\n");
            return;
        }
        
        frame->allocations = new_allocations;
        frame->allocation_capacity = new_capacity;
    }
    
    frame->allocations[frame->allocation_count++] = array;
}

#ifdef ELEGANT_DEBUG_MEMORY
void elegant_memory_debug_dump(void) {
    printf("Elegant Memory Debug:\n");
    printf("  Allocated bytes: %zu\n", elegant_allocated_bytes);
    printf("  Active allocations: %zu\n", elegant_allocation_count);
    printf("  Current memory mode: %d\n", elegant_current_memory_mode);
}
#endif

bool elegant_scope_is_valid(void) {
    return elegant_current_scope != NULL;
}

/* Collection operation implementations */

elegant_array_t* elegant_map_int(elegant_array_t* src, int (*func)(int)) {
    if (!src || !func) return NULL;
    
    size_t len = elegant_array_get_length(src);
    elegant_array_t* result = elegant_array_create(sizeof(int), len);
    if (!result) return NULL;
    
    int* src_data = (int*)elegant_array_get_data(src);
    int* dst_data = (int*)elegant_array_get_data(result);
    
    for (size_t i = 0; i < len; i++) {
        dst_data[i] = func(src_data[i]);
    }
    
    return result;
}

elegant_array_t* elegant_map_float(elegant_array_t* src, float (*func)(float)) {
    if (!src || !func) return NULL;
    
    size_t len = elegant_array_get_length(src);
    elegant_array_t* result = elegant_array_create(sizeof(float), len);
    if (!result) return NULL;
    
    float* src_data = (float*)elegant_array_get_data(src);
    float* dst_data = (float*)elegant_array_get_data(result);
    
    for (size_t i = 0; i < len; i++) {
        dst_data[i] = func(src_data[i]);
    }
    
    return result;
}

elegant_array_t* elegant_map_double(elegant_array_t* src, double (*func)(double)) {
    if (!src || !func) return NULL;
    
    size_t len = elegant_array_get_length(src);
    elegant_array_t* result = elegant_array_create(sizeof(double), len);
    if (!result) return NULL;
    
    double* src_data = (double*)elegant_array_get_data(src);
    double* dst_data = (double*)elegant_array_get_data(result);
    
    for (size_t i = 0; i < len; i++) {
        dst_data[i] = func(src_data[i]);
    }
    
    return result;
}

elegant_array_t* elegant_filter_int(elegant_array_t* src, int (*predicate)(int)) {
    if (!src || !predicate) return NULL;
    
    size_t len = elegant_array_get_length(src);
    int* src_data = (int*)elegant_array_get_data(src);
    
    // First pass: count matching elements
    size_t count = 0;
    for (size_t i = 0; i < len; i++) {
        if (predicate(src_data[i])) count++;
    }
    
    // Create result array
    elegant_array_t* result = elegant_array_create(sizeof(int), count);
    if (!result) return NULL;
    
    // Second pass: copy matching elements
    int* dst_data = (int*)elegant_array_get_data(result);
    size_t idx = 0;
    for (size_t i = 0; i < len; i++) {
        if (predicate(src_data[i])) {
            dst_data[idx++] = src_data[i];
        }
    }
    
    return result;
}

elegant_array_t* elegant_filter_float(elegant_array_t* src, int (*predicate)(float)) {
    if (!src || !predicate) return NULL;
    
    size_t len = elegant_array_get_length(src);
    float* src_data = (float*)elegant_array_get_data(src);
    
    // First pass: count matching elements
    size_t count = 0;
    for (size_t i = 0; i < len; i++) {
        if (predicate(src_data[i])) count++;
    }
    
    // Create result array
    elegant_array_t* result = elegant_array_create(sizeof(float), count);
    if (!result) return NULL;
    
    // Second pass: copy matching elements
    float* dst_data = (float*)elegant_array_get_data(result);
    size_t idx = 0;
    for (size_t i = 0; i < len; i++) {
        if (predicate(src_data[i])) {
            dst_data[idx++] = src_data[i];
        }
    }
    
    return result;
}

elegant_array_t* elegant_filter_double(elegant_array_t* src, int (*predicate)(double)) {
    if (!src || !predicate) return NULL;
    
    size_t len = elegant_array_get_length(src);
    double* src_data = (double*)elegant_array_get_data(src);
    
    // First pass: count matching elements
    size_t count = 0;
    for (size_t i = 0; i < len; i++) {
        if (predicate(src_data[i])) count++;
    }
    
    // Create result array
    elegant_array_t* result = elegant_array_create(sizeof(double), count);
    if (!result) return NULL;
    
    // Second pass: copy matching elements
    double* dst_data = (double*)elegant_array_get_data(result);
    size_t idx = 0;
    for (size_t i = 0; i < len; i++) {
        if (predicate(src_data[i])) {
            dst_data[idx++] = src_data[i];
        }
    }
    
    return result;
}

int elegant_reduce_int(elegant_array_t* src, int (*func)(int, int), int initial) {
    if (!src || !func) return initial;
    
    size_t len = elegant_array_get_length(src);
    int* src_data = (int*)elegant_array_get_data(src);
    
    int accumulator = initial;
    for (size_t i = 0; i < len; i++) {
        accumulator = func(accumulator, src_data[i]);
    }
    
    return accumulator;
}

float elegant_reduce_float(elegant_array_t* src, float (*func)(float, float), float initial) {
    if (!src || !func) return initial;
    
    size_t len = elegant_array_get_length(src);
    float* src_data = (float*)elegant_array_get_data(src);
    
    float accumulator = initial;
    for (size_t i = 0; i < len; i++) {
        accumulator = func(accumulator, src_data[i]);
    }
    
    return accumulator;
}

double elegant_reduce_double(elegant_array_t* src, double (*func)(double, double), double initial) {
    if (!src || !func) return initial;
    
    size_t len = elegant_array_get_length(src);
    double* src_data = (double*)elegant_array_get_data(src);
    
    double accumulator = initial;
    for (size_t i = 0; i < len; i++) {
        accumulator = func(accumulator, src_data[i]);
    }
    
    return accumulator;
}

int* elegant_find_int(elegant_array_t* src, int (*predicate)(int)) {
    if (!src || !predicate) return NULL;
    
    size_t len = elegant_array_get_length(src);
    int* src_data = (int*)elegant_array_get_data(src);
    
    for (size_t i = 0; i < len; i++) {
        if (predicate(src_data[i])) {
            return &src_data[i];
        }
    }
    
    return NULL;
}

float* elegant_find_float(elegant_array_t* src, int (*predicate)(float)) {
    if (!src || !predicate) return NULL;
    
    size_t len = elegant_array_get_length(src);
    float* src_data = (float*)elegant_array_get_data(src);
    
    for (size_t i = 0; i < len; i++) {
        if (predicate(src_data[i])) {
            return &src_data[i];
        }
    }
    
    return NULL;
}

double* elegant_find_double(elegant_array_t* src, int (*predicate)(double)) {
    if (!src || !predicate) return NULL;
    
    size_t len = elegant_array_get_length(src);
    double* src_data = (double*)elegant_array_get_data(src);
    
    for (size_t i = 0; i < len; i++) {
        if (predicate(src_data[i])) {
            return &src_data[i];
        }
    }
    
    return NULL;
}
