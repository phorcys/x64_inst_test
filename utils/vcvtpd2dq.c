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
#define TEST_INSTR_LOONGARCH(x) __lsx_vftint_w_d(x, x)  // LoongArch 双精度转32位整数
#else
#define TEST_INSTR_X86(x) _mm256_cvtpd_epi32(x)      // x86 双精度转32位整数
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
    {1.0, "Positive one"},
    {-1.0, "Negative one"},
    {12345.6789, "Positive fractional"},
    {-12345.6789, "Negative fractional"},
    {NAN, "NaN"},
    {-NAN, "-NaN"},
    {INFINITY, "+Inf"},
    {-INFINITY, "-Inf"},
    {DBL_MAX, "DBL_MAX"},
    {DBL_MIN, "DBL_MIN"},
    {-DBL_MAX, "-DBL_MAX"},
    {-DBL_MIN, "-DBL_MIN"},
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
    const int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    const int num_modes = sizeof(rounding_modes) / sizeof(rounding_modes[0]);
    
    // 打印CSV表头
    printf("Input,Input Hex,Description,Rounding Mode,Result,Result Hex\n");
    
    for (int i = 0; i < num_cases; i++) {
        test_case tc = test_cases[i];
        double a = tc.a;
        
        for (int m = 0; m < num_modes; m++) {
            set_rounding(rounding_modes[m].mode);
            
            int32_t result = 0;
            
            // X86实现
            #if defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
            // 创建包含4个相同值的256位向量
            double a_vec_x86[4] = {a, a, a, a};
            __m256d a_val = _mm256_loadu_pd(a_vec_x86);
            __m128i res = TEST_INSTR_X86(a_val);
            int32_t res_arr[4];
            _mm_storeu_si128((__m128i*)res_arr, res);
            result = res_arr[0];
            #endif
            
            // LoongArch实现
            #if defined(LOONGARCH) || defined(__loongarch__)
            // 创建包含2个相同值的128位向量
            double a_vec_loong[2] = {a, a};
            __m128d a_val = (__m128d)__lsx_vld(a_vec_loong, 0);
            __m128i res = TEST_INSTR_LOONGARCH(a_val);
            int32_t res_arr[4];
            __lsx_vst(res, res_arr, 0);
            result = res_arr[0];
            #endif
            
            // CSV格式输出
            printf("%.17g,0x%llx,\"%s\",\"%s\",%d,0x%x\n",
                   a,
                   *(uint64_t*)(&a),
                   tc.desc,
                   rounding_modes[m].name,
                   result,
                   result);
        }
    }
     union {
        double d;
        uint64_t u;
    } a_union;
    union  {
        int32_t i;
        uint32_t u;
    } t1,t2;

    t1.u = 0x7fffffff;
    t2.u = 0x7ffffffe;
    for (int m = 0; m < num_modes; m++) {
        set_rounding(rounding_modes[m].mode);
        a_union.u = 0x41e0000000000000;
        int32_t print1 =0;
        int32_t print2 =0;
        do{
            
            int32_t result = 0;
            double last = a_union.d;
            uint64_t last_u = a_union.u;
            int32_t last_result = 0;
            // X86实现
            #if defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
            // 创建包含4个相同值的256位向量
            double a_vec_x86[4] = {a_union.d, a_union.d, a_union.d, a_union.d};
            __m256d a_val = _mm256_loadu_pd(a_vec_x86);
            __m128i res = TEST_INSTR_X86(a_val);
            int32_t res_arr[4];
            _mm_storeu_si128((__m128i*)res_arr, res);
            result = res_arr[0];
            #endif
            
            // LoongArch实现
            #if defined(LOONGARCH) || defined(__loongarch__)
            // 创建包含2个相同值的128位向量
            double a_vec_loong[2] = {a_union.d, a_union.d};
            __m128d a_val = (__m128d)__lsx_vld(a_vec_loong, 0);
            __m128i res = TEST_INSTR_LOONGARCH(a_val);
            int32_t res_arr[4];
            __lsx_vst(res, res_arr, 0);
            result = res_arr[0];
            #endif
            
            if(print1 == 0&& result == t1.i){
                print1 = 1;
                printf("%.17g,0x%llx,\"%s\",\"%s\",%d,0x%x\n",
                last,
                last_u,
                "int32 max border 1 prev",
                rounding_modes[m].name,
                last_result,
                last_result);
                printf("%.17g,0x%llx,\"%s\",\"%s\",%d,0x%x\n",
                a_union.d,
                a_union.u,
                "int32 max border 1",
                rounding_modes[m].name,
                result,
                result);
            }
            if(print2 == 0 && result == t2.i){
                print2 = 1;
                printf("%.17g,0x%llx,\"%s\",\"%s\",%d,0x%x\n",
                last,
                last_u,
                "int32 max border2 prev",
                rounding_modes[m].name,
                last_result,
                last_result);
                printf("%.17g,0x%llx,\"%s\",\"%s\",%d,0x%x\n",
                a_union.d,
                a_union.u,
                "int32 max border2",
                rounding_modes[m].name,
                result,
                result);
                break;
            }
            last = a_union.d;
            last_u = a_union.u;
            last_result = result;
            a_union.u -= 1;
        }while(1);

    }
}

int main() {
    // 初始化FPU环境
    feclearexcept(FE_ALL_EXCEPT);
    run_tests();
   

    return 0;
}
