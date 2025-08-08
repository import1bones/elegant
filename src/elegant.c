/*
 * Elegant - Core Implementation
 * Version 0.0.1
 */

#include "elegant.h"
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <errno.h>

/* Thread-local memory mode */
__thread elegant_memory_mode_t elegant_current_memory_mode = ELEGANT_MEMORY_STACK_ARENA;

/* Thread-local scope stack */
__thread elegant_scope_frame_t* elegant_current_scope = NULL;

/* Thread-local memory statistics */
__thread size_t elegant_allocated_bytes = 0;
__thread size_t elegant_allocation_count = 0;

/* Safe memory copy implementation */
int elegant_memcpy_safe(void* dest, size_t dest_size, const void* src, size_t copy_size) {
    if (!dest || !src) {
        errno = EINVAL;
        return EINVAL;
    }
    
    if (copy_size == 0) {
        return 0;  // Success, nothing to copy
    }
    
    if (copy_size > dest_size) {
        errno = ERANGE;
        return ERANGE;  // Buffer overflow
    }
    
    // Check for overlapping memory regions (like memmove)
    const char* src_bytes = (const char*)src;
    char* dest_bytes = (char*)dest;
    
    if ((src_bytes < dest_bytes && src_bytes + copy_size > dest_bytes) ||
        (dest_bytes < src_bytes && dest_bytes + dest_size > src_bytes)) {
        // Use memmove for overlapping regions
        memmove(dest, src, copy_size);
    } else {
        memcpy(dest, src, copy_size);
    }
    
    return 0;  // Success
}

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
        size_t copy_bytes = arr->length * arr->element_size;
        if (elegant_memcpy_safe(new_arr->data, copy_bytes, arr->data, copy_bytes) != 0) {
            elegant_array_destroy(new_arr);
            return NULL;
        }
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
        size_t copy_bytes = length * element_size;
        if (elegant_memcpy_safe(arr->data, copy_bytes, data, copy_bytes) != 0) {
            elegant_array_destroy(arr);
            return NULL;
        }
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

/* Array concatenation */
elegant_array_t* elegant_concat_arrays(size_t count, ...) {
    if (count == 0) return NULL;
    
    va_list args;
    va_start(args, count);
    
    // First pass: calculate total length and get element size
    size_t total_length = 0;
    size_t element_size = 0;
    elegant_array_t* arrays[count];
    
    for (size_t i = 0; i < count; i++) {
        arrays[i] = va_arg(args, elegant_array_t*);
        if (arrays[i]) {
            size_t len = elegant_array_get_length(arrays[i]);
            total_length += len;
            if (element_size == 0) {
                element_size = arrays[i]->element_size;
            }
        }
    }
    va_end(args);
    
    if (total_length == 0 || element_size == 0) return NULL;
    
    // Create result array
    elegant_array_t* result = elegant_create_array_impl(element_size, NULL, total_length);
    if (!result) return NULL;
    
    // Second pass: copy data
    char* dest_data = (char*)elegant_array_get_data(result);
    size_t offset = 0;
    
    for (size_t i = 0; i < count; i++) {
        if (arrays[i]) {
            size_t len = elegant_array_get_length(arrays[i]);
            if (len > 0) {
                char* src_data = (char*)elegant_array_get_data(arrays[i]);
                size_t copy_bytes = len * element_size;
                size_t remaining_bytes = (total_length * element_size) - offset;
                
                if (elegant_memcpy_safe(dest_data + offset, remaining_bytes, 
                                      src_data, copy_bytes) != 0) {
                    elegant_array_destroy(result);
                    return NULL;
                }
                offset += copy_bytes;
            }
        }
    }
    
    return result;
}

/* Generic collection operations */

elegant_array_t* elegant_map_generic(elegant_array_t* src, void* (*func)(void*), size_t element_size) {
    if (!src || !func) return NULL;
    
    size_t len = elegant_array_get_length(src);
    elegant_array_t* result = elegant_array_create(element_size, len);
    if (!result) return NULL;
    
    char* src_data = (char*)elegant_array_get_data(src);
    char* dst_data = (char*)elegant_array_get_data(result);
    
    for (size_t i = 0; i < len; i++) {
        void* element = src_data + (i * element_size);
        void* mapped = func(element);
        if (elegant_memcpy_safe(dst_data + (i * element_size), element_size, mapped, element_size) != 0) {
            elegant_array_destroy(result);
            return NULL;
        }
    }
    
    return result;
}

