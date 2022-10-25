/* 
 * trans.c - 矩阵转置B=A^T
 *每个转置函数都必须具有以下形式的原型：
 *void trans（int M，int N，int a[N][M]，int B[M][N]）；
 *通过计算，块大小为32字节的1KB直接映射缓存上的未命中数来计算转置函数。
 */ 
#include <stdio.h>
#include "cachelab.h"
#define BLOCK 16
int is_transpose(int M, int N, int A[N][M], int B[M][N]);
char transpose_submit_desc[] = "Transpose submission";  //请不要修改“Transpose_submission”


void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k;
    int temp0,temp1,temp2,temp3,temp4,temp5,temp6,temp7;
    if(M==32&&N==32){  //4个Cache大小，每8行一个Cache，需要Cache复用
        for (i = 0; i < 32; i+=8){  //8*8分块，刚好可以在处理每个块时，在相同Cache上处理
            for (j = 0; j < 32; j+=8){
                for(k = i; k < i+8; k++){
                        temp0 = A[k][j];
                        temp1 = A[k][j+1];
                        temp2 = A[k][j+2];
                        temp3 = A[k][j+3];
                        temp4 = A[k][j+4];
                        temp5 = A[k][j+5];
                        temp6 = A[k][j+6];
                        temp7 = A[k][j+7];
                        B[j][k] = temp0;
                        B[j+1][k] = temp1;
                        B[j+2][k] = temp2;
                        B[j+3][k] = temp3;
                        B[j+4][k] = temp4;
                        B[j+5][k] = temp5;
                        B[j+6][k] = temp6;
                        B[j+7][k] = temp7;
                }
            }
        }
    }

    if(M==64&&N==64){  //16个Cache大小，每4行一个Cache，需要进行Cache复用
        for (i = 0; i < 64; i+=8){  //分成8*8分块
            for (j = 0; j < 64; j+=8){   //前4行在一个Cache中，后4行在另一个Cache中，避免了Cache复用的冲突
                for(k = i; k < i+4; k++){   //将A11转置到B11，A12转置到B12
                        temp0 = A[k][j];
                        temp1 = A[k][j+1];
                        temp2 = A[k][j+2];
                        temp3 = A[k][j+3];
                        temp4 = A[k][j+4];
                        temp5 = A[k][j+5];
                        temp6 = A[k][j+6];
                        temp7 = A[k][j+7];
                        B[j][k] = temp0;
                        B[j+1][k] = temp1;
                        B[j+2][k] = temp2;
                        B[j+3][k] = temp3;
                        B[j][k+4] = temp4;
                        B[j+1][k+4] = temp5;
                        B[j+2][k+4] = temp6;
                        B[j+3][k+4] = temp7;
                }
                // for(k = i+4; k < i+8; k++){  //将B12平移到B21，将A21转置到B12，按行移
                //     temp0 = A[k][j];
                //     temp1 = A[k][j+1];
                //     temp2 = A[k][j+2];
                //     temp3 = A[k][j+3];
                //     temp4 = B[j][k];
                //     temp5 = B[j+1][k];
                //     temp6 = B[j+2][k];
                //     temp7 = B[j+3][k];
                //     B[j][k] = temp0;
                //     B[j+1][k] = temp1;
                //     B[j+2][k] = temp2;
                //     B[j+3][k] = temp3;
                //     B[j+4][k-4] = temp4;
                //     B[j+5][k-4] = temp5;
                //     B[j+6][k-4] = temp6;
                //     B[j+7][k-4] = temp7;
                // }
                for(k = j; k < j+4; k++){   //将B12平移到B21，将A21转置到B12，按列移
                    temp0 = A[i+4][k];
                    temp1 = A[i+5][k];
                    temp2 = A[i+6][k];
                    temp3 = A[i+7][k];
                    temp4 = B[k][i+4];
                    temp5 = B[k][i+5];
                    temp6 = B[k][i+6];
                    temp7 = B[k][i+7];
                    B[k][i+4] = temp0;
                    B[k][i+5] = temp1;
                    B[k][i+6] = temp2;
                    B[k][i+7] = temp3;
                    B[k+4][i] = temp4;
                    B[k+4][i+1] = temp5;
                    B[k+4][i+2] = temp6;
                    B[k+4][i+3] = temp7;
                }
                for(k = i+4; k < i+8; k++){   //将A22转置到B22
                    temp0 = A[k][j+4];
                    temp1 = A[k][j+5];
                    temp2 = A[k][j+6];
                    temp3 = A[k][j+7];
                    B[j+4][k] = temp0;
                    B[j+5][k] = temp1;
                    B[j+6][k] = temp2;
                    B[j+7][k] = temp3;
                }
            }
        }
    }

    if(M==61&&N==67){  //A为67*61的矩阵
        for(i = 0; i < 67; i += BLOCK){
            for(j = 0; j < 61; j += BLOCK){
                for(temp0 = i; temp0<i+BLOCK && temp0<67; temp0++){
                    for(temp1 = j; temp1<j+BLOCK && temp1<61; temp1++){
                        B[temp1][temp0] = A[temp0][temp1];
                    }
                }
            }
        }
    }
    /*以8*8为块：2118*/
    /*以16*16为块：1992，满足要求*/
}



/*
 * registerFunctions-此函数向驱动程序注册转置函数。
 *在运行时，驱动程序将评估每个注册的函数并总结它们的性能。这是一种试验不同转置策略的简便方法。
 */
void registerFunctions()
{
    /* 注册解决方案函数  */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

}

/* 
 * is_transpose - 函数检查B是否是A的转置。在从转置函数返回之前，可以通过调用它来检查转置的正确性。
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

