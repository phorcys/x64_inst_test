#include "avx.h"
#include <stdio.h>
#include <stdint.h>

static void test_vandnpd() {
    printf("--- Testing vandnpd (packed double-precision bitwise AND NOT) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test
    printf("Test 1: 256-bit register-register\n");
    uint64_t src1_u64[4] ALIGNED(32) = {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFF00000000, 
                                        0x00000000FFFFFFFF, 0xAAAAAAAAAAAAAAAA};
    uint64_t src2_u64[4] ALIGNED(32) = {0xAAAAAAAAAAAAAAAA, 0x5555555555555555,
                                        0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF};
    uint64_t expected_u64[4] = {0};
    uint64_t result_u64[4] ALIGNED(32) = {0};
    double* src1 = (double*)src1_u64;
    double* src2 = (double*)src2_u64;
    double* expected = (double*)expected_u64;
    double* result = (double*)result_u64;
    total_tests++;
    
    // Calculate expected values: ~src1 & src2
    for (int i = 0; i < 4; i++) {
        expected_u64[i] = ~src1_u64[i] & src2_u64[i];
    }
    
    // 检查内存对齐
    if (((uintptr_t)src1 % 32) != 0 || ((uintptr_t)src2 % 32) != 0 || ((uintptr_t)result % 32) != 0) {
        printf("Memory alignment error! src1: %p, src2: %p, result: %p\n", src1, src2, result);
        return;
    }
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vandnpd %%ymm1, %%ymm0, %%ymm2\n\t"  // 修正操作数顺序: ymm2 = ~ymm0 & ymm1
        "vmovapd %%ymm2, %0\n\t"
        : "=m"(*result)
        : "m"(*src1), "m"(*src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("256-bit register-register test:\n");
    print_double_vec_hex("Expected", expected, 4);
    print_double_vec_hex("Result", result, 4);
    
    int pass = 1;
    for (int i = 0; i < 4; i++) {
        if (result_u64[i] != expected_u64[i]) {
            printf("Mismatch at position %d: expected 0x%016lx, got 0x%016lx\n", 
                   i, expected_u64[i], result_u64[i]);
            pass = 0;
        }
    }
    if (pass) {
        printf("[PASS] Test 1: 256-bit register-register\n\n");
        passed_tests++;
    }
    
    // 128-bit register-register test
    printf("Test 2: 128-bit register-register\n");
    uint64_t src1_128_u64[2] ALIGNED(16) = {0x5555555555555555, 0xAAAAAAAAAAAAAAAA};
    uint64_t src2_128_u64[2] ALIGNED(16) = {0xFFFFFFFFFFFFFFFF, 0x0000000000000000};
    uint64_t expected_128_u64[2] = {0};
    uint64_t result_128_u64[2] ALIGNED(16) = {0};
    double* src1_128 = (double*)src1_128_u64;
    double* src2_128 = (double*)src2_128_u64;
    double* expected_128 = (double*)expected_128_u64;
    double* result_128 = (double*)result_128_u64;
    total_tests++;
    
    // Calculate expected values: ~src1 & src2
    for (int i = 0; i < 2; i++) {
        expected_128_u64[i] = ~src1_128_u64[i] & src2_128_u64[i];
    }
    
    // 检查内存对齐
    if (((uintptr_t)src1_128 % 16) != 0 || ((uintptr_t)src2_128 % 16) != 0 || ((uintptr_t)result_128 % 16) != 0) {
        printf("Memory alignment error! src1_128: %p, src2_128: %p, result_128: %p\n", src1_128, src2_128, result_128);
        return;
    }
    
    __asm__ __volatile__(
        "vmovapd %1, %%xmm0\n\t"
        "vmovapd %2, %%xmm1\n\t"
        "vandnpd %%xmm1, %%xmm0, %%xmm2\n\t"  // 修正操作数顺序: xmm2 = ~xmm0 & xmm1
        "vmovapd %%xmm2, %0\n\t"
        : "=m"(*result_128)
        : "m"(*src1_128), "m"(*src2_128)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("128-bit register-register test:\n");
    print_double_vec_hex("Expected", expected_128, 2);
    print_double_vec_hex("Result", result_128, 2);
    
    pass = 1;
    for (int i = 0; i < 2; i++) {
        if (result_128_u64[i] != expected_128_u64[i]) {
            printf("Mismatch at position %d: expected 0x%016lx, got 0x%016lx\n", 
                   i, expected_128_u64[i], result_128_u64[i]);
            pass = 0;
        }
    }
    if (pass) {
        printf("[PASS] Test 2: 128-bit register-register\n\n");
        passed_tests++;
    }
    
    // Test summary
    printf("--- Test Summary ---\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vandnpd tests passed!\n");
    } else {
        printf("Some vandnpd tests failed\n");
    }
}

int main() {
    test_vandnpd();
    return 0;
}
