# Elegant Security Library

A functional programming approach to common security operations in C.

## Overview

The Elegant Security Library demonstrates how functional programming patterns can be applied to security-critical operations. It provides a collection of composable, type-safe security utilities that leverage Elegant's functional programming features.

## Key Features

### 1. **Functional Password Validation**
- Composable validation rules
- Password strength scoring using functional reduce patterns
- Common password detection
- Character class validation (uppercase, lowercase, digits, special chars)

### 2. **Input Sanitization**
- SQL injection prevention
- HTML/XSS prevention  
- Path traversal protection
- Functional filter/map operations on strings
- Email validation and sanitization

### 3. **Cryptographic Utilities**
- Multiple hash functions (DJB2, SDBM, FNV-1a)
- Functional hash composition
- Password hashing with salt
- Demonstrates MAP/REDUCE patterns for cryptographic operations

### 4. **Secure String Operations**
- Bounds-checked string operations
- Constant-time string comparison (timing attack prevention)
- Secure memory zeroing (prevents compiler optimization)

## Design Philosophy

The security library showcases Elegant's power through:

1. **Functional Composition**: Security rules and transformations can be composed together
2. **Type Safety**: Strong typing helps prevent common security mistakes
3. **Immutability**: Operations return new values rather than modifying inputs
4. **Declarative Style**: Security policies are expressed clearly and concisely
5. **Testability**: Pure functions are easy to test and verify

## API Reference

### Password Validation

```c
// Password strength scoring (functional reduce pattern)
int elegant_password_score(const char* password);
elegant_password_strength_t elegant_password_strength(const char* password);

// Composable validation rules
elegant_security_result_t elegant_validate_password(
    const char* password,
    elegant_password_rule_t* rules,
    size_t rule_count
);

// Built-in rules
bool elegant_rule_min_length(const char* password, char* message, size_t message_size);
bool elegant_rule_has_uppercase(const char* password, char* message, size_t message_size);
bool elegant_rule_has_lowercase(const char* password, char* message, size_t message_size);
bool elegant_rule_has_digit(const char* password, char* message, size_t message_size);
bool elegant_rule_has_special(const char* password, char* message, size_t message_size);
bool elegant_rule_no_common_passwords(const char* password, char* message, size_t message_size);
```

### Input Sanitization

```c
// Functional filter pattern - keep only safe characters
char* elegant_filter_string(const char* input, elegant_security_predicate_t predicate);

// Functional map pattern - transform each character
char* elegant_map_string(const char* input, char (*transform)(char));

// SQL injection prevention
char* elegant_escape_sql(const char* input);

// HTML/XSS prevention
char* elegant_escape_html(const char* input);

// Path traversal prevention
char* elegant_sanitize_filename(const char* filename);

// Email validation and sanitization
bool elegant_validate_email(const char* email);
char* elegant_sanitize_email(const char* email);
```

### Cryptographic Hash Functions

```c
// Individual hash functions (functional reduce pattern)
uint64_t elegant_hash_djb2(const uint8_t* data, size_t length);
uint64_t elegant_hash_sdbm(const uint8_t* data, size_t length);
uint64_t elegant_hash_fnv1a(const uint8_t* data, size_t length);

// Functional composition - combine multiple hash functions
uint64_t elegant_hash_combine(
    const uint8_t* data,
    size_t length,
    elegant_hash_func_t* hash_funcs,
    size_t func_count
);

// Password hashing with salt
uint64_t elegant_hash_password(const char* password, const char* salt);
```

### Secure String Operations

```c
// Bounds-checked string operations
size_t elegant_secure_strcpy(char* dest, size_t dest_size, const char* src);
size_t elegant_secure_strcat(char* dest, size_t dest_size, const char* src);

// Constant-time comparison (timing attack prevention)
bool elegant_secure_strcmp(const char* a, const char* b);

// Secure memory clearing (prevents compiler optimization)
void elegant_secure_memzero(void* ptr, size_t size);
```

### Character Predicates (for Functional Filtering)

```c
// Character class checks
bool elegant_is_uppercase(char c);
bool elegant_is_lowercase(char c);
bool elegant_is_digit(char c);
bool elegant_is_special_char(char c);

// Safety checks for different contexts
bool elegant_is_safe_alphanumeric(char c);
bool elegant_is_safe_filename_char(char c);
bool elegant_is_safe_email_char(char c);
bool elegant_is_sql_metachar(char c);
bool elegant_is_html_metachar(char c);
```

## Usage Examples

### Password Validation

