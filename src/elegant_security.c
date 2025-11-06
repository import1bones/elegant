/*
 * Elegant Security Library Implementation
 * Demonstrates functional programming patterns for security operations
 */

#define _POSIX_C_SOURCE 200809L
#include "../inc/elegant.h"
#include "../inc/elegant_security.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>

/* =============================================================================
 * Character Class Predicates (for Functional Filtering)
 * ============================================================================= */

bool elegant_is_uppercase(char c) {
    return c >= 'A' && c <= 'Z';
}

bool elegant_is_lowercase(char c) {
    return c >= 'a' && c <= 'z';
}

bool elegant_is_digit(char c) {
    return c >= '0' && c <= '9';
}

bool elegant_is_special_char(char c) {
    const char* special = "!@#$%^&*()_+-=[]{}|;:,.<>?";
    return strchr(special, c) != NULL;
}

bool elegant_is_safe_alphanumeric(char c) {
    return elegant_is_uppercase(c) || elegant_is_lowercase(c) || elegant_is_digit(c);
}

bool elegant_is_safe_filename_char(char c) {
    return elegant_is_safe_alphanumeric(c) || c == '_' || c == '-' || c == '.';
}

bool elegant_is_safe_email_char(char c) {
    return elegant_is_safe_alphanumeric(c) || c == '@' || c == '.' || c == '_' || c == '-' || c == '+';
}

bool elegant_is_sql_metachar(char c) {
    return c == '\'' || c == '"' || c == ';' || c == '-' || c == '\\';
}

bool elegant_is_html_metachar(char c) {
    return c == '<' || c == '>' || c == '&' || c == '"' || c == '\'';
}

/* =============================================================================
 * Password Validation Rules
 * ============================================================================= */

bool elegant_rule_min_length(const char* password, char* message, size_t message_size) {
    size_t len = strlen(password);
    if (len < 8) {
        snprintf(message, message_size, "Password must be at least 8 characters (current: %zu)", len);
        return false;
    }
    return true;
}

bool elegant_rule_has_uppercase(const char* password, char* message, size_t message_size) {
    for (const char* p = password; *p; p++) {
        if (elegant_is_uppercase(*p)) {
            return true;
        }
    }
    snprintf(message, message_size, "Password must contain at least one uppercase letter");
    return false;
}

bool elegant_rule_has_lowercase(const char* password, char* message, size_t message_size) {
    for (const char* p = password; *p; p++) {
        if (elegant_is_lowercase(*p)) {
            return true;
        }
    }
    snprintf(message, message_size, "Password must contain at least one lowercase letter");
    return false;
}

bool elegant_rule_has_digit(const char* password, char* message, size_t message_size) {
    for (const char* p = password; *p; p++) {
        if (elegant_is_digit(*p)) {
            return true;
        }
    }
    snprintf(message, message_size, "Password must contain at least one digit");
    return false;
}

bool elegant_rule_has_special(const char* password, char* message, size_t message_size) {
    for (const char* p = password; *p; p++) {
        if (elegant_is_special_char(*p)) {
            return true;
        }
    }
    snprintf(message, message_size, "Password must contain at least one special character");
    return false;
}

bool elegant_rule_no_common_passwords(const char* password, char* message, size_t message_size) {
    const char* common[] = {
        "password", "123456", "12345678", "qwerty", "abc123",
        "monkey", "letmein", "trustno1", "dragon", "baseball",
        "iloveyou", "master", "sunshine", "ashley", "bailey"
    };
    
    for (size_t i = 0; i < sizeof(common) / sizeof(common[0]); i++) {
        if (strcasecmp(password, common[i]) == 0) {
            snprintf(message, message_size, "Password is too common");
            return false;
        }
    }
    return true;
}

/* =============================================================================
 * Password Strength Calculation (Functional Reduce Pattern)
 * ============================================================================= */

int elegant_password_score(const char* password) {
    if (!password) return 0;
    
    int score = 0;
    size_t len = strlen(password);
    
    // Length contribution (functional map and reduce)
    if (len >= 8) score += 10;
    if (len >= 12) score += 10;
    if (len >= 16) score += 10;
    
    // Character class diversity (functional filter and count)
    bool has_upper = false, has_lower = false, has_digit = false, has_special = false;
    
    for (const char* p = password; *p; p++) {
        if (elegant_is_uppercase(*p)) has_upper = true;
        if (elegant_is_lowercase(*p)) has_lower = true;
        if (elegant_is_digit(*p)) has_digit = true;
        if (elegant_is_special_char(*p)) has_special = true;
    }
    
    if (has_upper) score += 15;
    if (has_lower) score += 15;
    if (has_digit) score += 15;
    if (has_special) score += 15;
    
    // Bonus for character diversity
    int char_types = has_upper + has_lower + has_digit + has_special;
    if (char_types >= 3) score += 10;
    if (char_types == 4) score += 10;
    
    return score;
}

