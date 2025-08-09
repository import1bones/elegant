#ifndef ELEGANT_COLLECTION_H
#define ELEGANT_COLLECTION_H

/*
 * Functional collection operations: Generic MAP, FILTER, REDUCE, PIPE, PARTIAL, FOLD
 * Advanced functional programming features for Elegant v0.0.1+
 */

/* Core generic function implementations */
elegant_array_t* elegant_map_generic(elegant_array_t* src, void* (*func)(void*), size_t element_size);
elegant_array_t* elegant_filter_generic(elegant_array_t* src, int (*predicate)(void*), size_t element_size);
void* elegant_reduce_generic(elegant_array_t* src, void* (*func)(void*, void*), void* initial, size_t element_size);
void* elegant_fold_left_generic(elegant_array_t* src, void* (*func)(void*, void*), void* initial, size_t element_size);
void* elegant_fold_right_generic(elegant_array_t* src, void* (*func)(void*, void*), void* initial, size_t element_size);

/* Generic MAP macro - works with any type */
#define MAP(arr, expr, type) ({ \
    void* _map_func(void* elem_ptr) { \
        type x = *(type*)elem_ptr; \
        static __thread type _temp_result; \
        _temp_result = (expr); \
        return &_temp_result; \
    } \
    elegant_array_t* _result = elegant_map_generic((arr), _map_func, sizeof(type)); \
    _result; \
})

/* Generic FILTER macro - works with any type */
#define FILTER(arr, predicate, type) ({ \
    int _filter_func(void* elem_ptr) { \
        type x = *(type*)elem_ptr; \
        return (predicate); \
    } \
    elegant_filter_generic((arr), _filter_func, sizeof(type)); \
})

/* Generic REDUCE macro - works with any type */
#define REDUCE(arr, func_expr, initial, type) ({ \
    void* _reduce_func(void* acc_ptr, void* elem_ptr) { \
        static type _temp_result; \
        type acc = *(type*)acc_ptr; \
        type x = *(type*)elem_ptr; \
        _temp_result = (func_expr); \
        return &_temp_result; \
    } \
    type _initial = (initial); \
    type* _result = (type*)elegant_reduce_generic((arr), _reduce_func, &_initial, sizeof(type)); \
    _result ? *_result : _initial; \
})

/* FOLD_LEFT - fold from left to right */
#define FOLD_LEFT(arr, func_expr, initial, type) ({ \
    void* _fold_func(void* acc_ptr, void* elem_ptr) { \
        static type _temp_result; \
        type acc = *(type*)acc_ptr; \
        type x = *(type*)elem_ptr; \
        _temp_result = (func_expr); \
        return &_temp_result; \
    } \
    type _initial = (initial); \
    type* _result = (type*)elegant_fold_left_generic((arr), _fold_func, &_initial, sizeof(type)); \
    _result ? *_result : _initial; \
})

/* FOLD_RIGHT - fold from right to left */
#define FOLD_RIGHT(arr, func_expr, initial, type) ({ \
    void* _fold_func(void* elem_ptr, void* acc_ptr) { \
        static type _temp_result; \
        type x = *(type*)elem_ptr; \
        type acc = *(type*)acc_ptr; \
        _temp_result = (func_expr); \
        return &_temp_result; \
    } \
    type _initial = (initial); \
    type* _result = (type*)elegant_fold_right_generic((arr), _fold_func, &_initial, sizeof(type)); \
    _result ? *_result : _initial; \
})

/* PIPE operator for function composition - supports up to 5 functions */
#define PIPE1(val, f1) f1(val)
#define PIPE2(val, f1, f2) f2(f1(val))
#define PIPE3(val, f1, f2, f3) f3(f2(f1(val)))
#define PIPE4(val, f1, f2, f3, f4) f4(f3(f2(f1(val))))
#define PIPE5(val, f1, f2, f3, f4, f5) f5(f4(f3(f2(f1(val)))))

#define GET_PIPE_MACRO(_1,_2,_3,_4,_5,_6,NAME,...) NAME
#define PIPE(...) GET_PIPE_MACRO(__VA_ARGS__, PIPE5, PIPE4, PIPE3, PIPE2, PIPE1)(__VA_ARGS__)

/* PARTIAL application macro for currying */
#define PARTIAL(func, arg1) ({ \
    __typeof__(arg1) _captured_arg = (arg1); \
    struct { \
        __typeof__(arg1) captured; \
        __typeof__(func) original_func; \
    } _partial_data = { _captured_arg, func }; \
    _partial_data; \
})

/* Compiler compatibility check and type-specific macros */
#if defined(__GNUC__) && !defined(__clang__)
/* GCC version with nested functions support */
#define MAP_INT(arr, expr) ({ \
    int _map_func(int x) { return (expr); } \
    elegant_map_int((arr), _map_func); \
})
#define MAP_FLOAT(arr, expr) ({ \
    float _map_func(float x) { return (expr); } \
    elegant_map_float((arr), _map_func); \
})
#define MAP_DOUBLE(arr, expr) ({ \
    double _map_func(double x) { return (expr); } \
    elegant_map_double((arr), _map_func); \
})

