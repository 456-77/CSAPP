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
    int temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
    for (int i; i < N; i++)
    {
        for (int j = 0; j < M; j = j + 8)
            for (int k = i; k < i + 8; k++)
            {
                temp1 = A[k][j];
                temp2 = A[k][j + 1];
                temp3 = A[k][j + 2];
                temp4 = A[k][j + 3];
                temp5 = A[k][j + 4];
                temp6 = A[k][j + 5];
                temp7 = A[k][j + 6];
                temp8 = A[k][j + 7];
                B[j][k] = temp1;
                B[j + 1][k] = temp2;
                B[j + 2][k] = temp3;
                B[j + 3][k] = temp4;
                B[j + 4][k] = temp5;
                B[j + 5][k] = temp6;
                B[j + 6][k] = temp7;
                B[j + 7][k] = temp8;
            }
    }
}
// 针对64*64进行优化
char transpose_64_desc[] = "Transpose 64";
void transpose_64(int M, int N, int A[N][M], int B[M][N])
{
    int temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
    for (int i = 0; i < M; i += 8) // A,B的行
    {
        for (int j = 0; j < N; j += 8) // A,B的列
        {
            // k用来遍历4*4小块的行,列不需要另设
            // 将A的上面两块倒置并复制到B的上面
            for (int k = i; k < i + 4; k++)
            {
                // 获取一行A
                temp1 = A[k][j + 0]; // miss其他都hit
                temp2 = A[k][j + 1];
                temp3 = A[k][j + 2];
                temp4 = A[k][j + 3];
                temp5 = A[k][j + 4];
                temp6 = A[k][j + 5];
                temp7 = A[k][j + 6];
                temp8 = A[k][j + 7];
                // 复制给B
                B[j + 0][k] = temp1; // 这部分位置已经正确,后续不需要修改
                B[j + 1][k] = temp2;
                B[j + 2][k] = temp3;
                B[j + 3][k] = temp4;
                B[j + 0][k + 4] = temp5; // 后续任然需要修改
                B[j + 1][k + 4] = temp6;
                B[j + 2][k + 4] = temp7;
                B[j + 3][k + 4] = temp8;
            }

            // 现在缓存中依然保存有B上面两小块
            // 用本地变量把B的两小块存储下来
            for (int k = j; k < j + 4; k++)
            {
                // 本地变量存储B的右上部分(最开始是A的左下部分),每次遍历行
                temp1 = B[k][i + 4];
                temp2 = B[k][i + 5];
                temp3 = B[k][i + 6];
                temp4 = B[k][i + 7];
                // 本地变量存储A的左下部分,每次遍历列,因为循环的变量是j
                temp5 = A[i + 4][k];
                temp6 = A[i + 5][k];
                temp7 = A[i + 6][k];
                temp8 = A[i + 7][k];
                // 利用存储的本地变量,把A的左下复制给B的右上,按行遍历
                B[k][i + 4] = temp5;
                B[k][i + 5] = temp6;
                B[k][i + 6] = temp7;
                B[k][i + 7] = temp8;
                // 利用存储的本地变量,把A的左下复制给B的右上,按行遍历
                B[k + 4][i + 0] = temp1;
                B[k + 4][i + 1] = temp2;
                B[k + 4][i + 2] = temp3;
                B[k + 4][i + 3] = temp4;
            }
            // 现在缓存中存的都是B
            // 将A的右下复制给B的右下
            for (int k = i + 4; k < i + 8; k++)
            {
                // 利用本地变量存储A的右下部分,每次遍历行
                temp1 = A[k][j + 4];
                temp2 = A[k][j + 5];
                temp3 = A[k][j + 6];
                temp4 = A[k][j + 7];

                B[j + 4][k] = temp1;
                B[j + 5][k] = temp2;
                B[j + 6][k] = temp3;
                B[j + 7][k] = temp4;
            }
        }
    }
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

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
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
    registerTransFunction(transpose_64, transpose_64_desc);
}

/*
 * is_transpose - 该辅助函数检查 B 是否为 A 的转置。
 * 你可以在转置函数返回之前调用此函数以检查转置的正确性。
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
