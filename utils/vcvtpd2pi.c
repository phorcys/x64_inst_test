#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <fenv.h>
#include <float.h>

// 架构特定头文件
#if defined(LOONGARCH) || defined(__loongarch__)
#include <lasxintrin.h>
#elif defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#include <immintrin.h>
#else
#error "Unsupported architecture"
#endif

// 定义指令集宏 (CVTPD2PI)
#ifdef LOONGARCH
#define TEST_INSTR(x) __lasx_xvftint_w_d(x)
#else
#define TEST_INSTR(x) _mm256_cvtpd_epi32(x)
#endif

// 测试用例结构体
typedef struct {
    double a;
    const char *desc;
} test_case;

// 扩展测试用例数组
static test_case test_cases[] = {
    {0.0, "Positive zero"},
    {-0.0, "Negative zero"},
    {1.0, "Positive normal"},
    {-1.0, "Negative normal"},
    {NAN, "NaN"},
    {-NAN, "-NaN"},
    {INFINITY, "+Inf"},
    {-INFINITY, "-Inf"},
    {DBL_MAX, "DBL_MAX"},
    {DBL_MIN, "DBL_MIN"},
    {1e-308, "Subnormal"},
    {3.1415926535, "Pi"},
    {-2.7182818284, "-e"},
};

// 舍入模式名称和值
typedef struct {
    int mode;
    const char *name;
} rounding_mode;

static rounding_mode rounding_modes[] = {
    {FE_TONEAREST, "FE_TONEAREST"},
    {FE_DOWNWARD, "FE_DOWNWARD"},
    {FE_UPWARD, "FE_UPWARD"},
    {FE_TOWARDZERO, "FE_TOWARDZERO"}
};

// 设置舍入模式
void set_rounding(int mode) {
    fesetround(mode);
}

// 打印double值的十六进制表示
void print_double_hex(double d, char *buffer) {
    uint64_t bits = *(uint64_t*)&d;
    sprintf(buffer, "0x%016lx", bits);
}

// 打印整数值的十六进制表示
void print_int_hex(int i, char *buffer) {
    sprintf(buffer, "0x%08x", i);
}

// 主测试函数 - 输出CSV格式
void run_tests() {
    const int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    const int num_modes = sizeof(rounding_modes) / sizeof(rounding_modes[0]);
    
    // 打印CSV表头
    printf("Rounding Mode,Description,A (dec),A (hex),Result (dec),Result (hex)\n");
    
    for (int m = 0; m < num_modes; m++) {
        set_rounding(rounding_modes[m].mode);
        
        for (int i = 0; i < num_cases; i++) {
            test_case tc = test_cases[i];
            
            // 通用向量处理
            double a_vec[4] = {tc.a, 0.0, 0.0, 0.0};
            int res_vec[4];
            
            #ifdef LOONGARCH
            __m256d a = (__m256d)__lasx_xvld(a_vec, 0);
            __m256i res = TEST_INSTR(a);
            __lasx_xvst(res, res_vec, 0);
            #else
            __m256d a = _mm256_loadu_pd(a_vec);
            __m256i res = TEST_INSTR(a);
            _mm256_storeu_si256((__m256i*)res_vec, res);
            #endif
            
            int result = res_vec[0];
            
            // 准备十六进制表示
            char a_hex[20], res_hex[20];
            print_double_hex(tc.a, a_hex);
            print_int_hex(result, res_hex);
            
            // CSV格式输出
            printf("\"%s\",\"%s\",%.17g,%s,%d,%s\n",
                   rounding_modes[m].name,
                   tc.desc,
                   tc.a, a_hex,
                   result, res_hex);
        }
    }
}

int main() {
    // 初始化FPU环境
    feclearexcept(FE_ALL_EXCEPT);
    run_tests();
    return 0;
}
