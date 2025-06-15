#include "avx.h"
#include <stdio.h>
#include <stdint.h>

void test_vpbroadcastw() {
    printf("=== Testing VPBROADCASTW ===\n");
    
    // 测试数据
    uint16_t src = 0xABCD;
    ALIGNED(16) uint16_t dst128[8];
    ALIGNED(32) uint16_t dst256[16];
    
    // 128位测试
    ALIGNED(2) uint16_t src_mem = src;
    __m128i xmm;
    asm volatile (
        "vpbroadcastw %1, %0"
        : "=x"(xmm)
        : "m"(src_mem)
    );
    _mm_store_si128((__m128i*)dst128, xmm);
    
    printf("Test1 - 128bit broadcast:\n");
    printf("Expected: ");
    for(int i=0; i<8; i++) printf("ABCD ");
    printf("\nResult  : ");
    for(int i=0; i<8; i++) printf("%04x ", dst128[i]);
    printf("\nMatch: %s\n\n", 
        (dst128[0] == src && dst128[1] == src && 
         dst128[2] == src && dst128[3] == src &&
         dst128[4] == src && dst128[5] == src &&
         dst128[6] == src && dst128[7] == src) ? "YES" : "NO");

    // 256位测试
    __m256i ymm;
    asm volatile (
        "vpbroadcastw %1, %0"
        : "=x"(ymm)
        : "m"(src_mem)
    );
    _mm256_store_si256((__m256i*)dst256, ymm);
    
    printf("Test2 - 256bit broadcast:\n");
    printf("Expected: ");
    for(int i=0; i<16; i++) printf("ABCD ");
    printf("\nResult  : ");
    for(int i=0; i<16; i++) printf("%04x ", dst256[i]);
    printf("\nMatch: %s\n\n", 
        (dst256[0] == src && dst256[1] == src && 
         dst256[2] == src && dst256[3] == src &&
         dst256[4] == src && dst256[5] == src &&
         dst256[6] == src && dst256[7] == src &&
         dst256[8] == src && dst256[9] == src &&
         dst256[10] == src && dst256[11] == src &&
         dst256[12] == src && dst256[13] == src &&
         dst256[14] == src && dst256[15] == src) ? "YES" : "NO");

    // 边界值测试
    uint16_t edge_cases[] = {0, 0xFFFF, 0x8000, 0x7FFF};
    for(int i=0; i<4; i++) {
        ALIGNED(2) uint16_t edge_src = edge_cases[i];
        __m128i xmm_edge;
        asm volatile (
            "vpbroadcastw %1, %0"
            : "=x"(xmm_edge)
            : "m"(edge_src)
        );
        _mm_store_si128((__m128i*)dst128, xmm_edge);
        
        printf("Test%d - Edge case 0x%04x:\n", i+3, edge_src);
        printf("Result  : ");
        for(int j=0; j<8; j++) printf("%04x ", dst128[j]);
        printf("\nMatch: %s\n\n", 
            (dst128[0] == edge_src && dst128[1] == edge_src && 
             dst128[2] == edge_src && dst128[3] == edge_src &&
             dst128[4] == edge_src && dst128[5] == edge_src &&
             dst128[6] == edge_src && dst128[7] == edge_src) ? "YES" : "NO");
    }
}

int main() {
    test_vpbroadcastw();
    return 0;
}