elegant_password_strength_t elegant_password_strength(const char* password) {
    int score = elegant_password_score(password);
    
    if (score >= 80) return ELEGANT_PASSWORD_VERY_STRONG;
    if (score >= 60) return ELEGANT_PASSWORD_STRONG;
    if (score >= 40) return ELEGANT_PASSWORD_GOOD;
    if (score >= 20) return ELEGANT_PASSWORD_FAIR;
    return ELEGANT_PASSWORD_WEAK;
}

elegant_security_result_t elegant_validate_password(
    const char* password,
    elegant_password_rule_t* rules,
    size_t rule_count
) {
    elegant_security_result_t result = {0};
    
    if (!password || !rules || rule_count == 0) {
        result.valid = false;
        result.message = "Invalid parameters";
        return result;
    }
    
    // Allocate space for failed rules
    char** failed_messages = calloc(rule_count, sizeof(char*));
    size_t failed_count = 0;
    
    // Apply all rules (functional filter pattern)
    for (size_t i = 0; i < rule_count; i++) {
        char message[256] = {0};
        if (!rules[i](password, message, sizeof(message))) {
            failed_messages[failed_count] = strdup(message);
            failed_count++;
        }
    }
    
    result.valid = (failed_count == 0);
    result.score = elegant_password_score(password);
    result.failed_rules = (const char**)failed_messages;
    result.failed_rules_count = failed_count;
    
    if (result.valid) {
        result.message = "Password meets all requirements";
    } else {
        result.message = "Password validation failed";
    }
    
    return result;
}

void elegant_security_result_free(elegant_security_result_t* result) {
    if (result && result->failed_rules) {
        for (size_t i = 0; i < result->failed_rules_count; i++) {
            free((void*)result->failed_rules[i]);
        }
        free((void*)result->failed_rules);
        result->failed_rules = NULL;
        result->failed_rules_count = 0;
    }
}

/* =============================================================================
 * Input Sanitization (Functional Map/Filter)
 * ============================================================================= */

char* elegant_filter_string(const char* input, elegant_security_predicate_t predicate) {
    if (!input || !predicate) return NULL;
    
    size_t len = strlen(input);
    char* result = malloc(len + 1);
    if (!result) return NULL;
    
    size_t j = 0;
    // Functional FILTER pattern: keep only characters passing predicate
    for (size_t i = 0; i < len; i++) {
        if (predicate(input[i])) {
            result[j++] = input[i];
        }
    }
    result[j] = '\0';
    
    return result;
}

char* elegant_map_string(const char* input, char (*transform)(char)) {
    if (!input || !transform) return NULL;
    
    size_t len = strlen(input);
    char* result = malloc(len + 1);
    if (!result) return NULL;
    
    // Functional MAP pattern: transform each character
    for (size_t i = 0; i < len; i++) {
        result[i] = transform(input[i]);
    }
    result[len] = '\0';
    
    return result;
}

char* elegant_sanitize_string(const char* input, elegant_sanitize_mode_t mode) {
    if (!input) return NULL;
    
    if (mode & ELEGANT_SANITIZE_ALPHANUMERIC) {
        return elegant_filter_string(input, elegant_is_safe_alphanumeric);
    }
    
    if (mode & ELEGANT_SANITIZE_FILENAME) {
        return elegant_sanitize_filename(input);
    }
    
    if (mode & ELEGANT_SANITIZE_EMAIL) {
        return elegant_sanitize_email(input);
    }
    
    // Default: return copy
    return strdup(input);
}

char* elegant_escape_sql(const char* input) {
    if (!input) return NULL;
    
    size_t len = strlen(input);
    // Worst case: every character needs escaping
    char* result = malloc(len * 2 + 1);
    if (!result) return NULL;
    
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        if (input[i] == '\'') {
            result[j++] = '\'';
            result[j++] = '\'';
        } else if (input[i] == '\\') {
            result[j++] = '\\';
            result[j++] = '\\';
        } else {
            result[j++] = input[i];
        }
    }
    result[j] = '\0';
    
    return result;
}

char* elegant_escape_html(const char* input) {
    if (!input) return NULL;
    
    size_t len = strlen(input);
    // Worst case: every character becomes &xxxx;
    char* result = malloc(len * 6 + 1);
    if (!result) return NULL;
    
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        switch (input[i]) {
            case '<':
                strcpy(result + j, "&lt;");
                j += 4;
                break;
            case '>':
                strcpy(result + j, "&gt;");
                j += 4;
                break;
            case '&':
                strcpy(result + j, "&amp;");
                j += 5;
                break;
            case '"':
                strcpy(result + j, "&quot;");
                j += 6;
                break;
            case '\'':
                strcpy(result + j, "&#39;");
                j += 5;
                break;
            default:
                result[j++] = input[i];
                break;
        }
    }
    result[j] = '\0';
    
    return result;
}

