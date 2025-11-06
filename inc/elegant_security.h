#ifndef ELEGANT_SECURITY_H
#define ELEGANT_SECURITY_H

/*
 * Elegant Security Library
 * Demonstrates functional programming patterns for security operations
 * 
 * Features:
 * - Password strength validation
 * - Input sanitization using functional transformations
 * - Secure string operations with bounds checking
 * - Simple hash functions using MAP/REDUCE patterns
 * - Security rule composition
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Security validation result */
typedef struct {
    bool valid;
    int score;
    const char* message;
    const char** failed_rules;
    size_t failed_rules_count;
} elegant_security_result_t;

/* Password strength levels */
typedef enum {
    ELEGANT_PASSWORD_WEAK = 0,
    ELEGANT_PASSWORD_FAIR = 1,
    ELEGANT_PASSWORD_GOOD = 2,
    ELEGANT_PASSWORD_STRONG = 3,
    ELEGANT_PASSWORD_VERY_STRONG = 4
} elegant_password_strength_t;

/* Password validation rules (function pointer type) */
typedef bool (*elegant_password_rule_t)(const char* password, char* message, size_t message_size);

/* Input sanitization modes */
typedef enum {
    ELEGANT_SANITIZE_ALPHANUMERIC = 0x01,
    ELEGANT_SANITIZE_NO_SPECIAL_CHARS = 0x02,
    ELEGANT_SANITIZE_NO_SQL = 0x04,
    ELEGANT_SANITIZE_NO_HTML = 0x08,
    ELEGANT_SANITIZE_NO_SCRIPT = 0x10,
    ELEGANT_SANITIZE_EMAIL = 0x20,
    ELEGANT_SANITIZE_FILENAME = 0x40
} elegant_sanitize_mode_t;

/* Hash function type for functional composition */
typedef uint64_t (*elegant_hash_func_t)(const uint8_t* data, size_t length);

/* Security filter predicate */
typedef bool (*elegant_security_predicate_t)(char c);

/* =============================================================================
 * Core Security Functions
 * ============================================================================= */

/* Password validation and strength checking */
elegant_security_result_t elegant_validate_password(
    const char* password,
    elegant_password_rule_t* rules,
    size_t rule_count
);

elegant_password_strength_t elegant_password_strength(const char* password);

int elegant_password_score(const char* password);

/* Built-in password validation rules */
bool elegant_rule_min_length(const char* password, char* message, size_t message_size);
bool elegant_rule_has_uppercase(const char* password, char* message, size_t message_size);
bool elegant_rule_has_lowercase(const char* password, char* message, size_t message_size);
bool elegant_rule_has_digit(const char* password, char* message, size_t message_size);
bool elegant_rule_has_special(const char* password, char* message, size_t message_size);
bool elegant_rule_no_common_passwords(const char* password, char* message, size_t message_size);

/* =============================================================================
 * Input Sanitization (Functional Style)
 * ============================================================================= */

/* Sanitize input string by filtering/transforming characters */
char* elegant_sanitize_string(const char* input, elegant_sanitize_mode_t mode);

/* Filter characters using a predicate function (functional filter) */
char* elegant_filter_string(const char* input, elegant_security_predicate_t predicate);

/* Transform string using character mapping (functional map) */
char* elegant_map_string(const char* input, char (*transform)(char));

/* SQL injection prevention */
char* elegant_escape_sql(const char* input);

/* HTML/XSS prevention */
char* elegant_escape_html(const char* input);

/* Path traversal prevention */
char* elegant_sanitize_filename(const char* filename);

/* Email validation and sanitization */
bool elegant_validate_email(const char* email);
char* elegant_sanitize_email(const char* email);

/* =============================================================================
 * Cryptographic Utilities (Using Functional Patterns)
 * ============================================================================= */

/* Simple hash functions using MAP/REDUCE patterns */
uint64_t elegant_hash_djb2(const uint8_t* data, size_t length);
uint64_t elegant_hash_sdbm(const uint8_t* data, size_t length);
uint64_t elegant_hash_fnv1a(const uint8_t* data, size_t length);

/* Compose hash functions (functional composition) */
uint64_t elegant_hash_combine(
    const uint8_t* data,
    size_t length,
    elegant_hash_func_t* hash_funcs,
    size_t func_count
);

/* Simple password hashing (demonstration - not cryptographically secure) */
uint64_t elegant_hash_password(const char* password, const char* salt);

/* =============================================================================
 * Secure String Operations
 * ============================================================================= */

/* Bounds-checked string copy */
size_t elegant_secure_strcpy(char* dest, size_t dest_size, const char* src);

/* Bounds-checked string concatenation */
size_t elegant_secure_strcat(char* dest, size_t dest_size, const char* src);

/* Constant-time string comparison (timing attack prevention) */
bool elegant_secure_strcmp(const char* a, const char* b);

/* Secure memory clearing (prevents optimization removal) */
void elegant_secure_memzero(void* ptr, size_t size);

/* =============================================================================
 * Security Rule Composition (Functional Combinators)
 * ============================================================================= */

/* Combine multiple rules with AND logic */
elegant_password_rule_t elegant_rule_and(
    elegant_password_rule_t rule1,
    elegant_password_rule_t rule2
);

/* Combine multiple rules with OR logic */
elegant_password_rule_t elegant_rule_or(
    elegant_password_rule_t rule1,
    elegant_password_rule_t rule2
);

/* Negate a rule (NOT logic) */
elegant_password_rule_t elegant_rule_not(elegant_password_rule_t rule);

/* =============================================================================
 * Security Utilities
 * ============================================================================= */

/* Check if character is safe for various contexts */
bool elegant_is_safe_alphanumeric(char c);
bool elegant_is_safe_filename_char(char c);
bool elegant_is_safe_email_char(char c);
bool elegant_is_sql_metachar(char c);
bool elegant_is_html_metachar(char c);

/* Character class predicates (for functional filtering) */
bool elegant_is_uppercase(char c);
bool elegant_is_lowercase(char c);
bool elegant_is_digit(char c);
bool elegant_is_special_char(char c);

/* Free security result resources */
void elegant_security_result_free(elegant_security_result_t* result);

#ifdef __cplusplus
}
#endif

#endif /* ELEGANT_SECURITY_H */
