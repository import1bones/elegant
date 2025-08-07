#ifndef ELEGANT_CORE_H
#define ELEGANT_CORE_H

/*
 * Core macros and basic array operations
 */

/* Array creation macro - compile-time initialization */
#define ELEGANT_ARRAY(type, ...) \
    elegant_create_array_##type(__VA_ARGS__)

/* Helper macro for array creation */
#define elegant_create_array_int(...) \
    elegant_create_array_impl(sizeof(int), \
        (int[]){__VA_ARGS__}, \
        sizeof((int[]){__VA_ARGS__}) / sizeof(int))

#define elegant_create_array_float(...) \
    elegant_create_array_impl(sizeof(float), \
        (float[]){__VA_ARGS__}, \
        sizeof((float[]){__VA_ARGS__}) / sizeof(float))

#define elegant_create_array_double(...) \
    elegant_create_array_impl(sizeof(double), \
        (double[]){__VA_ARGS__}, \
        sizeof((double[]){__VA_ARGS__}) / sizeof(double))

#define elegant_create_array_char(...) \
    elegant_create_array_impl(sizeof(char), \
        (char[]){__VA_ARGS__}, \
        sizeof((char[]){__VA_ARGS__}) / sizeof(char))

/* Generic array creation function */
elegant_array_t* elegant_create_array_impl(size_t element_size, void* data, size_t length);

/* Array accessor macros */
#define ELEGANT_LENGTH(arr) elegant_array_get_length(arr)
#define ELEGANT_RAW_PTR(arr) elegant_array_get_data(arr)

/* Type-generic array element access */
#define ELEGANT_GET(arr, index, type) \
    (((type*)elegant_array_get_data(arr))[index])

#define ELEGANT_SET(arr, index, value, type) \
    do { \
        ((type*)elegant_array_get_data(arr))[index] = (value); \
    } while(0)

/* Static assertions for compile-time checks */
#define ELEGANT_STATIC_ASSERT(cond) \
    _Static_assert(cond, #cond)

/* Array size checking */
#define ELEGANT_CHECK_ARRAY_SIZE(size) \
    ELEGANT_STATIC_ASSERT(size <= ELEGANT_MAX_ARRAY_SIZE)

/* Safe memory copy function */
int elegant_memcpy_safe(void* dest, size_t dest_size, const void* src, size_t copy_size);

#endif /* ELEGANT_CORE_H */