```c
#include <elegant_security.h>

// Define validation rules
elegant_password_rule_t rules[] = {
    elegant_rule_min_length,
    elegant_rule_has_uppercase,
    elegant_rule_has_lowercase,
    elegant_rule_has_digit,
    elegant_rule_has_special
};

// Validate password against all rules
elegant_security_result_t result = elegant_validate_password(
    "MyP@ssw0rd",
    rules,
    sizeof(rules) / sizeof(rules[0])
);

if (result.valid) {
    printf("Password is valid! Score: %d\n", result.score);
} else {
    printf("Password validation failed:\n");
    for (size_t i = 0; i < result.failed_rules_count; i++) {
        printf("  - %s\n", result.failed_rules[i]);
    }
}

elegant_security_result_free(&result);
```

### Input Sanitization

```c
// SQL injection prevention
char* user_input = "admin' OR '1'='1";
char* safe_sql = elegant_escape_sql(user_input);
printf("Safe SQL: %s\n", safe_sql);
free(safe_sql);

// HTML/XSS prevention
char* html_input = "<script>alert('XSS')</script>";
char* safe_html = elegant_escape_html(html_input);
printf("Safe HTML: %s\n", safe_html);
free(safe_html);

// Functional filtering - extract only alphanumeric
char* mixed = "Hello123!@#World456";
char* clean = elegant_filter_string(mixed, elegant_is_safe_alphanumeric);
printf("Clean: %s\n", clean); // Output: "Hello123World456"
free(clean);
```

### Hash Functions and Composition

```c
// Single hash function
const char* data = "Hello, Security!";
uint64_t hash = elegant_hash_djb2((const uint8_t*)data, strlen(data));
printf("Hash: 0x%016lx\n", hash);

// Compose multiple hash functions (functional composition)
elegant_hash_func_t funcs[] = {
    elegant_hash_djb2,
    elegant_hash_sdbm,
    elegant_hash_fnv1a
};

uint64_t combined = elegant_hash_combine(
    (const uint8_t*)data,
    strlen(data),
    funcs,
    3
);
printf("Combined hash: 0x%016lx\n", combined);

// Password hashing with salt
uint64_t pwd_hash = elegant_hash_password("MyPassword", "random_salt");
printf("Password hash: 0x%016lx\n", pwd_hash);
```

### Secure String Operations

```c
// Bounds-checked copy
char buffer[20];
elegant_secure_strcpy(buffer, sizeof(buffer), "This is a very long string");
// buffer now contains "This is a very long" (truncated safely)

// Constant-time comparison (prevents timing attacks)
if (elegant_secure_strcmp(user_password, stored_hash)) {
    printf("Password correct!\n");
}

// Secure memory zeroing (prevents compiler optimization)
char sensitive_data[256] = "Secret information";
// ... use the data ...
elegant_secure_memzero(sensitive_data, sizeof(sensitive_data));
// Guaranteed to be zeroed, even with optimizations enabled
```

### Functional Pattern Demonstration

```c
// Filter pattern: extract specific character types
const char* input = "Hello123World!@#456";

// Extract only digits
char* digits = elegant_filter_string(input, elegant_is_digit);
printf("Digits: %s\n", digits); // Output: "123456"
free(digits);

// Extract only uppercase
char* uppercase = elegant_filter_string(input, elegant_is_uppercase);
printf("Uppercase: %s\n", uppercase); // Output: "HW"
free(uppercase);

// Extract only lowercase
char* lowercase = elegant_filter_string(input, elegant_is_lowercase);
printf("Lowercase: %s\n", lowercase); // Output: "elloorld"
free(lowercase);

// Extract only special characters
char* special = elegant_filter_string(input, elegant_is_special_char);
printf("Special: %s\n", special); // Output: "!@#"
free(special);
```

## Functional Programming Patterns

### 1. Filter Pattern

The library extensively uses the functional FILTER pattern to extract or remove characters based on predicates:

```c
char* elegant_filter_string(const char* input, elegant_security_predicate_t predicate) {
    // Keeps only characters where predicate(char) returns true
    // This is equivalent to Haskell's: filter predicate input
}
```

### 2. Map Pattern

String transformation using character mapping:

```c
char* elegant_map_string(const char* input, char (*transform)(char)) {
    // Transforms each character using the transform function
    // Equivalent to Haskell's: map transform input
}
```

### 3. Reduce Pattern

Hash functions demonstrate the REDUCE pattern:

```c
uint64_t elegant_hash_djb2(const uint8_t* data, size_t length) {
    uint64_t hash = 5381;
    for (size_t i = 0; i < length; i++) {
        hash = ((hash << 5) + hash) + data[i];  // Accumulate hash
    }
    return hash;
    // Equivalent to Haskell's: foldl (\h c -> (h*33 + c)) 5381 data
}
```

### 4. Function Composition

Multiple hash functions can be composed:

```c
uint64_t elegant_hash_combine(
    const uint8_t* data,
    size_t length,
    elegant_hash_func_t* hash_funcs,
    size_t func_count
) {
    // Combines results from multiple hash functions
    // Demonstrates functional composition pattern
}
```

### 5. Rule Composition

