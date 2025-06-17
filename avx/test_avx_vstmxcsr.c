#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

void test_avx_vstmxcsr() {
    printf("Testing VSTMXCSR instruction\n");

    // 测试不同的MXCSR值
    uint32_t test_values[] = {
        0x00000000,  // 默认值
        0x00001F80,  // 默认值(所有异常掩码)
        0x00009FC0,  // FTZ + DAZ + 所有异常掩码
        0x00008000,  // FTZ
        0x00004000,  // DAZ
        0x00003F00   // 清除所有异常掩码
    };

    for (size_t i = 0; i < sizeof(test_values)/sizeof(test_values[0]); ++i) {
        uint32_t mxcsr = test_values[i];
        uint32_t stored_mxcsr = 0;
        
        // 设置MXCSR
        set_mxcsr(mxcsr);
        
        // 存储MXCSR到内存
        __asm__ volatile("vstmxcsr %0" : "=m"(stored_mxcsr));
        
        printf("\nTest case %zu:\n", i+1);
        printf("Expected MXCSR: 0x%08X\n", mxcsr & 0xFFFF);
        printf("Stored MXCSR:   0x%08X\n", stored_mxcsr & 0xFFFF);
        
        if ((stored_mxcsr & 0xFFFF) != (mxcsr & 0xFFFF)) {
            printf("ERROR: MXCSR value mismatch!\n");
        }
    }

    printf("\nVSTMXCSR test completed\n");
}

int main() {
    test_avx_vstmxcsr();
    return 0;
}
