#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 测试EMMS指令
void test_emms() {
    uint64_t before = 0, after = 0;
    uint16_t fpu_status = 0;
    
    printf("=== Testing EMMS instruction ===\n");
    
    // 1. 设置MMX状态
    uint64_t mm0_val = 0x1122334455667788;
    uint64_t mm1_val = 0x99AABBCCDDEEFF00;
    asm volatile (
        "movq %0, %%mm0\n\t"
        "movq %1, %%mm1\n\t"
        : : "m"(mm0_val), "m"(mm1_val) : "mm0", "mm1"
    );
    
    // 获取FPU状态字
    asm volatile ("fnstsw %0" : "=m"(fpu_status));
    printf("FPU status before EMMS: 0x%04X\n", fpu_status);
    
    // 2. 执行EMMS
    asm volatile ("emms");
    
    // 再次获取FPU状态字
    asm volatile ("fnstsw %0" : "=m"(fpu_status));
    printf("FPU status after EMMS: 0x%04X\n", fpu_status);
    
    // 3. 验证可以正常使用FPU
    float f1 = 1.0, f2 = 2.0, result;
    asm volatile (
        "flds %1\n\t"
        "flds %2\n\t"
        "faddp\n\t"
        "fstps %0\n\t"
        : "=m"(result)
        : "m"(f1), "m"(f2)
    );
    printf("FPU test after EMMS: %f + %f = %f\n", f1, f2, result);
    
    printf("EMMS test %s\n", (fpu_status & 0x3800) == 0 ? "PASSED" : "FAILED");
}

int main() {
    test_emms();
    return 0;
}
