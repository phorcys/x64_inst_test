#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// VPBROADCASTB指令测试
void test_vpbroadcastb() {
    printf("=== Testing VPBROADCASTB ===\n");
    
    // 测试数据
    uint8_t test_values[] = {0x00, 0xFF, 0x7F, 0x80};
    const char* value_names[] = {"0x00", "0xFF", "0x7F", "0x80"};
    
    for (size_t i = 0; i < sizeof(test_values)/sizeof(test_values[0]); i++) {
        uint8_t val = test_values[i];
        printf("\nTesting value: %s\n", value_names[i]);
        __m128i val128 = _mm_set1_epi8(val);
        
        // 测试128位版本
        __m128i xmm;
        asm volatile (
            "vpbroadcastb %1, %0"
            : "=x"(xmm)
            : "m"(val)
        );
        
        printf("XMM result: ");
        print_m128i_hex(xmm, "");

        asm volatile (
            "vmovd %1, %%xmm2\n\t"
            "vpbroadcastb %%xmm2, %0\n\t"
            : "=x"(xmm)
            : "m"(val)
            : "xmm2"
        );
        
        printf("XMM Reg result: ");
        print_m128i_hex(xmm, "");

        // 测试256位版本
        __m256i ymm;
        asm volatile (
            "vpbroadcastb %1, %0"
            : "=x"(ymm)
            : "m"(val128)
        );
        
        printf("YMM result: ");
        print_m256i_hex(ymm, "");
        
        // 测试256位版本
        asm volatile (
            "vmovd %1, %%xmm2\n\t"
            "vpbroadcastb %%xmm2, %0"
            : "=x"(ymm)
            : "m"(val128)
            : "xmm2"
        );
        
        printf("YMM REG result: ");
        print_m256i_hex(ymm, "");

        // 输出MXCSR状态
        //print_mxcsr(get_mxcsr());
    }
}

int main() {
    test_vpbroadcastb();
    return 0;
}