elegant_array_t* elegant_filter_generic(elegant_array_t* src, int (*predicate)(void*), size_t element_size) {
    if (!src || !predicate) return NULL;
    
    size_t len = elegant_array_get_length(src);
    char* src_data = (char*)elegant_array_get_data(src);
    
    // First pass: count matching elements
    size_t count = 0;
    for (size_t i = 0; i < len; i++) {
        void* element = src_data + (i * element_size);
        if (predicate(element)) count++;
    }
    
    // Create result array
    elegant_array_t* result = elegant_array_create(element_size, count);
    if (!result) return NULL;
    
    // Second pass: copy matching elements
    char* dst_data = (char*)elegant_array_get_data(result);
    size_t idx = 0;
    for (size_t i = 0; i < len; i++) {
        void* element = src_data + (i * element_size);
        if (predicate(element)) {
            if (elegant_memcpy_safe(dst_data + (idx * element_size), element_size, element, element_size) != 0) {
                elegant_array_destroy(result);
                return NULL;
            }
            idx++;
        }
    }
    
    return result;
}

void* elegant_reduce_generic(elegant_array_t* src, void* (*func)(void*, void*), void* initial, size_t element_size) {
    if (!src || !func || !initial) return initial;
    
    size_t len = elegant_array_get_length(src);
    char* src_data = (char*)elegant_array_get_data(src);
    
    void* accumulator = malloc(element_size);
    if (!accumulator) return initial;
    
    if (elegant_memcpy_safe(accumulator, element_size, initial, element_size) != 0) {
        free(accumulator);
        return initial;
    }
    
    for (size_t i = 0; i < len; i++) {
        void* element = src_data + (i * element_size);
        void* new_acc = func(accumulator, element);
        if (elegant_memcpy_safe(accumulator, element_size, new_acc, element_size) != 0) {
            free(accumulator);
            return initial;
        }
    }
    
    return accumulator;
}

void* elegant_fold_left_generic(elegant_array_t* src, void* (*func)(void*, void*), void* initial, size_t element_size) {
    return elegant_reduce_generic(src, func, initial, element_size);
}

void* elegant_fold_right_generic(elegant_array_t* src, void* (*func)(void*, void*), void* initial, size_t element_size) {
    if (!src || !func || !initial) return initial;
    
    size_t len = elegant_array_get_length(src);
    char* src_data = (char*)elegant_array_get_data(src);
    
    void* accumulator = malloc(element_size);
    if (!accumulator) return initial;
    
    if (elegant_memcpy_safe(accumulator, element_size, initial, element_size) != 0) {
        free(accumulator);
        return initial;
    }
    
    // Process from right to left
    for (size_t i = len; i > 0; i--) {
        void* element = src_data + ((i-1) * element_size);
        void* new_acc = func(element, accumulator);
        if (elegant_memcpy_safe(accumulator, element_size, new_acc, element_size) != 0) {
            free(accumulator);
            return initial;
        }
    }
    
    return accumulator;
}

void* elegant_find_generic(elegant_array_t* src, int (*predicate)(void*), size_t element_size) {
    if (!src || !predicate) return NULL;
    
    size_t len = elegant_array_get_length(src);
    char* src_data = (char*)elegant_array_get_data(src);
    
    for (size_t i = 0; i < len; i++) {
        void* element = src_data + (i * element_size);
        if (predicate(element)) {
            return element;
        }
    }
    
    return NULL;
}

elegant_array_t* elegant_reverse(elegant_array_t* arr) {
    if (!arr) return NULL;
    
    size_t len = elegant_array_get_length(arr);
    size_t element_size = arr->element_size;
    elegant_array_t* result = elegant_array_create(element_size, len);
    if (!result) return NULL;
    
    char* src_data = (char*)elegant_array_get_data(arr);
    char* dst_data = (char*)elegant_array_get_data(result);
    
    for (size_t i = 0; i < len; i++) {
        void* src_element = src_data + (i * element_size);
        void* dst_element = dst_data + ((len - 1 - i) * element_size);
        if (elegant_memcpy_safe(dst_element, element_size, src_element, element_size) != 0) {
            elegant_array_destroy(result);
            return NULL;
        }
    }
    
    return result;
}

