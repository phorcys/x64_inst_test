#include "avx.h"
#include <stdio.h>

// 临时调试函数
static void print_xmmd_debug(const char* name, __m128d xmm) {
    double d[2];
    _mm_storeu_pd(d, xmm);
    printf("%s: [%.6f, %.6f]\n", name, d[0], d[1]);
}

static void print_ymmd_debug(const char* name, __m256d ymm) {
    double d[4];
    _mm256_storeu_pd(d, ymm);
    printf("%s: [%.6f, %.6f, %.6f, %.6f]\n", name, d[0], d[1], d[2], d[3]);
}

static void print_control_bits_debug(const char* name, __m128d ctrl) {
    uint64_t *vals = (uint64_t*)&ctrl;
    printf("%s control bits: [%lu, %lu] (hex: 0x%lx, 0x%lx)\n", 
           name, vals[0] & 1, vals[1] & 1, vals[0], vals[1]);
}

static void print_control_bits_ymm_debug(const char* name, __m256d ctrl) {
    uint64_t *vals = (uint64_t*)&ctrl;
    printf("%s control bits: [%lu, %lu, %lu, %lu] (hex: 0x%lx, 0x%lx, 0x%lx, 0x%lx)\n", 
           name, vals[0] & 1, vals[1] & 1, vals[2] & 1, vals[3] & 1,
           vals[0], vals[1], vals[2], vals[3]);
}

// 测试128位立即数版本
static int test_imm_xmm() {
    int errors = 0;
    const char* test_name = "vpermilpd imm8 (xmm)";
    printf("Testing %s\n", test_name);
    
    // 测试数据：各种边界值组合
    double in[2] = {0.0, -0.0};           // +0和-0
    double in2[2] = {1.0, -1.0};          // 正负1
    double in3[2] = {INFINITY, -INFINITY}; // 正负无穷
    double in4[2] = {NAN, 123.456};       // NaN和普通值
    double out[2] = {0};
    
    // 测试所有控制位组合 (2位)
    for (int imm = 0; imm < 4; imm++) {
        // 测试每组输入数据
        double* inputs[] = {in, in2, in3, in4};
        for (int i = 0; i < 4; i++) {
            __m128d src = _mm_loadu_pd(inputs[i]);
            __m128d result;
            
            // 内联汇编执行指令
            switch(imm) {
                case 0: 
                    asm("vpermilpd $0, %1, %0" : "=x"(result) : "x"(src));
                    break;
                case 1: 
                    asm("vpermilpd $1, %1, %0" : "=x"(result) : "x"(src));
                    break;
                case 2: 
                    asm("vpermilpd $2, %1, %0" : "=x"(result) : "x"(src));
                    break;
                case 3: 
                    asm("vpermilpd $3, %1, %0" : "=x"(result) : "x"(src));
                    break;
            }
            
            // 存储结果
            _mm_storeu_pd(out, result);
            
            // 计算预期结果
            double expected[2];
            expected[0] = inputs[i][(imm & 1) ? 1 : 0];  // 低位控制
            expected[1] = inputs[i][(imm & 2) ? 1 : 0];  // 高位控制
            
            // 验证结果
            int valid = 1;
            for (int j = 0; j < 2; j++) {
                // 特殊处理NaN
                if (isnan(expected[j]) && isnan(out[j])) continue;
                // 比较其他值
                if (expected[j] != out[j]) {
                    valid = 0;
                    break;
                }
            }
            
            if (!valid) {
                printf("  Error: imm=%d, input=[%f, %f]\n", imm, inputs[i][0], inputs[i][1]);
                printf("    Expected: [%f, %f]\n", expected[0], expected[1]);
                printf("    Got     : [%f, %f]\n\n", out[0], out[1]);
                errors++;
            }
            printf("\n");
        }
    }
    
    return errors;
}

