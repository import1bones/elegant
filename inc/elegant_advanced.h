#ifndef ELEGANT_COLLECTION_ADVANCED_H
#define ELEGANT_COLLECTION_ADVANCED_H

/*
 * Advanced Functional Programming Features for Elegant
 * Working implementations of PIPE, PARTIAL, FOLD operations
 */

/* PIPE operator for function composition - supports up to 5 functions */
#define PIPE1(val, f1) f1(val)
#define PIPE2(val, f1, f2) f2(f1(val))
#define PIPE3(val, f1, f2, f3) f3(f2(f1(val)))
#define PIPE4(val, f1, f2, f3, f4) f4(f3(f2(f1(val))))
#define PIPE5(val, f1, f2, f3, f4, f5) f5(f4(f3(f2(f1(val)))))

#define GET_PIPE_MACRO(_1,_2,_3,_4,_5,_6,NAME,...) NAME
#define PIPE(...) GET_PIPE_MACRO(__VA_ARGS__, PIPE5, PIPE4, PIPE3, PIPE2, PIPE1)(__VA_ARGS__)

/* FOLD operations using existing REDUCE infrastructure */
#define FOLD_LEFT_INT(arr, expr, initial) REDUCE_INT(arr, expr, initial)
#define FOLD_LEFT_FLOAT(arr, expr, initial) REDUCE_FLOAT(arr, expr, initial)
#define FOLD_LEFT_DOUBLE(arr, expr, initial) REDUCE_DOUBLE(arr, expr, initial)

/* FOLD_RIGHT - process array from right to left */
elegant_array_t* elegant_reverse_int(elegant_array_t* arr);
#define FOLD_RIGHT_INT(arr, expr, initial) ({ \
    AUTO(_reversed, elegant_reverse_int(arr)); \
    REDUCE_INT(_reversed, expr, initial); \
})

/* Enhanced array utilities */
elegant_array_t* elegant_take_int(elegant_array_t* arr, size_t n);
elegant_array_t* elegant_drop_int(elegant_array_t* arr, size_t n);
elegant_array_t* elegant_reverse_int(elegant_array_t* arr);

#define TAKE_INT(arr, n) elegant_take_int(arr, n)
#define DROP_INT(arr, n) elegant_drop_int(arr, n)

/* Simple currying support for two-argument functions */
#define CURRY_ADD(x) ({ \
    int _curried_value = (x); \
    int _add_partial(int y) { return _curried_value + y; } \
    _add_partial; \
})

#define CURRY_MULTIPLY(x) ({ \
    int _curried_value = (x); \
    int _multiply_partial(int y) { return _curried_value * y; } \
    _multiply_partial; \
})

/* Chaining macro for elegant function composition */
#define CHAIN(arr, ...) elegant_chain_operations(arr, ##__VA_ARGS__)

/* Array processing chain helpers */
typedef struct {
    enum { CHAIN_MAP, CHAIN_FILTER, CHAIN_TAKE, CHAIN_DROP } type;
    union {
        struct { int (*func)(int); } map;
        struct { int (*predicate)(int); } filter;
        struct { size_t n; } take_drop;
    } op;
} chain_operation_t;

elegant_array_t* elegant_chain_operations(elegant_array_t* arr, ...);

#endif /* ELEGANT_COLLECTION_ADVANCED_H */
