#include "avx.h"
#include <stdio.h>
#include <math.h>

static void print_control_bits_xmm_ps(const char* name, __m128 ctrl) {
    uint32_t *vals = (uint32_t*)&ctrl;
    printf("%s control bits: [%u, %u, %u, %u] (hex: 0x%x, 0x%x, 0x%x, 0x%x)\n", 
           name, vals[0] & 3, vals[1] & 3, vals[2] & 3, vals[3] & 3,
           vals[0], vals[1], vals[2], vals[3]);
}

static void print_control_bits_ymm_ps(const char* name, __m256 ctrl) {
    uint32_t *vals = (uint32_t*)&ctrl;
    printf("%s control bits: [%u, %u, %u, %u, %u, %u, %u, %u] (hex: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)\n", 
           name, vals[0] & 3, vals[1] & 3, vals[2] & 3, vals[3] & 3,
           vals[4] & 3, vals[5] & 3, vals[6] & 3, vals[7] & 3,
           vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6], vals[7]);
}

// 测试128位立即数版本
static int test_imm_xmm() {
    int errors = 0;
    const char* test_name = "vpermilps imm8 (xmm)";
    printf("Testing %s\n", test_name);
    
    // 测试数据：各种边界值组合
    float in[4] = {0.0f, -0.0f, INFINITY, -INFINITY}; // +0, -0, 正负无穷
    float in2[4] = {1.0f, -1.0f, NAN, 123.456f};     // 正负1, NaN, 普通值
    float out[4] = {0};
    
    // 测试所有控制位组合 (4个元素 * 2位控制)
    for (int imm = 0; imm < 256; imm++) {
        // 测试每组输入数据
        float* inputs[] = {in, in2};
        for (int i = 0; i < 2; i++) {
            __m128 src = _mm_loadu_ps(inputs[i]);
            __m128 result;
            
            // 创建正确的控制向量：每个元素独立的控制位
            uint32_t controls[4] = {
                (imm >> 0) & 3,
                (imm >> 2) & 3,
                (imm >> 4) & 3,
                (imm >> 6) & 3
            };
            switch(imm) {
                case 0: asm("vpermilps $0, %1, %0" : "=x"(result) : "x"(src)); break;
                case 1: asm("vpermilps $1, %1, %0" : "=x"(result) : "x"(src)); break;
                case 2: asm("vpermilps $2, %1, %0" : "=x"(result) : "x"(src)); break;
                case 3: asm("vpermilps $3, %1, %0" : "=x"(result) : "x"(src)); break;
                case 4: asm("vpermilps $4, %1, %0" : "=x"(result) : "x"(src)); break;
                case 5: asm("vpermilps $5, %1, %0" : "=x"(result) : "x"(src)); break;
                case 6: asm("vpermilps $6, %1, %0" : "=x"(result) : "x"(src)); break;
                case 7: asm("vpermilps $7, %1, %0" : "=x"(result) : "x"(src)); break;
                case 8: asm("vpermilps $8, %1, %0" : "=x"(result) : "x"(src)); break;
                case 9: asm("vpermilps $9, %1, %0" : "=x"(result) : "x"(src)); break;
                case 10: asm("vpermilps $10, %1, %0" : "=x"(result) : "x"(src)); break;
                case 11: asm("vpermilps $11, %1, %0" : "=x"(result) : "x"(src)); break;
                case 12: asm("vpermilps $12, %1, %0" : "=x"(result) : "x"(src)); break;
                case 13: asm("vpermilps $13, %1, %0" : "=x"(result) : "x"(src)); break;
                case 14: asm("vpermilps $14, %1, %0" : "=x"(result) : "x"(src)); break;
                case 15: asm("vpermilps $15, %1, %0" : "=x"(result) : "x"(src)); break;
                case 16: asm("vpermilps $16, %1, %0" : "=x"(result) : "x"(src)); break;
                case 17: asm("vpermilps $17, %1, %0" : "=x"(result) : "x"(src)); break;
                case 18: asm("vpermilps $18, %1, %0" : "=x"(result) : "x"(src)); break;
                case 19: asm("vpermilps $19, %1, %0" : "=x"(result) : "x"(src)); break;
                case 20: asm("vpermilps $20, %1, %0" : "=x"(result) : "x"(src)); break;
                case 21: asm("vpermilps $21, %1, %0" : "=x"(result) : "x"(src)); break;
                case 22: asm("vpermilps $22, %1, %0" : "=x"(result) : "x"(src)); break;
                case 23: asm("vpermilps $23, %1, %0" : "=x"(result) : "x"(src)); break;
                case 24: asm("vpermilps $24, %1, %0" : "=x"(result) : "x"(src)); break;
                case 25: asm("vpermilps $25, %1, %0" : "=x"(result) : "x"(src)); break;
                case 26: asm("vpermilps $26, %1, %0" : "=x"(result) : "x"(src)); break;
                case 27: asm("vpermilps $27, %1, %0" : "=x"(result) : "x"(src)); break;
                case 28: asm("vpermilps $28, %1, %0" : "=x"(result) : "x"(src)); break;
                case 29: asm("vpermilps $29, %1, %0" : "=x"(result) : "x"(src)); break;
                case 30: asm("vpermilps $30, %1, %0" : "=x"(result) : "x"(src)); break;
                case 31: asm("vpermilps $31, %1, %0" : "=x"(result) : "x"(src)); break;
                case 32: asm("vpermilps $32, %1, %0" : "=x"(result) : "x"(src)); break;
                case 33: asm("vpermilps $33, %1, %0" : "=x"(result) : "x"(src)); break;
                case 34: asm("vpermilps $34, %1, %0" : "=x"(result) : "x"(src)); break;
                case 35: asm("vpermilps $35, %1, %0" : "=x"(result) : "x"(src)); break;
                case 36: asm("vpermilps $36, %1, %0" : "=x"(result) : "x"(src)); break;
                case 37: asm("vpermilps $37, %1, %0" : "=x"(result) : "x"(src)); break;
                case 38: asm("vpermilps $38, %1, %0" : "=x"(result) : "x"(src)); break;
                case 39: asm("vpermilps $39, %1, %0" : "=x"(result) : "x"(src)); break;
                case 40: asm("vpermilps $40, %1, %0" : "=x"(result) : "x"(src)); break;
                case 41: asm("vpermilps $41, %1, %0" : "=x"(result) : "x"(src)); break;
                case 42: asm("vpermilps $42, %1, %0" : "=x"(result) : "x"(src)); break;
                case 43: asm("vpermilps $43, %1, %0" : "=x"(result) : "x"(src)); break;
                case 44: asm("vpermilps $44, %1, %0" : "=x"(result) : "x"(src)); break;
                case 45: asm("vpermilps $45, %1, %0" : "=x"(result) : "x"(src)); break;
                case 46: asm("vpermilps $46, %1, %0" : "=x"(result) : "x"(src)); break;
                case 47: asm("vpermilps $47, %1, %0" : "=x"(result) : "x"(src)); break;
                case 48: asm("vpermilps $48, %1, %0" : "=x"(result) : "x"(src)); break;
                case 49: asm("vpermilps $49, %1, %0" : "=x"(result) : "x"(src)); break;
                case 50: asm("vpermilps $50, %1, %0" : "=x"(result) : "x"(src)); break;
                case 51: asm("vpermilps $51, %1, %0" : "=x"(result) : "x"(src)); break;
                case 52: asm("vpermilps $52, %1, %0" : "=x"(result) : "x"(src)); break;
                case 53: asm("vpermilps $53, %1, %0" : "=x"(result) : "x"(src)); break;
                case 54: asm("vpermilps $54, %1, %0" : "=x"(result) : "x"(src)); break;
                case 55: asm("vpermilps $55, %1, %0" : "=x"(result) : "x"(src)); break;
                case 56: asm("vpermilps $56, %1, %0" : "=x"(result) : "x"(src)); break;
                case 57: asm("vpermilps $57, %1, %0" : "=x"(result) : "x"(src)); break;
                case 58: asm("vpermilps $58, %1, %0" : "=x"(result) : "x"(src)); break;
                case 59: asm("vpermilps $59, %1, %0" : "=x"(result) : "x"(src)); break;
                case 60: asm("vpermilps $60, %1, %0" : "=x"(result) : "x"(src)); break;
                case 61: asm("vpermilps $61, %1, %0" : "=x"(result) : "x"(src)); break;
                case 62: asm("vpermilps $62, %1, %0" : "=x"(result) : "x"(src)); break;
                case 63: asm("vpermilps $63, %1, %0" : "=x"(result) : "x"(src)); break;
                case 64: asm("vpermilps $64, %1, %0" : "=x"(result) : "x"(src)); break;
                case 65: asm("vpermilps $65, %1, %0" : "=x"(result) : "x"(src)); break;
                case 66: asm("vpermilps $66, %1, %0" : "=x"(result) : "x"(src)); break;
                case 67: asm("vpermilps $67, %1, %0" : "=x"(result) : "x"(src)); break;
                case 68: asm("vpermilps $68, %1, %0" : "=x"(result) : "x"(src)); break;
                case 69: asm("vpermilps $69, %1, %0" : "=x"(result) : "x"(src)); break;
                case 70: asm("vpermilps $70, %1, %0" : "=x"(result) : "x"(src)); break;
                case 71: asm("vpermilps $71, %1, %0" : "=x"(result) : "x"(src)); break;
                case 72: asm("vpermilps $72, %1, %0" : "=x"(result) : "x"(src)); break;
                case 73: asm("vpermilps $73, %1, %0" : "=x"(result) : "x"(src)); break;
                case 74: asm("vpermilps $74, %1, %0" : "=x"(result) : "x"(src)); break;
                case 75: asm("vpermilps $75, %1, %0" : "=x"(result) : "x"(src)); break;
                case 76: asm("vpermilps $76, %1, %0" : "=x"(result) : "x"(src)); break;
                case 77: asm("vpermilps $77, %1, %0" : "=x"(result) : "x"(src)); break;
                case 78: asm("vpermilps $78, %1, %0" : "=x"(result) : "x"(src)); break;
                case 79: asm("vpermilps $79, %1, %0" : "=x"(result) : "x"(src)); break;
                case 80: asm("vpermilps $80, %1, %0" : "=x"(result) : "x"(src)); break;
                case 81: asm("vpermilps $81, %1, %0" : "=x"(result) : "x"(src)); break;
                case 82: asm("vpermilps $82, %1, %0" : "=x"(result) : "x"(src)); break;
                case 83: asm("vpermilps $83, %1, %0" : "=x"(result) : "x"(src)); break;
                case 84: asm("vpermilps $84, %1, %0" : "=x"(result) : "x"(src)); break;
                case 85: asm("vpermilps $85, %1, %0" : "=x"(result) : "x"(src)); break;
                case 86: asm("vpermilps $86, %1, %0" : "=x"(result) : "x"(src)); break;
                case 87: asm("vpermilps $87, %1, %0" : "=x"(result) : "x"(src)); break;
                case 88: asm("vpermilps $88, %1, %0" : "=x"(result) : "x"(src)); break;
                case 89: asm("vpermilps $89, %1, %0" : "=x"(result) : "x"(src)); break;
                case 90: asm("vpermilps $90, %1, %0" : "=x"(result) : "x"(src)); break;
                case 91: asm("vpermilps $91, %1, %0" : "=x"(result) : "x"(src)); break;
                case 92: asm("vpermilps $92, %1, %0" : "=x"(result) : "x"(src)); break;
                case 93: asm("vpermilps $93, %1, %0" : "=x"(result) : "x"(src)); break;
                case 94: asm("vpermilps $94, %1, %0" : "=x"(result) : "x"(src)); break;
                case 95: asm("vpermilps $95, %1, %0" : "=x"(result) : "x"(src)); break;
                case 96: asm("vpermilps $96, %1, %0" : "=x"(result) : "x"(src)); break;
                case 97: asm("vpermilps $97, %1, %0" : "=x"(result) : "x"(src)); break;
                case 98: asm("vpermilps $98, %1, %0" : "=x"(result) : "x"(src)); break;
                case 99: asm("vpermilps $99, %1, %0" : "=x"(result) : "x"(src)); break;
                case 100: asm("vpermilps $100, %1, %0" : "=x"(result) : "x"(src)); break;
                case 101: asm("vpermilps $101, %1, %0" : "=x"(result) : "x"(src)); break;
                case 102: asm("vpermilps $102, %1, %0" : "=x"(result) : "x"(src)); break;
                case 103: asm("vpermilps $103, %1, %0" : "=x"(result) : "x"(src)); break;
                case 104: asm("vpermilps $104, %1, %0" : "=x"(result) : "x"(src)); break;
                case 105: asm("vpermilps $105, %1, %0" : "=x"(result) : "x"(src)); break;
                case 106: asm("vpermilps $106, %1, %0" : "=x"(result) : "x"(src)); break;
                case 107: asm("vpermilps $107, %1, %0" : "=x"(result) : "x"(src)); break;
                case 108: asm("vpermilps $108, %1, %0" : "=x"(result) : "x"(src)); break;
                case 109: asm("vpermilps $109, %1, %0" : "=x"(result) : "x"(src)); break;
                case 110: asm("vpermilps $110, %1, %0" : "=x"(result) : "x"(src)); break;
                case 111: asm("vpermilps $111, %1, %0" : "=x"(result) : "x"(src)); break;
                case 112: asm("vpermilps $112, %1, %0" : "=x"(result) : "x"(src)); break;
                case 113: asm("vpermilps $113, %1, %0" : "=x"(result) : "x"(src)); break;
                case 114: asm("vpermilps $114, %1, %0" : "=x"(result) : "x"(src)); break;
                case 115: asm("vpermilps $115, %1, %0" : "=x"(result) : "x"(src)); break;
                case 116: asm("vpermilps $116, %1, %0" : "=x"(result) : "x"(src)); break;
                case 117: asm("vpermilps $117, %1, %0" : "=x"(result) : "x"(src)); break;
                case 118: asm("vpermilps $118, %1, %0" : "=x"(result) : "x"(src)); break;
                case 119: asm("vpermilps $119, %1, %0" : "=x"(result) : "x"(src)); break;
                case 120: asm("vpermilps $120, %1, %0" : "=x"(result) : "x"(src)); break;
                case 121: asm("vpermilps $121, %1, %0" : "=x"(result) : "x"(src)); break;
                case 122: asm("vpermilps $122, %1, %0" : "=x"(result) : "x"(src)); break;
                case 123: asm("vpermilps $123, %1, %0" : "=x"(result) : "x"(src)); break;
                case 124: asm("vpermilps $124, %1, %0" : "=x"(result) : "x"(src)); break;
                case 125: asm("vpermilps $125, %1, %0" : "=x"(result) : "x"(src)); break;
                case 126: asm("vpermilps $126, %1, %0" : "=x"(result) : "x"(src)); break;
                case 127: asm("vpermilps $127, %1, %0" : "=x"(result) : "x"(src)); break;
                case 128: asm("vpermilps $128, %1, %0" : "=x"(result) : "x"(src)); break;
                case 129: asm("vpermilps $129, %1, %0" : "=x"(result) : "x"(src)); break;
                case 130: asm("vpermilps $130, %1, %0" : "=x"(result) : "x"(src)); break;
                case 131: asm("vpermilps $131, %1, %0" : "=x"(result) : "x"(src)); break;
                case 132: asm("vpermilps $132, %1, %0" : "=x"(result) : "x"(src)); break;
                case 133: asm("vpermilps $133, %1, %0" : "=x"(result) : "x"(src)); break;
                case 134: asm("vpermilps $134, %1, %0" : "=x"(result) : "x"(src)); break;
                case 135: asm("vpermilps $135, %1, %0" : "=x"(result) : "x"(src)); break;
                case 136: asm("vpermilps $136, %1, %0" : "=x"(result) : "x"(src)); break;
                case 137: asm("vpermilps $137, %1, %0" : "=x"(result) : "x"(src)); break;
                case 138: asm("vpermilps $138, %1, %0" : "=x"(result) : "x"(src)); break;
                case 139: asm("vpermilps $139, %1, %0" : "=x"(result) : "x"(src)); break;
                case 140: asm("vpermilps $140, %1, %0" : "=x"(result) : "x"(src)); break;
                case 141: asm("vpermilps $141, %1, %0" : "=x"(result) : "x"(src)); break;
                case 142: asm("vpermilps $142, %1, %0" : "=x"(result) : "x"(src)); break;
                case 143: asm("vpermilps $143, %1, %0" : "=x"(result) : "x"(src)); break;
                case 144: asm("vpermilps $144, %1, %0" : "=x"(result) : "x"(src)); break;
                case 145: asm("vpermilps $145, %1, %0" : "=x"(result) : "x"(src)); break;
                case 146: asm("vpermilps $146, %1, %0" : "=x"(result) : "x"(src)); break;
                case 147: asm("vpermilps $147, %1, %0" : "=x"(result) : "x"(src)); break;
                case 148: asm("vpermilps $148, %1, %0" : "=x"(result) : "x"(src)); break;
                case 149: asm("vpermilps $149, %1, %0" : "=x"(result) : "x"(src)); break;
                case 150: asm("vpermilps $150, %1, %0" : "=x"(result) : "x"(src)); break;
                case 151: asm("vpermilps $151, %1, %0" : "=x"(result) : "x"(src)); break;
                case 152: asm("vpermilps $152, %1, %0" : "=x"(result) : "x"(src)); break;
                case 153: asm("vpermilps $153, %1, %0" : "=x"(result) : "x"(src)); break;
                case 154: asm("vpermilps $154, %1, %0" : "=x"(result) : "x"(src)); break;
                case 155: asm("vpermilps $155, %1, %0" : "=x"(result) : "x"(src)); break;
                case 156: asm("vpermilps $156, %1, %0" : "=x"(result) : "x"(src)); break;
                case 157: asm("vpermilps $157, %1, %0" : "=x"(result) : "x"(src)); break;
                case 158: asm("vpermilps $158, %1, %0" : "=x"(result) : "x"(src)); break;
                case 159: asm("vpermilps $159, %1, %0" : "=x"(result) : "x"(src)); break;
                case 160: asm("vpermilps $160, %1, %0" : "=x"(result) : "x"(src)); break;
                case 161: asm("vpermilps $161, %1, %0" : "=x"(result) : "x"(src)); break;
                case 162: asm("vpermilps $162, %1, %0" : "=x"(result) : "x"(src)); break;
                case 163: asm("vpermilps $163, %1, %0" : "=x"(result) : "x"(src)); break;
                case 164: asm("vpermilps $164, %1, %0" : "=x"(result) : "x"(src)); break;
                case 165: asm("vpermilps $165, %1, %0" : "=x"(result) : "x"(src)); break;
                case 166: asm("vpermilps $166, %1, %0" : "=x"(result) : "x"(src)); break;
                case 167: asm("vpermilps $167, %1, %0" : "=x"(result) : "x"(src)); break;
                case 168: asm("vpermilps $168, %1, %0" : "=x"(result) : "x"(src)); break;
                case 169: asm("vpermilps $169, %1, %0" : "=x"(result) : "x"(src)); break;
                case 170: asm("vpermilps $170, %1, %0" : "=x"(result) : "x"(src)); break;
                case 171: asm("vpermilps $171, %1, %0" : "=x"(result) : "x"(src)); break;
                case 172: asm("vpermilps $172, %1, %0" : "=x"(result) : "x"(src)); break;
                case 173: asm("vpermilps $173, %1, %0" : "=x"(result) : "x"(src)); break;
                case 174: asm("vpermilps $174, %1, %0" : "=x"(result) : "x"(src)); break;
                case 175: asm("vpermilps $175, %1, %0" : "=x"(result) : "x"(src)); break;
                case 176: asm("vpermilps $176, %1, %0" : "=x"(result) : "x"(src)); break;
                case 177: asm("vpermilps $177, %1, %0" : "=x"(result) : "x"(src)); break;
                case 178: asm("vpermilps $178, %1, %0" : "=x"(result) : "x"(src)); break;
                case 179: asm("vpermilps $179, %1, %0" : "=x"(result) : "x"(src)); break;
                case 180: asm("vpermilps $180, %1, %0" : "=x"(result) : "x"(src)); break;
                case 181: asm("vpermilps $181, %1, %0" : "=x"(result) : "x"(src)); break;
                case 182: asm("vpermilps $182, %1, %0" : "=x"(result) : "x"(src)); break;
                case 183: asm("vpermilps $183, %1, %0" : "=x"(result) : "x"(src)); break;
                case 184: asm("vpermilps $184, %1, %0" : "=x"(result) : "x"(src)); break;
                case 185: asm("vpermilps $185, %1, %0" : "=x"(result) : "x"(src)); break;
                case 186: asm("vpermilps $186, %1, %0" : "=x"(result) : "x"(src)); break;
                case 187: asm("vpermilps $187, %1, %0" : "=x"(result) : "x"(src)); break;
                case 188: asm("vpermilps $188, %1, %0" : "=x"(result) : "x"(src)); break;
                case 189: asm("vpermilps $189, %1, %0" : "=x"(result) : "x"(src)); break;
                case 190: asm("vpermilps $190, %1, %0" : "=x"(result) : "x"(src)); break;
                case 191: asm("vpermilps $191, %1, %0" : "=x"(result) : "x"(src)); break;
                case 192: asm("vpermilps $192, %1, %0" : "=x"(result) : "x"(src)); break;
                case 193: asm("vpermilps $193, %1, %0" : "=x"(result) : "x"(src)); break;
                case 194: asm("vpermilps $194, %1, %0" : "=x"(result) : "x"(src)); break;
                case 195: asm("vpermilps $195, %1, %0" : "=x"(result) : "x"(src)); break;
                case 196: asm("vpermilps $196, %1, %0" : "=x"(result) : "x"(src)); break;
                case 197: asm("vpermilps $197, %1, %0" : "=x"(result) : "x"(src)); break;
                case 198: asm("vpermilps $198, %1, %0" : "=x"(result) : "x"(src)); break;
                case 199: asm("vpermilps $199, %1, %0" : "=x"(result) : "x"(src)); break;
                case 200: asm("vpermilps $200, %1, %0" : "=x"(result) : "x"(src)); break;
                case 201: asm("vpermilps $201, %1, %0" : "=x"(result) : "x"(src)); break;
                case 202: asm("vpermilps $202, %1, %0" : "=x"(result) : "x"(src)); break;
                case 203: asm("vpermilps $203, %1, %0" : "=x"(result) : "x"(src)); break;
                case 204: asm("vpermilps $204, %1, %0" : "=x"(result) : "x"(src)); break;
                case 205: asm("vpermilps $205, %1, %0" : "=x"(result) : "x"(src)); break;
                case 206: asm("vpermilps $206, %1, %0" : "=x"(result) : "x"(src)); break;
                case 207: asm("vpermilps $207, %1, %0" : "=x"(result) : "x"(src)); break;
                case 208: asm("vpermilps $208, %1, %0" : "=x"(result) : "x"(src)); break;
                case 209: asm("vpermilps $209, %1, %0" : "=x"(result) : "x"(src)); break;
                case 210: asm("vpermilps $210, %1, %0" : "=x"(result) : "x"(src)); break;
                case 211: asm("vpermilps $211, %1, %0" : "=x"(result) : "x"(src)); break;
                case 212: asm("vpermilps $212, %1, %0" : "=x"(result) : "x"(src)); break;
                case 213: asm("vpermilps $213, %1, %0" : "=x"(result) : "x"(src)); break;
                case 214: asm("vpermilps $214, %1, %0" : "=x"(result) : "x"(src)); break;
                case 215: asm("vpermilps $215, %1, %0" : "=x"(result) : "x"(src)); break;
                case 216: asm("vpermilps $216, %1, %0" : "=x"(result) : "x"(src)); break;
                case 217: asm("vpermilps $217, %1, %0" : "=x"(result) : "x"(src)); break;
                case 218: asm("vpermilps $218, %1, %0" : "=x"(result) : "x"(src)); break;
                case 219: asm("vpermilps $219, %1, %0" : "=x"(result) : "x"(src)); break;
                case 220: asm("vpermilps $220, %1, %0" : "=x"(result) : "x"(src)); break;
                case 221: asm("vpermilps $221, %1, %0" : "=x"(result) : "x"(src)); break;
                case 222: asm("vpermilps $222, %1, %0" : "=x"(result) : "x"(src)); break;
                case 223: asm("vpermilps $223, %1, %0" : "=x"(result) : "x"(src)); break;
                case 224: asm("vpermilps $224, %1, %0" : "=x"(result) : "x"(src)); break;
                case 225: asm("vpermilps $225, %1, %0" : "=x"(result) : "x"(src)); break;
                case 226: asm("vpermilps $226, %1, %0" : "=x"(result) : "x"(src)); break;
                case 227: asm("vpermilps $227, %1, %0" : "=x"(result) : "x"(src)); break;
                case 228: asm("vpermilps $228, %1, %0" : "=x"(result) : "x"(src)); break;
                case 229: asm("vpermilps $229, %1, %0" : "=x"(result) : "x"(src)); break;
                case 230: asm("vpermilps $230, %1, %0" : "=x"(result) : "x"(src)); break;
                case 231: asm("vpermilps $231, %1, %0" : "=x"(result) : "x"(src)); break;
                case 232: asm("vpermilps $232, %1, %0" : "=x"(result) : "x"(src)); break;
                case 233: asm("vpermilps $233, %1, %0" : "=x"(result) : "x"(src)); break;
                case 234: asm("vpermilps $234, %1, %0" : "=x"(result) : "x"(src)); break;
                case 235: asm("vpermilps $235, %1, %0" : "=x"(result) : "x"(src)); break;
                case 236: asm("vpermilps $236, %1, %0" : "=x"(result) : "x"(src)); break;
                case 237: asm("vpermilps $237, %1, %0" : "=x"(result) : "x"(src)); break;
                case 238: asm("vpermilps $238, %1, %0" : "=x"(result) : "x"(src)); break;
                case 239: asm("vpermilps $239, %1, %0" : "=x"(result) : "x"(src)); break;
                case 240: asm("vpermilps $240, %1, %0" : "=x"(result) : "x"(src)); break;
                case 241: asm("vpermilps $241, %1, %0" : "=x"(result) : "x"(src)); break;
                case 242: asm("vpermilps $242, %1, %0" : "=x"(result) : "x"(src)); break;
                case 243: asm("vpermilps $243, %1, %0" : "=x"(result) : "x"(src)); break;
                case 244: asm("vpermilps $244, %1, %0" : "=x"(result) : "x"(src)); break;
                case 245: asm("vpermilps $245, %1, %0" : "=x"(result) : "x"(src)); break;
                case 246: asm("vpermilps $246, %1, %0" : "=x"(result) : "x"(src)); break;
                case 247: asm("vpermilps $247, %1, %0" : "=x"(result) : "x"(src)); break;
                case 248: asm("vpermilps $248, %1, %0" : "=x"(result) : "x"(src)); break;
                case 249: asm("vpermilps $249, %1, %0" : "=x"(result) : "x"(src)); break;
                case 250: asm("vpermilps $250, %1, %0" : "=x"(result) : "x"(src)); break;
                case 251: asm("vpermilps $251, %1, %0" : "=x"(result) : "x"(src)); break;
                case 252: asm("vpermilps $252, %1, %0" : "=x"(result) : "x"(src)); break;
                case 253: asm("vpermilps $253, %1, %0" : "=x"(result) : "x"(src)); break;
                case 254: asm("vpermilps $254, %1, %0" : "=x"(result) : "x"(src)); break;
                case 255: asm("vpermilps $255, %1, %0" : "=x"(result) : "x"(src)); break;
            }
            
            // 存储结果
            _mm_storeu_ps(out, result);
            
            // 计算预期结果：每个元素独立选择同一通道内的源元素
            float expected[4];
            for (int j = 0; j < 4; j++) {
                int sel = (imm >> (j * 2)) & 3;
                expected[j] = inputs[i][sel]; // 直接使用选择索引
                
                // 特殊处理-0.0的显示问题
                if (expected[j] == 0.0f) expected[j] = 0.0f;
                if (out[j] == 0.0f) out[j] = 0.0f;
            }
            
            // 验证结果
            int valid = 1;
            for (int j = 0; j < 4; j++) {
                // 特殊处理NaN
                if (isnan(expected[j]) && isnan(out[j])) continue;
                // 比较其他值
                if (expected[j] != out[j]) {
                    valid = 0;
                    break;
                }
            }
            print_hex_float_vec("Input", inputs[i], 4);
            printf("Control imm= %d\n", imm);
            print_hex_float_vec("Output", out, 4);
            if (!valid) {
                printf("  Error: imm=%d\n", imm);
                print_hex_float_vec("    Input", inputs[i], 4);
                print_hex_float_vec("    Expected", expected, 4);
                print_hex_float_vec("    Got", out, 4);
                errors++;
                if (errors > 10) return errors; // 防止过多错误输出
            }
        }
        printf("\n");
    }
    
    return errors;
}

