#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

static void test_vbroadcastf128() {
    printf("--- Testing vbroadcastf128 (broadcast 128-bit memory to 256-bit register) ---\n");

    // 测试数据
    float src_f32[4] ALIGNED(16) = {1.1f, 2.2f, 3.3f, 4.4f};
    double src_f64[2] ALIGNED(16) = {1.1, 2.2};

    // 浮点数测试
    float result_f32[8] ALIGNED(32) = {0};
    __asm__ __volatile__(
        "vbroadcastf128 %1, %%ymm0\n\t"
        "vmovaps %%ymm0, %0\n\t"
        : "=m"(result_f32)
        : "m"(src_f32)
        : "ymm0"
    );
    printf("--- Float32 Test ---\n");
    print_float_vec("Source", src_f32, 4);
    print_float_vec("Result", result_f32, 8);

    // 验证浮点数结果
    int pass = 1;
    for (int i = 0; i < 4; i++) {
        if (result_f32[i] != src_f32[i] || result_f32[i+4] != src_f32[i]) {
            pass = 0;
            break;
        }
    }

    // 双精度测试
    double result_f64[4] ALIGNED(32) = {0};
    __asm__ __volatile__(
        "vbroadcastf128 %1, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result_f64)
        : "m"(src_f64)
        : "ymm0"
    );
    printf("\n--- Float64 Test ---\n");
    print_double_vec("Source", src_f64, 2);
    print_double_vec("Result", result_f64, 4);

    // 验证双精度结果
    for (int i = 0; i < 2; i++) {
        if (result_f64[i] != src_f64[i] || result_f64[i+2] != src_f64[i]) {
            pass = 0;
            break;
        }
    }

    // 边界值测试
    float boundary[4] ALIGNED(16) = {INFINITY, -INFINITY, NAN, 0.0f};
    float result_boundary[8] ALIGNED(32) = {0};
    __asm__ __volatile__(
        "vbroadcastf128 %1, %%ymm0\n\t"
        "vmovaps %%ymm0, %0\n\t"
        : "=m"(result_boundary)
        : "m"(boundary)
        : "ymm0"
    );
    printf("\n--- Boundary Test ---\n");
    print_float_vec("Expected", (float[]){
        INFINITY, -INFINITY, NAN, 0.0f,
        INFINITY, -INFINITY, NAN, 0.0f
    }, 8);
    print_float_vec("Result", result_boundary, 8);

    // 验证边界测试结果
    if (!isinf(result_boundary[0]) || !isinf(result_boundary[4])) pass = 0;
    if (!isinf(result_boundary[1]) || result_boundary[1] > 0 || 
        !isinf(result_boundary[5]) || result_boundary[5] > 0) pass = 0;
    if (!isnan(result_boundary[2]) || !isnan(result_boundary[6])) pass = 0;
    if (result_boundary[3] != 0.0f || result_boundary[7] != 0.0f) pass = 0;

    if (pass) {
        printf("\nvbroadcastf128 all tests passed\n");
    } else {
        printf("\nvbroadcastf128 some tests failed\n");
    }
}

int main() {
    test_vbroadcastf128();
    return 0;
}