// 测试128位寄存器控制版本
static int test_reg_xmm() {
    int errors = 0;
    const char* test_name = "vpermilpd reg (xmm)";
    printf("Testing %s\n", test_name);
    
    // 测试数据
    double in[2] = {0.0, -0.0};
    double in2[2] = {1.0, -1.0};
    double in3[2] = {INFINITY, -INFINITY};
    double in4[2] = {NAN, 123.456};
    
    // 控制向量：每个元素的最低1位作为选择位
    uint64_t ctrl0[2] = {0, 0};  // 选择 [0,0]
    uint64_t ctrl1[2] = {1, 0};  // 选择 [1,0]
    uint64_t ctrl2[2] = {0, 1};  // 选择 [0,1]
    uint64_t ctrl3[2] = {1, 1};  // 选择 [1,1]
    uint64_t* ctrls[] = {ctrl0, ctrl1, ctrl2, ctrl3};
    
    for (int c = 0; c < 4; c++) {
        double* inputs[] = {in, in2, in3, in4};
        for (int i = 0; i < 4; i++) {
            __m128d src = _mm_loadu_pd(inputs[i]);
            __m128d result;
            
            // 正确设置控制向量：每个元素只使用最低位
            __m128i ctrl_int = _mm_set_epi64x(0, 0);
            uint64_t* ctrl_ptr = (uint64_t*)&ctrl_int;
            // 确保控制位只使用最低位
            ctrl_ptr[0] = (ctrls[c][0] & 0x1) ? 0xFFFFFFFFFFFFFFFF : 0x0;
            ctrl_ptr[1] = (ctrls[c][1] & 0x1) ? 0xFFFFFFFFFFFFFFFF : 0x0;
            
            // 打印输入和控制向量
            print_xmmd_debug("Input", src);
            print_control_bits_debug("Control", _mm_castsi128_pd(ctrl_int));
            
            // 直接使用向量寄存器传递控制向量
            asm("vpermilpd %1, %2, %0" 
                : "=v"(result) 
                : "v"(_mm_castsi128_pd(ctrl_int)), "v"(src));
            
            // 调试输出
            uint64_t *vals = (uint64_t*)&ctrl_int;
            printf("Control int: [%lu, %lu] (bits: [%lu, %lu])\n", 
                   vals[0], vals[1],
                   vals[0] & 1,
                   vals[1] & 1);
            
            double out[2];
            _mm_storeu_pd(out, result);
            print_xmmd("Output", result);
            
            // 正确计算预期结果：使用控制向量的最低位选择源元素
            double expected[2];
            expected[0] = inputs[i][(ctrls[c][0] & 1)];
            expected[1] = inputs[i][(ctrls[c][1] & 1)];  // 直接使用控制位选择
            
            // 特殊处理-0.0的显示问题
            if (expected[0] == 0.0) expected[0] = 0.0;
            if (expected[1] == 0.0) expected[1] = 0.0;
            if (out[0] == 0.0) out[0] = 0.0;
            if (out[1] == 0.0) out[1] = 0.0;
            
            // 验证结果
            int valid = 1;
            for (int j = 0; j < 2; j++) {
                if (isnan(expected[j]) && isnan(out[j])) continue;
                if (expected[j] != out[j]) {
                    valid = 0;
                    break;
                }
            }
            
            if (!valid) {
                printf("  Error: ctrl=[%lu,%lu], input=[%f, %f]\n", 
                      ctrls[c][0], ctrls[c][1], inputs[i][0], inputs[i][1]);
                printf("    Expected: [%f, %f]\n", expected[0], expected[1]);
                printf("    Got     : [%f, %f]\n\n", out[0], out[1]);
                errors++;
            }
            printf("\n");
        }
    }
    
    return errors;
}

