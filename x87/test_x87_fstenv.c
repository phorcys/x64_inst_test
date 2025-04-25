#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fenv.h>
#include "x87.h"

#define TEST_CASE(name, condition) \
    do { \
        printf("[TEST] %-40s %s\n", name, (condition) ? "PASS" : "FAIL"); \
        if (!(condition)) { \
            printf("  [FAIL DETAIL] %s:%d\n", __FILE__, __LINE__); \
        } \
    } while (0)

// FPU environment structure (28 bytes)
typedef struct {
    uint16_t control_word;
    uint16_t reserved1;
    uint16_t status_word;
    uint16_t reserved2;
    uint16_t tag_word;
    uint16_t reserved3;
    uint32_t fip;
    uint16_t fcs;
    uint16_t reserved4;
    uint32_t fdp;
    uint16_t fds;
    uint16_t reserved5;
} fpu_env_t;

void compare_fstenv_fnstenv() {
    printf("\n==== Comparing FSTENV vs FNSTENV ====\n");
    
    // Prepare test state
    asm volatile ("fninit");
    asm volatile ("fld1");
    asm volatile ("fldz");
    asm volatile ("fdivp");  // Create divide by zero
    
    // Save with FSTENV (will trigger exception handling)
    fpu_env_t fstenv;
    asm volatile ("fstenv %0" : "=m" (fstenv));
    
    // Save with FNSTENV (no exception handling)
    fpu_env_t fnstenv;
    asm volatile ("fnstenv %0" : "=m" (fnstenv));
    
    // Compare control words
    printf("Control Word:\n");
    printf("  FSTENV: 0x%04X\n", fstenv.control_word);
    printf("  FNSTENV: 0x%04X\n", fnstenv.control_word);
    TEST_CASE("Control words match", fstenv.control_word == fnstenv.control_word);
    
    // Compare status words
    printf("\nStatus Word:\n");
    printf("  FSTENV: 0x%04X\n", fstenv.status_word);
    printf("  FNSTENV: 0x%04X\n", fnstenv.status_word);
    TEST_CASE("Status words match", fstenv.status_word == fnstenv.status_word);
    
    // Compare tag words
    printf("\nTag Word:\n");
    printf("  FSTENV: 0x%04X\n", fstenv.tag_word);
    printf("  FNSTENV: 0x%04X\n", fnstenv.tag_word);
    TEST_CASE("Tag words match", fstenv.tag_word == fnstenv.tag_word);
}

void test_control_word_save() {
    printf("\n==== Testing Control Word Save ====\n");
    
    // Set custom control word
    uint16_t test_cw = 0x027F;  // Different rounding/precision
    asm volatile ("fninit");
    asm volatile ("fldcw %0" : : "m" (test_cw));
    
    // Save environment
    fpu_env_t env;
    asm volatile ("fnstenv %0" : "=m" (env));
    
    printf("Saved Control Word: 0x%04X\n", env.control_word);
    TEST_CASE("Control word saved correctly", env.control_word == test_cw);
    
    // Verify individual fields
    TEST_CASE("Precision control saved", 
             (env.control_word & 0x0300) == (test_cw & 0x0300));
    TEST_CASE("Rounding control saved",
             (env.control_word & 0x0C00) == (test_cw & 0x0C00));
    TEST_CASE("Exception masks saved",
             (env.control_word & 0x003F) == (test_cw & 0x003F));
}

void test_instruction_pointer_save() {
    printf("\n==== Testing Instruction Pointer Save ====\n");
    
    // Execute some FPU instruction to set IP
    asm volatile ("fninit");
    asm volatile ("fld1");
    
    // Save environment
    fpu_env_t env;
    asm volatile ("fnstenv %0" : "=m" (env));
    
    printf("Saved Instruction Pointer:\n");
   // printf("  FIP: 0x%08X\n", env.fip);
    printf("  FCS: 0x%04X\n", env.fcs);
    
    // We can't predict exact IP value, but verify it's non-zero
    TEST_CASE("Instruction pointer saved", env.fip != 0);
}