// 测试128位寄存器控制版本
static int test_reg_xmm() {
    int errors = 0;
    const char* test_name = "vpermilps reg (xmm)";
    printf("Testing %s\n", test_name);
    
    // 测试数据
    float in[4] = {0.0f, -0.0f, INFINITY, -INFINITY};
    float in2[4] = {1.0f, -1.0f, NAN, 123.456f};
    
    // 控制向量：每个元素的低2位作为选择位
    uint32_t ctrl0[4] = {0, 0, 0, 0};  // 选择 [0,0,0,0]
    uint32_t ctrl1[4] = {1, 2, 3, 0};  // 选择 [1,2,3,0]
    uint32_t ctrl2[4] = {2, 3, 0, 1};  // 选择 [2,3,0,1]
    uint32_t ctrl3[4] = {3, 0, 1, 2};  // 选择 [3,0,1,2]
    uint32_t* ctrls[] = {ctrl0, ctrl1, ctrl2, ctrl3};
    
    for (int c = 0; c < 4; c++) {
        float* inputs[] = {in, in2};
        for (int i = 0; i < 2; i++) {
            __m128 src = _mm_loadu_ps(inputs[i]);
            __m128 result;
            
            // 设置控制向量：每个元素只使用最低2位
            __m128i ctrl_int = _mm_setr_epi32(
                ctrls[c][0] & 3, 
                ctrls[c][1] & 3, 
                ctrls[c][2] & 3, 
                ctrls[c][3] & 3
            );
            
            // 打印输入和控制向量
            float in_arr[4];
            _mm_storeu_ps(in_arr, src);

            
            // 执行指令
            asm("vpermilps %1, %2, %0" 
                : "=x"(result) 
                : "x"(_mm_castsi128_ps(ctrl_int)), "x"(src));
            
            float out_arr[4];
            _mm_storeu_ps(out_arr, result);
            print_hex_float_vec("Input", in_arr, 4);
            print_control_bits_xmm_ps("Control", _mm_castsi128_ps(ctrl_int));
            print_hex_float_vec("Output", out_arr, 4);
            
            // 计算预期结果：每个元素独立选择同一通道内的源元素
            float expected[4];
            for (int j = 0; j < 4; j++) {
                expected[j] = inputs[i][ctrls[c][j] & 3]; // 直接使用选择索引
                
                // 特殊处理-0.0的显示问题
                if (expected[j] == 0.0f) expected[j] = 0.0f;
                if (out_arr[j] == 0.0f) out_arr[j] = 0.0f;
            }
            
            // 验证结果
            int valid = 1;
            for (int j = 0; j < 4; j++) {
                if (isnan(expected[j]) && isnan(out_arr[j])) continue;
                if (expected[j] != out_arr[j]) {
                    valid = 0;
                    break;
                }
            }
            
            if (!valid) {
                printf("  Error: ctrl=[%u,%u,%u,%u]\n", 
                      ctrls[c][0] & 3, ctrls[c][1] & 3, ctrls[c][2] & 3, ctrls[c][3] & 3);
                print_hex_float_vec("    Input", inputs[i], 4);
                print_hex_float_vec("    Expected", expected, 4);
                print_hex_float_vec("    Got", out_arr, 4);
                errors++;
            }else {
                printf("  Passed: ctrl=[%u,%u,%u,%u]\n", 
                      ctrls[c][0] & 3, ctrls[c][1] & 3, ctrls[c][2] & 3, ctrls[c][3] & 3);
                print_hex_float_vec("    Input", inputs[i], 4);
                print_hex_float_vec("    Result", out_arr, 4);
            }
        }
        printf("\n");
    }
    
    return errors;
}

