#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

#define TEST_EXTRACT(index) \
    do { \
        printf("\nTest case %d (extract index %d):\n", index+1, index); \
        uint32_t dst; \
        __asm__ volatile ( \
            "movups %1, %%xmm0\n\t" \
            "extractps $"#index", %%xmm0, %0\n\t" \
            : "=r" (dst) \
            : "m" (a) \
            : "xmm0" \
        ); \
        float result; \
        memcpy(&result, &dst, sizeof(result)); \
        printf("Source: [%.1f, %.1f, %.1f, %.1f]\n", \
               src[0], src[1], src[2], src[3]); \
        printf("Extracted: %.1f (0x%08X)\n", result, dst); \
        printf("Expected: %.1f (0x%08X)\n", src[index], *(uint32_t*)&src[index]); \
        printf("Test %s\n", \
               memcmp(&result, &src[index], sizeof(float)) ? "FAILED" : "PASSED"); \
    } while(0)

// 测试EXTRACTPS指令
void test_extractps() {
    printf("Testing EXTRACTPS instruction\n");
    printf("---------------------------\n");
    
    // 测试数据
    float src[4] = {1.5f, 2.5f, 3.5f, 4.5f};
    __m128 a = _mm_loadu_ps(src);

    // 测试从不同位置提取
    TEST_EXTRACT(0);
    TEST_EXTRACT(1);
    TEST_EXTRACT(2);
    TEST_EXTRACT(3);

    // 测试提取到内存
    printf("\nTest case 5 (extract to memory):\n");
    float mem_result;
    __asm__ volatile (
        "movups %1, %%xmm0\n\t"
        "extractps $1, %%xmm0, %0\n\t"
        : "=m" (mem_result)
        : "m" (a)
        : "xmm0"
    );
    printf("Extracted to memory: %.1f\n", mem_result);
    printf("Expected: %.1f\n", src[1]);
    printf("Test %s\n", 
           memcmp(&mem_result, &src[1], sizeof(float)) ? "FAILED" : "PASSED");
}

int main() {
    test_extractps();
    return 0;
}
