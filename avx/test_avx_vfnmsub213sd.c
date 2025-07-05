#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <float.h>
#include "avx.h"
#include "fma.h"

static void test_scalar_reg_reg_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        double a = fma_cases_256_pd[t].a[0];
        double b = fma_cases_256_pd[t].b[0];
        double c = fma_cases_256_pd[t].c[0];
        
        __asm__ __volatile__(
            "vfnmsub213sd %[b], %[c], %[a]"
            : [a] "+x" (a)
            : [b] "x" (b), [c] "x" (c)
        );
        
        printf("Test Case: %s\n", fma_cases_256_pd[t].desc);
        printf("A     : %.17g\n", fma_cases_256_pd[t].a[0]);
        printf("B     : %.17g\n", fma_cases_256_pd[t].b[0]);
        printf("C     : %.17g\n", fma_cases_256_pd[t].c[0]);
        printf("Result: %.17g\n\n", a);
    }
    printf("vfnmsub213SD Scalar Register-Register Tests Completed\n\n");
}

static void test_scalar_reg_mem_operand() {
    for (int t = 0; t < FMA_TEST_CASE_COUNT; t++) {
        double a = fma_cases_256_pd[t].a[0];
        double c = fma_cases_256_pd[t].c[0];
        double b = fma_cases_256_pd[t].b[0];
        
        double a1 = a;
        __asm__ __volatile__(
            "vfnmsub213sd %[b], %[c], %[a]"
            : [a] "+x" (a1)
            : [b] "m" (b), [c] "x" (c)
        );
        
        printf("Memory Operand Test: %s\n", fma_cases_256_pd[t].desc);
        printf("A     : %.17g\n", fma_cases_256_pd[t].a[0]);
        printf("B     : %.17g\n", fma_cases_256_pd[t].b[0]);
        printf("C     : %.17g\n", fma_cases_256_pd[t].c[0]);
        printf("Result: %.17g\n\n", a1);
    }
    printf("vfnmsub213SD Scalar Register-Memory Tests Completed\n\n");
}

int main() {
    printf("==================================\n");
    printf("vfnmsub213SD Comprehensive Tests\n");
    printf("==================================\n\n");
    
    test_scalar_reg_reg_operand();
    test_scalar_reg_mem_operand();
    
    printf("vfnmsub213SD normal values tests completed.\n");
    
    return 0;
}
