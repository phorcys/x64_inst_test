#ifndef AVXFLOAT_H
#define AVXFLOAT_H

#include <float.h>
#include <math.h>

// 128位浮点测试用例
typedef struct {
    float ALIGNED(16) input1[4];
    float ALIGNED(16) input2[4];
    const char *name;
} FloatTest128;

// 256位浮点测试用例
typedef struct {
    float ALIGNED(32) input1[8];
    float ALIGNED(32) input2[8];
    const char *name;    
} FloatTest256;

// 128位通用测试用例（修正初始化顺序和格式）
static ALIGNED(16) FloatTest128 float_tests_128[] = {
    // 正常值
    {
        {1.5f, 2.5f, 3.5f, 4.5f},   // input1
        {0.5f, 1.5f, 2.5f, 3.5f},   // input2
        "128-bit normal values"      // name
    },
    
    // 边界值
    {
        {0.0f, -0.0f, INFINITY, -INFINITY},
        {1.0f, -1.0f, 1.0f, -1.0f},
        "128-bit boundary values"
    },
    
    {
        {INFINITY, INFINITY, -INFINITY, -INFINITY},
        {0.0f, -0.0f, 0.0f, -0.0f},
        "128-bit boundary values 2"
    },

    // NaN测试
    {
        {NAN, -NAN, 1.0f, -1.0f},
        {1.0f, -1.0f, NAN, -NAN},
        "128-bit NaN tests 2"
    },
    
    {
        {NAN, -NAN, NAN, -NAN},
        {NAN, -NAN, -NAN, NAN},
        "128-bit NaN tests 2"
    },

    {
        {NAN, -NAN, NAN, -NAN},
        {0.0f, 0.0f, -0.0f, -0.0f},
        "128-bit NaN tests 3"
    },

    // 内存操作数测试
    {
        {10.0f, 20.0f, 30.0f, 40.0f},
        {0.1f, 0.2f, 0.3f, 0.4f},
        "128-bit memory operand (reg-mem)"
    },
    
    // 非规格化数
    {
        {0.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 0.0f, 2.0f},
        "Denormal values (128-bit)"
    },
    
    // 特殊值排列组合测试：0.0f 与其他所有特殊值的组合
    {
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, -0.0f, INFINITY, -INFINITY},
        "128-bit special: 0.0f combinations 1"
    },
    {
        {0.0f, 0.0f, 0.0f, 0.0f},
        {NAN, -NAN, FLT_MAX, -FLT_MAX},
        "128-bit special: 0.0f combinations 2"
    },
    {
        {0.0f, 0.0f, 0.0f, 0.0f},
        {FLT_MIN, -FLT_MIN, NAN, -NAN},
        "128-bit special: 0.0f combinations 3"
    },
    
    // 特殊值排列组合测试：-0.0f 与其他所有特殊值的组合
    {
        {-0.0f, -0.0f, -0.0f, -0.0f},
        {0.0f, -0.0f, INFINITY, -INFINITY},
        "128-bit special: -0.0f combinations 1"
    },
    {
        {-0.0f, -0.0f, -0.0f, -0.0f},
        {NAN, -NAN, FLT_MAX, -FLT_MAX},
        "128-bit special: -0.0f combinations 2"
    },
    {
        {-0.0f, -0.0f, -0.0f, -0.0f},
        {FLT_MIN, -FLT_MIN, NAN, -NAN},
        "128-bit special: -0.0f combinations 3"
    },
    
    // 特殊值排列组合测试：INFINITY 与其他所有特殊值的组合
    {
        {INFINITY, INFINITY, INFINITY, INFINITY},
        {0.0f, -0.0f, INFINITY, -INFINITY},
        "128-bit special: INFINITY combinations 1"
    },
    {
        {INFINITY, INFINITY, INFINITY, INFINITY},
        {NAN, -NAN, FLT_MAX, -FLT_MAX},
        "128-bit special: INFINITY combinations 2"
    },
    {
        {INFINITY, INFINITY, INFINITY, INFINITY},
        {FLT_MIN, -FLT_MIN, NAN, -NAN},
        "128-bit special: INFINITY combinations 3"
    },
    
    // 特殊值排列组合测试：-INFINITY 与其他所有特殊值的组合
    {
        {-INFINITY, -INFINITY, -INFINITY, -INFINITY},
        {0.0f, -0.0f, INFINITY, -INFINITY},
        "128-bit special: -INFINITY combinations 1"
    },
    {
        {-INFINITY, -INFINITY, -INFINITY, -INFINITY},
        {NAN, -NAN, FLT_MAX, -FLT_MAX},
        "128-bit special: -INFINITY combinations 2"
    },
    {
        {-INFINITY, -INFINITY, -INFINITY, -INFINITY},
        {FLT_MIN, -FLT_MIN, NAN, -NAN},
        "128-bit special: -INFINITY combinations 3"
    },
    
    // 特殊值排列组合测试：NAN 与其他所有特殊值的组合
    {
        {NAN, NAN, NAN, NAN},
        {0.0f, -0.0f, INFINITY, -INFINITY},
        "128-bit special: NAN combinations 1"
    },
    {
        {NAN, NAN, NAN, NAN},
        {NAN, -NAN, FLT_MAX, -FLT_MAX},
        "128-bit special: NAN combinations 2"
    },
    {
        {NAN, NAN, NAN, NAN},
        {FLT_MIN, -FLT_MIN, NAN, -NAN},
        "128-bit special: NAN combinations 3"
    },
    
    // 特殊值排列组合测试：-NAN 与其他所有特殊值的组合
    {
        {-NAN, -NAN, -NAN, -NAN},
        {0.0f, -0.0f, INFINITY, -INFINITY},
        "128-bit special: -NAN combinations 1"
    },
    {
        {-NAN, -NAN, -NAN, -NAN},
        {NAN, -NAN, FLT_MAX, -FLT_MAX},
        "128-bit special: -NAN combinations 2"
    },
    {
        {-NAN, -NAN, -NAN, -NAN},
        {FLT_MIN, -FLT_MIN, NAN, -NAN},
        "128-bit special: -NAN combinations 3"
    },
    
    // 特殊值排列组合测试：FLT_MAX 与其他所有特殊值的组合
    {
        {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX},
        {0.0f, -0.0f, INFINITY, -INFINITY},
        "128-bit special: FLT_MAX combinations 1"
    },
    {
        {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX},
        {NAN, -NAN, FLT_MAX, -FLT_MAX},
        "128-bit special: FLT_MAX combinations 2"
    },
    {
        {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX},
        {FLT_MIN, -FLT_MIN, NAN, -NAN},
        "128-bit special: FLT_MAX combinations 3"
    },
    
    // 特殊值排列组合测试：-FLT_MAX 与其他所有特殊值的组合
    {
        {-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX},
        {0.0f, -0.0f, INFINITY, -INFINITY},
        "128-bit special: -FLT_MAX combinations 1"
    },
    {
        {-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX},
        {NAN, -NAN, FLT_MAX, -FLT_MAX},
        "128-bit special: -FLT_MAX combinations 2"
    },
    {
        {-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX},
        {FLT_MIN, -FLT_MIN, NAN, -NAN},
        "128-bit special: -FLT_MAX combinations 3"
    },
    
    // 特殊值排列组合测试：FLT_MIN 与其他所有特殊值的组合
    {
        {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN},
        {0.0f, -0.0f, INFINITY, -INFINITY},
        "128-bit special: FLT_MIN combinations 1"
    },
    {
        {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN},
        {NAN, -NAN, FLT_MAX, -FLT_MAX},
        "128-bit special: FLT_MIN combinations 2"
    },
    {
        {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN},
        {FLT_MIN, -FLT_MIN, NAN, -NAN},
        "128-bit special: FLT_MIN combinations 3"
    },
    
    // 特殊值排列组合测试：-FLT_MIN 与其他所有特殊值的组合
    {
        {-FLT_MIN, -FLT_MIN, -FLT_MIN, -FLT_MIN},
        {0.0f, -0.0f, INFINITY, -INFINITY},
        "128-bit special: -FLT_MIN combinations 1"
    },
    {
        {-FLT_MIN, -FLT_MIN, -FLT_MIN, -FLT_MIN},
        {NAN, -NAN, FLT_MAX, -FLT_MAX},
        "128-bit special: -FLT_MIN combinations 2"
    },
    {
        {-FLT_MIN, -FLT_MIN, -FLT_MIN, -FLT_MIN},
        {FLT_MIN, -FLT_MIN, NAN, -NAN},
        "128-bit special: -FLT_MIN combinations 3"
    }
};

