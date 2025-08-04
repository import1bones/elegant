#ifndef ELEGANT_COLLECTION_H
#define ELEGANT_COLLECTION_H

/*
 * Functional collection operations: MAP, FILTER, REDUCE
 * Simplified implementation for v0.0.1 with type-specific macros
 */

/* Helper function declarations */
elegant_array_t* elegant_map_int(elegant_array_t* src, int (*func)(int));
elegant_array_t* elegant_map_float(elegant_array_t* src, float (*func)(float));
elegant_array_t* elegant_map_double(elegant_array_t* src, double (*func)(double));

elegant_array_t* elegant_filter_int(elegant_array_t* src, int (*predicate)(int));
elegant_array_t* elegant_filter_float(elegant_array_t* src, int (*predicate)(float));
elegant_array_t* elegant_filter_double(elegant_array_t* src, int (*predicate)(double));

int elegant_reduce_int(elegant_array_t* src, int (*func)(int, int), int initial);
float elegant_reduce_float(elegant_array_t* src, float (*func)(float, float), float initial);
double elegant_reduce_double(elegant_array_t* src, double (*func)(double, double), double initial);

/* MAP macros with inline lambda support using statement expressions */
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

/* FILTER macros */
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

/* REDUCE macros */
#define REDUCE_INT(arr, expr, initial) ({ \
    int _reduce_func(int a, int b) { return (expr); } \
    elegant_reduce_int((arr), _reduce_func, (initial)); \
})

#define REDUCE_FLOAT(arr, expr, initial) ({ \
    float _reduce_func(float a, float b) { return (expr); } \
    elegant_reduce_float((arr), _reduce_func, (initial)); \
})

#define REDUCE_DOUBLE(arr, expr, initial) ({ \
    double _reduce_func(double a, double b) { return (expr); } \
    elegant_reduce_double((arr), _reduce_func, (initial)); \
})

/* Generic macros that detect type (simplified for common cases) */
#define MAP(x, arr, expr) ({ \
    size_t _elem_size = (arr)->element_size; \
    (_elem_size == sizeof(int)) ? MAP_INT(arr, expr) : \
    (_elem_size == sizeof(float)) ? MAP_FLOAT(arr, expr) : \
    (_elem_size == sizeof(double)) ? MAP_DOUBLE(arr, expr) : NULL; \
})

#define FILTER(x, arr, predicate) ({ \
    size_t _elem_size = (arr)->element_size; \
    (_elem_size == sizeof(int)) ? FILTER_INT(arr, predicate) : \
    (_elem_size == sizeof(float)) ? FILTER_FLOAT(arr, predicate) : \
    (_elem_size == sizeof(double)) ? FILTER_DOUBLE(arr, predicate) : NULL; \
})

#define REDUCE(a, b, arr, expr, initial) ({ \
    size_t _elem_size = (arr)->element_size; \
    (_elem_size == sizeof(int)) ? REDUCE_INT(arr, expr, initial) : \
    (_elem_size == sizeof(float)) ? REDUCE_FLOAT(arr, expr, initial) : \
    (_elem_size == sizeof(double)) ? REDUCE_DOUBLE(arr, expr, initial) : 0; \
})

/* Additional array operations */

/* REVERSE - reverse array order */
#define REVERSE(array) ({ \
    elegant_array_t* _src = (array); \
    size_t _len = elegant_array_get_length(_src); \
    size_t _elem_size = _src->element_size; \
    elegant_array_t* _result = elegant_array_create(_elem_size, _len); \
    char* _src_data = (char*)elegant_array_get_data(_src); \
    char* _dst_data = (char*)elegant_array_get_data(_result); \
    \
    for (size_t _i = 0; _i < _len; _i++) { \
        memcpy(_dst_data + _i * _elem_size, \
               _src_data + (_len - 1 - _i) * _elem_size, \
               _elem_size); \
    } \
    _result; \
})

/* TAKE - take first n elements */
#define TAKE(n, array) ({ \
    elegant_array_t* _src = (array); \
    size_t _len = elegant_array_get_length(_src); \
    size_t _take_len = ((n) < _len) ? (n) : _len; \
    size_t _elem_size = _src->element_size; \
    elegant_array_t* _result = elegant_array_create(_elem_size, _take_len); \
    \
    memcpy(elegant_array_get_data(_result), \
           elegant_array_get_data(_src), \
           _take_len * _elem_size); \
    _result; \
})

/* DROP - skip first n elements */
#define DROP(n, array) ({ \
    elegant_array_t* _src = (array); \
    size_t _len = elegant_array_get_length(_src); \
    size_t _drop_len = ((n) < _len) ? (n) : _len; \
    size_t _result_len = _len - _drop_len; \
    size_t _elem_size = _src->element_size; \
    elegant_array_t* _result = elegant_array_create(_elem_size, _result_len); \
    \
    if (_result_len > 0) { \
        memcpy(elegant_array_get_data(_result), \
               (char*)elegant_array_get_data(_src) + _drop_len * _elem_size, \
               _result_len * _elem_size); \
    } \
    _result; \
})

/* FIND - find first element matching predicate (returns pointer or NULL) */
#define FIND_INT(arr, predicate) ({ \
    int _find_func(int x) { return (predicate); } \
    elegant_find_int((arr), _find_func); \
})

#define FIND_FLOAT(arr, predicate) ({ \
    int _find_func(float x) { return (predicate); } \
    elegant_find_float((arr), _find_func); \
})

#define FIND_DOUBLE(arr, predicate) ({ \
    int _find_func(double x) { return (predicate); } \
    elegant_find_double((arr), _find_func); \
})

int* elegant_find_int(elegant_array_t* src, int (*predicate)(int));
float* elegant_find_float(elegant_array_t* src, int (*predicate)(float));
double* elegant_find_double(elegant_array_t* src, int (*predicate)(double));

#endif /* ELEGANT_COLLECTION_H */
