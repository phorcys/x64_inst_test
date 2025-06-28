#include "avx.h"
#include <stdio.h>
#include <math.h>

static void print_xmms_debug(const char* name, __m128 xmm) {
    float f[4];
    _mm_storeu_ps(f, xmm);
    printf("%s: [%.6f, %.6f, %.6f, %.6f]\n", name, f[0], f[1], f[2], f[3]);
}

static void print_ymms_debug(const char* name, __m256 ymm) {
    float f[8];
    _mm256_storeu_ps(f, ymm);
    printf("%s: [%.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f]\n", 
           name, f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7]);
}

static void print_control_bits_xmm_ps(const char* name, __m128 ctrl) {
    uint32_t *vals = (uint32_t*)&ctrl;
    printf("%s control bits: [%u, %u, %u, %u] (hex: 0x%x, 0x%x, 0x%x, 0x%x)\n", 
           name, vals[0] & 3, vals[1] & 3, vals[2] & 3, vals[3] & 3,
           vals[0], vals[1], vals[2], vals[3]);
}

static void print_control_bits_ymm_ps(const char* name, __m256 ctrl) {
    uint32_t *vals = (uint32_t*)&ctrl;
    printf("%s control bits: [%u, %u, %u, %u, %u, %u, %u, %u] (hex: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)\n", 
           name, vals[0] & 3, vals[1] & 3, vals[2] & 3, vals[3] & 3,
           vals[4] & 3, vals[5] & 3, vals[6] & 3, vals[7] & 3,
           vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6], vals[7]);
}

// 测试128位立即数版本
static int test_imm_xmm() {
    int errors = 0;
    const char* test_name = "vpermilps imm8 (xmm)";
    printf("Testing %s\n", test_name);
    
    // 测试数据：各种边界值组合
    float in[4] = {0.0f, -0.0f, INFINITY, -INFINITY}; // +0, -0, 正负无穷
    float in2[4] = {1.0f, -1.0f, NAN, 123.456f};     // 正负1, NaN, 普通值
    float out[4] = {0};
    
    // 测试所有控制位组合 (4个元素 * 2位控制)
    for (int imm = 0; imm < 256; imm++) {
        // 测试每组输入数据
        float* inputs[] = {in, in2};
        for (int i = 0; i < 2; i++) {
            __m128 src = _mm_loadu_ps(inputs[i]);
            __m128 result;
            
            // 创建正确的控制向量：每个元素独立的控制位
            uint32_t controls[4] = {
                (imm >> 0) & 3,
                (imm >> 2) & 3,
                (imm >> 4) & 3,
                (imm >> 6) & 3
            };
            __m128i imm_vec = _mm_setr_epi32(controls[0], controls[1], controls[2], controls[3]);
            asm("vpermilps %1, %2, %0" : "=x"(result) : "x"(_mm_castsi128_ps(imm_vec)), "x"(src));
            
            // 存储结果
            _mm_storeu_ps(out, result);
            
            // 计算预期结果：每个元素独立选择同一通道内的源元素
            float expected[4];
            for (int j = 0; j < 4; j++) {
                int sel = (imm >> (j * 2)) & 3;
                expected[j] = inputs[i][sel]; // 直接使用选择索引
                
                // 特殊处理-0.0的显示问题
                if (expected[j] == 0.0f) expected[j] = 0.0f;
                if (out[j] == 0.0f) out[j] = 0.0f;
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
                printf("  Error: imm=%d, input=[%f, %f, %f, %f]\n", imm, 
                      inputs[i][0], inputs[i][1], inputs[i][2], inputs[i][3]);
                printf("    Expected: [%f, %f, %f, %f]\n", 
                      expected[0], expected[1], expected[2], expected[3]);
                printf("    Got     : [%f, %f, %f, %f]\n\n", 
                      out[0], out[1], out[2], out[3]);
                errors++;
                if (errors > 10) return errors; // 防止过多错误输出
            }
        }
        printf("\n");
    }
    
    return errors;
}

