#include "avx.h"
#include "avxfloat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

static int run_test_case(const char *desc, int op_type, 
                         double src1, double src2) {
    printf("--- %s ---\n", desc);
    
    double result;
    
    if (op_type == 0) { // reg-reg
        __asm__ __volatile__(
            "vmovsd %1, %%xmm0\n\t"
            "vmovsd %2, %%xmm1\n\t"
            "vsubsd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovsd %%xmm2, %0\n\t"
            : "=m"(result)
            : "m"(src1), "m"(src2)
            : "xmm0", "xmm1", "xmm2"
        );
    } else { // reg-mem
        __asm__ __volatile__(
            "vmovsd %1, %%xmm0\n\t"
            "vsubsd %2, %%xmm0, %%xmm1\n\t"
            "vmovsd %%xmm1, %0\n\t"
            : "=m"(result)
            : "m"(src1), "m"(src2)
            : "xmm0", "xmm1"
        );
    }
    
    double inputs1[1] = {src1};
    double inputs2[1] = {src2};
    double results[1] = {result};
    
    print_double_vec("Input1", inputs1, 1);
    print_double_vec("Input2", inputs2, 1);
    print_double_vec("Result", results, 1);
    print_double_vec_hex("Hex   ", results, 1);
    
    printf("--- End of test ---\n\n");
    return 1;
}

static void test_vsubsd() {
    printf("--- Testing vsubsd (scalar double-precision subtraction) ---\n");
    
    // 测试128位用例（仅取第一个元素）
    int num_128 = sizeof(double_tests_128) / sizeof(double_tests_128[0]);
    for (int i = 0; i < num_128; i++) {
        DoubleTest128 *test = &double_tests_128[i];
        run_test_case(test->name, 0, test->input1[0], test->input2[0]);
        run_test_case(test->name, 1, test->input1[0], test->input2[0]);
    }
    
    // 测试256位用例（仅取第一个元素）
    int num_256 = sizeof(double_tests_256) / sizeof(double_tests_256[0]);
    for (int i = 0; i < num_256; i++) {
        DoubleTest256 *test = &double_tests_256[i];
        run_test_case(test->name, 0, test->input1[0], test->input2[0]);
        run_test_case(test->name, 1, test->input1[0], test->input2[0]);
    }
    
    printf("\n--- TEST COMPLETED ---\n");
}

int main() {
    test_vsubsd();
    return 0;
}
