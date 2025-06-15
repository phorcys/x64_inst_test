#include "avx.h"
#include <stdio.h>
#include <stdint.h>

void test_vpbroadcastb() {
    printf("=== Testing VPBROADCASTB ===\n");
    
    // 测试数据
    uint8_t src = 0xAB;
    ALIGNED(16) uint8_t dst128[16];
    ALIGNED(32) uint8_t dst256[32];
    
    // 128位测试
    ALIGNED(1) uint8_t src_mem = src;
    __m128i xmm;
    asm volatile (
        "vpbroadcastb %1, %0"
        : "=x"(xmm)
        : "m"(src_mem)
    );
    _mm_store_si128((__m128i*)dst128, xmm);
    
    printf("Test1 - 128bit broadcast:\n");
    printf("Expected: ");
    for(int i=0; i<16; i++) printf("ab ");
    printf("\nResult  : ");
    for(int i=0; i<16; i++) printf("%02x ", dst128[i]);
    printf("\nMatch: %s\n\n", memcmp(dst128, "\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB", 16) == 0 ? "YES" : "NO");

    // 256位测试
    __m256i ymm;
    asm volatile (
        "vpbroadcastb %1, %0"
        : "=x"(ymm)
        : "m"(src_mem)
    );
    _mm256_store_si256((__m256i*)dst256, ymm);
    
    printf("Test2 - 256bit broadcast:\n");
    printf("Expected: ");
    for(int i=0; i<32; i++) printf("ab ");
    printf("\nResult  : ");
    for(int i=0; i<32; i++) printf("%02x ", dst256[i]);
    printf("\nMatch: %s\n\n", memcmp(dst256, "\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB\xAB", 32) == 0 ? "YES" : "NO");

    // 内存操作测试
    ALIGNED(1) uint8_t mem_src = 0xCD;
    __m128i xmm_mem;
    asm volatile (
        "vpbroadcastb (%1), %0"
        : "=x"(xmm_mem)
        : "r"(&mem_src)
    );
    _mm_store_si128((__m128i*)dst128, xmm_mem);
    
    printf("Test3 - Memory broadcast:\n");
    printf("Expected: ");
    for(int i=0; i<16; i++) printf("cd ");
    printf("\nResult  : ");
    for(int i=0; i<16; i++) printf("%02x ", dst128[i]);
    printf("\nMatch: %s\n\n", memcmp(dst128, "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD", 16) == 0 ? "YES" : "NO");
}

int main() {
    test_vpbroadcastb();
    return 0;
}