// 测试256位立即数版本
static int test_imm_ymm() {
    int errors = 0;
    const char* test_name = "vpermilpd imm8 (ymm)";
    printf("Testing %s\n", test_name);
    
    // 测试数据：各种边界值组合
    double in[4] = {0.0, -0.0, INFINITY, -INFINITY};
    double in2[4] = {1.0, -1.0, 123.456, NAN};
    double out[4] = {0};
    
    // 测试所有控制位组合 (4位)
    for (int imm = 0; imm < 16; imm++) {
        // 测试每组输入数据
        double* inputs[] = {in, in2};
        for (int i = 0; i < 2; i++) {
            __m256d src = _mm256_loadu_pd(inputs[i]);
            __m256d result;
            
            // 内联汇编执行指令
            switch(imm) {
                case 0: asm("vpermilpd $0, %1, %0" : "=x"(result) : "x"(src)); break;
                case 1: asm("vpermilpd $1, %1, %0" : "=x"(result) : "x"(src)); break;
                case 2: asm("vpermilpd $2, %1, %0" : "=x"(result) : "x"(src)); break;
                case 3: asm("vpermilpd $3, %1, %0" : "=x"(result) : "x"(src)); break;
                case 4: asm("vpermilpd $4, %1, %0" : "=x"(result) : "x"(src)); break;
                case 5: asm("vpermilpd $5, %1, %0" : "=x"(result) : "x"(src)); break;
                case 6: asm("vpermilpd $6, %1, %0" : "=x"(result) : "x"(src)); break;
                case 7: asm("vpermilpd $7, %1, %0" : "=x"(result) : "x"(src)); break;
                case 8: asm("vpermilpd $8, %1, %0" : "=x"(result) : "x"(src)); break;
                case 9: asm("vpermilpd $9, %1, %0" : "=x"(result) : "x"(src)); break;
                case 10: asm("vpermilpd $10, %1, %0" : "=x"(result) : "x"(src)); break;
                case 11: asm("vpermilpd $11, %1, %0" : "=x"(result) : "x"(src)); break;
                case 12: asm("vpermilpd $12, %1, %0" : "=x"(result) : "x"(src)); break;
                case 13: asm("vpermilpd $13, %1, %0" : "=x"(result) : "x"(src)); break;
                case 14: asm("vpermilpd $14, %1, %0" : "=x"(result) : "x"(src)); break;
                case 15: asm("vpermilpd $15, %1, %0" : "=x"(result) : "x"(src)); break;
            }
            
            // 存储结果
            _mm256_storeu_pd(out, result);
            
            // 正确计算256位预期结果：每个通道独立选择
            double expected[4];
            expected[0] = inputs[i][0 + ((imm >> 0) & 1)];
            expected[1] = inputs[i][0 + ((imm >> 1) & 1)];
            expected[2] = inputs[i][2 + ((imm >> 2) & 1)];
            expected[3] = inputs[i][2 + ((imm >> 3) & 1)];
            
            // 特殊处理-0.0的显示问题
            for (int j = 0; j < 4; j++) {
                if (expected[j] == 0.0) expected[j] = 0.0;
                if (out[j] == 0.0) out[j] = 0.0;
            }
            
            // 验证结果
            int valid = 1;
            for (int j = 0; j < 4; j++) {
                // 特殊处理NaN
                if (isnan(expected[j]) && isnan(out[j])) continue;
                // 比较其他值
                if (expected[j] != out[j]) {
                    valid = 0;
                    break;
                }
            }
            
            if (!valid) {
                printf("  Error: imm=%d, input=[%f, %f, %f, %f]\n", 
                      imm, inputs[i][0], inputs[i][1], inputs[i][2], inputs[i][3]);
                printf("    Expected: [%f, %f, %f, %f]\n", 
                      expected[0], expected[1], expected[2], expected[3]);
                printf("    Got     : [%f, %f, %f, %f]\n\n", 
                      out[0], out[1], out[2], out[3]);
                errors++;
            }
            printf("\n");
        }
    }
    
    return errors;
}

