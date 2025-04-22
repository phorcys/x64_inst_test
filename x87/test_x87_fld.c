#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fenv.h>
#include <math.h>
#include <float.h>

// 测试结果宏
#define TEST_CASE(name, condition) \
    do { \
        printf("[TEST] %-40s %s\n", name, (condition) ? "PASS" : "FAIL"); \
        if (!(condition)) { \
            printf("  [FAIL DETAIL] %s:%d\n", __FILE__, __LINE__); \
        } \
    } while (0)

// 打印浮点值
void print_float(const char* name, float value) {
    printf("%s: ", name);
    if (isnan(value)) {
        printf("%sNaN\n", signbit(value) ? "-" : "+");
    } else if (isinf(value)) {
        printf("%sINF\n", signbit(value) ? "-" : "+");
    } else {
        printf("%.15g\n", value);
    }
}

void print_double(const char* name, double value) {
    printf("%s: ", name);
    if (isnan(value)) {
        printf("%sNaN\n", signbit(value) ? "-" : "+");
    } else if (isinf(value)) {
        printf("%sINF\n", signbit(value) ? "-" : "+");
    } else {
        printf("%.15g\n", value);
    }
}

// 测试FLD指令
void test_fld() {
    float f32_values[] = {
        0.0f, -0.0f,
        1.0f, -1.0f,
        123.456f, -123.456f,
        FLT_MAX, -FLT_MAX,
        FLT_MIN, -FLT_MIN,
        INFINITY, -INFINITY,
        NAN, -NAN
    };

    double f64_values[] = {
        0.0, -0.0,
        1.0, -1.0,
        123.456789, -123.456789,
        DBL_MAX, -DBL_MAX,
        DBL_MIN, -DBL_MIN,
        INFINITY, -INFINITY,
        NAN, -NAN
    };

    printf("\n==== Testing FLD (float) ====\n");
    for (size_t i = 0; i < sizeof(f32_values)/sizeof(f32_values[0]); i++) {
        float value = f32_values[i];
        float loaded_value;
        
        // 使用内联汇编加载浮点值
        __asm__ volatile (
            "flds %1\n\t"
            "fstps %0\n\t"
            : "=m" (loaded_value)
            : "m" (value)
        );

        print_float("Original", value);
        print_float("Loaded", loaded_value);
        
        // 比较原始值和加载后的值
        if (isnan(value) && isnan(loaded_value)) {
            TEST_CASE("NaN value loaded correctly", 1);
        } else {
            TEST_CASE("Value loaded correctly", value == loaded_value);
        }
    }

    printf("\n==== Testing FLD (double) ====\n");
    for (size_t i = 0; i < sizeof(f64_values)/sizeof(f64_values[0]); i++) {
        double value = f64_values[i];
        double loaded_value;
        
        // 使用内联汇编加载双精度浮点值
        __asm__ volatile (
            "fldl %1\n\t"
            "fstpl %0\n\t"
            : "=m" (loaded_value)
            : "m" (value)
        );

        print_double("Original", value);
        print_double("Loaded", loaded_value);
        
        // 比较原始值和加载后的值
        if (isnan(value) && isnan(loaded_value)) {
            TEST_CASE("NaN value loaded correctly", 1);
        } else {
            TEST_CASE("Value loaded correctly", value == loaded_value);
        }
    }

    // 测试栈指针变化
    printf("\n==== Testing FLD stack pointer ====\n");
    unsigned short fpu_status;
    __asm__ volatile ("fnstsw %0" : "=m" (fpu_status));
    int initial_top = (fpu_status >> 11) & 0x7;
    
    float temp = 1.0f;
    __asm__ volatile ("flds %0" : : "m" (temp));
    
    __asm__ volatile ("fnstsw %0" : "=m" (fpu_status));
    int new_top = (fpu_status >> 11) & 0x7;
    
    TEST_CASE("Stack pointer decremented", ((initial_top - 1) & 0x7) == new_top);
}

int main() {
    // 设置浮点环境
    fesetround(FE_TONEAREST);
    feclearexcept(FE_ALL_EXCEPT);

    test_fld();

    // 检查浮点异常
    int exceptions = fetestexcept(FE_ALL_EXCEPT);
    if (exceptions) {
        printf("\n[WARNING] Floating-point exceptions occurred: ");
        if (exceptions & FE_INVALID) printf("FE_INVALID ");
        if (exceptions & FE_DIVBYZERO) printf("FE_DIVBYZERO ");
        if (exceptions & FE_OVERFLOW) printf("FE_OVERFLOW ");
        if (exceptions & FE_UNDERFLOW) printf("FE_UNDERFLOW ");
        if (exceptions & FE_INEXACT) printf("FE_INEXACT ");
        printf("\n");
    }

    return 0;
}
