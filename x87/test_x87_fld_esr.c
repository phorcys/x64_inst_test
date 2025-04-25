#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <fenv.h>
#include <float.h>
#include "x87.h"

#define TEST_CASE(name, condition) \
    do { \
        printf("[TEST] %-40s %s\n", name, (condition) ? "PASS" : "FAIL"); \
        if (!(condition)) { \
            printf("  [FAIL DETAIL] %s:%d\n", __FILE__, __LINE__); \
        } \
    } while (0)

void test_fld_esr_basic() {
    printf("\n==== Testing FLD ST(i) Basic ====\n");
    long double val1 = 1.23456789L;
    long double val2 = 2.34567890L;
    
    // Test 1: FLD ST(0)
    asm volatile ("finit");
    asm volatile ("fldt %0" : : "m" (val1));
    asm volatile ("fldt %0" : : "m" (val2));
    
    printf("Initial stack:\n");
    print_x87_status();
    
    asm volatile ("fld %%st(0)" : : : "st");
    
    printf("\nAfter FLD ST(0):\n");
    print_x87_status();
    
    long double top1, top2;
    asm volatile ("fstpt %0" : "=m" (top1));
    asm volatile ("fstpt %0" : "=m" (top2));
    
    printf("Top value: "); print_float80(top1);
    printf("Second value: "); print_float80(top2);
    TEST_CASE("FLD ST(0) duplicates value", top1 == val2 && top2 == val2);
    
    // Test 2: FLD ST(1)
    asm volatile ("finit");
    asm volatile ("fldt %0" : : "m" (val1));
    asm volatile ("fldt %0" : : "m" (val2));
    asm volatile ("fld %%st(1)" : : : "st");
    
    printf("\nAfter FLD ST(1):\n");
    print_x87_status();
    
    long double t1, t2, t3;
    asm volatile ("fstpt %0" : "=m" (t1));
    asm volatile ("fstpt %0" : "=m" (t2));
    asm volatile ("fstpt %0" : "=m" (t3));
    
    printf("Top value: "); print_float80(t1);
    printf("Second value: "); print_float80(t2);
    printf("Third value: "); print_float80(t3);
    TEST_CASE("FLD ST(1) works correctly", t1 == val1 && t2 == val2 && t3 == val1);
}

void test_fld_esr_boundary() {
    printf("\n==== Testing FLD ST(i) Boundary ====\n");
    
    // Test with full stack
    asm volatile ("finit");
    for (int i = 0; i < 8; i++) {
        asm volatile ("fld1");
    }
    
    printf("Before FLD ST(0) (full stack):\n");
    print_x87_status();
    
    uint16_t sw_before = get_x87_sw();
    asm volatile ("fld %%st(0)" : : : "st");
    uint16_t sw_after = get_x87_sw();
    
    printf("\nAfter FLD ST(0):\n");
    print_x87_status();
    
    TEST_CASE("Stack overflow detected", sw_after & 0x4000);
    TEST_CASE("Stack pointer updated", ((sw_after >> 11) & 0x7) == 7);
    
    // Test with empty stack
    asm volatile ("finit");
    printf("\nBefore FLD ST(0) (empty stack):\n");
    print_x87_status();
    
    sw_before = get_x87_sw();
    asm volatile ("fld %%st(0)" : : : "st");
    sw_after = get_x87_sw();
    
    printf("\nAfter FLD ST(0):\n");
    print_x87_status();
    
    TEST_CASE("Stack underflow detected", sw_after & 0x4000);
}

void test_fld_esr_status() {
    printf("\n==== Testing FLD ST(i) Status ====\n");
    
    long double val = 1.23456789L;
    asm volatile ("finit");
    asm volatile ("fldt %0" : : "m" (val));
    
    uint16_t cw_before = get_x87_cw();
    uint16_t sw_before = get_x87_sw();
    uint16_t tw_before = get_x87_tw();
    
    printf("Before FLD ST(0):\n");
    print_x87_status();
    
    asm volatile ("fld %%st(0)" : : : "st");
    
    printf("\nAfter FLD ST(0):\n");
    print_x87_status();
    
    uint16_t cw_after = get_x87_cw();
    uint16_t sw_after = get_x87_sw();
    uint16_t tw_after = get_x87_tw();
    
    TEST_CASE("Control word unchanged", cw_before == cw_after);
    TEST_CASE("Status word updated", sw_after != sw_before);
    TEST_CASE("Tag word updated", tw_after != tw_before);
}

void test_fld_esr_exceptions() {
    printf("\n==== Testing FLD ST(i) Exceptions ====\n");
    
    // Test with invalid stack index
    asm volatile ("finit");
    asm volatile ("fld1");
    
    feclearexcept(FE_ALL_EXCEPT);
    uint16_t sw_before = get_x87_sw();
    
    printf("Before FLD ST(7):\n");
    print_x87_status();
    
    // Fill stack to test boundary
    for (int i = 0; i < 7; i++) {
        asm volatile ("fld1");
    }
    
    asm volatile ("fld %%st(7)" : : : "st");  // Valid index but will cause stack overflow
    
    uint16_t sw_after = get_x87_sw();
    int exceptions = fetestexcept(FE_ALL_EXCEPT);
    
    printf("\nAfter FLD ST(7):\n");
    print_x87_status();
    
    TEST_CASE("Stack overflow detected", exceptions & FE_INVALID);
    TEST_CASE("Stack fault flag set", sw_after & 0x4000);
}

int main() {
    printf("=== Testing FLD ST(i) instruction ===\n");
    
    test_fld_esr_basic();
    test_fld_esr_boundary();
    test_fld_esr_status();
    test_fld_esr_exceptions();
    
    printf("\n=== FLD ST(i) tests completed ===\n");
    return 0;
}
