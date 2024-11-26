/* 
 * trans.c - 矩阵转置 B = A^T
 *
 * 每个转置函数必须具有如下形式的原型：
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * 转置函数的评价标准是计算在一个 1KB 直接映射缓存中，
 * 使用 32 字节的块大小时的缓存未命中次数。
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - 这是作业 Part B 中将被评分的转置函数。
 * 请不要修改描述字符串 "Transpose submission"，因为驱动程序会
 * 搜索该字符串来识别需要评分的转置函数。
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    
}

/* 
 * 你可以在下面定义额外的转置函数。以下是一个简单的示例函数，
 * 它并未针对缓存进行优化。
 */ 
/* 
 * trans - 一个简单的基线转置函数，按行扫描，未针对缓存进行优化。
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - 此函数用于注册你的转置函数。
 * 在运行时，驱动程序会评估每个注册的函数，并总结它们的性能。
 * 这是一个用于尝试不同转置策略的方便方式。
 */
void registerFunctions()
{
    /* 注册你的解决方案函数 */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* 注册其他任何额外的转置函数 */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - 该辅助函数检查 B 是否为 A 的转置。
 * 你可以在转置函数返回之前调用此函数以检查转置的正确性。
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

