/*
 * Elegant Security Library Unit Tests
 * Basic automated testing for security functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <elegant.h>
#include <elegant_security.h>

/* Test counter */
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    printf("  Testing %s... ", name); \
    fflush(stdout);

#define PASS() \
    printf("✓ PASS\n"); \
    tests_passed++;

#define FAIL(msg) \
    printf("✗ FAIL: %s\n", msg); \
    tests_failed++;

/* Test password scoring */
void test_password_scoring(void) {
    TEST("password scoring");
    
    int score_weak = elegant_password_score("weak");
    int score_strong = elegant_password_score("Str0ng!Pass");
    
    assert(score_weak < score_strong);
    assert(score_weak < 30);
    assert(score_strong > 70);
    
    PASS();
}

/* Test password strength classification */
void test_password_strength(void) {
    TEST("password strength classification");
    
    elegant_password_strength_t weak = elegant_password_strength("weak");
    elegant_password_strength_t strong = elegant_password_strength("V3ry$tr0ng!P@ssw0rd");
    
    assert(weak == ELEGANT_PASSWORD_WEAK || weak == ELEGANT_PASSWORD_FAIR);
    assert(strong >= ELEGANT_PASSWORD_STRONG);
    
    PASS();
}

/* Test password validation rules */
void test_password_validation(void) {
    TEST("password validation rules");
    
    elegant_password_rule_t rules[] = {
        elegant_rule_min_length,
        elegant_rule_has_uppercase,
        elegant_rule_has_lowercase,
        elegant_rule_has_digit
    };
    
    // Should pass
    elegant_security_result_t result1 = elegant_validate_password(
        "GoodPass123", rules, 4
    );
    assert(result1.valid == true);
    elegant_security_result_free(&result1);
    
    // Should fail (no uppercase)
    elegant_security_result_t result2 = elegant_validate_password(
        "badpass123", rules, 4
    );
    assert(result2.valid == false);
    assert(result2.failed_rules_count > 0);
    elegant_security_result_free(&result2);
    
    PASS();
}

/* Test character predicates */
void test_character_predicates(void) {
    TEST("character predicates");
    
    assert(elegant_is_uppercase('A') == true);
    assert(elegant_is_uppercase('a') == false);
    assert(elegant_is_lowercase('a') == true);
    assert(elegant_is_lowercase('A') == false);
    assert(elegant_is_digit('5') == true);
    assert(elegant_is_digit('a') == false);
    assert(elegant_is_special_char('!') == true);
    assert(elegant_is_special_char('a') == false);
    
    PASS();
}

/* Test functional string filtering */
void test_string_filtering(void) {
    TEST("functional string filtering");
    
    const char* input = "Hello123World!@#456";
    
    // Filter digits
    char* digits = elegant_filter_string(input, elegant_is_digit);
    assert(digits != NULL);
    assert(strcmp(digits, "123456") == 0);
    free(digits);
    
    // Filter uppercase
    char* upper = elegant_filter_string(input, elegant_is_uppercase);
    assert(upper != NULL);
    assert(strcmp(upper, "HW") == 0);
    free(upper);
    
    // Filter alphanumeric
    char* alnum = elegant_filter_string(input, elegant_is_safe_alphanumeric);
    assert(alnum != NULL);
    assert(strcmp(alnum, "Hello123World456") == 0);
    free(alnum);
    
    PASS();
}

/* Test SQL escaping */
void test_sql_escaping(void) {
    TEST("SQL injection prevention");
    
    const char* malicious = "admin' OR '1'='1";
    char* escaped = elegant_escape_sql(malicious);
    
    assert(escaped != NULL);
    assert(strstr(escaped, "''") != NULL);  // Single quotes should be doubled
    
    free(escaped);
    PASS();
}

/* Test HTML escaping */
void test_html_escaping(void) {
    TEST("HTML/XSS prevention");
    
    const char* xss = "<script>alert('XSS')</script>";
    char* escaped = elegant_escape_html(xss);
    
    assert(escaped != NULL);
    assert(strstr(escaped, "<script>") == NULL);  // < should be escaped
    assert(strstr(escaped, "&lt;") != NULL);      // Should contain &lt;
    assert(strstr(escaped, "&gt;") != NULL);      // Should contain &gt;
    
    free(escaped);
    PASS();
}

/* Test filename sanitization */
void test_filename_sanitization(void) {
    TEST("path traversal prevention");
    
    const char* dangerous = "../../../etc/passwd";
    char* safe = elegant_sanitize_filename(dangerous);
    
    assert(safe != NULL);
    assert(strstr(safe, "..") == NULL);     // .. should be removed
    assert(strstr(safe, "/") == NULL);      // / should be removed
    assert(strcmp(safe, "passwd") == 0);    // Only basename should remain
    
    free(safe);
    PASS();
}