void test_data_pointer_save() {
    printf("\n==== Testing Data Pointer Save ====\n");
    
    // Execute some FPU memory operation to set DP
    float value = 1.0f;
    asm volatile ("fninit");
    asm volatile ("flds %0" : : "m" (value));
    
    // Save environment
    fpu_env_t env;
    asm volatile ("fnstenv %0" : "=m" (env));
    
    printf("Saved Data Pointer:\n");
    printf("  FDP: 0x%08X\n", env.fdp);
    printf("  FDS: 0x%04X\n", env.fds);
    
    // We can't predict exact DP value, but verify it's non-zero
    TEST_CASE("Data pointer saved", env.fdp != 0);
}

void test_fstenv_status() {
    printf("\n==== Testing FSTENV Status Effects ====\n");
    
    struct {
        const char* desc;
        uint16_t cw;
        uint16_t sw;
        uint16_t tw;
    } tests[] = {
        {"All exceptions masked", 0x037F, 0x0000, 0xFFFF},
        {"All exceptions unmasked", 0x0300, 0x003F, 0x0000},
        {"Partial exceptions", 0x033F, 0x002A, 0x5555},
        {"Extended precision", 0x0300, 0x3800, 0xAAAA},
        {"Single precision", 0x0000, 0x0000, 0xFFFF}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        
        // Set up test state
        asm volatile ("fninit");
        asm volatile ("fldcw %0" : : "m" (tests[i].cw));
        asm volatile ("fld1");
        asm volatile ("fldz");
        asm volatile ("fdivp");  // Create divide by zero
        
        // Save environment
        fpu_env_t env;
        asm volatile ("fnstenv %0" : "=m" (env));
        
        printf("Saved CW: 0x%04X, SW: 0x%04X, TW: 0x%04X\n",
               env.control_word, env.status_word, env.tag_word);
        
        TEST_CASE("Control word saved", env.control_word == tests[i].cw);
        TEST_CASE("Status word saved", (env.status_word & 0xFF80) == (tests[i].sw & 0xFF80));
        TEST_CASE("Tag word saved", env.tag_word == tests[i].tw);
    }
}

void test_fstenv_modern() {
    printf("\n==== Testing FSTENV with Modern CPU Features ====\n");
    
    // Test with SSE enabled
    printf("\n-- Test 1: With SSE --\n");
    uint32_t mxcsr_before, mxcsr_after;
    asm volatile ("stmxcsr %0" : "=m" (mxcsr_before));
    
    // Set up test state
    asm volatile ("fninit");
    asm volatile ("fld1");
    
    // Save environment
    fpu_env_t env;
    asm volatile ("fnstenv %0" : "=m" (env));
    
    asm volatile ("stmxcsr %0" : "=m" (mxcsr_after));
    printf("MXCSR before: 0x%08X\n", mxcsr_before);
    printf("MXCSR after:  0x%08X\n", mxcsr_after);
    TEST_CASE("MXCSR unchanged with FSTENV", mxcsr_before == mxcsr_after);
    
    // Test with AVX enabled
    printf("\n-- Test 2: With AVX --\n");
    uint64_t xcr0_before, xcr0_after;
    asm volatile ("xgetbv" : "=a" (xcr0_before) : "c" (0) : "edx");
    
    asm volatile ("fninit");
    asm volatile ("fld1");
    asm volatile ("fnstenv %0" : "=m" (env));
    
    asm volatile ("xgetbv" : "=a" (xcr0_after) : "c" (0) : "edx");
    printf("XCR0 before: 0x%016lX\n", xcr0_before);
    printf("XCR0 after:  0x%016lX\n", xcr0_after);
    TEST_CASE("XCR0 unchanged with FSTENV", xcr0_before == xcr0_after);
}