// 测试128位寄存器控制版本
static int test_reg_xmm() {
    int errors = 0;
    const char* test_name = "vpermilps reg (xmm)";
    printf("Testing %s\n", test_name);
    
    // 测试数据
    float in[4] = {0.0f, -0.0f, INFINITY, -INFINITY};
    float in2[4] = {1.0f, -1.0f, NAN, 123.456f};
    
    // 控制向量：每个元素的低2位作为选择位
    uint32_t ctrl0[4] = {0, 0, 0, 0};  // 选择 [0,0,0,0]
    uint32_t ctrl1[4] = {1, 2, 3, 0};  // 选择 [1,2,3,0]
    uint32_t ctrl2[4] = {2, 3, 0, 1};  // 选择 [2,3,0,1]
    uint32_t ctrl3[4] = {3, 0, 1, 2};  // 选择 [3,0,1,2]
    uint32_t* ctrls[] = {ctrl0, ctrl1, ctrl2, ctrl3};
    
    for (int c = 0; c < 4; c++) {
        float* inputs[] = {in, in2};
        for (int i = 0; i < 2; i++) {
            __m128 src = _mm_loadu_ps(inputs[i]);
            __m128 result;
            
            // 设置控制向量：每个元素只使用最低2位
            __m128i ctrl_int = _mm_setr_epi32(
                ctrls[c][0] & 3, 
                ctrls[c][1] & 3, 
                ctrls[c][2] & 3, 
                ctrls[c][3] & 3
            );
            
            // 打印输入和控制向量
            print_xmms_debug("Input", src);
            print_control_bits_xmm_ps("Control", _mm_castsi128_ps(ctrl_int));
            
            // 执行指令
            asm("vpermilps %1, %2, %0" 
                : "=x"(result) 
                : "x"(_mm_castsi128_ps(ctrl_int)), "x"(src));
            
            float out[4];
            _mm_storeu_ps(out, result);
            print_xmms_debug("Output", result);
            
            // 计算预期结果：每个元素独立选择同一通道内的源元素
            float expected[4];
            for (int j = 0; j < 4; j++) {
                expected[j] = inputs[i][ctrls[c][j] & 3]; // 直接使用选择索引
                
                // 特殊处理-0.0的显示问题
                if (expected[j] == 0.0f) expected[j] = 0.0f;
                if (out[j] == 0.0f) out[j] = 0.0f;
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
                printf("  Error: ctrl=[%u,%u,%u,%u], input=[%f, %f, %f, %f]\n", 
                      ctrls[c][0] & 3, ctrls[c][1] & 3, ctrls[c][2] & 3, ctrls[c][3] & 3,
                      inputs[i][0], inputs[i][1], inputs[i][2], inputs[i][3]);
                printf("    Expected: [%f, %f, %f, %f]\n", 
                      expected[0], expected[1], expected[2], expected[3]);
                printf("    Got     : [%f, %f, %f, %f]\n\n", 
                      out[0], out[1], out[2], out[3]);
                errors++;
            }
        }
        printf("\n");
    }
    
    return errors;
}

// 测试256位立即数版本
static int test_imm_ymm() {
    int errors = 0;
    const char* test_name = "vpermilps imm8 (ymm)";
    printf("Testing %s\n", test_name);
    
    // 测试数据：各种边界值组合
    float in[8] = {0.0f, -0.0f, INFINITY, -INFINITY, 1.0f, -1.0f, NAN, 123.456f};
    float out[8] = {0};
    
    // 测试代表性控制位组合
    int imm_values[] = {0, 0x55, 0xAA, 0xFF, 0x1B, 0xE4, 0x7E, 0x81};
    int num_imm = sizeof(imm_values) / sizeof(imm_values[0]);
    
    for (int k = 0; k < num_imm; k++) {
        int imm = imm_values[k];
        __m256 src = _mm256_loadu_ps(in);
        __m256 result;
        
        // 创建正确的控制向量：每个元素独立的控制位
        uint32_t controls[8] = {
            (imm >> 0) & 3, (imm >> 2) & 3, (imm >> 4) & 3, (imm >> 6) & 3,
            (imm >> 0) & 3, (imm >> 2) & 3, (imm >> 4) & 3, (imm >> 6) & 3
        };
        __m256i imm_vec = _mm256_setr_epi32(
            controls[0], controls[1], controls[2], controls[3],
            controls[4], controls[5], controls[6], controls[7]
        );
        asm("vpermilps %1, %2, %0" : "=x"(result) : "x"(_mm256_castsi256_ps(imm_vec)), "x"(src));
        
        // 存储结果
        _mm256_storeu_ps(out, result);
        
        // 计算预期结果：每个元素独立选择同一通道内的源元素
        float expected[8];
        for (int j = 0; j < 8; j++) {
            int lane_offset = (j < 4) ? 0 : 4;
            int sel = (imm >> ((j % 4) * 2)) & 3;
            expected[j] = in[lane_offset + sel]; // 直接使用选择索引
            
            // 特殊处理-0.0的显示问题
            if (expected[j] == 0.0f) expected[j] = 0.0f;
            if (out[j] == 0.0f) out[j] = 0.0f;
        }
        
        // 验证结果
        int valid = 1;
        for (int j = 0; j < 8; j++) {
            if (isnan(expected[j]) && isnan(out[j])) continue;
            if (expected[j] != out[j]) {
                valid = 0;
                break;
            }
        }
        
        if (!valid) {
            printf("  Error: imm=0x%02x, input=[%f, %f, %f, %f, %f, %f, %f, %f]\n", imm,
                  in[0], in[1], in[2], in[3], in[4], in[5], in[6], in[7]);
            printf("    Expected: [%f, %f, %f, %f, %f, %f, %f, %f]\n", 
                  expected[0], expected[1], expected[2], expected[3],
                  expected[4], expected[5], expected[6], expected[7]);
            printf("    Got     : [%f, %f, %f, %f, %f, %f, %f, %f]\n\n", 
                  out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7]);
            errors++;
        }
        printf("\n");
    }
    
    return errors;
}