// 测试256位寄存器控制版本
static int test_reg_ymm() {
    int errors = 0;
    const char* test_name = "vpermilpd reg (ymm)";
    printf("Testing %s\n", test_name);
    
    // 测试数据
    double in[4] = {0.0, -0.0, INFINITY, -INFINITY};
    double in2[4] = {1.0, -1.0, 123.456, NAN};
    
    // 控制向量：每个元素的最低1位作为选择位
    uint64_t ctrl0[4] = {0, 0, 0, 0};  // 选择 [0,0,0,0]
    uint64_t ctrl1[4] = {1, 0, 1, 0};  // 选择 [1,0,1,0]
    uint64_t ctrl2[4] = {0, 1, 0, 1};  // 选择 [0,1,0,1]
    uint64_t ctrl3[4] = {1, 1, 1, 1};  // 选择 [1,1,1,1]
    uint64_t* ctrls[] = {ctrl0, ctrl1, ctrl2, ctrl3};
    
    for (int c = 0; c < 4; c++) {
        double* inputs[] = {in, in2};
        for (int i = 0; i < 2; i++) {
            __m256d src = _mm256_loadu_pd(inputs[i]);
            __m256d result;
            
            // 正确设置控制向量：每个元素只使用最低位
            __m256i ctrl_int = _mm256_setzero_si256();
            uint64_t* ctrl_ptr = (uint64_t*)&ctrl_int;
            // 确保控制位只使用最低位
            ctrl_ptr[0] = (ctrls[c][0] & 0x1) ? 0xFFFFFFFFFFFFFFFF : 0x0;
            ctrl_ptr[1] = (ctrls[c][1] & 0x1) ? 0xFFFFFFFFFFFFFFFF : 0x0;
            ctrl_ptr[2] = (ctrls[c][2] & 0x1) ? 0xFFFFFFFFFFFFFFFF : 0x0;
            ctrl_ptr[3] = (ctrls[c][3] & 0x1) ? 0xFFFFFFFFFFFFFFFF : 0x0;
            
            // 打印输入和控制向量
            print_ymmd_debug("Input", src);
            print_control_bits_ymm_debug("Control", _mm256_castsi256_pd(ctrl_int));
            
            // 直接使用向量寄存器传递控制向量
            asm("vpermilpd %1, %2, %0" 
                : "=v"(result) 
                : "v"(_mm256_castsi256_pd(ctrl_int)), "v"(src));
            
            // 调试输出
            uint64_t *vals = (uint64_t*)&ctrl_int;
            printf("Control int: [%lu, %lu, %lu, %lu] (bits: [%lu, %lu, %lu, %lu])\n", 
                   vals[0], vals[1], vals[2], vals[3],
                   vals[0] & 1,
                   vals[1] & 1,
                   vals[2] & 1,
                   vals[3] & 1);
            
            double out[4];
            _mm256_storeu_pd(out, result);
            print_ymmd("Output", result);
            
            // 正确计算256位预期结果：每个128位通道独立处理
            double expected[4];
            // 低128位通道
            expected[0] = inputs[i][(ctrls[c][0] & 1) ? 1 : 0];
            expected[1] = inputs[i][(ctrls[c][1] & 1) ? 1 : 0];
            // 高128位通道
            expected[2] = inputs[i][(ctrls[c][2] & 1) ? 3 : 2];
            expected[3] = inputs[i][(ctrls[c][3] & 1) ? 3 : 2];
            
            // 特殊处理-0.0的显示问题
            for (int j = 0; j < 4; j++) {
                if (expected[j] == 0.0) expected[j] = 0.0;
                if (out[j] == 0.0) out[j] = 0.0;
            }
            
            // 验证结果
            int valid = 1;
            for (int j = 0; j < 4; j++) {
                if (isnan(expected[j]) && isnan(out[j])) continue;
                if (expected[j] != out[j]) {
                    valid = 0;
                    break;
                }
            }
            
            if (!valid) {
                printf("  Error: ctrl=[%lu,%lu,%lu,%lu], input=[%f, %f, %f, %f]\n", 
                      ctrls[c][0], ctrls[c][1], ctrls[c][2], ctrls[c][3],
                      inputs[i][0], inputs[i][1], inputs[i][2], inputs[i][3]);
                printf("    Expected: [%f, %f, %f, %f]\n", 
                      expected[0], expected[1], expected[2], expected[3]);
                printf("    Got     : [%f, %f, %f, %f]\n\n", 
                      out[0], out[1], out[2], out[3]);
                errors++;
            }
            printf("\n");
        }
    }
    
    return errors;
}

int main() {
    int total_errors = 0;
    
    total_errors += test_imm_xmm();
    total_errors += test_reg_xmm();
    total_errors += test_imm_ymm();
    total_errors += test_reg_ymm();
    
    if (total_errors) {
        printf("Tests failed with %d errors\n", total_errors);
        return 1;
    } else {
        printf("All tests passed!\n");
        return 0;
    }
}
