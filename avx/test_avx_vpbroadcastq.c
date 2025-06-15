#include "avx.h"
#include <stdio.h>
#include <stdint.h>

void test_vpbroadcastq() {
    printf("=== Testing VPBROADCASTQ ===\n");
    
    // 测试数据
    uint64_t src = 0x123456789ABCDEF0;
    ALIGNED(16) uint64_t dst128[2];
    ALIGNED(32) uint64_t dst256[4];
    
    // 128位测试
    ALIGNED(8) uint64_t src_mem = src;
    __m128i xmm;
    asm volatile (
        "vpbroadcastq %1, %0"
        : "=x"(xmm)
        : "m"(src_mem)
    );
    _mm_store_si128((__m128i*)dst128, xmm);
    
    printf("Test1 - 128bit broadcast:\n");
    printf("Expected: ");
    for(int i=0; i<2; i++) printf("123456789ABCDEF0 ");
    printf("\nResult  : ");
    for(int i=0; i<2; i++) printf("%016lx ", dst128[i]);
    printf("\nMatch: %s\n\n", 
        (dst128[0] == src && dst128[1] == src) ? "YES" : "NO");

    // 256位测试
    __m256i ymm;
    asm volatile (
        "vpbroadcastq %1, %0"
        : "=x"(ymm)
        : "m"(src_mem)
    );
    _mm256_store_si256((__m256i*)dst256, ymm);
    
    printf("Test2 - 256bit broadcast:\n");
    printf("Expected: ");
    for(int i=0; i<4; i++) printf("123456789ABCDEF0 ");
    printf("\nResult  : ");
    for(int i=0; i<4; i++) printf("%016lx ", dst256[i]);
    printf("\nMatch: %s\n\n", 
        (dst256[0] == src && dst256[1] == src &&
         dst256[2] == src && dst256[3] == src) ? "YES" : "NO");

    // 边界值测试
    uint64_t edge_cases[] = {0, 0xFFFFFFFFFFFFFFFF, 0x8000000000000000, 0x7FFFFFFFFFFFFFFF};
    for(int i=0; i<4; i++) {
        ALIGNED(8) uint64_t edge_src = edge_cases[i];
        __m128i xmm_edge;
        asm volatile (
            "vpbroadcastq %1, %0"
            : "=x"(xmm_edge)
            : "m"(edge_src)
        );
        _mm_store_si128((__m128i*)dst128, xmm_edge);
        
        printf("Test%d - Edge case 0x%016lx:\n", i+3, edge_src);
        printf("Result  : ");
        for(int j=0; j<2; j++) printf("%016lx ", dst128[j]);
        printf("\nMatch: %s\n\n", 
            (dst128[0] == edge_src && dst128[1] == edge_src) ? "YES" : "NO");
    }
}

int main() {
    test_vpbroadcastq();
    return 0;
}