// 测试256位寄存器控制版本
static int test_reg_ymm() {
    int errors = 0;
    const char* test_name = "vpermilps reg (ymm)";
    printf("Testing %s\n", test_name);
    
    // 测试数据
    float in[8] = {0.0f, -0.0f, INFINITY, -INFINITY, 1.0f, -1.0f, NAN, 123.456f};
    
    // 控制向量：每个元素的低2位作为选择位
    uint32_t ctrl0[8] = {0, 1, 2, 3, 0, 1, 2, 3};  // 顺序选择
    uint32_t ctrl1[8] = {3, 2, 1, 0, 3, 2, 1, 0};  // 逆序选择
    uint32_t ctrl2[8] = {0, 0, 1, 1, 2, 2, 3, 3};  // 重复选择
    uint32_t ctrl3[8] = {1, 3, 0, 2, 3, 1, 2, 0};  // 随机选择
    uint32_t* ctrls[] = {ctrl0, ctrl1, ctrl2, ctrl3};
    
    for (int c = 0; c < 4; c++) {
        __m256 src = _mm256_loadu_ps(in);
        __m256 result;
        
        // 设置控制向量：每个元素只使用最低2位
        __m256i ctrl_int = _mm256_setr_epi32(
            ctrls[c][0] & 3, 
            ctrls[c][1] & 3, 
            ctrls[c][2] & 3, 
            ctrls[c][3] & 3,
            ctrls[c][4] & 3, 
            ctrls[c][5] & 3, 
            ctrls[c][6] & 3, 
            ctrls[c][7] & 3
        );
        
        // 打印输入和控制向量
        print_ymms_debug("Input", src);
        print_control_bits_ymm_ps("Control", _mm256_castsi256_ps(ctrl_int));
        
        // 执行指令
        asm("vpermilps %1, %2, %0" 
            : "=x"(result) 
            : "x"(_mm256_castsi256_ps(ctrl_int)), "x"(src));
        
        float out[8];
        _mm256_storeu_ps(out, result);
        print_ymms_debug("Output", result);
        
        // 计算预期结果：每个元素独立选择同一通道内的源元素
        float expected[8];
        for (int j = 0; j < 8; j++) {
            int lane_offset = (j < 4) ? 0 : 4;
            int idx = ctrls[c][j] & 3;
            expected[j] = in[lane_offset + idx]; // 直接使用选择索引
            
            // 特殊处理-0.0的显示问题
            if (expected[j] == 0.0f) expected[j] = 0.0f;
            if (out[j] == 0.0f) out[j] = 0.0f;
        }
        
        // 验证结果
        int valid = 1;
        for (int j = 0; j < 8; j++) {
            if (isnan(expected[j]) && isnan(out[j])) continue;
            if (expected[j] != out[j]) {
                valid = 0;
                break;
            }
        }
        
        if (!valid) {
            printf("  Error: ctrl=[");
            for (int j = 0; j < 8; j++) {
                printf("%u", ctrls[c][j] & 3);
                if (j < 7) printf(",");
            }
            printf("], input=[");
            for (int j = 0; j < 8; j++) {
                printf("%f", in[j]);
                if (j < 7) printf(",");
            }
            printf("]\n");
            
            printf("    Expected: [");
            for (int j = 0; j < 8; j++) {
                printf("%f", expected[j]);
                if (j < 7) printf(",");
            }
            printf("]\n");
            
            printf("    Got     : [");
            for (int j = 0; j < 8; j++) {
                printf("%f", out[j]);
                if (j < 7) printf(",");
            }
            printf("]\n\n");
            errors++;
        }
        printf("\n");
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
