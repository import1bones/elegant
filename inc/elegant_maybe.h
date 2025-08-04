#ifndef ELEGANT_MAYBE_H
#define ELEGANT_MAYBE_H

/*
 * Maybe/Option types for safe error handling
 */

/* Maybe type definition */
#define ELEGANT_MAYBE(type) \
    struct { \
        bool has_value; \
        type value; \
    }

/* Maybe constructor macros */
#define ELEGANT_SOME(type, val) \
    ((ELEGANT_MAYBE(type)){.has_value = true, .value = (val)})

#define ELEGANT_NONE(type) \
    ((ELEGANT_MAYBE(type)){.has_value = false, .value = {0}})

/* Maybe utility macros */
#define ELEGANT_IS_SOME(maybe) ((maybe).has_value)
#define ELEGANT_IS_NONE(maybe) (!(maybe).has_value)
#define ELEGANT_UNWRAP(maybe) ((maybe).value)

/* Safe unwrap with default value */
#define ELEGANT_UNWRAP_OR(maybe, default_val) \
    (ELEGANT_IS_SOME(maybe) ? ELEGANT_UNWRAP(maybe) : (default_val))

/* Maybe bind operation (flatMap) */
#define ELEGANT_MAYBE_BIND(maybe, func) \
    (ELEGANT_IS_SOME(maybe) ? (func)(ELEGANT_UNWRAP(maybe)) : ELEGANT_NONE(__typeof__(ELEGANT_UNWRAP(maybe))))

/* Maybe map operation */
#define ELEGANT_MAYBE_MAP(var, maybe, expr) \
    (ELEGANT_IS_SOME(maybe) ? ({ \
        __typeof__(ELEGANT_UNWRAP(maybe)) var = ELEGANT_UNWRAP(maybe); \
        ELEGANT_SOME(__typeof__(expr), (expr)); \
    }) : ELEGANT_NONE(__typeof__(expr)))

/* Common Maybe operations */

/* Safe division */
static inline ELEGANT_MAYBE(int) elegant_maybe_divide_int(int a, int b) {
    if (b == 0) {
        return ELEGANT_NONE(int);
    }
    return ELEGANT_SOME(int, a / b);
}

static inline ELEGANT_MAYBE(double) elegant_maybe_divide_double(double a, double b) {
    if (b == 0.0) {
        return ELEGANT_NONE(double);
    }
    return ELEGANT_SOME(double, a / b);
}

/* Maybe array access */
#define ELEGANT_MAYBE_GET(array, index, type) ({ \
    elegant_array_t* _arr = (array); \
    size_t _idx = (index); \
    size_t _len = elegant_array_get_length(_arr); \
    (_idx < _len) ? \
        ELEGANT_SOME(type, ELEGANT_GET(_arr, _idx, type)) : \
        ELEGANT_NONE(type); \
})

#endif /* ELEGANT_MAYBE_H */