char* elegant_sanitize_filename(const char* filename) {
    if (!filename) return NULL;
    
    // Remove path traversal attempts
    const char* basename = filename;
    const char* last_slash = strrchr(filename, '/');
    if (last_slash) basename = last_slash + 1;
    
    last_slash = strrchr(basename, '\\');
    if (last_slash) basename = last_slash + 1;
    
    // Filter to safe characters only
    return elegant_filter_string(basename, elegant_is_safe_filename_char);
}

bool elegant_validate_email(const char* email) {
    if (!email) return false;
    
    size_t len = strlen(email);
    if (len < 3 || len > 254) return false;
    
    // Must have exactly one @ symbol
    const char* at = strchr(email, '@');
    if (!at || at == email || strchr(at + 1, '@')) return false;
    
    // Must have at least one dot after @
    const char* dot = strchr(at, '.');
    if (!dot || dot == at + 1 || dot[1] == '\0') return false;
    
    return true;
}

char* elegant_sanitize_email(const char* email) {
    if (!email) return NULL;
    return elegant_filter_string(email, elegant_is_safe_email_char);
}

/* =============================================================================
 * Cryptographic Hash Functions (Using Functional Patterns)
 * ============================================================================= */

uint64_t elegant_hash_djb2(const uint8_t* data, size_t length) {
    if (!data) return 0;
    
    uint64_t hash = 5381;
    // Functional REDUCE pattern: accumulate hash value
    for (size_t i = 0; i < length; i++) {
        hash = ((hash << 5) + hash) + data[i]; // hash * 33 + c
    }
    return hash;
}

uint64_t elegant_hash_sdbm(const uint8_t* data, size_t length) {
    if (!data) return 0;
    
    uint64_t hash = 0;
    // Another functional REDUCE pattern
    for (size_t i = 0; i < length; i++) {
        hash = data[i] + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

uint64_t elegant_hash_fnv1a(const uint8_t* data, size_t length) {
    if (!data) return 0;
    
    uint64_t hash = 14695981039346656037ULL; // FNV offset basis
    const uint64_t prime = 1099511628211ULL; // FNV prime
    
    // Functional REDUCE with XOR and multiply
    for (size_t i = 0; i < length; i++) {
        hash ^= data[i];
        hash *= prime;
    }
    return hash;
}

uint64_t elegant_hash_combine(
    const uint8_t* data,
    size_t length,
    elegant_hash_func_t* hash_funcs,
    size_t func_count
) {
    if (!data || !hash_funcs || func_count == 0) return 0;
    
    uint64_t combined = 0;
    // Functional REDUCE: combine multiple hash functions
    for (size_t i = 0; i < func_count; i++) {
        uint64_t h = hash_funcs[i](data, length);
        combined ^= h + 0x9e3779b9 + (combined << 6) + (combined >> 2);
    }
    return combined;
}

uint64_t elegant_hash_password(const char* password, const char* salt) {
    if (!password) return 0;
    
    size_t pass_len = strlen(password);
    size_t salt_len = salt ? strlen(salt) : 0;
    size_t total_len = pass_len + salt_len;
    
    uint8_t* combined = malloc(total_len);
    if (!combined) return 0;
    
    memcpy(combined, password, pass_len);
    if (salt) {
        memcpy(combined + pass_len, salt, salt_len);
    }
    
    // Use multiple hash functions for better distribution
    elegant_hash_func_t funcs[] = {
        elegant_hash_djb2,
        elegant_hash_sdbm,
        elegant_hash_fnv1a
    };
    
    uint64_t hash = elegant_hash_combine(combined, total_len, funcs, 3);
    
    free(combined);
    return hash;
}

/* =============================================================================
 * Secure String Operations
 * ============================================================================= */

size_t elegant_secure_strcpy(char* dest, size_t dest_size, const char* src) {
    if (!dest || !src || dest_size == 0) return 0;
    
    size_t i;
    for (i = 0; i < dest_size - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
    
    return i;
}

size_t elegant_secure_strcat(char* dest, size_t dest_size, const char* src) {
    if (!dest || !src || dest_size == 0) return 0;
    
    size_t dest_len = strnlen(dest, dest_size);
    if (dest_len >= dest_size) return 0;
    
    size_t available = dest_size - dest_len;
    return elegant_secure_strcpy(dest + dest_len, available, src);
}

bool elegant_secure_strcmp(const char* a, const char* b) {
    if (!a || !b) return false;
    
    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    
    // Constant-time comparison to prevent timing attacks
    size_t max_len = len_a > len_b ? len_a : len_b;
    int result = 0;
    
    for (size_t i = 0; i < max_len; i++) {
        char ca = i < len_a ? a[i] : 0;
        char cb = i < len_b ? b[i] : 0;
        result |= ca ^ cb;
    }
    
    return result == 0 && len_a == len_b;
}

void elegant_secure_memzero(void* ptr, size_t size) {
    if (!ptr) return;
    
    // Use volatile to prevent compiler optimization
    volatile uint8_t* p = (volatile uint8_t*)ptr;
    for (size_t i = 0; i < size; i++) {
        p[i] = 0;
    }
}
