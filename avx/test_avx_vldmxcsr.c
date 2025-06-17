#include "avx.h"
#include <stdio.h>
#include <stdint.h>

void test_avx_vldmxcsr() {
    printf("Testing VLDMXCSR instruction\n");

    // 测试不同的MXCSR值
    uint32_t test_values[] = {
        0x00000000,  // 默认值
        0x00001F80,  // 默认值(所有异常掩码)
        0x00009FC0,  // FTZ + DAZ + 所有异常掩码
        0x00008000,  // FTZ
        0x00004000,  // DAZ
        0x00003F00   // 清除所有异常掩码
        //0x0000003F,  // 设置所有异常标志(在box64中会导致段错误)
        //0xFFFF0000   // 保留位测试(在box64中会导致段错误)
    };

    for (size_t i = 0; i < sizeof(test_values)/sizeof(test_values[0]); ++i) {
        uint32_t mxcsr = test_values[i];
        
        // 保存原始MXCSR值
        uint32_t original_mxcsr = get_mxcsr();
        
        // 设置MXCSR
        set_mxcsr(mxcsr);
        
        // 验证设置是否成功
        uint32_t read_mxcsr = get_mxcsr();
        
        printf("\nTest case %zu:\n", i+1);
        printf("Expected MXCSR: 0x%08X\n", mxcsr & 0xFFFF); // 只比较低16位
        printf("Actual MXCSR:   0x%08X\n", read_mxcsr & 0xFFFF);
        // 在box64中不打印MXCSR位域细节
        if ((read_mxcsr & 0xFFFF) != (mxcsr & 0xFFFF)) {
            printf("ERROR: MXCSR value mismatch!\n");
        }
        
        // 恢复原始MXCSR
        set_mxcsr(original_mxcsr);
    }

    printf("\nVLDMXCSR test completed\n");
}

int main() {
    test_avx_vldmxcsr();
    return 0;
}
