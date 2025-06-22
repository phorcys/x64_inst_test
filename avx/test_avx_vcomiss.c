#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>

// 测试单个比较操作
static int test_case(const char *name, float a, float b, int exp_zf, int exp_pf, int exp_cf) {
    // 保存结果标志位
    int zf, pf, cf;
    int of, sf, af;
    
    // 使用内联汇编执行 vcomiss 并获取标志位
    uint64_t flags;
    asm volatile(
        "vcomiss %2, %1\n\t"   // 执行比较
        "pushfq\n\t"            // 保存标志寄存器
        "popq %0\n\t"           // 弹出到flags
        : "=r"(flags)
        : "x"(_mm_set_ss(a)), "x"(_mm_set_ss(b))
        : "cc"
    );
    
    // 提取标志位
    zf = (flags >> 6) & 1;   // ZF 在 bit 6
    pf = (flags >> 2) & 1;   // PF 在 bit 2
    cf = (flags >> 0) & 1;   // CF 在 bit 0
    of = (flags >> 11) & 1;  // OF 在 bit 11
    sf = (flags >> 7) & 1;   // SF 在 bit 7
    af = (flags >> 4) & 1;   // AF 在 bit 4
    
    // 打印输入和输出
    printf("\nTest: %s\n", name);
    printf("Operand A: %a (%f)\n", a, a);
    printf("Operand B: %a (%f)\n", b, b);
    printf("Expected flags: ZF=%d, PF=%d, CF=%d\n", exp_zf, exp_pf, exp_cf);
    printf("Result flags:   ZF=%d, PF=%d, CF=%d, OF=%d, SF=%d, AF=%d\n", 
           zf, pf, cf, of, sf, af);
    
    // 检查标志位
    int ret = (zf == exp_zf) && (pf == exp_pf) && (cf == exp_cf) &&
              (of == 0) && (sf == 0) && (af == 0); // OF, SF, AF 应为0
    
    printf("Result: %s\n", ret ? "PASS" : "FAIL");
    return ret;
}

// 测试内存操作数
static int test_mem_operand(const char *name, float a, float b, int exp_zf, int exp_pf, int exp_cf) {
    // 保存结果标志位
    int zf, pf, cf;
    int of, sf, af;
    
    // 对齐内存操作数
    ALIGNED(16) float mem_b = b;
    
    // 使用内联汇编执行 vcomiss 并获取标志位
    uint64_t flags;
    asm volatile(
        "vcomiss %2, %1\n\t"   // 执行比较
        "pushfq\n\t"            // 保存标志寄存器
        "popq %0\n\t"           // 弹出到flags
        : "=r"(flags)
        : "x"(_mm_set_ss(a)), "m"(mem_b)
        : "cc"
    );
    
    // 提取标志位
    zf = (flags >> 6) & 1;
    pf = (flags >> 2) & 1;
    cf = (flags >> 0) & 1;
    of = (flags >> 11) & 1;
    sf = (flags >> 7) & 1;
    af = (flags >> 4) & 1;
    
    // 打印输入和输出
    printf("\nTest (mem operand): %s\n", name);
    printf("Operand A: %a (%f)\n", a, a);
    printf("Operand B (mem): %a (%f)\n", b, b);
    printf("Expected flags: ZF=%d, PF=%d, CF=%d\n", exp_zf, exp_pf, exp_cf);
    printf("Result flags:   ZF=%d, PF=%d, CF=%d, OF=%d, SF=%d, AF=%d\n", 
           zf, pf, cf, of, sf, af);
    
    // 检查标志位
    int ret = (zf == exp_zf) && (pf == exp_pf) && (cf == exp_cf) &&
              (of == 0) && (sf == 0) && (af == 0);
    
    printf("Result: %s\n", ret ? "PASS" : "FAIL");
    return ret;
}

int main() {
    int total = 0, passed = 0;
    
    // 定义测试值
    float qnan = NAN;
    // 手动构造一个信号NaN (SNaN) - 指数全1，尾数最高位为0且尾数非零
    float snan;
    uint32_t snan_bits = 0x7f800001;
    memcpy(&snan, &snan_bits, sizeof(snan));
    float inf = INFINITY;
    float neg_inf = -INFINITY;
    float zero = 0.0f;
    float neg_zero = -0.0f;
    
    // 正常数比较
    passed += test_case("1.5 > 1.0", 1.5f, 1.0f, 0,0,0); // 大于
    total++;
    
    passed += test_case("1.0 < 1.5", 1.0f, 1.5f, 0,0,1); // 小于
    total++;
    
    passed += test_case("1.0 == 1.0", 1.0f, 1.0f, 1,0,0); // 相等
    total++;
    
    // 零值比较
    passed += test_case("+0.0 == -0.0", zero, neg_zero, 1,0,0); // 相等
    total++;
    
    // 无穷大比较
    passed += test_case("+Inf == +Inf", inf, inf, 1,0,0); // 相等
    total++;
    
    passed += test_case("+Inf > -Inf", inf, neg_inf, 0,0,0); // 大于
    total++;
    
    passed += test_case("-Inf < +Inf", neg_inf, inf, 0,0,1); // 小于
    total++;
    
    // NaN比较
    passed += test_case("QNaN vs 1.0", qnan, 1.0f, 1,1,1); // 无序
    total++;
    
    passed += test_case("1.0 vs SNaN", 1.0f, snan, 1,1,1); // 无序
    total++;
    
    passed += test_case("QNaN vs QNaN", qnan, qnan, 1,1,1); // 无序
    total++;
    
    // 边界值比较
    passed += test_case("FLT_MAX vs FLT_MIN", FLT_MAX, FLT_MIN, 0,0,0); // 大于
    total++;
    
    passed += test_case("FLT_MIN vs FLT_MAX", FLT_MIN, FLT_MAX, 0,0,1); // 小于
    total++;
    
    // 内存操作数测试
    passed += test_mem_operand("1.5 > 1.0 (mem)", 1.5f, 1.0f, 0,0,0); // 大于
    total++;
    
    passed += test_mem_operand("QNaN vs 1.0 (mem)", qnan, 1.0f, 1,1,1); // 无序
    total++;
    
    // 测试总结
    printf("\nSummary: %d/%d tests passed\n", passed, total);
    return passed == total ? 0 : 1;
}