elegant_array_t* elegant_take(elegant_array_t* arr, size_t n) {
    if (!arr) return NULL;
    
    size_t len = elegant_array_get_length(arr);
    size_t take_len = (n < len) ? n : len;
    size_t element_size = arr->element_size;
    
    elegant_array_t* result = elegant_array_create(element_size, take_len);
    if (!result) return NULL;
    
    if (take_len > 0) {
        char* src_data = (char*)elegant_array_get_data(arr);
        char* dst_data = (char*)elegant_array_get_data(result);
        size_t copy_bytes = take_len * element_size;
        
        if (elegant_memcpy_safe(dst_data, copy_bytes, src_data, copy_bytes) != 0) {
            elegant_array_destroy(result);
            return NULL;
        }
    }
    
    return result;
}

elegant_array_t* elegant_drop(elegant_array_t* arr, size_t n) {
    if (!arr) return NULL;
    
    size_t len = elegant_array_get_length(arr);
    if (n >= len) {
        return elegant_array_create(arr->element_size, 0);
    }
    
    size_t drop_len = len - n;
    size_t element_size = arr->element_size;
    
    elegant_array_t* result = elegant_array_create(element_size, drop_len);
    if (!result) return NULL;
    
    char* src_data = (char*)elegant_array_get_data(arr);
    char* dst_data = (char*)elegant_array_get_data(result);
    char* src_start = src_data + (n * element_size);
    size_t copy_bytes = drop_len * element_size;
    
    if (elegant_memcpy_safe(dst_data, copy_bytes, src_start, copy_bytes) != 0) {
        elegant_array_destroy(result);
        return NULL;
    }
    
    return result;
}

elegant_array_t* elegant_zip(elegant_array_t* arr1, elegant_array_t* arr2, void* (*combiner)(void*, void*), size_t result_element_size) {
    if (!arr1 || !arr2 || !combiner) return NULL;
    
    size_t len1 = elegant_array_get_length(arr1);
    size_t len2 = elegant_array_get_length(arr2);
    size_t min_len = (len1 < len2) ? len1 : len2;
    
    elegant_array_t* result = elegant_array_create(result_element_size, min_len);
    if (!result) return NULL;
    
    char* data1 = (char*)elegant_array_get_data(arr1);
    char* data2 = (char*)elegant_array_get_data(arr2);
    char* result_data = (char*)elegant_array_get_data(result);
    
    for (size_t i = 0; i < min_len; i++) {
        void* elem1 = data1 + (i * arr1->element_size);
        void* elem2 = data2 + (i * arr2->element_size);
        void* combined = combiner(elem1, elem2);
        
        if (elegant_memcpy_safe(result_data + (i * result_element_size), result_element_size, combined, result_element_size) != 0) {
            elegant_array_destroy(result);
            return NULL;
        }
    }
    
    return result;
}


/* Advanced array operations */

elegant_array_t* elegant_reverse_int(elegant_array_t* arr) {
    if (!arr) return NULL;
    
    size_t len = elegant_array_get_length(arr);
    elegant_array_t* result = elegant_array_create(sizeof(int), len);
    if (!result) return NULL;
    
    int* src_data = (int*)elegant_array_get_data(arr);
    int* dst_data = (int*)elegant_array_get_data(result);
    
    for (size_t i = 0; i < len; i++) {
        dst_data[i] = src_data[len - 1 - i];
    }
    
    return result;
}

elegant_array_t* elegant_take_int(elegant_array_t* arr, size_t n) {
    if (!arr) return NULL;
    
    size_t len = elegant_array_get_length(arr);
    size_t take_len = (n < len) ? n : len;
    
    elegant_array_t* result = elegant_array_create(sizeof(int), take_len);
    if (!result) return NULL;
    
    if (take_len > 0) {
        int* src_data = (int*)elegant_array_get_data(arr);
        int* dst_data = (int*)elegant_array_get_data(result);
        
        for (size_t i = 0; i < take_len; i++) {
            dst_data[i] = src_data[i];
        }
    }
    
    return result;
}

elegant_array_t* elegant_drop_int(elegant_array_t* arr, size_t n) {
    if (!arr) return NULL;
    
    size_t len = elegant_array_get_length(arr);
    if (n >= len) {
        return elegant_array_create(sizeof(int), 0);
    }
    
    size_t drop_len = len - n;
    elegant_array_t* result = elegant_array_create(sizeof(int), drop_len);
    if (!result) return NULL;
    
    int* src_data = (int*)elegant_array_get_data(arr);
    int* dst_data = (int*)elegant_array_get_data(result);
    
    for (size_t i = 0; i < drop_len; i++) {
        dst_data[i] = src_data[n + i];
    }
    
    return result;
}