// 256位通用测试用例（修正初始化顺序和格式）
static ALIGNED(32) FloatTest256 float_tests_256[] = {
    // 正常值
    {
        {1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f},
        {0.9f, 1.8f, 2.7f, 3.6f, 4.5f, 5.4f, 6.3f, 7.2f},
        "256-bit normal values (reg-reg)"
    },
    
    // 边界值
    {
        {0.0f, -0.0f, INFINITY, -INFINITY, FLT_MAX, -FLT_MAX, 0.0f, -0.0f},
        {1.0f, -1.0f, 1.0f, -1.0f, 0.0f, -0.0f, FLT_MAX, -FLT_MAX},
        "256-bit boundary values (reg-reg)"
    },
    
    // NaN测试
    {
        {NAN, -NAN, 1.0f, -1.0f, NAN, -NAN, 0.0f, -0.0f},
        {1.0f, -1.0f, NAN, -NAN, NAN, -NAN, NAN, -NAN},
        "256-bit NaN tests (reg-reg)"
    },
    
    // 特殊组合
    {
        {INFINITY, -INFINITY, NAN, 0.0f, INFINITY, -INFINITY, NAN, 0.0f},
        {-INFINITY, INFINITY, INFINITY, NAN, NAN, NAN, -INFINITY, -0.0f},
        "256-bit special combinations (reg-reg)"
    },
    
    // 内存操作数测试
    {
        {10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f},
        {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f},
        "256-bit memory operand (reg-mem)"
    },
    
    // 非规格化数
    {
        {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 2.0f},
        "Denormal values (256-bit)"
    },
    
    // 特殊值排列组合测试：256位完整覆盖
    {
        {0.0f, -0.0f, INFINITY, -INFINITY, NAN, -NAN, FLT_MAX, -FLT_MAX},
        {0.0f, 0.0f, -0.0f, -0.0f, INFINITY, -INFINITY, NAN, -NAN},
        "256-bit special: full combinations 1"
    },
    {
        {FLT_MIN, -FLT_MIN, NAN, -NAN, 0.0f, -0.0f, INFINITY, -INFINITY},
        {FLT_MAX, -FLT_MAX, FLT_MIN, -FLT_MIN, NAN, -NAN, FLT_MAX, -FLT_MAX},
        "256-bit special: full combinations 2"
    },
    {
        {-INFINITY, INFINITY, -NAN, NAN, -FLT_MAX, FLT_MAX, -FLT_MIN, FLT_MIN},
        {0.0f, -0.0f, INFINITY, -INFINITY, NAN, -NAN, FLT_MAX, -FLT_MAX},
        "256-bit special: full combinations 3"
    },
    {
        {FLT_MIN, -FLT_MIN, NAN, -NAN, 0.0f, -0.0f, INFINITY, -INFINITY},
        {-FLT_MAX, FLT_MAX, -FLT_MIN, FLT_MIN, -NAN, NAN, -0.0f, 0.0f},
        "256-bit special: full combinations 4"
    }
};

