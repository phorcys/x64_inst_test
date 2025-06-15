#include "avx.h"
#include <stdio.h>
#include <stdint.h>

static void test_vandnps() {
    printf("--- Testing vandnps (packed single-precision bitwise AND NOT) ---\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 256-bit register-register test
    printf("Test 1: 256-bit register-register\n");
    uint32_t src1_u32[8] ALIGNED(32) = {
        0xFFFFFFFF, 0x00000000, 0xAAAAAAAA, 0x55555555,
        0x12345678, 0x87654321, 0xF0F0F0F0, 0x0F0F0F0F
    };
    uint32_t src2_u32[8] ALIGNED(32) = {
        0xAAAAAAAA, 0x55555555, 0xFFFFFFFF, 0x00000000,
        0x87654321, 0x12345678, 0x0F0F0F0F, 0xF0F0F0F0
    };
    uint32_t expected_u32[8] = {0};
    uint32_t result_u32[8] ALIGNED(32) = {0};
    float* src1 = (float*)src1_u32;
    float* src2 = (float*)src2_u32;
    float* expected = (float*)expected_u32;
    float* result = (float*)result_u32;
    total_tests++;
    
    // Calculate expected values: ~src1 & src2
    for (int i = 0; i < 8; i++) {
        expected_u32[i] = ~src1_u32[i] & src2_u32[i];
    }
    
    // 检查内存对齐
    if (((uintptr_t)src1 % 32) != 0 || ((uintptr_t)src2 % 32) != 0 || ((uintptr_t)result % 32) != 0) {
        printf("Memory alignment error! src1: %p, src2: %p, result: %p\n", src1, src2, result);
        return;
    }
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vandnps %%ymm1, %%ymm0, %%ymm2\n\t"  // ymm2 = ~ymm0 & ymm1
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(*result)
        : "m"(*src1), "m"(*src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("256-bit register-register test:\n");
    print_int32_vec("Expected", (int32_t*)expected, 8);
    print_int32_vec("Result", (int32_t*)result, 8);
    
    int pass = 1;
    for (int i = 0; i < 8; i++) {
        if (result_u32[i] != expected_u32[i]) {
            printf("Mismatch at position %d: expected 0x%08x, got 0x%08x\n", 
                   i, expected_u32[i], result_u32[i]);
            pass = 0;
        }
    }
    if (pass) {
        printf("[PASS] Test 1: 256-bit register-register\n\n");
        passed_tests++;
    }
    
    // 128-bit register-register test
    printf("Test 2: 128-bit register-register\n");
    uint32_t src1_128_u32[4] ALIGNED(16) = {
        0x11111111, 0x22222222, 0x33333333, 0x44444444
    };
    uint32_t src2_128_u32[4] ALIGNED(16) = {
        0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC, 0xDDDDDDDD
    };
    uint32_t expected_128_u32[4] = {0};
    uint32_t result_128_u32[4] ALIGNED(16) = {0};
    float* src1_128 = (float*)src1_128_u32;
    float* src2_128 = (float*)src2_128_u32;
    float* expected_128 = (float*)expected_128_u32;
    float* result_128 = (float*)result_128_u32;
    total_tests++;
    
    // Calculate expected values: ~src1 & src2
    for (int i = 0; i < 4; i++) {
        expected_128_u32[i] = ~src1_128_u32[i] & src2_128_u32[i];
    }
    
    // 检查内存对齐
    if (((uintptr_t)src1_128 % 16) != 0 || ((uintptr_t)src2_128 % 16) != 0 || ((uintptr_t)result_128 % 16) != 0) {
        printf("Memory alignment error! src1_128: %p, src2_128: %p, result_128: %p\n", src1_128, src2_128, result_128);
        return;
    }
    
    __asm__ __volatile__(
        "vmovaps %1, %%xmm0\n\t"
        "vmovaps %2, %%xmm1\n\t"
        "vandnps %%xmm1, %%xmm0, %%xmm2\n\t"  // xmm2 = ~xmm0 & xmm1
        "vmovaps %%xmm2, %0\n\t"
        : "=m"(*result_128)
        : "m"(*src1_128), "m"(*src2_128)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("128-bit register-register test:\n");
    print_int32_vec("Expected", (int32_t*)expected_128, 4);
    print_int32_vec("Result", (int32_t*)result_128, 4);
    
    pass = 1;
    for (int i = 0; i < 4; i++) {
        if (result_128_u32[i] != expected_128_u32[i]) {
            printf("Mismatch at position %d: expected 0x%08x, got 0x%08x\n", 
                   i, expected_128_u32[i], result_128_u32[i]);
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
        printf("All vandnps tests passed!\n");
    } else {
        printf("Some vandnps tests failed\n");
    }
}

int main() {
    test_vandnps();
    return 0;
}