/* Test email validation */
void test_email_validation(void) {
    TEST("email validation");
    
    assert(elegant_validate_email("valid@example.com") == true);
    assert(elegant_validate_email("user.name@example.co.uk") == true);
    assert(elegant_validate_email("invalid@") == false);
    assert(elegant_validate_email("@invalid.com") == false);
    assert(elegant_validate_email("no-at-sign.com") == false);
    assert(elegant_validate_email(NULL) == false);
    
    PASS();
}

/* Test hash functions */
void test_hash_functions(void) {
    TEST("hash functions");
    
    const char* data = "Hello, Security!";
    size_t len = strlen(data);
    
    uint64_t hash1 = elegant_hash_djb2((const uint8_t*)data, len);
    uint64_t hash2 = elegant_hash_sdbm((const uint8_t*)data, len);
    uint64_t hash3 = elegant_hash_fnv1a((const uint8_t*)data, len);
    
    // All hashes should be non-zero
    assert(hash1 != 0);
    assert(hash2 != 0);
    assert(hash3 != 0);
    
    // Different algorithms should produce different hashes
    assert(hash1 != hash2);
    assert(hash2 != hash3);
    assert(hash1 != hash3);
    
    // Same input should produce same hash
    uint64_t hash1_repeat = elegant_hash_djb2((const uint8_t*)data, len);
    assert(hash1 == hash1_repeat);
    
    PASS();
}

/* Test hash composition */
void test_hash_composition(void) {
    TEST("hash function composition");
    
    const char* data = "Test data";
    size_t len = strlen(data);
    
    elegant_hash_func_t funcs[] = {
        elegant_hash_djb2,
        elegant_hash_sdbm,
        elegant_hash_fnv1a
    };
    
    uint64_t combined = elegant_hash_combine(
        (const uint8_t*)data, len, funcs, 3
    );
    
    assert(combined != 0);
    
    // Different from individual hashes
    uint64_t individual = elegant_hash_djb2((const uint8_t*)data, len);
    assert(combined != individual);
    
    PASS();
}

/* Test secure string operations */
void test_secure_string_ops(void) {
    TEST("secure string operations");
    
    // Test secure copy
    char dest[10];
    size_t copied = elegant_secure_strcpy(dest, sizeof(dest), "Very long string");
    assert(copied < sizeof(dest));
    assert(dest[sizeof(dest) - 1] == '\0');  // Null terminated
    
    // Test secure concat
    char buffer[20] = "Hello";
    elegant_secure_strcat(buffer, sizeof(buffer), " World");
    assert(strcmp(buffer, "Hello World") == 0);
    
    // Test constant-time comparison
    assert(elegant_secure_strcmp("test", "test") == true);
    assert(elegant_secure_strcmp("test", "wrong") == false);
    
    PASS();
}

/* Test secure memory zeroing */
void test_secure_memzero(void) {
    TEST("secure memory zeroing");
    
    char sensitive[16];
    memset(sensitive, 'X', sizeof(sensitive));
    
    elegant_secure_memzero(sensitive, sizeof(sensitive));
    
    // Verify all bytes are zero
    for (size_t i = 0; i < sizeof(sensitive); i++) {
        assert(sensitive[i] == 0);
    }
    
    PASS();
}

/* Test NULL safety */
void test_null_safety(void) {
    TEST("NULL pointer safety");
    
    // Functions should handle NULL gracefully
    assert(elegant_password_score(NULL) == 0);
    assert(elegant_filter_string(NULL, elegant_is_digit) == NULL);
    assert(elegant_escape_sql(NULL) == NULL);
    assert(elegant_escape_html(NULL) == NULL);
    assert(elegant_sanitize_filename(NULL) == NULL);
    assert(elegant_validate_email(NULL) == false);
    
    PASS();
}

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║      Elegant Security Library Unit Tests                ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");
    
    /* Run all tests */
    test_password_scoring();
    test_password_strength();
    test_password_validation();
    test_character_predicates();
    test_string_filtering();
    test_sql_escaping();
    test_html_escaping();
    test_filename_sanitization();
    test_email_validation();
    test_hash_functions();
    test_hash_composition();
    test_secure_string_ops();
    test_secure_memzero();
    test_null_safety();
    
    /* Print summary */
    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                    Test Summary                          ║\n");
    printf("╠═══════════════════════════════════════════════════════════╣\n");
    printf("║  Total Tests:  %3d                                       ║\n", tests_passed + tests_failed);
    printf("║  Passed:       %3d                                       ║\n", tests_passed);
    printf("║  Failed:       %3d                                       ║\n", tests_failed);
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    
    if (tests_failed == 0) {
        printf("\n✓ All security tests passed successfully!\n\n");
        return 0;
    } else {
        printf("\n✗ Some tests failed. Please review the output above.\n\n");
        return 1;
    }
}