#define FILTER_INT(arr, predicate) ({ \
    int _filter_func(int x) { return (predicate); } \
    elegant_filter_int((arr), _filter_func); \
})
#define FILTER_FLOAT(arr, predicate) ({ \
    int _filter_func(float x) { return (predicate); } \
    elegant_filter_float((arr), _filter_func); \
})
#define FILTER_DOUBLE(arr, predicate) ({ \
    int _filter_func(double x) { return (predicate); } \
    elegant_filter_double((arr), _filter_func); \
})

#define REDUCE_INT(arr, expr, initial) ({ \
    int _reduce_func(int acc, int x) { return (expr); } \
    elegant_reduce_int((arr), _reduce_func, (initial)); \
})
#define REDUCE_FLOAT(arr, expr, initial) ({ \
    float _reduce_func(float acc, float x) { return (expr); } \
    elegant_reduce_float((arr), _reduce_func, (initial)); \
})
#define REDUCE_DOUBLE(arr, expr, initial) ({ \
    double _reduce_func(double acc, double x) { return (expr); } \
    elegant_reduce_double((arr), _reduce_func, (initial)); \
})

#define FIND_INT(arr, predicate) ({ \
    int _find_func(int x) { return (predicate); } \
    elegant_find_int((arr), _find_func); \
})

#else
/* Unsupported compiler */
#error "ELEGANT LIBRARY ERROR: This library requires GCC with nested functions support. Current compiler (likely Clang) is not supported. On macOS: 'brew install gcc', then use 'CC=gcc-13 make' (replace 13 with your GCC version)."

#endif /* defined(__GNUC__) && !defined(__clang__) */

/* Additional macros for backwards compatibility */
#define REVERSE(arr) elegant_reverse(arr)
#define TAKE(n, arr) elegant_take(arr, n)
#define DROP(n, arr) elegant_drop(arr, n)

/* Additional functional operations */

/* REVERSE - reverse array order */
elegant_array_t* elegant_reverse(elegant_array_t* arr);

/* ZIP - combine two arrays element-wise */
elegant_array_t* elegant_zip(elegant_array_t* arr1, elegant_array_t* arr2, void* (*combiner)(void*, void*), size_t result_element_size);

#define ZIP(arr1, arr2, expr, type1, type2, result_type) ({ \
    result_type _zip_func(type1 a, type2 b) { return (expr); } \
    elegant_zip((arr1), (arr2), (void*(*)(void*,void*))_zip_func, sizeof(result_type)); \
})

/* TAKE - take first n elements */
elegant_array_t* elegant_take(elegant_array_t* arr, size_t n);

/* DROP - drop first n elements */
elegant_array_t* elegant_drop(elegant_array_t* arr, size_t n);

/* FIND - find first element matching predicate (returns pointer or NULL) */
#define FIND(arr, predicate, type) ({ \
    int _find_func(type x) { return (predicate); } \
    void* _result = elegant_find_generic((arr), (int(*)(void*))_find_func, sizeof(type)); \
    _result ? (type*)_result : NULL; \
})

void* elegant_find_generic(elegant_array_t* src, int (*predicate)(void*), size_t element_size);

/* CONCAT - concatenate multiple arrays into a new array */
elegant_array_t* elegant_concat_arrays(size_t count, ...);

#define ELEGANT_CONCAT(...) elegant_concat_arrays( \
    sizeof((elegant_array_t*[]){__VA_ARGS__}) / sizeof(elegant_array_t*), \
    __VA_ARGS__)

/* Helper functions for array operations */
elegant_array_t* elegant_map_int(elegant_array_t* src, int (*func)(int));
elegant_array_t* elegant_map_float(elegant_array_t* src, float (*func)(float));
elegant_array_t* elegant_map_double(elegant_array_t* src, double (*func)(double));

elegant_array_t* elegant_filter_int(elegant_array_t* src, int (*predicate)(int));
elegant_array_t* elegant_filter_float(elegant_array_t* src, int (*predicate)(float));
elegant_array_t* elegant_filter_double(elegant_array_t* src, int (*predicate)(double));

int elegant_reduce_int(elegant_array_t* src, int (*func)(int, int), int initial);
float elegant_reduce_float(elegant_array_t* src, float (*func)(float, float), float initial);
double elegant_reduce_double(elegant_array_t* src, double (*func)(double, double), double initial);

int* elegant_find_int(elegant_array_t* src, int (*predicate)(int));
float* elegant_find_float(elegant_array_t* src, int (*predicate)(float));
double* elegant_find_double(elegant_array_t* src, int (*predicate)(double));

#endif /* ELEGANT_COLLECTION_H */
