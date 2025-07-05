#ifndef FMA_H
#define FMA_H

#include "avx.h"
#include <float.h>

#define FMA_TEST_CASE_COUNT 23
#define FMA_TEST_CASE_COUNT_256_PS 23
#define FMA_TEST_CASE_COUNT_256_PD 23

// 256位双精度测试用例
typedef struct {
    double a[4];
    double b[4];
    double c[4];
    const char *desc;
} fma_test_case_256_pd;

static fma_test_case_256_pd fma_cases_256_pd[FMA_TEST_CASE_COUNT] = {
    // 正常值
    {
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        {9.0, 10.0, 11.0, 12.0},
        "Normal values (256-bit)"
    },
    // NaN
    {
        {NAN, -NAN, NAN, -NAN},
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        "NaN values in A(256-bit)"
    },
    {
        {1.0, 2.0, 3.0, 4.0},
        {NAN, -NAN, NAN, -NAN},
        {5.0, 6.0, 7.0, 8.0},
        "NaN values in B (256-bit)"
    },
    {
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        {NAN, -NAN, NAN, -NAN},
        "NaN values in C (256-bit)"
    },
    {
        {NAN, -NAN, NAN, NAN},
        {NAN, -NAN, -NAN, -NAN},
        {5.0, 6.0, 7.0, 8.0},
        "NaN values in A & B (256-bit)"
    },
    {
        {NAN, -NAN, NAN, -NAN},
        {5.0, 6.0, 7.0, 8.0},
        {NAN, -NAN, -NAN, NAN},
        "NaN values in A & C (256-bit)"
    },
    {
        {1.0, 2.0, 3.0, 4.0},
        {NAN, -NAN, NAN, NAN},
        {NAN, -NAN, -NAN, -NAN},
        "NaN values in B & C (256-bit)"
    },
    {
        {-1.0, -2.0, -3.0, -4.0},
        {NAN, -NAN, NAN, NAN},
        {NAN, -NAN, -NAN, -NAN},
        "NaN values in B & C , A negtive val (256-bit)"
    },
    {
        {NAN, -NAN, NAN, -NAN},
        {NAN, -NAN, -NAN, NAN},
        {NAN, -NAN, -NAN, -NAN},
        "NaN values in A & B & C (256-bit)"
    },    
    // INF
    {
        {INFINITY, -INFINITY, INFINITY, -INFINITY},
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        "INF values in A (256-bit)"
    },
    // 补充的INF组合用例
    {
        {1.0, 2.0, 3.0, 4.0},
        {INFINITY, -INFINITY, INFINITY, -INFINITY},
        {5.0, 6.0, 7.0, 8.0},
        "INF values in B (256-bit)"
    },
    {
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        {INFINITY, -INFINITY, INFINITY, -INFINITY},
        "INF values in C (256-bit)"
    },
    {
        {INFINITY, -INFINITY, INFINITY, -INFINITY},
        {INFINITY, -INFINITY, -INFINITY, INFINITY},
        {5.0, 6.0, 7.0, 8.0},
        "INF values in A and B (256-bit)"
    },
    {
        {INFINITY, -INFINITY, INFINITY, -INFINITY},
        {5.0, 6.0, 7.0, 8.0},
        {INFINITY, -INFINITY, -INFINITY, INFINITY},
        "INF values in A and C (256-bit)"
    },
    {
        {1.0, 2.0, 3.0, 4.0},
        {INFINITY, -INFINITY, INFINITY, -INFINITY},
        {INFINITY, -INFINITY, -INFINITY, INFINITY},
        "INF values in B and C (256-bit)"
    },
    {
        {-1.0, -2.0, -3.0, -4.0},
        {INFINITY, -INFINITY, INFINITY, -INFINITY},
        {INFINITY, -INFINITY, -INFINITY, INFINITY},
        "INF values in B and C A negtive val(256-bit)"
    },    
    {
        {INFINITY, -INFINITY, INFINITY, -INFINITY},
        {INFINITY, -INFINITY, INFINITY, -INFINITY},
        {INFINITY, -INFINITY, INFINITY, -INFINITY},
        "INF values in A, B and C (256-bit)"
    },
    // 0.0
    {
        {0.0, -0.0, 0.0, -0.0},
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        "Zero values (256-bit)"
    },
    // 边界值
    {
        {DBL_MAX, DBL_MIN, -DBL_MAX, -DBL_MIN},
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        "Boundary values (256-bit)"
    },
    // NaN与INF组合
    {
        {NAN, INFINITY, -NAN, -INFINITY},
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        "NaN and INF mix (256-bit)"
    },
    // 0.0与边界值组合
    {
        {0.0, DBL_MAX, -0.0, -DBL_MIN},
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        "Zero and boundary mix (256-bit)"
    },
    // 非规格化数
    {
        {DBL_MIN/2.0, -DBL_MIN/2.0, DBL_MIN/2.0, -DBL_MIN/2.0},
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        "Denormal values (256-bit)"
    },
    // 溢出情况
    {
        {DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX},
        {2.0, 2.0, 2.0, 2.0},
        {DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX},
        "Overflow case (256-bit)"
    },
};

