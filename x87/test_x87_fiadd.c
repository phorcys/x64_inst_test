#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t bytes[10];
} f80_t;

static inline long double f80_to_ld(f80_t x) {
    long double val;
    memcpy(&val, x.bytes, sizeof(x.bytes));
    return val;
}

static inline f80_t ld_to_f80(long double x) {
    f80_t val;
    memcpy(val.bytes, &x, sizeof(val.bytes));
    return val;
}

void test_fiadd() {
    f80_t result;
    unsigned short status = 0;
    
    printf("=== Testing FIADD ===\n");
    
    struct {
        int32_t i32;
        int64_t i64;
        f80_t fval;
        const char* desc;
    } tests[] = {
        {100, 100L, ld_to_f80(50.0L), "Positive integers"},
        {-50, -50L, ld_to_f80(100.0L), "Negative integers"},
        {INT32_MAX, 1000000000L, ld_to_f80(1.0L), "Large positive integers"},
        {INT32_MIN, -1000000000L, ld_to_f80(1.0L), "Large negative integers"},
        {0, 0L, ld_to_f80(0.0L), "Zero values"}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        // Test 32-bit integer addition
        {
            asm volatile ("fldt %1\n\t"
                         "fiaddl %2\n\t"
                         "fstpt %0\n\t"
                         "fnstsw %3"
                         : "=m" (result)
                         : "m" (tests[i].fval), "m" (tests[i].i32), "m" (status)
                         : "memory", "st", "st(1)");
        }
        
        printf("\nTest %zu.1: %s (32-bit)\n", i+1, tests[i].desc);
        printf("Operation: %Lf + %d\n", f80_to_ld(tests[i].fval), tests[i].i32);
        printf("Result: %Lf\n", f80_to_ld(result));
        printf("Status Word: 0x%04x\n", status);
        print_x87_status();
        
        // Test 64-bit integer addition
        {
            asm volatile ("fldt %1\n\t"
                         "fiaddl %2\n\t"
                         "fstpt %0\n\t"
                         "fnstsw %3"
                         : "=m" (result)
                         : "m" (tests[i].fval), "m" (tests[i].i64), "m" (status)
                         : "memory", "st", "st(1)");
        }
        
        printf("\nTest %zu.2: %s (64-bit)\n", i+1, tests[i].desc);
        printf("Operation: %Lf + %ld\n", f80_to_ld(tests[i].fval), tests[i].i64);
        printf("Result: %.15Lf\n", f80_to_ld(result));
        printf("Status Word: 0x%04x\n", status);
        print_x87_status();
    }
}

int main() {
    test_fiadd();
    return 0;
}
