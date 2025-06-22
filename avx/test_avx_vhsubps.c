#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

static void test_vhsubps() {
    printf("Testing VHSUBPS\n");
    
    struct TestCase {
        float a, b, c, d, e, f, g, h;
        float expected[8];
    } tests[] = {
        // 正常数值
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 
         {1.0f-2.0f, 3.0f-4.0f, 1.0f-2.0f, 3.0f-4.0f, 
          5.0f-6.0f, 7.0f-8.0f, 5.0f-6.0f, 7.0f-8.0f}},
        
        // 负值
        {-1.0f, 1.0f, -2.0f, 2.0f, -3.0f, 3.0f, -4.0f, 4.0f,
         {-2.0f, -4.0f, -2.0f, -4.0f, -6.0f, -8.0f, -6.0f, -8.0f}},
        
        // 零值
        {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        
        // 无穷大
        {INFINITY, 1.0f, -INFINITY, 1.0f, INFINITY, -1.0f, -INFINITY, -1.0f,
         {INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY, -INFINITY}},
        
        // NaN
        {NAN, 1.0f, NAN, 1.0f, NAN, 1.0f, NAN, 1.0f,
         {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN}},
        
        // 边界值
        {1.17549435e-38f, 3.40282347e+38f, -1.17549435e-38f, -3.40282347e+38f,
         1.17549435e-38f, 3.40282347e+38f, -1.17549435e-38f, -3.40282347e+38f,
         {-3.40282347e+38f, 3.40282347e+38f, -3.40282347e+38f, 3.40282347e+38f,
          -3.40282347e+38f, 3.40282347e+38f, -3.40282347e+38f, 3.40282347e+38f}}
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        float res[8] = {0};
        float input[8] = {
            tests[i].a, tests[i].b, tests[i].c, tests[i].d,
            tests[i].e, tests[i].f, tests[i].g, tests[i].h
        };
        
        __asm__ __volatile__(
            "vmovups %1, %%ymm0\n\t"
            "vhsubps %%ymm0, %%ymm0, %%ymm1\n\t"
            "vmovups %%ymm1, %0\n\t"
            : "=m"(res)
            : "m"(input)
            : "ymm0", "ymm1"
        );

        printf("Test %zu: [%g,%g,%g,%g,%g,%g,%g,%g]\n", i+1, 
              tests[i].a, tests[i].b, tests[i].c, tests[i].d,
              tests[i].e, tests[i].f, tests[i].g, tests[i].h);
        
        int passed = 1;
        for (int j = 0; j < 8; j++) {
            printf("Result[%d]: %g\tExpected: %g\n", j, res[j], tests[i].expected[j]);
            
            if (isnan(res[j]) && isnan(tests[i].expected[j])) {
                continue;
            }
            
            if (fabs(res[j] - tests[i].expected[j]) > 1e-6) {
                passed = 0;
            }
        }
        
        if (passed) {
            printf("Passed\n");
        } else {
            printf("Failed\n");
        }
        printf("\n");
    }
}

int main() {
    test_vhsubps();
    printf("VHSUBPS tests completed\n");
    return 0;
}