Password validation rules can be composed using boolean logic:

```c
// Rules are composable predicates
elegant_password_rule_t rules[] = {
    elegant_rule_min_length,      // Rule 1
    elegant_rule_has_uppercase,   // Rule 2
    elegant_rule_has_digit,       // Rule 3
};

// All rules are applied (AND composition)
elegant_validate_password(password, rules, 3);
```

## Performance Considerations

### Memory Management

All functions that return allocated strings (`char*`) must be freed by the caller:

```c
char* sanitized = elegant_sanitize_filename("../path/file.txt");
// Use sanitized...
free(sanitized);  // Don't forget to free!
```

### Constant-Time Operations

The `elegant_secure_strcmp()` function performs constant-time comparison to prevent timing attacks:

```c
bool elegant_secure_strcmp(const char* a, const char* b) {
    // Compares all bytes regardless of mismatch position
    // Prevents timing attack information leakage
}
```

### Secure Memory Clearing

The `elegant_secure_memzero()` function uses `volatile` to prevent compiler optimization:

```c
void elegant_secure_memzero(void* ptr, size_t size) {
    volatile uint8_t* p = (volatile uint8_t*)ptr;
    for (size_t i = 0; i < size; i++) {
        p[i] = 0;  // Guaranteed not to be optimized away
    }
}
```

## Security Considerations

### ⚠️ Important Notes

1. **Cryptographic Hashes**: The hash functions provided (DJB2, SDBM, FNV-1a) are **NOT cryptographically secure**. They are demonstration of functional patterns. For real cryptographic applications, use established libraries like OpenSSL or libsodium.

2. **Password Storage**: The `elegant_hash_password()` function is for **demonstration only**. Real applications should use proper password hashing algorithms like bcrypt, scrypt, or Argon2.

3. **Timing Attacks**: Only `elegant_secure_strcmp()` provides constant-time comparison. Other string operations may be vulnerable to timing attacks.

4. **Input Validation**: Always validate input before sanitization. Sanitization is defense-in-depth, not the primary security control.

5. **Memory Safety**: All heap-allocated results must be properly freed to prevent memory leaks.

## Testing

The security library can be tested with the included example:

```bash
# Build with examples enabled
./configure --enable-examples
make

# Run the security example
./examples/security_example
```

The example demonstrates:
- Password validation with various strengths
- SQL and HTML injection prevention
- Email validation
- Hash function composition
- Secure string operations
- Functional programming patterns

## Integration

### Including in Your Project

```c
#include <elegant.h>
#include <elegant_security.h>

int main(void) {
    // Use security functions here
    return 0;
}
```

### Compilation

```bash
# Using pkg-config (recommended)
gcc myprogram.c $(pkg-config --cflags --libs elegant) -o myprogram

# Manual compilation
gcc myprogram.c -I/usr/local/include/elegant -lelegant -lm -o myprogram
```

## Extending the Library

### Adding Custom Password Rules

```c
bool custom_rule(const char* password, char* message, size_t message_size) {
    // Your validation logic here
    if (/* condition fails */) {
        snprintf(message, message_size, "Custom rule failed");
        return false;
    }
    return true;
}

// Use with other rules
elegant_password_rule_t rules[] = {
    elegant_rule_min_length,
    custom_rule  // Your custom rule
};
```

### Adding Custom Character Predicates

```c
bool my_predicate(char c) {
    // Your character validation logic
    return c >= 'a' && c <= 'z';
}

// Use with filter
char* result = elegant_filter_string(input, my_predicate);
```

### Adding Custom Hash Functions

```c
uint64_t my_hash(const uint8_t* data, size_t length) {
    // Your hash implementation
    uint64_t hash = 0;
    for (size_t i = 0; i < length; i++) {
        hash = hash * 31 + data[i];
    }
    return hash;
}

// Compose with existing hashes
elegant_hash_func_t funcs[] = {
    elegant_hash_djb2,
    elegant_hash_fnv1a,
    my_hash  // Your custom hash
};
```

## Best Practices

1. **Always validate input** before using it in security-sensitive operations
2. **Free allocated memory** from sanitization functions
3. **Use constant-time comparison** for secrets and passwords
4. **Clear sensitive data** from memory after use
5. **Combine multiple validation rules** for defense-in-depth
6. **Test edge cases** including empty strings, NULL pointers, and malicious input
7. **Use proper cryptographic libraries** for production security needs

## License

The Elegant Security Library is part of the Elegant project and follows the same license terms.

## Contributing

Contributions are welcome! Please ensure that:
- Code follows the functional programming patterns established in the library
- Security functions are well-tested
- Documentation is updated for new features
- Examples demonstrate real-world usage

## References

- [Elegant Core Documentation](./overview.md)
- [Functional Programming Patterns](./api_reference.md)
- [Example Programs](../examples/)
