/*
 * Elegant Security Library Example
 * Demonstrates functional programming patterns for security operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elegant.h>
#include <elegant_security.h>

/* Helper function to print password strength */
const char* strength_to_string(elegant_password_strength_t strength) {
    switch (strength) {
        case ELEGANT_PASSWORD_WEAK: return "Weak";
        case ELEGANT_PASSWORD_FAIR: return "Fair";
        case ELEGANT_PASSWORD_GOOD: return "Good";
        case ELEGANT_PASSWORD_STRONG: return "Strong";
        case ELEGANT_PASSWORD_VERY_STRONG: return "Very Strong";
        default: return "Unknown";
    }
}

void demo_password_validation(void) {
    printf("\n=== Password Validation Demo ===\n");
    
    const char* test_passwords[] = {
        "weak",
        "password123",
        "GoodPass1",
        "Str0ng!Pass",
        "V3ry$tr0ng!P@ssw0rd"
    };
    
    for (size_t i = 0; i < sizeof(test_passwords) / sizeof(test_passwords[0]); i++) {
        const char* pwd = test_passwords[i];
        int score = elegant_password_score(pwd);
        elegant_password_strength_t strength = elegant_password_strength(pwd);
        
        printf("\nPassword: \"%s\"\n", pwd);
        printf("  Score: %d/100\n", score);
        printf("  Strength: %s\n", strength_to_string(strength));
        
        // Test with validation rules
        elegant_password_rule_t rules[] = {
            elegant_rule_min_length,
            elegant_rule_has_uppercase,
            elegant_rule_has_lowercase,
            elegant_rule_has_digit,
            elegant_rule_has_special
        };
        
        elegant_security_result_t result = elegant_validate_password(
            pwd, rules, sizeof(rules) / sizeof(rules[0])
        );
        
        if (result.valid) {
            printf("  ✓ Validation: PASSED\n");
        } else {
            printf("  ✗ Validation: FAILED\n");
            for (size_t j = 0; j < result.failed_rules_count; j++) {
                printf("    - %s\n", result.failed_rules[j]);
            }
        }
        
        elegant_security_result_free(&result);
    }
}

void demo_input_sanitization(void) {
    printf("\n\n=== Input Sanitization Demo ===\n");
    
    // Test SQL injection prevention
    const char* sql_input = "admin' OR '1'='1";
    printf("\nOriginal SQL input: %s\n", sql_input);
    char* escaped_sql = elegant_escape_sql(sql_input);
    if (escaped_sql) {
        printf("Escaped SQL: %s\n", escaped_sql);
        free(escaped_sql);
    }
    
    // Test HTML/XSS prevention
    const char* html_input = "<script>alert('XSS')</script>";
    printf("\nOriginal HTML input: %s\n", html_input);
    char* escaped_html = elegant_escape_html(html_input);
    if (escaped_html) {
        printf("Escaped HTML: %s\n", escaped_html);
        free(escaped_html);
    }
    
    // Test filename sanitization (path traversal prevention)
    const char* filename_input = "../../../etc/passwd";
    printf("\nOriginal filename: %s\n", filename_input);
    char* safe_filename = elegant_sanitize_filename(filename_input);
    if (safe_filename) {
        printf("Sanitized filename: %s\n", safe_filename);
        free(safe_filename);
    }
    
    // Test functional filtering
    const char* mixed_input = "Hello123!@#World456";
    printf("\nOriginal mixed input: %s\n", mixed_input);
    
    char* alphanumeric = elegant_filter_string(mixed_input, elegant_is_safe_alphanumeric);
    if (alphanumeric) {
        printf("Alphanumeric only: %s\n", alphanumeric);
        free(alphanumeric);
    }
}

void demo_email_validation(void) {
    printf("\n\n=== Email Validation Demo ===\n");
    
    const char* test_emails[] = {
        "valid@example.com",
        "user.name+tag@example.co.uk",
        "invalid@",
        "@invalid.com",
        "no-at-sign.com",
        "multiple@@example.com"
    };
    
    for (size_t i = 0; i < sizeof(test_emails) / sizeof(test_emails[0]); i++) {
        const char* email = test_emails[i];
        bool valid = elegant_validate_email(email);
        printf("Email: %-30s %s\n", email, valid ? "✓ Valid" : "✗ Invalid");
    }
}

