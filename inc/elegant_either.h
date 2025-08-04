#ifndef ELEGANT_EITHER_H
#define ELEGANT_EITHER_H

/*
 * Either types for error handling with error values
 */

/* Either type definition */
#define ELEGANT_EITHER(left_type, right_type) \
    struct { \
        bool is_right; \
        union { \
            left_type left; \
            right_type right; \
        } value; \
    }

/* Either constructor macros */
#define ELEGANT_LEFT(left_type, val) \
    ((ELEGANT_EITHER(left_type, __typeof__(val))){.is_right = false, .value.left = (val)})

#define ELEGANT_RIGHT(right_type, val) \
    ((ELEGANT_EITHER(__typeof__(val), right_type)){.is_right = true, .value.right = (val)})

/* Either utility macros */
#define ELEGANT_IS_LEFT(either) (!(either).is_right)
#define ELEGANT_IS_RIGHT(either) ((either).is_right)
#define ELEGANT_UNWRAP_LEFT(either) ((either).value.left)
#define ELEGANT_UNWRAP_RIGHT(either) ((either).value.right)

/* Either map operations */
#define ELEGANT_EITHER_MAP_LEFT(var, either, expr) \
    (ELEGANT_IS_LEFT(either) ? ({ \
        __typeof__(ELEGANT_UNWRAP_LEFT(either)) var = ELEGANT_UNWRAP_LEFT(either); \
        ELEGANT_LEFT(__typeof__(expr), (expr)); \
    }) : (either))

#define ELEGANT_EITHER_MAP_RIGHT(var, either, expr) \
    (ELEGANT_IS_RIGHT(either) ? ({ \
        __typeof__(ELEGANT_UNWRAP_RIGHT(either)) var = ELEGANT_UNWRAP_RIGHT(either); \
        ELEGANT_RIGHT(__typeof__(expr), (expr)); \
    }) : (either))

/* Either bind (flatMap) operations */
#define ELEGANT_EITHER_BIND_LEFT(either, func) \
    (ELEGANT_IS_LEFT(either) ? (func)(ELEGANT_UNWRAP_LEFT(either)) : (either))

#define ELEGANT_EITHER_BIND_RIGHT(either, func) \
    (ELEGANT_IS_RIGHT(either) ? (func)(ELEGANT_UNWRAP_RIGHT(either)) : (either))

/* Pattern matching helper */
#define ELEGANT_EITHER_MATCH(either, left_var, left_expr, right_var, right_expr) \
    (ELEGANT_IS_LEFT(either) ? ({ \
        __typeof__(ELEGANT_UNWRAP_LEFT(either)) left_var = ELEGANT_UNWRAP_LEFT(either); \
        (left_expr); \
    }) : ({ \
        __typeof__(ELEGANT_UNWRAP_RIGHT(either)) right_var = ELEGANT_UNWRAP_RIGHT(either); \
        (right_expr); \
    }))

/* Common Either operations */

/* Safe string to integer parsing */
static inline ELEGANT_EITHER(const char*, int) elegant_parse_int(const char* str) {
    if (!str) {
        return ELEGANT_LEFT(const char*, "Null string");
    }
    
    char* endptr;
    long val = strtol(str, &endptr, 10);
    
    if (endptr == str) {
        return ELEGANT_LEFT(const char*, "No digits found");
    }
    
    if (*endptr != '\0') {
        return ELEGANT_LEFT(const char*, "Invalid characters after number");
    }
    
    if (val > INT_MAX || val < INT_MIN) {
        return ELEGANT_LEFT(const char*, "Number out of range");
    }
    
    return ELEGANT_RIGHT(int, (int)val);
}

#endif /* ELEGANT_EITHER_H */
