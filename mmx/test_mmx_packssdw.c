#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mmintrin.h>
#include <stdlib.h>

void print_m64(const char* label, __m64 value) {
    int16_t* v = (int16_t*)&value;
    printf("%s: %04hx %04hx %04hx %04hx\n", 
           label, v[0], v[1], v[2], v[3]);
}

int main() {
    printf("=== Testing PACKSSDW ===\n");
    // Test case 1: Basic packing with register operands
    {
        // 修正操作数顺序和寄存器使用
        __m64 a = _mm_set_pi32(0x80000000, 0x7FFFFFFF); // 交换高低位
        __m64 b = _mm_set_pi32(0x56789ABC, 0x12345678); // 交换高低位
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "packssdw %%mm0, %%mm1\n\t"  // 交换操作数顺序
            "movq %%mm1, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 1: Basic packing\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        // 修正预期结果顺序（注意操作数交换后的结果排列）
        int16_t expected[4] = {0x7FFF, 0x7FFF, 0x7FFF, -0x8000};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 1 PASSED\n");
        } else {
            printf("Test 1 FAILED\n");
        }
    }
    
    // Test case 2: Boundary values with register operands
    {
        __m64 a = _mm_set_pi32(0x7FFFFFFF, -0x80000001);
        __m64 b = _mm_set_pi32(0x00000000, 0x00008000);
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "packssdw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
        printf("\nTest 2: Boundary values\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        // 修正预期结果：0x80000001->-0x7FFF, 0x00008000->0x7FFF
        int16_t expected[4] = {0x7FFF, 0x7FFF, 0x7FFF, 0x0000};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 2 PASSED\n");
        } else {
            printf("Test 2 FAILED\n");
        }
    }
    
    // Test case 3: Memory operands
    {
        __m64 a = _mm_set_pi32(0x9ABCDEF0, 0x89ABCDEF);
        __m64 b = _mm_set_pi32(0x12345678, 0x01234567);
        __m64 res;
        
        asm volatile (
            "packssdw %2, %0\n\t"
            : "=y"(res)
            : "0"(a), "m"(b)
        );
        
        printf("\nTest 3: Memory operands\n");
        print_m64("Input A", a);
        print_m64("Input B", b);
        print_m64("Result", res);
        
        // 修正内存操作数测试预期结果
        int16_t expected[4] = {-0x8000, -0x8000, 0x7FFF, 0x7FFF};
        if(memcmp(&res, expected, 8) == 0) {
            printf("Test 3 PASSED\n");
        } else {
            printf("Test 3 FAILED\n");
        }
    }
    
    // Test case 4: Fixed pattern values
    {
        struct {
            int32_t a[2];
            int32_t b[2];
        } tests[] = {
            {{0x12345678, 0x9ABCDEF0}, {0x7FFFFFFF, 0x80000000}},
            {{0x00000000, 0x00008000}, {0x7FFFFFFF, 0x80000001}},
            {{0x55555555, 0xAAAAAAAA}, {0x33333333, 0xCCCCCCCC}}
        };
        
        for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
            __m64 a = *(__m64*)tests[i].a;
            __m64 b = *(__m64*)tests[i].b;
        __m64 res;
        
        asm volatile (
            "movq %1, %%mm0\n\t"
            "movq %2, %%mm1\n\t"
            "packssdw %%mm1, %%mm0\n\t"
            "movq %%mm0, %0\n\t"
            : "=m"(res)
            : "m"(a), "m"(b)
            : "mm0", "mm1"
        );
        
            printf("\nTest 4.%zu: Fixed pattern\n", i+1);
            print_m64("Input A", a);
            print_m64("Input B", b);
            print_m64("Result", res);
            
            int16_t expected[4];
            // 修正循环索引和饱和逻辑
            for(int j=0; j<2; j++) {
                // 处理a的dword元素
                int32_t a_val = tests[i].a[j];
                expected[j] = (a_val > 0x7FFF) ? 0x7FFF : 
                            (a_val < -0x8000) ? -0x8000 : (int16_t)a_val;
                
                // 处理b的dword元素
                int32_t b_val = tests[i].b[j];
                expected[j+2] = (b_val > 0x7FFF) ? 0x7FFF : 
                               (b_val < -0x8000) ? -0x8000 : (int16_t)b_val;
            }
            
            if(memcmp(&res, expected, 8) == 0) {
                printf("Test 4.%zu PASSED\n", i+1);
            } else {
                printf("Test 4.%zu FAILED\n", i+1);
            }
        }
    }
    
    _mm_empty();
    return 0;
}