void demo_hash_functions(void) {
    printf("\n\n=== Hash Functions Demo ===\n");
    
    const char* data = "Hello, Elegant Security!";
    size_t len = strlen(data);
    
    printf("Data: \"%s\"\n\n", data);
    
    uint64_t hash_djb2 = elegant_hash_djb2((const uint8_t*)data, len);
    printf("DJB2 Hash:  0x%016lx\n", hash_djb2);
    
    uint64_t hash_sdbm = elegant_hash_sdbm((const uint8_t*)data, len);
    printf("SDBM Hash:  0x%016lx\n", hash_sdbm);
    
    uint64_t hash_fnv = elegant_hash_fnv1a((const uint8_t*)data, len);
    printf("FNV-1a Hash: 0x%016lx\n", hash_fnv);
    
    // Demonstrate hash combination (functional composition)
    elegant_hash_func_t funcs[] = {
        elegant_hash_djb2,
        elegant_hash_sdbm,
        elegant_hash_fnv1a
    };
    
    uint64_t combined = elegant_hash_combine(
        (const uint8_t*)data, len,
        funcs, sizeof(funcs) / sizeof(funcs[0])
    );
    printf("Combined Hash: 0x%016lx\n", combined);
    
    // Demonstrate password hashing with salt
    const char* password = "MySecretPassword";
    const char* salt = "random_salt_123";
    uint64_t pwd_hash = elegant_hash_password(password, salt);
    printf("\nPassword hash (with salt): 0x%016lx\n", pwd_hash);
}

void demo_secure_operations(void) {
    printf("\n\n=== Secure String Operations Demo ===\n");
    
    // Secure string copy
    char dest[20] = {0};
    const char* src = "This is a very long string that will be truncated";
    size_t copied = elegant_secure_strcpy(dest, sizeof(dest), src);
    printf("Secure copy: \"%s\" (%zu chars)\n", dest, copied);
    
    // Secure string concatenation
    char buffer[50] = "Hello";
    elegant_secure_strcat(buffer, sizeof(buffer), " ");
    elegant_secure_strcat(buffer, sizeof(buffer), "World");
    printf("Secure concat: \"%s\"\n", buffer);
    
    // Constant-time string comparison (timing attack prevention)
    const char* password1 = "secret123";
    const char* password2 = "secret123";
    const char* password3 = "wrong";
    
    printf("\nConstant-time comparison:\n");
    printf("  \"%s\" == \"%s\": %s\n", password1, password2,
           elegant_secure_strcmp(password1, password2) ? "Equal" : "Not equal");
    printf("  \"%s\" == \"%s\": %s\n", password1, password3,
           elegant_secure_strcmp(password1, password3) ? "Equal" : "Not equal");
}

void demo_functional_patterns(void) {
    printf("\n\n=== Functional Programming Patterns ===\n");
    
    const char* input = "Functional123Programming!@#456";
    printf("Input string: \"%s\"\n\n", input);
    
    // Filter pattern: extract only digits
    printf("Filter (digits only):\n");
    char* digits = elegant_filter_string(input, elegant_is_digit);
    if (digits) {
        printf("  Result: \"%s\"\n", digits);
        free(digits);
    }
    
    // Filter pattern: extract only uppercase
    printf("\nFilter (uppercase only):\n");
    char* uppercase = elegant_filter_string(input, elegant_is_uppercase);
    if (uppercase) {
        printf("  Result: \"%s\"\n", uppercase);
        free(uppercase);
    }
    
    // Filter pattern: extract only lowercase
    printf("\nFilter (lowercase only):\n");
    char* lowercase = elegant_filter_string(input, elegant_is_lowercase);
    if (lowercase) {
        printf("  Result: \"%s\"\n", lowercase);
        free(lowercase);
    }
    
    // Filter pattern: extract only special characters
    printf("\nFilter (special chars only):\n");
    char* special = elegant_filter_string(input, elegant_is_special_char);
    if (special) {
        printf("  Result: \"%s\"\n", special);
        free(special);
    }
}

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║         Elegant Security Library Demonstration           ║\n");
    printf("║    Functional Programming for Security Operations        ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    
    demo_password_validation();
    demo_input_sanitization();
    demo_email_validation();
    demo_hash_functions();
    demo_secure_operations();
    demo_functional_patterns();
    
    printf("\n\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║              All Security Demos Completed!               ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