// 128位双精度浮点测试用例
typedef struct {
    double ALIGNED(16) input1[2];
    double ALIGNED(16) input2[2];
    const char *name;
} DoubleTest128;

// 256位双精度浮点测试用例
typedef struct {
    double ALIGNED(32) input1[4];
    double ALIGNED(32) input2[4];
    const char *name;    
} DoubleTest256;

// 128位双精度测试用例
static ALIGNED(16) DoubleTest128 double_tests_128[] = {
    // 正常值
    {
        {1.5, 2.5},   // input1
        {0.5, 1.5},    // input2
        "128-bit double normal values"
    },
    
    // 边界值
    {
        {0.0, -0.0},
        {INFINITY, -INFINITY},
        "128-bit double boundary values"
    },
    
    // NaN测试
    {
        {NAN, -NAN},
        {1.0, -1.0},
        "128-bit double NaN tests"
    },
    
    // 内存操作数测试
    {
        {10.0, 20.0},
        {0.1, 0.2},
        "128-bit double memory operand (reg-mem)"
    },
    
    // 非规格化数
    {
        {0.0, DBL_TRUE_MIN},
        {0.0, DBL_TRUE_MIN},
        "128-bit double denormal values"
    },
    
    // 特殊值组合
    {
        {0.0, -0.0},
        {INFINITY, -INFINITY},
        "128-bit double special: 0.0 combinations"
    },
    {
        {DBL_MAX, -DBL_MAX},
        {DBL_MIN, -DBL_MIN},
        "128-bit double special: min/max combinations"
    }
};

// 256位双精度测试用例
static ALIGNED(32) DoubleTest256 double_tests_256[] = {
    // 正常值
    {
        {1.1, 2.2, 3.3, 4.4},
        {0.9, 1.8, 2.7, 3.6},
        "256-bit double normal values"
    },
    
    // 边界值
    {
        {0.0, -0.0, INFINITY, -INFINITY},
        {1.0, -1.0, 0.0, -0.0},
        "256-bit double boundary values"
    },
    
    // NaN测试
    {
        {NAN, -NAN, 1.0, -1.0},
        {1.0, -1.0, NAN, -NAN},
        "256-bit double NaN tests"
    },
    
    // 特殊组合
    {
        {INFINITY, -INFINITY, NAN, 0.0},
        {-INFINITY, INFINITY, INFINITY, NAN},
        "256-bit double special combinations"
    },
    
    // 内存操作数测试
    {
        {10.0, 20.0, 30.0, 40.0},
        {0.1, 0.2, 0.3, 0.4},
        "256-bit double memory operand (reg-mem)"
    },
    
    // 非规格化数
    {
        {0.0, DBL_TRUE_MIN, 0.0, DBL_TRUE_MIN},
        {0.0, DBL_TRUE_MIN, 0.0, DBL_TRUE_MIN},
        "256-bit double denormal values"
    },
    
    // 特殊值组合
    {
        {DBL_MAX, -DBL_MAX, DBL_MIN, -DBL_MIN},
        {0.0, -0.0, INFINITY, -INFINITY},
        "256-bit double special: min/max combinations"
    }
};

#endif // AVXFLOAT_H
