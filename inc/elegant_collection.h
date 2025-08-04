#ifndef ELEGANT_COLLECTION_H
#define ELEGANT_COLLECTION_H

/*
 * Functional collection operations: MAP, FILTER, REDUCE
 * Using statement expressions for inline lambda-like syntax
 */

/* MAP operation - transform each element */
#define MAP(var, array, expr) ({ \
    elegant_array_t* _src = (array); \
    size_t _len = elegant_array_get_length(_src); \
    size_t _elem_size = _src->element_size; \
    elegant_array_t* _result = elegant_array_create(_elem_size, _len); \
    \
    for (size_t _i = 0; _i < _len; _i++) { \
        __typeof__(expr) var = ((__typeof__(expr)*)elegant_array_get_data(_src))[_i]; \
        ((__typeof__(expr)*)elegant_array_get_data(_result))[_i] = (expr); \
    } \
    _result; \
})

/* FILTER operation - select elements matching predicate */
#define FILTER(var, array, predicate) ({ \
    elegant_array_t* _src = (array); \
    size_t _len = elegant_array_get_length(_src); \
    size_t _elem_size = _src->element_size; \
    elegant_array_t* _temp = elegant_array_create(_elem_size, _len); \
    size_t _count = 0; \
    \
    for (size_t _i = 0; _i < _len; _i++) { \
        __typeof__(*((__typeof__(var)*)elegant_array_get_data(_src))) var = \
            ((__typeof__(var)*)elegant_array_get_data(_src))[_i]; \
        if (predicate) { \
            ((__typeof__(var)*)elegant_array_get_data(_temp))[_count++] = var; \
        } \
    } \
    \
    /* Create result array with exact size */ \
    elegant_array_t* _result = elegant_array_create(_elem_size, _count); \
    memcpy(elegant_array_get_data(_result), elegant_array_get_data(_temp), _count * _elem_size); \
    elegant_array_destroy(_temp); \
    _result; \
})

/* REDUCE operation - fold array into single value */
#define REDUCE(acc_var, elem_var, array, expr, initial) ({ \
    elegant_array_t* _src = (array); \
    size_t _len = elegant_array_get_length(_src); \
    __typeof__(initial) acc_var = (initial); \
    \
    for (size_t _i = 0; _i < _len; _i++) { \
        __typeof__(*((__typeof__(elem_var)*)elegant_array_get_data(_src))) elem_var = \
            ((__typeof__(elem_var)*)elegant_array_get_data(_src))[_i]; \
        acc_var = (expr); \
    } \
    acc_var; \
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

/* FIND - find first element matching predicate */
#define FIND(var, array, predicate) ({ \
    elegant_array_t* _src = (array); \
    size_t _len = elegant_array_get_length(_src); \
    __typeof__(*((__typeof__(var)*)elegant_array_get_data(_src)))*  _result = NULL; \
    \
    for (size_t _i = 0; _i < _len; _i++) { \
        __typeof__(*((__typeof__(var)*)elegant_array_get_data(_src))) var = \
            ((__typeof__(var)*)elegant_array_get_data(_src))[_i]; \
        if (predicate) { \
            static __typeof__(var) _found; \
            _found = var; \
            _result = &_found; \
            break; \
        } \
    } \
    _result; \
})

#endif /* ELEGANT_COLLECTION_H */