// 测试256位立即数版本
static int test_imm_ymm() {
    int errors = 0;
    const char* test_name = "vpermilps imm8 (ymm)";
    printf("Testing %s\n", test_name);
    
    // 测试数据：各种边界值组合
    float in[8] = {0.0f, -0.0f, INFINITY, -INFINITY, 1.0f, -1.0f, NAN, 123.456f};
    float out[8] = {0};
    
    for (int imm = 0; imm < 256; imm++) {
        __m256 src = _mm256_loadu_ps(in);
        __m256 result;
        
            switch(imm) {
                case 0: asm("vpermilps $0, %1, %0" : "=x"(result) : "x"(src)); break;
                case 1: asm("vpermilps $1, %1, %0" : "=x"(result) : "x"(src)); break;
                case 2: asm("vpermilps $2, %1, %0" : "=x"(result) : "x"(src)); break;
                case 3: asm("vpermilps $3, %1, %0" : "=x"(result) : "x"(src)); break;
                case 4: asm("vpermilps $4, %1, %0" : "=x"(result) : "x"(src)); break;
                case 5: asm("vpermilps $5, %1, %0" : "=x"(result) : "x"(src)); break;
                case 6: asm("vpermilps $6, %1, %0" : "=x"(result) : "x"(src)); break;
                case 7: asm("vpermilps $7, %1, %0" : "=x"(result) : "x"(src)); break;
                case 8: asm("vpermilps $8, %1, %0" : "=x"(result) : "x"(src)); break;
                case 9: asm("vpermilps $9, %1, %0" : "=x"(result) : "x"(src)); break;
                case 10: asm("vpermilps $10, %1, %0" : "=x"(result) : "x"(src)); break;
                case 11: asm("vpermilps $11, %1, %0" : "=x"(result) : "x"(src)); break;
                case 12: asm("vpermilps $12, %1, %0" : "=x"(result) : "x"(src)); break;
                case 13: asm("vpermilps $13, %1, %0" : "=x"(result) : "x"(src)); break;
                case 14: asm("vpermilps $14, %1, %0" : "=x"(result) : "x"(src)); break;
                case 15: asm("vpermilps $15, %1, %0" : "=x"(result) : "x"(src)); break;
                case 16: asm("vpermilps $16, %1, %0" : "=x"(result) : "x"(src)); break;
                case 17: asm("vpermilps $17, %1, %0" : "=x"(result) : "x"(src)); break;
                case 18: asm("vpermilps $18, %1, %0" : "=x"(result) : "x"(src)); break;
                case 19: asm("vpermilps $19, %1, %0" : "=x"(result) : "x"(src)); break;
                case 20: asm("vpermilps $20, %1, %0" : "=x"(result) : "x"(src)); break;
                case 21: asm("vpermilps $21, %1, %0" : "=x"(result) : "x"(src)); break;
                case 22: asm("vpermilps $22, %1, %0" : "=x"(result) : "x"(src)); break;
                case 23: asm("vpermilps $23, %1, %0" : "=x"(result) : "x"(src)); break;
                case 24: asm("vpermilps $24, %1, %0" : "=x"(result) : "x"(src)); break;
                case 25: asm("vpermilps $25, %1, %0" : "=x"(result) : "x"(src)); break;
                case 26: asm("vpermilps $26, %1, %0" : "=x"(result) : "x"(src)); break;
                case 27: asm("vpermilps $27, %1, %0" : "=x"(result) : "x"(src)); break;
                case 28: asm("vpermilps $28, %1, %0" : "=x"(result) : "x"(src)); break;
                case 29: asm("vpermilps $29, %1, %0" : "=x"(result) : "x"(src)); break;
                case 30: asm("vpermilps $30, %1, %0" : "=x"(result) : "x"(src)); break;
                case 31: asm("vpermilps $31, %1, %0" : "=x"(result) : "x"(src)); break;
                case 32: asm("vpermilps $32, %1, %0" : "=x"(result) : "x"(src)); break;
                case 33: asm("vpermilps $33, %1, %0" : "=x"(result) : "x"(src)); break;
                case 34: asm("vpermilps $34, %1, %0" : "=x"(result) : "x"(src)); break;
                case 35: asm("vpermilps $35, %1, %0" : "=x"(result) : "x"(src)); break;
                case 36: asm("vpermilps $36, %1, %0" : "=x"(result) : "x"(src)); break;
                case 37: asm("vpermilps $37, %1, %0" : "=x"(result) : "x"(src)); break;
                case 38: asm("vpermilps $38, %1, %0" : "=x"(result) : "x"(src)); break;
                case 39: asm("vpermilps $39, %1, %0" : "=x"(result) : "x"(src)); break;
                case 40: asm("vpermilps $40, %1, %0" : "=x"(result) : "x"(src)); break;
                case 41: asm("vpermilps $41, %1, %0" : "=x"(result) : "x"(src)); break;
                case 42: asm("vpermilps $42, %1, %0" : "=x"(result) : "x"(src)); break;
                case 43: asm("vpermilps $43, %1, %0" : "=x"(result) : "x"(src)); break;
                case 44: asm("vpermilps $44, %1, %0" : "=x"(result) : "x"(src)); break;
                case 45: asm("vpermilps $45, %1, %0" : "=x"(result) : "x"(src)); break;
                case 46: asm("vpermilps $46, %1, %0" : "=x"(result) : "x"(src)); break;
                case 47: asm("vpermilps $47, %1, %0" : "=x"(result) : "x"(src)); break;
                case 48: asm("vpermilps $48, %1, %0" : "=x"(result) : "x"(src)); break;
                case 49: asm("vpermilps $49, %1, %0" : "=x"(result) : "x"(src)); break;
                case 50: asm("vpermilps $50, %1, %0" : "=x"(result) : "x"(src)); break;
                case 51: asm("vpermilps $51, %1, %0" : "=x"(result) : "x"(src)); break;
                case 52: asm("vpermilps $52, %1, %0" : "=x"(result) : "x"(src)); break;
                case 53: asm("vpermilps $53, %1, %0" : "=x"(result) : "x"(src)); break;
                case 54: asm("vpermilps $54, %1, %0" : "=x"(result) : "x"(src)); break;
                case 55: asm("vpermilps $55, %1, %0" : "=x"(result) : "x"(src)); break;
                case 56: asm("vpermilps $56, %1, %0" : "=x"(result) : "x"(src)); break;
                case 57: asm("vpermilps $57, %1, %0" : "=x"(result) : "x"(src)); break;
                case 58: asm("vpermilps $58, %1, %0" : "=x"(result) : "x"(src)); break;
                case 59: asm("vpermilps $59, %1, %0" : "=x"(result) : "x"(src)); break;
                case 60: asm("vpermilps $60, %1, %0" : "=x"(result) : "x"(src)); break;
                case 61: asm("vpermilps $61, %1, %0" : "=x"(result) : "x"(src)); break;
                case 62: asm("vpermilps $62, %1, %0" : "=x"(result) : "x"(src)); break;
                case 63: asm("vpermilps $63, %1, %0" : "=x"(result) : "x"(src)); break;
                case 64: asm("vpermilps $64, %1, %0" : "=x"(result) : "x"(src)); break;
                case 65: asm("vpermilps $65, %1, %0" : "=x"(result) : "x"(src)); break;
                case 66: asm("vpermilps $66, %1, %0" : "=x"(result) : "x"(src)); break;
                case 67: asm("vpermilps $67, %1, %0" : "=x"(result) : "x"(src)); break;
                case 68: asm("vpermilps $68, %1, %0" : "=x"(result) : "x"(src)); break;
                case 69: asm("vpermilps $69, %1, %0" : "=x"(result) : "x"(src)); break;
                case 70: asm("vpermilps $70, %1, %0" : "=x"(result) : "x"(src)); break;
                case 71: asm("vpermilps $71, %1, %0" : "=x"(result) : "x"(src)); break;
                case 72: asm("vpermilps $72, %1, %0" : "=x"(result) : "x"(src)); break;
                case 73: asm("vpermilps $73, %1, %0" : "=x"(result) : "x"(src)); break;
                case 74: asm("vpermilps $74, %1, %0" : "=x"(result) : "x"(src)); break;
                case 75: asm("vpermilps $75, %1, %0" : "=x"(result) : "x"(src)); break;
                case 76: asm("vpermilps $76, %1, %0" : "=x"(result) : "x"(src)); break;
                case 77: asm("vpermilps $77, %1, %0" : "=x"(result) : "x"(src)); break;
                case 78: asm("vpermilps $78, %1, %0" : "=x"(result) : "x"(src)); break;
                case 79: asm("vpermilps $79, %1, %0" : "=x"(result) : "x"(src)); break;
                case 80: asm("vpermilps $80, %1, %0" : "=x"(result) : "x"(src)); break;
                case 81: asm("vpermilps $81, %1, %0" : "=x"(result) : "x"(src)); break;
                case 82: asm("vpermilps $82, %1, %0" : "=x"(result) : "x"(src)); break;
                case 83: asm("vpermilps $83, %1, %0" : "=x"(result) : "x"(src)); break;
                case 84: asm("vpermilps $84, %1, %0" : "=x"(result) : "x"(src)); break;
                case 85: asm("vpermilps $85, %1, %0" : "=x"(result) : "x"(src)); break;
                case 86: asm("vpermilps $86, %1, %0" : "=x"(result) : "x"(src)); break;
                case 87: asm("vpermilps $87, %1, %0" : "=x"(result) : "x"(src)); break;
                case 88: asm("vpermilps $88, %1, %0" : "=x"(result) : "x"(src)); break;
                case 89: asm("vpermilps $89, %1, %0" : "=x"(result) : "x"(src)); break;
                case 90: asm("vpermilps $90, %1, %0" : "=x"(result) : "x"(src)); break;
                case 91: asm("vpermilps $91, %1, %0" : "=x"(result) : "x"(src)); break;
                case 92: asm("vpermilps $92, %1, %0" : "=x"(result) : "x"(src)); break;
                case 93: asm("vpermilps $93, %1, %0" : "=x"(result) : "x"(src)); break;
                case 94: asm("vpermilps $94, %1, %0" : "=x"(result) : "x"(src)); break;
                case 95: asm("vpermilps $95, %1, %0" : "=x"(result) : "x"(src)); break;
                case 96: asm("vpermilps $96, %1, %0" : "=x"(result) : "x"(src)); break;
                case 97: asm("vpermilps $97, %1, %0" : "=x"(result) : "x"(src)); break;
                case 98: asm("vpermilps $98, %1, %0" : "=x"(result) : "x"(src)); break;
                case 99: asm("vpermilps $99, %1, %0" : "=x"(result) : "x"(src)); break;
                case 100: asm("vpermilps $100, %1, %0" : "=x"(result) : "x"(src)); break;
                case 101: asm("vpermilps $101, %1, %0" : "=x"(result) : "x"(src)); break;
                case 102: asm("vpermilps $102, %1, %0" : "=x"(result) : "x"(src)); break;
                case 103: asm("vpermilps $103, %1, %0" : "=x"(result) : "x"(src)); break;
                case 104: asm("vpermilps $104, %1, %0" : "=x"(result) : "x"(src)); break;
                case 105: asm("vpermilps $105, %1, %0" : "=x"(result) : "x"(src)); break;
                case 106: asm("vpermilps $106, %1, %0" : "=x"(result) : "x"(src)); break;
                case 107: asm("vpermilps $107, %1, %0" : "=x"(result) : "x"(src)); break;
                case 108: asm("vpermilps $108, %1, %0" : "=x"(result) : "x"(src)); break;
                case 109: asm("vpermilps $109, %1, %0" : "=x"(result) : "x"(src)); break;
                case 110: asm("vpermilps $110, %1, %0" : "=x"(result) : "x"(src)); break;
                case 111: asm("vpermilps $111, %1, %0" : "=x"(result) : "x"(src)); break;
                case 112: asm("vpermilps $112, %1, %0" : "=x"(result) : "x"(src)); break;
                case 113: asm("vpermilps $113, %1, %0" : "=x"(result) : "x"(src)); break;
                case 114: asm("vpermilps $114, %1, %0" : "=x"(result) : "x"(src)); break;
                case 115: asm("vpermilps $115, %1, %0" : "=x"(result) : "x"(src)); break;
                case 116: asm("vpermilps $116, %1, %0" : "=x"(result) : "x"(src)); break;
                case 117: asm("vpermilps $117, %1, %0" : "=x"(result) : "x"(src)); break;
                case 118: asm("vpermilps $118, %1, %0" : "=x"(result) : "x"(src)); break;
                case 119: asm("vpermilps $119, %1, %0" : "=x"(result) : "x"(src)); break;
                case 120: asm("vpermilps $120, %1, %0" : "=x"(result) : "x"(src)); break;
                case 121: asm("vpermilps $121, %1, %0" : "=x"(result) : "x"(src)); break;
                case 122: asm("vpermilps $122, %1, %0" : "=x"(result) : "x"(src)); break;
                case 123: asm("vpermilps $123, %1, %0" : "=x"(result) : "x"(src)); break;
                case 124: asm("vpermilps $124, %1, %0" : "=x"(result) : "x"(src)); break;
                case 125: asm("vpermilps $125, %1, %0" : "=x"(result) : "x"(src)); break;
                case 126: asm("vpermilps $126, %1, %0" : "=x"(result) : "x"(src)); break;
                case 127: asm("vpermilps $127, %1, %0" : "=x"(result) : "x"(src)); break;
                case 128: asm("vpermilps $128, %1, %0" : "=x"(result) : "x"(src)); break;
                case 129: asm("vpermilps $129, %1, %0" : "=x"(result) : "x"(src)); break;
                case 130: asm("vpermilps $130, %1, %0" : "=x"(result) : "x"(src)); break;
                case 131: asm("vpermilps $131, %1, %0" : "=x"(result) : "x"(src)); break;
                case 132: asm("vpermilps $132, %1, %0" : "=x"(result) : "x"(src)); break;
                case 133: asm("vpermilps $133, %1, %0" : "=x"(result) : "x"(src)); break;
                case 134: asm("vpermilps $134, %1, %0" : "=x"(result) : "x"(src)); break;
                case 135: asm("vpermilps $135, %1, %0" : "=x"(result) : "x"(src)); break;
                case 136: asm("vpermilps $136, %1, %0" : "=x"(result) : "x"(src)); break;
                case 137: asm("vpermilps $137, %1, %0" : "=x"(result) : "x"(src)); break;
                case 138: asm("vpermilps $138, %1, %0" : "=x"(result) : "x"(src)); break;
                case 139: asm("vpermilps $139, %1, %0" : "=x"(result) : "x"(src)); break;
                case 140: asm("vpermilps $140, %1, %0" : "=x"(result) : "x"(src)); break;
                case 141: asm("vpermilps $141, %1, %0" : "=x"(result) : "x"(src)); break;
                case 142: asm("vpermilps $142, %1, %0" : "=x"(result) : "x"(src)); break;
                case 143: asm("vpermilps $143, %1, %0" : "=x"(result) : "x"(src)); break;
                case 144: asm("vpermilps $144, %1, %0" : "=x"(result) : "x"(src)); break;
                case 145: asm("vpermilps $145, %1, %0" : "=x"(result) : "x"(src)); break;
                case 146: asm("vpermilps $146, %1, %0" : "=x"(result) : "x"(src)); break;
                case 147: asm("vpermilps $147, %1, %0" : "=x"(result) : "x"(src)); break;
                case 148: asm("vpermilps $148, %1, %0" : "=x"(result) : "x"(src)); break;
                case 149: asm("vpermilps $149, %1, %0" : "=x"(result) : "x"(src)); break;
                case 150: asm("vpermilps $150, %1, %0" : "=x"(result) : "x"(src)); break;
                case 151: asm("vpermilps $151, %1, %0" : "=x"(result) : "x"(src)); break;
                case 152: asm("vpermilps $152, %1, %0" : "=x"(result) : "x"(src)); break;
                case 153: asm("vpermilps $153, %1, %0" : "=x"(result) : "x"(src)); break;
                case 154: asm("vpermilps $154, %1, %0" : "=x"(result) : "x"(src)); break;
                case 155: asm("vpermilps $155, %1, %0" : "=x"(result) : "x"(src)); break;
                case 156: asm("vpermilps $156, %1, %0" : "=x"(result) : "x"(src)); break;
                case 157: asm("vpermilps $157, %1, %0" : "=x"(result) : "x"(src)); break;
                case 158: asm("vpermilps $158, %1, %0" : "=x"(result) : "x"(src)); break;
                case 159: asm("vpermilps $159, %1, %0" : "=x"(result) : "x"(src)); break;
                case 160: asm("vpermilps $160, %1, %0" : "=x"(result) : "x"(src)); break;
                case 161: asm("vpermilps $161, %1, %0" : "=x"(result) : "x"(src)); break;
                case 162: asm("vpermilps $162, %1, %0" : "=x"(result) : "x"(src)); break;
                case 163: asm("vpermilps $163, %1, %0" : "=x"(result) : "x"(src)); break;
                case 164: asm("vpermilps $164, %1, %0" : "=x"(result) : "x"(src)); break;
                case 165: asm("vpermilps $165, %1, %0" : "=x"(result) : "x"(src)); break;
                case 166: asm("vpermilps $166, %1, %0" : "=x"(result) : "x"(src)); break;
                case 167: asm("vpermilps $167, %1, %0" : "=x"(result) : "x"(src)); break;
                case 168: asm("vpermilps $168, %1, %0" : "=x"(result) : "x"(src)); break;
                case 169: asm("vpermilps $169, %1, %0" : "=x"(result) : "x"(src)); break;
                case 170: asm("vpermilps $170, %1, %0" : "=x"(result) : "x"(src)); break;
                case 171: asm("vpermilps $171, %1, %0" : "=x"(result) : "x"(src)); break;
                case 172: asm("vpermilps $172, %1, %0" : "=x"(result) : "x"(src)); break;
                case 173: asm("vpermilps $173, %1, %0" : "=x"(result) : "x"(src)); break;
                case 174: asm("vpermilps $174, %1, %0" : "=x"(result) : "x"(src)); break;
                case 175: asm("vpermilps $175, %1, %0" : "=x"(result) : "x"(src)); break;
                case 176: asm("vpermilps $176, %1, %0" : "=x"(result) : "x"(src)); break;
                case 177: asm("vpermilps $177, %1, %0" : "=x"(result) : "x"(src)); break;
                case 178: asm("vpermilps $178, %1, %0" : "=x"(result) : "x"(src)); break;
                case 179: asm("vpermilps $179, %1, %0" : "=x"(result) : "x"(src)); break;
                case 180: asm("vpermilps $180, %1, %0" : "=x"(result) : "x"(src)); break;
                case 181: asm("vpermilps $181, %1, %0" : "=x"(result) : "x"(src)); break;
                case 182: asm("vpermilps $182, %1, %0" : "=x"(result) : "x"(src)); break;
                case 183: asm("vpermilps $183, %1, %0" : "=x"(result) : "x"(src)); break;
                case 184: asm("vpermilps $184, %1, %0" : "=x"(result) : "x"(src)); break;
                case 185: asm("vpermilps $185, %1, %0" : "=x"(result) : "x"(src)); break;
                case 186: asm("vpermilps $186, %1, %0" : "=x"(result) : "x"(src)); break;
                case 187: asm("vpermilps $187, %1, %0" : "=x"(result) : "x"(src)); break;
                case 188: asm("vpermilps $188, %1, %0" : "=x"(result) : "x"(src)); break;
                case 189: asm("vpermilps $189, %1, %0" : "=x"(result) : "x"(src)); break;
                case 190: asm("vpermilps $190, %1, %0" : "=x"(result) : "x"(src)); break;
                case 191: asm("vpermilps $191, %1, %0" : "=x"(result) : "x"(src)); break;
                case 192: asm("vpermilps $192, %1, %0" : "=x"(result) : "x"(src)); break;
                case 193: asm("vpermilps $193, %1, %0" : "=x"(result) : "x"(src)); break;
                case 194: asm("vpermilps $194, %1, %0" : "=x"(result) : "x"(src)); break;
                case 195: asm("vpermilps $195, %1, %0" : "=x"(result) : "x"(src)); break;
                case 196: asm("vpermilps $196, %1, %0" : "=x"(result) : "x"(src)); break;
                case 197: asm("vpermilps $197, %1, %0" : "=x"(result) : "x"(src)); break;
                case 198: asm("vpermilps $198, %1, %0" : "=x"(result) : "x"(src)); break;
                case 199: asm("vpermilps $199, %1, %0" : "=x"(result) : "x"(src)); break;
                case 200: asm("vpermilps $200, %1, %0" : "=x"(result) : "x"(src)); break;
                case 201: asm("vpermilps $201, %1, %0" : "=x"(result) : "x"(src)); break;
                case 202: asm("vpermilps $202, %1, %0" : "=x"(result) : "x"(src)); break;
                case 203: asm("vpermilps $203, %1, %0" : "=x"(result) : "x"(src)); break;
                case 204: asm("vpermilps $204, %1, %0" : "=x"(result) : "x"(src)); break;
                case 205: asm("vpermilps $205, %1, %0" : "=x"(result) : "x"(src)); break;
                case 206: asm("vpermilps $206, %1, %0" : "=x"(result) : "x"(src)); break;
                case 207: asm("vpermilps $207, %1, %0" : "=x"(result) : "x"(src)); break;
                case 208: asm("vpermilps $208, %1, %0" : "=x"(result) : "x"(src)); break;
                case 209: asm("vpermilps $209, %1, %0" : "=x"(result) : "x"(src)); break;
                case 210: asm("vpermilps $210, %1, %0" : "=x"(result) : "x"(src)); break;
                case 211: asm("vpermilps $211, %1, %0" : "=x"(result) : "x"(src)); break;
                case 212: asm("vpermilps $212, %1, %0" : "=x"(result) : "x"(src)); break;
                case 213: asm("vpermilps $213, %1, %0" : "=x"(result) : "x"(src)); break;
                case 214: asm("vpermilps $214, %1, %0" : "=x"(result) : "x"(src)); break;
                case 215: asm("vpermilps $215, %1, %0" : "=x"(result) : "x"(src)); break;
                case 216: asm("vpermilps $216, %1, %0" : "=x"(result) : "x"(src)); break;
                case 217: asm("vpermilps $217, %1, %0" : "=x"(result) : "x"(src)); break;
                case 218: asm("vpermilps $218, %1, %0" : "=x"(result) : "x"(src)); break;
                case 219: asm("vpermilps $219, %1, %0" : "=x"(result) : "x"(src)); break;
                case 220: asm("vpermilps $220, %1, %0" : "=x"(result) : "x"(src)); break;
                case 221: asm("vpermilps $221, %1, %0" : "=x"(result) : "x"(src)); break;
                case 222: asm("vpermilps $222, %1, %0" : "=x"(result) : "x"(src)); break;
                case 223: asm("vpermilps $223, %1, %0" : "=x"(result) : "x"(src)); break;
                case 224: asm("vpermilps $224, %1, %0" : "=x"(result) : "x"(src)); break;
                case 225: asm("vpermilps $225, %1, %0" : "=x"(result) : "x"(src)); break;
                case 226: asm("vpermilps $226, %1, %0" : "=x"(result) : "x"(src)); break;
                case 227: asm("vpermilps $227, %1, %0" : "=x"(result) : "x"(src)); break;
                case 228: asm("vpermilps $228, %1, %0" : "=x"(result) : "x"(src)); break;
                case 229: asm("vpermilps $229, %1, %0" : "=x"(result) : "x"(src)); break;
                case 230: asm("vpermilps $230, %1, %0" : "=x"(result) : "x"(src)); break;
                case 231: asm("vpermilps $231, %1, %0" : "=x"(result) : "x"(src)); break;
                case 232: asm("vpermilps $232, %1, %0" : "=x"(result) : "x"(src)); break;
                case 233: asm("vpermilps $233, %1, %0" : "=x"(result) : "x"(src)); break;
                case 234: asm("vpermilps $234, %1, %0" : "=x"(result) : "x"(src)); break;
                case 235: asm("vpermilps $235, %1, %0" : "=x"(result) : "x"(src)); break;
                case 236: asm("vpermilps $236, %1, %0" : "=x"(result) : "x"(src)); break;
                case 237: asm("vpermilps $237, %1, %0" : "=x"(result) : "x"(src)); break;
                case 238: asm("vpermilps $238, %1, %0" : "=x"(result) : "x"(src)); break;
                case 239: asm("vpermilps $239, %1, %0" : "=x"(result) : "x"(src)); break;
                case 240: asm("vpermilps $240, %1, %0" : "=x"(result) : "x"(src)); break;
                case 241: asm("vpermilps $241, %1, %0" : "=x"(result) : "x"(src)); break;
                case 242: asm("vpermilps $242, %1, %0" : "=x"(result) : "x"(src)); break;
                case 243: asm("vpermilps $243, %1, %0" : "=x"(result) : "x"(src)); break;
                case 244: asm("vpermilps $244, %1, %0" : "=x"(result) : "x"(src)); break;
                case 245: asm("vpermilps $245, %1, %0" : "=x"(result) : "x"(src)); break;
                case 246: asm("vpermilps $246, %1, %0" : "=x"(result) : "x"(src)); break;
                case 247: asm("vpermilps $247, %1, %0" : "=x"(result) : "x"(src)); break;
                case 248: asm("vpermilps $248, %1, %0" : "=x"(result) : "x"(src)); break;
                case 249: asm("vpermilps $249, %1, %0" : "=x"(result) : "x"(src)); break;
                case 250: asm("vpermilps $250, %1, %0" : "=x"(result) : "x"(src)); break;
                case 251: asm("vpermilps $251, %1, %0" : "=x"(result) : "x"(src)); break;
                case 252: asm("vpermilps $252, %1, %0" : "=x"(result) : "x"(src)); break;
                case 253: asm("vpermilps $253, %1, %0" : "=x"(result) : "x"(src)); break;
                case 254: asm("vpermilps $254, %1, %0" : "=x"(result) : "x"(src)); break;
                case 255: asm("vpermilps $255, %1, %0" : "=x"(result) : "x"(src)); break;
            }
        
        // 存储结果
        _mm256_storeu_ps(out, result);
        
        // 计算预期结果：每个元素独立选择同一通道内的源元素
        float expected[8];
        for (int j = 0; j < 8; j++) {
            int lane_offset = (j < 4) ? 0 : 4;
            int sel = (imm >> ((j % 4) * 2)) & 3;
            expected[j] = in[lane_offset + sel]; // 直接使用选择索引
            
            // 特殊处理-0.0的显示问题
            if (expected[j] == 0.0f) expected[j] = 0.0f;
            if (out[j] == 0.0f) out[j] = 0.0f;
        }
        
        // 验证结果
        int valid = 1;
        for (int j = 0; j < 8; j++) {
            if (isnan(expected[j]) && isnan(out[j])) continue;
            if (expected[j] != out[j]) {
                valid = 0;
                break;
            }
        }


        if (!valid) {
            printf("  Error: imm=0x%02x\n", imm);
            print_hex_float_vec("    Input", in, 8);
            print_hex_float_vec("    Expected", expected, 8);
            print_hex_float_vec("    Got", out, 8);
            errors++;
        }else{
            print_hex_float_vec("Input", in, 4);
            printf("Control imm= %d\n", imm);
            print_hex_float_vec("Output", out, 4);
        }
        printf("\n");
    }
    
    return errors;
}

