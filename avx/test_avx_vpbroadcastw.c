#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// VPBROADCASTW指令测试
void test_vpbroadcastw() {
    printf("=== Testing VPBROADCASTW ===\n");
    
    // 测试数据
    uint16_t test_values[] = {0x0000, 0xFFFF, 0x7FFF, 0x8000};
    const char* value_names[] = {"0x0000", "0xFFFF", "0x7FFF", "0x8000"};
    
    for (size_t i = 0; i < sizeof(test_values)/sizeof(test_values[0]); i++) {
        uint16_t val = test_values[i];
        printf("\nTesting value: %s\n", value_names[i]);
        
        // 测试128位版本
        __m128i xmm;
        asm volatile (
            "vpbroadcastw %1, %0"
            : "=x"(xmm)
            : "m"(val)
        );
        
        printf("XMM result: ");
        print_m128i_hex(xmm, "");

        asm volatile (
            "vmovd %1, %%xmm2\n\t"
            "vpbroadcastw %1, %0"
            : "=x"(xmm)
            : "m"(val)
            : "xmm2"
        );
        
        printf("XMM REG result: ");
        print_m128i_hex(xmm, "");
        
        // 测试256位版本
        __m256i ymm;
        asm volatile (
            "vpbroadcastw %1, %0"
            : "=x"(ymm)
            : "m"(val)
        );
        
        printf("YMM result: ");
        print_m256i_hex(ymm, "");
        
        asm volatile (
            "vmovd %1, %%xmm2\n\t"
            "vpbroadcastw %1, %0"
            : "=x"(ymm)
            : "m"(val)
            : "xmm2"
        );
        
        printf("YMM REG result: ");
        print_m256i_hex(ymm, "");
        

        // 输出MXCSR状态
        //print_mxcsr(get_mxcsr());
    }
}

int main() {
    test_vpbroadcastw();
    return 0;
}
