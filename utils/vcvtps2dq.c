#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <fenv.h>
#include <float.h>
#include <limits.h>
#include <string.h>

// 架构特定头文件
#if defined(LOONGARCH) || defined(__loongarch__)
#include <lsxintrin.h>
#define LOONGARCH 1
#elif defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#include <immintrin.h>
#else
#error "Unsupported architecture"
#endif

// 定义指令集宏 (VVCVTPD2DQ)
#ifdef LOONGARCH
#define TEST_INSTR_LOONGARCH(x) __lsx_vftint_w_s(x)  // LoongArch 双精度转32位整数
#else
#define TEST_INSTR_X86(x) _mm_cvtps_epi32(x)      // x86 双精度转32位整数
#endif

// 测试用例结构体
typedef struct {
    float a;
    const char *desc;
} test_case;

// 扩展测试用例数组
static test_case test_cases[] = {
    {0.0, "Positive zero"},
    {-0.0, "Negative zero"},
    {1.0, "Positive one"},
    {-1.0, "Negative one"},
    {12345.6789, "Positive fractional"},
    {-12345.6789, "Negative fractional"},
    {NAN, "NaN"},
    {-NAN, "-NaN"},
    {INFINITY, "+Inf"},
    {-INFINITY, "-Inf"},
    {FLT_MAX, "FLT_MAX"},
    {FLT_MIN, "FLT_MIN"},
    {-FLT_MAX, "-FLT_MAX"},
    {-FLT_MIN, "-FLT_MIN"},
    {1e-308, "Subnormal"},
    {3.1415926535, "Pi"},
    {-2.7182818284, "-e"},
    {2147483647.0, "Max int32"},   // 2^31-1
    {2147483648.0, "> Max int32"}, // 2^31，将溢出
    {-2147483648.0, "Min int32"},
    {-2147483649.0, "< Min int32"},
};

// 舍入模式名称和值
typedef struct {
    int mode;
    const char *name;
} rounding_mode;

static rounding_mode rounding_modes[] = {
    {FE_TONEAREST, "Round to nearest"},
    {FE_DOWNWARD, "Round toward -inf"},
    {FE_UPWARD, "Round toward +inf"},
    {FE_TOWARDZERO, "Round toward zero"}
};

// 设置舍入模式
void set_rounding(int mode) {
    fesetround(mode);
}

// 打印整数结果的十六进制表示
void print_int_hex(int32_t i, char *buffer) {
    sprintf(buffer, "0x%08x", i);
}

// 主测试函数 - 输出CSV格式
void run_tests() {
    const int num_cases = 21; //sizeof(test_cases) / sizeof(test_cases[0]);
    const int num_modes = sizeof(rounding_modes) / sizeof(rounding_modes[0]);
    
    // 打印CSV表头
    printf("Input,Input Hex,Description,Rounding Mode,Result,Result Hex\n");
    
    for (int i = 0; i < num_cases; i++) {
        test_case tc = test_cases[i];
        float a = tc.a;
        
        for (int m = 0; m < num_modes; m++) {
            set_rounding(rounding_modes[m].mode);
            
            int32_t result = 0;
            
            // X86实现
            #if defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
            // 创建包含4个相同值的256位向量
            float a_vec_x86[4] = {a, a, a, a};
            __m128 a_val = _mm_loadu_ps(a_vec_x86);
            __m128i res = TEST_INSTR_X86(a_val);
            int32_t res_arr[4];
            _mm_storeu_si128((__m128i*)res_arr, res);
            result = res_arr[0];
            #endif
            
            // LoongArch实现
            #if defined(LOONGARCH) || defined(__loongarch__)
            // 创建包含2个相同值的128位向量
            float a_vec_loong[2] = {a, a};
            __m128 a_val = (__m128)__lsx_vld(a_vec_loong, 0);
            __m128i res = TEST_INSTR_LOONGARCH(a_val);
            int32_t res_arr[4];
            __lsx_vst(res, res_arr, 0);
            result = res_arr[0];
            #endif
            
            // CSV格式输出
            printf("%.17g,0x%x,\"%s\",\"%s\",%d,0x%x\n",
                   a,
                   *(uint32_t*)&a,
                   tc.desc,
                   rounding_modes[m].name,
                   result,
                   result);
        }
    }
}

int main() {
    // 初始化FPU环境
    feclearexcept(FE_ALL_EXCEPT);
    run_tests();
    return 0;
}