// 256位单精度测试用例
typedef struct {
    float a[8];
    float b[8];
    float c[8];
    const char *desc;
} fma_test_case_256_ps;

static fma_test_case_256_ps fma_cases_256_ps[FMA_TEST_CASE_COUNT] = {
    // 正常值
    {
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f},
        {17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f},
        "Normal values (256-bit single precision)"
    },
    // NaN
    {
        {NAN, -NAN, NAN, -NAN, NAN, -NAN, NAN, -NAN},
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f},
        "NaN values in A (256-bit single precision)"
    },
    {
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        {NAN, -NAN, NAN, -NAN, NAN, -NAN, NAN, -NAN},
        {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f},
        "NaN values in B (256-bit single precision)"
    },
    {
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f},
        {NAN, -NAN, NAN, -NAN, NAN, -NAN, NAN, -NAN},
        "NaN values in C (256-bit single precision)"
    },
   // 补充的NaN组合用例（单精度）
    {
        {NAN, -NAN, NAN, -NAN, NAN, -NAN, NAN, -NAN},
        {NAN, -NAN, -NAN, NAN, NAN, -NAN, -NAN, NAN},
        {5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f},
        "NaN values in A and B (256-bit single precision)"
    },
    {
        {NAN, -NAN, NAN, -NAN, NAN, -NAN, NAN, -NAN},
        {5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f},
        {NAN, -NAN, -NAN, NAN, NAN, -NAN, -NAN, NAN},
        "NaN values in A and C (256-bit single precision)"
    },
    {
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        {NAN, -NAN, NAN, -NAN, NAN, -NAN, NAN, -NAN},
        {NAN, -NAN, -NAN, NAN, NAN, -NAN, -NAN, NAN},
        "NaN values in B and C (256-bit single precision)"
    },
    {
        {-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f},
        {NAN, -NAN, NAN, -NAN, NAN, -NAN, NAN, -NAN},
        {NAN, -NAN, -NAN, NAN, NAN, -NAN, -NAN, NAN},
        "NaN values in B and C , A negtive val (256-bit single precision)"
    },
    {
        {NAN, -NAN, NAN, -NAN, NAN, -NAN, NAN, -NAN},
        {NAN, -NAN, -NAN, NAN, NAN, -NAN, -NAN, NAN},
        {NAN, -NAN, -NAN, -NAN, NAN, -NAN, -NAN, NAN},
        "NaN values in A and B and C (256-bit single precision)"
    },    
    // INF
    {
        {INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY},
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f},
        "INF values in A(256-bit single precision)"
    },
        // 补充的INF组合用例（单精度）
    {
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        {INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY},
        {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f},
        "INF values in B (256-bit single precision)"
    },
    {
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f},
        {INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY},
        "INF values in C (256-bit single precision)"
    },
    {
        {INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY},
        {INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY},
        {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f},
        "INF values in A and B (256-bit single precision)"
    },
    {
        {INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY},
        {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f},
        {INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY},
        "INF values in A and C (256-bit single precision)"
    },
    {
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        {INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY},
        {INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY},
        "INF values in B and C (256-bit single precision)"
    },
    {
        {-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f},
        {INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY},
        {INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY},
        "INF values in B and C A negtive val(256-bit single precision)"
    },    
    {
        {INFINITY, -INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY},
        {INFINITY, -INFINITY, INFINITY, -INFINITY, -INFINITY, INFINITY, INFINITY, -INFINITY},
        {INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY, -INFINITY, INFINITY},
        "INF values in A, B and C (256-bit single precision)"
    },
    // 0.0
    {
        {0.0f, -0.0f, 0.0f, -0.0f, 0.0f, -0.0f, 0.0f, -0.0f},
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f},
        "Zero values (256-bit single precision)"
    },
    // 边界值
    {
        {FLT_MAX, FLT_MIN, -FLT_MAX, -FLT_MIN, FLT_MAX, FLT_MIN, -FLT_MAX, -FLT_MIN},
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f},
        "Boundary values (256-bit single precision)"
    },
    // NaN与INF组合
    {
        {NAN, INFINITY, -NAN, -INFINITY, NAN, INFINITY, -NAN, -INFINITY},
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f},
        "NaN and INF mix (256-bit single precision)"
    },
    // 非规格化数
    {
        {FLT_MIN/2.0f, -FLT_MIN/2.0f, FLT_MIN/2.0f, -FLT_MIN/2.0f, FLT_MIN/2.f, -FLT_MIN/2.0f, FLT_MIN/2.0f, -FLT_MIN/2.0f},
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f},
        "Denormal values (256-bit single precision)"
    },
    // 零值与边界值混合
    {
        {0.0f, -0.0f, 0.0f, -0.0f, FLT_MAX, -FLT_MAX, FLT_MIN, -FLT_MIN},
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f},
        {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f},
        "Zero and boundary mix (256-bit single precision)"
    },
    // 溢出情况
    {
        {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX},
        {2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f},
        {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX},
        "Overflow case (256-bit single precision)"
    },
};

#endif // FMA_H