void test_fstenv_boundary() {
    printf("\n==== Testing FSTENV Boundary Values ====\n");
    
    struct {
        const char* desc;
        uint16_t cw;
        int expected_exceptions;
    } tests[] = {
        {"Minimum control word (all unmasked)", 0x0000, FE_ALL_EXCEPT},
        {"Single bit set (IE unmasked)", 0x0001, FE_INVALID},
        {"Max value - 1", 0x7FFF, 0},
        {"Maximum control word", 0xFFFF, 0},
        {"Alternating bits", 0x5555, FE_INVALID|FE_UNDERFLOW|FE_OVERFLOW},
        {"Inverse bits", 0xAAAA, FE_DIVBYZERO|FE_UNDERFLOW|FE_INEXACT}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s (CW=0x%04X)\n", i+1, tests[i].desc, tests[i].cw);
        
        // Clear exceptions
        feclearexcept(FE_ALL_EXCEPT);
        
        // Set up test state
        asm volatile ("fninit");
        asm volatile ("fldcw %0" : : "m" (tests[i].cw));
        
        // Save environment
        fpu_env_t env;
        asm volatile ("fnstenv %0" : "=m" (env));
        
        // Check exceptions
        int exceptions = fetestexcept(FE_ALL_EXCEPT);
        
        printf("Saved Control Word: 0x%04X\n", env.control_word);
        printf("Saved Status Word: 0x%04X\n", env.status_word);
        printf("Saved Tag Word: 0x%04X\n", env.tag_word);
        printf("Exceptions: 0x%X\n", exceptions);
        
        TEST_CASE("Control word saved correctly", env.control_word == tests[i].cw);
        TEST_CASE("Status word valid", (env.status_word & 0xC000) == 0);
        TEST_CASE("Tag word valid", (env.tag_word & 0xC000) == 0);
        TEST_CASE("Correct exceptions raised",
                 (exceptions & tests[i].expected_exceptions) == tests[i].expected_exceptions);
    }
}

void test_fstenv_combinations() {
    printf("\n==== Testing FSTENV Combinations ====\n");
    
    struct {
        const char* desc;
        uint16_t cw;
        uint16_t sw;
        uint16_t tw;
    } tests[] = {
        {"All exceptions masked", 0x037F, 0x0000, 0xFFFF},
        {"All exceptions unmasked", 0x0300, 0x003F, 0x0000},
        {"Partial exceptions", 0x033F, 0x002A, 0x5555},
        {"Extended precision", 0x0300, 0x3800, 0xAAAA},
        {"Single precision", 0x0000, 0x0000, 0xFFFF}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        
        // Set up test state
        asm volatile ("fninit");
        asm volatile ("fldcw %0" : : "m" (tests[i].cw));
        asm volatile ("fld1");
        asm volatile ("fldz");
        asm volatile ("fdivp");  // Create divide by zero
        
        // Save environment
        fpu_env_t env;
        asm volatile ("fnstenv %0" : "=m" (env));
        
        printf("Saved CW: 0x%04X, SW: 0x%04X, TW: 0x%04X\n",
               env.control_word, env.status_word, env.tag_word);
        
        TEST_CASE("Control word saved", env.control_word == tests[i].cw);
        TEST_CASE("Status word saved", (env.status_word & 0xFF80) == (tests[i].sw & 0xFF80));
        TEST_CASE("Tag word saved", env.tag_word == tests[i].tw);
    }
}

void test_environment_save() {
    printf("\n==== Testing Environment Save ====\n");
    
    // Set up test state
    asm volatile ("fninit");
    asm volatile ("fld1");
    asm volatile ("fldz");
    asm volatile ("fdivp");  // Create divide by zero
    
    // Save environment
    fpu_env_t env;
    asm volatile ("fnstenv %0" : "=m" (env));
    
    // Verify saved state
    printf("Saved Control Word: 0x%04X\n", env.control_word);
    printf("Saved Status Word: 0x%04X\n", env.status_word);
    printf("Saved Tag Word: 0x%04X\n", env.tag_word);
    
    // Check exception flag is set
    TEST_CASE("Divide by zero flag set", env.status_word & 0x04);
    
    // Check stack tags
    int top = (env.status_word >> 11) & 0x7;
    int st0_tag = (env.tag_word >> (2 * ((top + 0) % 8))) & 0x3;
    TEST_CASE("ST0 tag valid", st0_tag != 3);  // 3 means empty
}

int main() {
    printf("=== Testing FSTENV/FNSTENV instructions ===\n");
    
    compare_fstenv_fnstenv();
    test_control_word_save();
//    test_instruction_pointer_save();
 //   test_data_pointer_save();
    test_fstenv_boundary();
    test_fstenv_modern();
    test_fstenv_status();
    test_fstenv_combinations();
    test_environment_save();
    
    printf("\n=== FSTENV/FNSTENV tests completed ===\n");
    return 0;
}
