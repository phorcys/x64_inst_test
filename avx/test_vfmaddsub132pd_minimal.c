#include <stdio.h>
#include <immintrin.h>

int main() {
    // 设置简单的测试值
    __m256d dst = _mm256_setr_pd(5.0, 6.0, 7.0, 8.0);   // 目标操作数（将被修改）
    __m256d src1 = _mm256_setr_pd(1.0, 2.0, 3.0, 4.0);  // 源操作数1
    __m256d src2 = _mm256_setr_pd(9.0, 10.0, 11.0, 12.0); // 源操作数2

    // 保存原始值
    __m256d orig_dst = dst;
    
    // 使用内在函数作为参考实现
    __m256d expected = _mm256_setr_pd(
        ((double*)&orig_dst)[0] * ((double*)&src1)[0] + ((double*)&src2)[0], // 元素0: 乘加
        ((double*)&orig_dst)[1] * ((double*)&src1)[1] - ((double*)&src2)[1], // 元素1: 乘减
        ((double*)&orig_dst)[2] * ((double*)&src1)[2] + ((double*)&src2)[2], // 元素2: 乘加
        ((double*)&orig_dst)[3] * ((double*)&src1)[3] - ((double*)&src2)[3]  // 元素3: 乘减
    );
    
    // 执行 VFMADDSUB132PD 指令
    __asm__ __volatile__(
        "vfmaddsub132pd %[src2], %[src1], %[dst]"
        : [dst] "+x" (dst)
        : [src1] "x" (src1), [src2] "x" (src2)
    );
    
    // 比较结果
    __m256d diff = _mm256_sub_pd(dst, expected);
    int cmp_result = _mm256_movemask_pd(_mm256_cmp_pd(diff, _mm256_setzero_pd(), _CMP_NEQ_OQ));
    
    // 提取结果
    double result[4];
    _mm256_storeu_pd(result, dst);
    
    // 打印结果
    printf("VFMADDSUB132PD Minimal Test\n");
    printf("Element 0 (even): %.1f * %.1f + %.1f = %.1f\n", 
           ((double*)&orig_dst)[0], ((double*)&src1)[0], ((double*)&src2)[0], result[0]);
    printf("Element 1 (odd):  %.1f * %.1f - %.1f = %.1f\n", 
           ((double*)&orig_dst)[1], ((double*)&src1)[1], ((double*)&src2)[1], result[1]);
    printf("Element 2 (even): %.1f * %.1f + %.1f = %.1f\n", 
           ((double*)&orig_dst)[2], ((double*)&src1)[2], ((double*)&src2)[2], result[2]);
    printf("Element 3 (odd):  %.1f * %.1f - %.1f = %.1f\n", 
           ((double*)&orig_dst)[3], ((double*)&src1)[3], ((double*)&src2)[3], result[3]);
    
    // 打印比较结果
    if (cmp_result == 0) {
        printf("Success: ASM result matches intrinsic\n");
    } else {
        printf("Mismatch: ASM result differs from intrinsic\n");
    }
    
    return 0;
}