// 测试256位寄存器控制版本
static int test_reg_ymm() {
    int errors = 0;
    const char* test_name = "vpermilps reg (ymm)";
    printf("Testing %s\n", test_name);
    
    // 测试数据
    float in[8] = {0.0f, -0.0f, INFINITY, -INFINITY, 1.0f, -1.0f, NAN, 123.456f};
    
    // 控制向量：每个元素的低2位作为选择位
    uint32_t ctrl0[8] = {0, 1, 2, 3, 0, 1, 2, 3};  // 顺序选择
    uint32_t ctrl1[8] = {3, 2, 1, 0, 3, 2, 1, 0};  // 逆序选择
    uint32_t ctrl2[8] = {0, 0, 1, 1, 2, 2, 3, 3};  // 重复选择
    uint32_t ctrl3[8] = {1, 3, 0, 2, 3, 1, 2, 0};  // 随机选择
    uint32_t* ctrls[] = {ctrl0, ctrl1, ctrl2, ctrl3};
    
    for (int c = 0; c < 4; c++) {
        __m256 src = _mm256_loadu_ps(in);
        __m256 result;
        
        // 设置控制向量：每个元素只使用最低2位
        __m256i ctrl_int = _mm256_setr_epi32(
            ctrls[c][0] & 3, 
            ctrls[c][1] & 3, 
            ctrls[c][2] & 3, 
            ctrls[c][3] & 3,
            ctrls[c][4] & 3, 
            ctrls[c][5] & 3, 
            ctrls[c][6] & 3, 
            ctrls[c][7] & 3
        );
        
            // 打印输入和控制向量
            float in_arr[8];
            _mm256_storeu_ps(in_arr, src);

            
            // 执行指令
            asm("vpermilps %1, %2, %0" 
                : "=x"(result) 
                : "x"(_mm256_castsi256_ps(ctrl_int)), "x"(src));
            
            float out_arr[8];
            _mm256_storeu_ps(out_arr, result);
            print_hex_float_vec("Input", in_arr, 8);
            print_control_bits_ymm_ps("Control", _mm256_castsi256_ps(ctrl_int));
            print_hex_float_vec("Output", out_arr, 8);
        
        // 计算预期结果：每个元素独立选择同一通道内的源元素
        float expected[8];
        for (int j = 0; j < 8; j++) {
            int lane_offset = (j < 4) ? 0 : 4;
            int idx = ctrls[c][j] & 3;
            expected[j] = in[lane_offset + idx]; // 直接使用选择索引
            
            // 特殊处理-0.0的显示问题
            if (expected[j] == 0.0f) expected[j] = 0.0f;
            if (out_arr[j] == 0.0f) out_arr[j] = 0.0f;
        }
        
        // 验证结果
        int valid = 1;
        for (int j = 0; j < 8; j++) {
            if (isnan(expected[j]) && isnan(out_arr[j])) continue;
            if (expected[j] != out_arr[j]) {
                valid = 0;
                break;
            }
        }
        
        if (!valid) {
            printf("  Error: ctrl=[");
            for (int j = 0; j < 8; j++) {
                printf("%u", ctrls[c][j] & 3);
                if (j < 7) printf(",");
            }
            printf("]\n");
            print_hex_float_vec("    Input", in, 8);
            print_hex_float_vec("    Expected", expected, 8);
            print_hex_float_vec("    Got", out_arr, 8);
            errors++;
        }
        printf("\n");
    }
    
    return errors;
}

int main() {
    int total_errors = 0;
    
    total_errors += test_imm_xmm();
    total_errors += test_reg_xmm();
    total_errors += test_imm_ymm();
    total_errors += test_reg_ymm();
    
    if (total_errors) {
        printf("Tests failed with %d errors\n", total_errors);
        return 1;
    } else {
        printf("All tests passed!\n");
        return 0;
    }
}
