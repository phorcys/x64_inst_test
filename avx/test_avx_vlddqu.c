#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cpuid.h>

// 检查CPU是否支持AVX
static int check_avx_support() {
    unsigned int eax, ebx, ecx, edx;
    __get_cpuid(1, &eax, &ebx, &ecx, &edx);
    return (ecx & bit_AVX) ? 1 : 0;
}

// 扩展测试
static void test_vlddqu() {
    printf("\n=== Extended VLDDQU Tests ===\n");
    
    // 测试1: 对齐内存访问
    {
        ALIGNED(16) uint8_t aligned_data[16] = {
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
        };
        
        printf("Test 1: Aligned memory access (128-bit)\n");
        printf("Input:  ");
        for(int i=0; i<16; i++) printf("%02X ", aligned_data[i]);
        
        __m128i result;
        __asm__ __volatile__(
            "vlddqu %1, %0\n\t"
            : "=x"(result)
            : "m"(*(__m128i*)aligned_data)
            : "memory"
        );
        
        uint8_t* res = (uint8_t*)&result;
        printf("\nResult: ");
        for(int i=0; i<16; i++) printf("%02X ", res[i]);
        printf("\nStatus: %s\n", 
            memcmp(aligned_data, res, 16) ? "FAIL" : "PASS");
    }

    // 测试2: 未对齐内存访问
    {
        uint8_t unaligned_data[20] = {
            0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
            0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
            0x11, 0x22, 0x33, 0x44
        };
        
        printf("\nTest 2: Unaligned memory access (128-bit)\n");
        printf("Input:  ");
        for(int i=1; i<17; i++) printf("%02X ", unaligned_data[i]);
        
        __m128i result;
        __asm__ __volatile__(
            "vlddqu %1, %0\n\t"
            : "=x"(result)
            : "m"(*(__m128i*)(unaligned_data+1))
            : "memory"
        );
        
        uint8_t* res = (uint8_t*)&result;
        printf("\nResult: ");
        for(int i=0; i<16; i++) printf("%02X ", res[i]);
        printf("\nStatus: %s\n", 
            memcmp(unaligned_data+1, res, 16) ? "FAIL" : "PASS");
    }

    // 测试3: 256位版本
    if(check_avx_support()) {
        ALIGNED(32) uint8_t data256[32] = {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
        };
        
        printf("\nTest 3: 256-bit VLDDQU\n");
        printf("Input:  ");
        for(int i=0; i<32; i++) printf("%02X ", data256[i]);
        
        __m256i result256;
        __asm__ __volatile__(
            "vlddqu %1, %0\n\t"
            : "=x"(result256)
            : "m"(*(__m256i*)data256)
            : "memory"
        );
        
        uint8_t* res256 = (uint8_t*)&result256;
        printf("\nResult: ");
        for(int i=0; i<32; i++) printf("%02X ", res256[i]);
        printf("\nStatus: %s\n", 
            memcmp(data256, res256, 32) ? "FAIL" : "PASS");
    } else {
        printf("\nTest 3: 256-bit VLDDQU - SKIPPED (AVX not supported)\n");
    }
}

#include <signal.h>
#include <setjmp.h>

static jmp_buf jmp_env;

static void sigsegv_handler(int sig) {
    (void)sig; // 显式标记未使用参数
    printf("Caught SIGSEGV, AVX instruction may not be supported properly\n");
    longjmp(jmp_env, 1);
}

int main() {
    signal(SIGSEGV, sigsegv_handler);
    
    printf("=== AVX Support Check ===\n");
    if (!check_avx_support()) {
        printf("ERROR: CPU does not support AVX instructions\n");
        return 1;
    }
    printf("AVX supported\n");

    if (setjmp(jmp_env)) {
        printf("ERROR: SIGSEGV caught\n");
        return 1;
    }

    printf("\n=== Basic VLDDQU Test ===\n");
    volatile uint8_t data[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                               0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    
    printf("Testing basic VLDDQU...\n");
    volatile __m128i result;
    __asm__ __volatile__(
        "vlddqu %1, %0\n\t"
        : "=x"(result)
        : "m"(*(volatile __m128i*)data)
        : "memory"
    );
    
    // 打印结果
    uint8_t* res = (uint8_t*)&result;
    printf("Result: ");
    for(int i=0; i<16; i++) {
        printf("%02X ", res[i]);
    }
    printf("\nTest %s\n", memcmp((const void*)data, (const void*)res, 16) == 0 ? "PASSED" : "FAILED");

    // 运行扩展测试
    test_vlddqu();
    
    return 0;
}
